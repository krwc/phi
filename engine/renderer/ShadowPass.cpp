#include "ShadowPass.h"

#include "math/Rect2D.h"

namespace phi {

ShadowPass<phi::DirLight>::ShadowPass(phi::Device &device, uint32_t resolution)
        : m_device(device),
          m_resolution(resolution),
          m_fbo(resolution, resolution),
          m_depth_program(),
          m_depth(resolution, resolution, phi::TextureFormat::DEPTH_24) {
    m_fbo.SetDepthAttachment(phi::DepthAttachment{ &m_depth });
    assert(m_fbo.IsReady());
    // FIXME: use program cache.
    m_depth_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/ShadowPass.vs");
    m_depth_program.SetSourceFromFile(phi::ShaderType::Fragment, "assets/shaders/ShadowPass.fs");
    m_depth_program.Link();
}

void ShadowPass<phi::DirLight>::RecomputeLightFrustum() {
    glm::vec3 light_direction = -glm::normalize(m_config->light->GetPosition());
    glm::mat4 light_view =
            glm::lookAtRH({ 0, 0, 0 }, light_direction, { 0, 1, 0 });
    // Note: this is a very rough approximation of the light frustum that does
    // not take into account actual camera view, but at least it is computed
    // dynamically.
    phi::AABB lightspace_aabb = light_view * (*m_config->aabb);
    glm::vec3 aabb_min = lightspace_aabb.GetMin();
    glm::vec3 aabb_max = lightspace_aabb.GetMax();
    // Achtung: heavy magic. Why the hell do I need to do this? What is wrong
    // with these matrices that they transform coordinates in such a weird way?
    std::swap(aabb_min.z, aabb_max.z);
    aabb_min.z *= -1;
    aabb_max.z *= -1;

    m_light_camera = phi::OrthoCamera(light_view, aabb_min, aabb_max);
}

void ShadowPass<phi::DirLight>::Setup(
        const phi::ShadowPass<phi::DirLight>::Config &config) {
    m_config = &config;
    RecomputeLightFrustum();
}

void ShadowPass<phi::DirLight>::Run() {
    assert(m_config);
    const phi::Camera &light_camera = GetLightCamera();
    phi::Rect2D viewport = m_device.GetViewport();
    phi::Rect2D scissor = m_device.GetScissor();
    const phi::Rect2D shadowmap_rect{ 0, 0, (int) m_resolution, (int) m_resolution };

    m_device.BindFrameBuffer(&m_fbo);
    m_device.SetViewport(shadowmap_rect);
    m_device.SetScissor(shadowmap_rect);
    m_device.ClearDepth();
    m_device.BindProgram(&m_depth_program);
    glm::mat4 proj_view =
            light_camera.GetProjMatrix() * light_camera.GetViewMatrix();

    for (const phi::DrawCall &draw_call : m_config->draw_calls) {
        m_device.BindLayout(draw_call.layout);
        m_device.BindVbo(draw_call.vbo);
        m_device.BindIbo(draw_call.ibo);
        m_depth_program.SetConstant("ProjViewModelMatrix",
                                    proj_view * draw_call.transform);
        m_device.Draw(draw_call.primitive, draw_call.offset, draw_call.count);
    }
    m_device.SetViewport(viewport);
    m_device.SetScissor(scissor);
}

const phi::Camera &ShadowPass<phi::DirLight>::GetLightCamera() const {
    return m_light_camera;
}

glm::mat4 ShadowPass<phi::DirLight>::GetShadowMatrix() const {
    const phi::Camera &light_camera = GetLightCamera();
    return light_camera.GetProjMatrix() * light_camera.GetViewMatrix();
}

} // namespace phi
