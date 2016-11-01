#include "ShadowMapPass.h"
#include "Renderer.h"

#include "device/Sampler.h"

#include "math/Rect2D.h"

#include "DebugDrawer.h"

namespace phi {

namespace {
const char *depth_vertex_shader = R"(
#version 430
layout(location=0) in vec4 in_position;
uniform mat4 g_ProjViewModelMatrix;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_position;
}
)";

const char *depth_fragment_shader = R"(
#version 430

void main() {
    // This is basically a noop (look at glspec for details).
    // Added just for clarity.
    gl_FragDepth = gl_FragCoord.z;
}
)";

const char *shadowmap_vertex_shader = R"(
#version 430
layout(location=0) in vec4 in_position;
uniform mat4 g_ShadowMatrix;
uniform mat4 g_ModelMatrix;
uniform mat4 g_ProjViewModelMatrix;

out vec4 ShadowCoord;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_position;
    ShadowCoord = g_ShadowMatrix * g_ModelMatrix * in_position;
}
)";

const char *shadowmap_fragment_shader = R"(
#version 430
uniform sampler2D DepthMap;
out vec4 FragColor;
in vec4 ShadowCoord;

void main() {
    const float bias = 0.0005;
    vec3 ProjCoords = 0.5 * ShadowCoord.xyz / ShadowCoord.w + 0.5;
    float CurrentDepth = ProjCoords.z - bias;
    float ClosestDepth = texture(DepthMap, ProjCoords.xy).r;

    if (CurrentDepth > ClosestDepth) {
        FragColor = vec4(0,0,0,1);
    } else {
        discard;
    }
}
)";
}

ShadowMapPass<phi::DirLight>::ShadowMapPass(uint32_t resolution)
        : m_resolution(resolution),
          m_needs_update(true),
          m_fbo(resolution, resolution),
          m_depth_program(),
          m_shadow_program(),
          m_depth(resolution, resolution, phi::TextureFormat::DEPTH_24) {
    m_fbo.SetDepthAttachment(phi::DepthAttachment{ &m_depth });
    assert(m_fbo.IsReady());
    // FIXME: use program cache.
    m_depth_program.SetSource(phi::ShaderType::Vertex, depth_vertex_shader);
    m_depth_program.SetSource(phi::ShaderType::Fragment, depth_fragment_shader);
    m_depth_program.Link();

    m_shadow_program.SetSource(phi::ShaderType::Vertex, shadowmap_vertex_shader);
    m_shadow_program.SetSource(phi::ShaderType::Fragment, shadowmap_fragment_shader);
    m_shadow_program.Link();
}

void ShadowMapPass<phi::DirLight>::RecomputeLightFrustum() const {
    glm::vec3 light_direction = -glm::normalize(m_light.GetPosition());
    glm::mat4 light_view =
            glm::lookAtRH({ 0, 0, 0 }, light_direction, { 0, 1, 0 });
    // Note: this is a very rough approximation of the light frustum that does
    // not take into account actual camera view, but at least it is computed
    // dynamically.
    phi::AABB lightspace_aabb = light_view * m_aabb;
    glm::vec3 aabb_min = lightspace_aabb.GetMin();
    glm::vec3 aabb_max = lightspace_aabb.GetMax();
    // Achtung: heavy magic. Why the hell do I need to do this? What is wrong
    // with these matrices that they transform coordinates in such a weird way?
    std::swap(aabb_min.z, aabb_max.z);
    aabb_min.z *= -1;
    aabb_max.z *= -1;

    m_light_camera = phi::OrthoCamera(light_view, aabb_min, aabb_max);
}

void ShadowMapPass<phi::DirLight>::SetLight(const phi::DirLight &light) {
    m_needs_update = true;
    m_light = light;
}

void ShadowMapPass<phi::DirLight>::SetDrawCalls(
        const std::vector<phi::DrawCall> &drawcalls) {
    m_needs_update = true;
    m_drawcalls.clear();
    m_drawcalls.reserve(drawcalls.size());
    for (const phi::DrawCall &drawcall : drawcalls) {
        phi::DrawCall shadow_drawcall{};
        shadow_drawcall.primitive = drawcall.primitive;
        shadow_drawcall.transform = drawcall.transform;
        shadow_drawcall.program = &m_depth_program;
        shadow_drawcall.layout = drawcall.layout;
        shadow_drawcall.vbo = drawcall.vbo;
        shadow_drawcall.ibo = drawcall.ibo;
        shadow_drawcall.program_constants = {};
        shadow_drawcall.texture_bindings = {};
        shadow_drawcall.point_lights = {};
        shadow_drawcall.dir_lights = {};
        shadow_drawcall.count = drawcall.count;
        shadow_drawcall.offset = drawcall.offset;
        m_drawcalls.push_back(shadow_drawcall);
    }
}

void ShadowMapPass<phi::DirLight>::SetObjectsAABB(const phi::AABB &aabb) {
    m_needs_update = true;
    m_aabb = aabb;
}

void ShadowMapPass<phi::DirLight>::Draw(phi::Renderer &renderer,
                                        const phi::Camera &camera) {
    const phi::Camera &light_camera = GetLightCamera();
    const phi::Rect2D shadowmap_rect{ 0, 0, (int) m_resolution, (int) m_resolution };
    phi::Rect2D viewport = renderer.GetViewport();
    phi::Rect2D scissor = renderer.GetScissor();

    renderer.SetFrameBuffer(m_fbo);
    renderer.SetViewport(shadowmap_rect);
    renderer.SetScissor(shadowmap_rect);
    renderer.ClearDepth();
    for (const phi::DrawCall &drawcall : m_drawcalls) {
        renderer.Execute(drawcall, light_camera);
    }
    renderer.SetViewport(viewport);
    renderer.SetScissor(scissor);

    renderer.SetFrameBuffer(DefaultFrameBuffer::Instance());
    glm::mat4 shadow_matrix = light_camera.GetProjMatrix() * light_camera.GetViewMatrix();
    renderer.SetZWrite(false);
    for (const phi::DrawCall &drawcall : m_drawcalls) {
        DrawCall shadow_drawcall = drawcall;
        shadow_drawcall.program = &m_shadow_program;
        shadow_drawcall.texture_bindings = {
            { "DepthMap", &m_depth, Sampler::Bilinear2D(WrapMode::Clamp) }
        };
        shadow_drawcall.program_constants = { { "g_ShadowMatrix",
                                                &shadow_matrix } };
        renderer.Execute(shadow_drawcall, camera);
    }
    renderer.SetZWrite(true);
}

const phi::Camera &ShadowMapPass<phi::DirLight>::GetLightCamera() const {
    if (m_needs_update) {
        m_needs_update = false;
        RecomputeLightFrustum();
    }
    return m_light_camera;
}

} // namespace phi
