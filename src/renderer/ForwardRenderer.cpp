#include "ForwardRenderer.h"
#include "DataBinding.h"
#include "Material.h"

#include "device/Buffer.h"
#include "device/Layout.h"
#include "device/Prototypes.h"
#include "device/Utils.h"

#include "scene/Camera.h"
#include "scene/Scene.h"
#include "scene/Light.h"

#include "materials/DebugMaterial.h"

#include <stdexcept>

namespace phi {
using namespace std;
using namespace glm;

namespace {
const char *depth_vertex_shader = R"(
#version 430
layout(location=0) in vec4 in_position;
uniform mat4 OrthoMatrix;

void main() {
    gl_Position = OrthoMatrix * in_position;
}
)";

const char *depth_fragment_shader = R"(
#version 430
void main() {
    gl_FragDepth = gl_FragCoord.z;
}
)";

} // namespace

ForwardRenderer::ShadowMap::ShadowMap(int width, int height)
        : fbo(width, height),
          depth(width, height, TextureFormat::DEPTH_16),
          program() {
    fbo.SetDepthAttachment(&depth);
    assert(fbo.IsReady());
    program.SetSource(ShaderType::Vertex, depth_vertex_shader);
    program.SetSource(ShaderType::Fragment, depth_fragment_shader);
    program.Link();
}

ForwardRenderer::ForwardRenderer(int width, int height)
        : m_shadow_map(), m_proj(), m_vao(GL_NONE), m_last() {
    Resize(width, height);
    CheckedCall(phi::glCreateVertexArrays, 1, &m_vao);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    const int shadow_resolution = 2048;
    m_shadow_map = make_unique<ShadowMap>(shadow_resolution, shadow_resolution);
}

namespace {

int NumAttributeComponents(GLenum type) {
    switch (type) {
    case GL_INT:
    case GL_BOOL:
    case GL_FLOAT:
        return 1;
    case GL_FLOAT_VEC2:
        return 2;
    case GL_FLOAT_VEC3:
        return 3;
    case GL_FLOAT_VEC4:
        return 4;
    default:
        throw invalid_argument("Not yet implemented");
    }
}

string ArrayMember(const string &name, int index) {
    return name + "[" + std::to_string(index) + "]";
}

}

void ForwardRenderer::BindGlobals(const glm::mat4 &view,
                                  const glm::mat4 &model) {
    if (!!m_last.program->FindConstant("g_ProjViewModelMatrix")) {
        m_last.program->SetConstant("g_ProjViewModelMatrix",
                                    m_proj * view * model);
    }
    if (!!m_last.program->FindConstant("g_ViewModelMatrix")) {
        m_last.program->SetConstant("g_ViewModelMatrix", view * model);
    }
    if (!!m_last.program->FindConstant("g_NormalMatrix")) {
        m_last.program->SetConstant("g_NormalMatrix",
                                    mat3(transpose(inverse(model))));
    }
    if (!!m_last.program->FindConstant("g_ModelMatrix")) {
        m_last.program->SetConstant("g_ModelMatrix", model);
    }
}

void ForwardRenderer::BindLights(const vector<DirectionalLight *> &lights) {
    if (!m_last.program->FindConstant("g_Light.NumDirectionalLights")) {
        return;
    }

    m_last.program->SetConstant("g_Light.NumDirectionalLights", (int) lights.size());
    for (size_t i = 0; i < lights.size(); ++i) {
        auto item = ArrayMember("g_Light.Directional", i);
        m_last.program->SetConstant(item + ".Position", lights[i]->GetPosition());
        m_last.program->SetConstant(item + ".Color", lights[i]->GetColor());
        if (lights[i]->IsCastingShadows()) {
            m_shadow_casters.push_back(lights[i]);
        }
    }
}

void ForwardRenderer::BindLights(const vector<PointLight *> &lights) {
    if (!m_last.program->FindConstant("g_Light.NumPointLights")) {
        return;
    }

    m_last.program->SetConstant("g_Light.NumPointLights", (int) lights.size());
    for (size_t i = 0; i < lights.size(); ++i) {
        auto item = ArrayMember("g_Light.Point", i);
        m_last.program->SetConstant(item + ".Position",
                                    lights[i]->GetPosition());
        m_last.program->SetConstant(item + ".Color", lights[i]->GetColor());
        m_last.program->SetConstant(item + ".ConstantAttenuation",
                                    lights[i]->GetConstantAttenuationFactor());
        m_last.program->SetConstant(item + ".LinearAttenuation",
                                    lights[i]->GetLinearAttenuationFactor());
        m_last.program->SetConstant(item + ".QuadraticAttenuation",
                                    lights[i]->GetQuadraticAttenuationFactor());
    }
}

void ForwardRenderer::BindLayout(const phi::Layout *layout) {
    assert(m_last.vbo && "VBO must be bound before layout");
    assert(m_last.program && "Program must be bound before layout");
    CheckedCall(phi::glBindVertexArray, m_vao);
    /* Clear enabled attributes state, to prepare for rendering data of
     * different layout */
    for (int enabled_array : m_last.arrays) {
        CheckedCall(phi::glDisableVertexAttribArray, enabled_array);
    }
    m_last.arrays.clear();

    for (const auto &entry : layout->GetEntries()) {
        auto info = m_last.program->FindAttribute(entry.name);
        if (!info) {
            PHI_LOG(WARNING, "Ignoring layout <-> shader attribute '%s' as "
                             "shader does not specify it",
                    entry.name.c_str());
            continue;
        }

        m_last.arrays.push_back(info->location);
        CheckedCall(phi::glEnableVertexAttribArray, info->location);
        CheckedCall(phi::glVertexAttribPointer, info->location,
                    NumAttributeComponents(info->type), (GLenum) entry.type,
                    GL_FALSE, entry.stride, (void *) (intptr_t) entry.offset);
    }
}

void ForwardRenderer::BindProgram(phi::Program *program) {
    if (m_last.program == program) {
        return;
    }
    CheckedCall(phi::glUseProgram, program->GetId());
    m_last.program = program;
}

void ForwardRenderer::BindVbo(const phi::Buffer *buffer) {
    if (m_last.vbo == buffer) {
        return;
    }
    assert(buffer->GetType() == BufferType::Vertex);
    CheckedCall(phi::glBindBuffer, GL_ARRAY_BUFFER, buffer->GetId());
    m_last.vbo = buffer;
}

void ForwardRenderer::BindIbo(const phi::Buffer *buffer) {
    if (m_last.ibo == buffer) {
        return;
    } else {
        assert(buffer->GetType() == BufferType::Index);
        CheckedCall(phi::glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, buffer->GetId());
        m_last.ibo = buffer;
    }
}

void ForwardRenderer::Draw(PrimitiveType type, int start, int count) {
    if (!m_last.ibo) {
        CheckedCall(phi::glDrawArrays, (GLenum) type, start, count);
    } else {
#warning "TODO: sometimes incorrect assumption that indices are unsigned ints"
        CheckedCall(phi::glDrawElements, (GLenum) type, count, GL_UNSIGNED_INT,
                    (void *) (intptr_t) start);
    }
}

void ForwardRenderer::Render(phi::Scene &scene) {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawCallQueue Q;
    scene.Render(&Q);
    auto camera = scene.GetCamera();
    auto view = camera->GetViewMatrix();
    for (const DrawCall &command : Q.GetDrawCalls()) {
        Render(*camera, command);
    }
    RenderShadows();
    m_last = ForwardRenderer::State{};
    m_shadow_casters.clear();
}

void ForwardRenderer::Render(const phi::Camera &camera,
                             const phi::DrawCall &command) {
    BindProgram(command.program_binding.program);
    BindGlobals(camera.GetViewMatrix(), *command.model);
    BindLights(command.directional_lights);
    BindLights(command.point_lights);
    for (const auto &constant : command.program_binding.constants) {
        m_last.program->SetConstant(constant.name, constant.value);
    }
    uint32_t texture_unit = 0;
    for (const auto &texture : command.texture_bindings) {
        // TODO: this doesn't work, because samplers are lacking
        CheckedCall(glActiveTexture, GL_TEXTURE0 + texture_unit);
        CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, texture.texture->GetId());
        m_last.program->SetConstant(texture.name, texture_unit++);
    }
    BindVbo(command.vbo);
    BindLayout(command.layout);
    BindIbo(command.ibo);
    Draw(command.primitive, command.offset, command.count);
}

void ForwardRenderer::RenderShadows() {
 //   CheckedCall(glBindFramebuffer, GL_FRAMEBUFFER, m_shadow_map.fbo.GetId());
//    BindProgram(&m_shadow_map->program);
    for (const Light *light : m_shadow_casters) {
        vec3 dir = -normalize(light->GetPosition());
#warning "TODO: compute enclosing matrix automatically"
//        mat4 ortho = ortho(-40, 40, -40, 40, -40, 40);
//        m_shadow_map->program.SetConstant("OrthoMatrix",

    }
  //  CheckedCall(glBindFramebuffer, GL_FRAMEBUFFER, GL_NONE);
}

void ForwardRenderer::Resize(int width, int height) {
    m_proj = perspectiveFov(radians(70.0f), float(width), float(height), 0.1f,
                            10000.0f);
    SetViewport(0, 0, width, height);
}

void ForwardRenderer::SetViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

} // namespace phi
