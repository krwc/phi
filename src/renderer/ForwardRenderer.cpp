#include "ForwardRenderer.h"
#include "DataBinding.h"
#include "DebugDrawer.h"
#include "Material.h"
#include "ShadowMapPass.h"

#include "device/Buffer.h"
#include "device/Layout.h"
#include "device/Prototypes.h"
#include "device/Sampler.h"
#include "device/Utils.h"

#include "scene/Camera.h"
#include "scene/Scene.h"
#include "scene/Light.h"

#include <stdexcept>

namespace phi {
using namespace std;
using namespace glm;

ForwardRenderer::ForwardRenderer(int width, int height)
        : m_width(width),
          m_height(height),
          m_zwrite(true),
          m_scissor({ 0, 0, width, height }),
          m_viewport({ 0, 0, width, height }),
          m_vao(GL_NONE),
          m_last() {
    Resize(width, height);
    CheckedCall(phi::glCreateVertexArrays, 1, &m_vao);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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

}

void ForwardRenderer::BindGlobals(const glm::mat4 &proj,
                                  const glm::mat4 &view,
                                  const glm::mat4 &model) {
    if (!!m_last.program->FindConstant("g_ProjViewModelMatrix")) {
        m_last.program->SetConstant("g_ProjViewModelMatrix",
                                    proj * view * model);
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

namespace {

const std::string POINT_LIGHT_PARAM_ATT_CONSTANT = ".ConstantAttenuation";
const std::string POINT_LIGHT_PARAM_ATT_LINEAR = ".LinearAttenuation";
const std::string POINT_LIGHT_PARAM_ATT_QUADRATIC = ".QuadraticAttenuation";

const std::string LIGHT_PARAM_POSITION = ".Position";
const std::string LIGHT_PARAM_COLOR = ".Color";

std::string ArrayMember(const std::string &name, uint32_t index) {
    return name + "[" + std::to_string(index) + "]";
}

}

void ForwardRenderer::BindLights(const std::vector<phi::DirLight *> &dir_lights,
                                 const std::vector<phi::PointLight *> &point_lights) {
    bool supports_directional_lights =
            !!m_last.program->FindConstant("g_Light.NumDirectionalLights");
    bool supports_point_lights =
            !!m_last.program->FindConstant("g_Light.NumPointLights");
    phi::Program *program = m_last.program;

    if (supports_directional_lights) {
        program->SetConstant("g_Light.NumDirectionalLights",
                             int(dir_lights.size()));

        for (uint32_t i = 0; i < dir_lights.size(); ++i) {
            auto &&item = ArrayMember("g_Light.Directional", i);
            program->SetConstant(item + LIGHT_PARAM_POSITION,
                                 dir_lights[i]->GetPosition());
            program->SetConstant(item + LIGHT_PARAM_COLOR,
                                 dir_lights[i]->GetColor());
        }
    }

    if (supports_point_lights) {
        program->SetConstant("g_Light.NumPointLights",
                             int(point_lights.size()));

        for (uint32_t i = 0; i < point_lights.size(); ++i) {
            auto &&item = ArrayMember("g_Light.Point", i);
            program->SetConstant(item + LIGHT_PARAM_POSITION,
                                 point_lights[i]->GetPosition());
            program->SetConstant(item + LIGHT_PARAM_COLOR,
                                 point_lights[i]->GetColor());
            program->SetConstant(item + POINT_LIGHT_PARAM_ATT_CONSTANT,
                                 point_lights[i]->GetConstantAttenuation());
            program->SetConstant(item + POINT_LIGHT_PARAM_ATT_LINEAR,
                                 point_lights[i]->GetLinearAttenuation());
            program->SetConstant(item + POINT_LIGHT_PARAM_ATT_QUADRATIC,
                                 point_lights[i]->GetQuadraticAttenuation());
        }
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

void ForwardRenderer::Draw(Primitive type, int start, int count) {
    if (!m_last.ibo) {
        CheckedCall(phi::glDrawArrays, (GLenum) type, start, count);
    } else {
#warning "TODO: sometimes incorrect assumption that indices are unsigned ints"
        CheckedCall(phi::glDrawElements, (GLenum) type, count, GL_UNSIGNED_INT,
                    (void *) (intptr_t) start);
    }
}

void ForwardRenderer::Execute(const glm::mat4 &proj,
                              const glm::mat4 &view,
                              const phi::DrawCall &draw) {
    BindProgram(draw.program);
    BindGlobals(proj, view, draw.transform);
    BindLights(draw.dir_lights, draw.point_lights);
    for (const auto &constant : draw.program_constants) {
        m_last.program->SetConstant(constant.name, constant.value);
    }
    uint32_t texture_unit = 0;
    for (const phi::TextureBinding &binding : draw.texture_bindings) {
        assert(binding.sampler);
        assert(binding.texture);
        CheckedCall(glActiveTexture, GL_TEXTURE0 + texture_unit);
        CheckedCall(phi::glBindSampler, texture_unit, binding.sampler->GetId());
        CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, binding.texture->GetId());
        m_last.program->SetConstant(binding.name, texture_unit++);
    }
    BindVbo(draw.vbo);
    BindLayout(draw.layout);
    BindIbo(draw.ibo);
    Draw(draw.primitive, draw.offset, draw.count);
}

void ForwardRenderer::Render(phi::Scene &scene) {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawCallQueue Q;
    scene.Render(&Q);
    for (const phi::DrawCall &drawcall : Q.GetDrawCalls()) {
        for (const phi::DirLight *light : drawcall.dir_lights) {
            if (light->IsCastingShadows()) {
                m_shadow_casters.push_back(light);
            }
        }
    }
    auto camera = scene.GetCamera();
    auto proj = camera->GetProjMatrix();
    auto view = camera->GetViewMatrix();

    // Yes, I know; But keep in mind this is temporary.
    const uint32_t shadowmap_res = 2048;
    static phi::ShadowMapPass<phi::DirLight> shadowpass(*this, shadowmap_res);
    shadowpass.SetDrawCalls(Q.GetDrawCalls());
    shadowpass.SetObjectsAABB(scene.GetAABB());
    for (const phi::DirLight *light: m_shadow_casters) {
        shadowpass.SetLight(*light);
        shadowpass.Draw(*camera);
    }

    auto shadow_matrix = shadowpass.GetShadowMatrix();
    float shadow_texel_size = 1./shadowmap_res;
    for (const phi::DrawCall &drawcall : Q.GetDrawCalls()) {
        phi::DrawCall current = drawcall;
        current.texture_bindings.push_back(
                { "DepthMap", &shadowpass.GetShadowMap(),
                  Sampler::Nearest2D(WrapMode::Clamp) });
        current.program_constants.push_back(
                { "g_ShadowMatrix", &shadow_matrix });
        current.program_constants.push_back(
                { "DepthTexelSize", &shadow_texel_size });
        Execute(proj, view, current);
    }
    m_last = ForwardRenderer::State{};
    m_shadow_casters.clear();
}

void ForwardRenderer::Execute(const phi::DrawCall &drawcall,
                              const phi::Camera &camera) {
    Execute(camera.GetProjMatrix(), camera.GetViewMatrix(), drawcall);
}

void ForwardRenderer::Resize(int width, int height) {
    SetViewport({ 0, 0, width, height });
    SetScissor({ 0, 0, width, height });
    m_width = width;
    m_height = height;
}

void ForwardRenderer::SetViewport(const phi::Rect2D &viewport) {
    m_viewport = viewport;
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
}

void ForwardRenderer::SetScissor(const phi::Rect2D &scissor) {
    m_scissor = scissor;
    glScissor(scissor.x, scissor.y, scissor.w, scissor.h);
}

void ForwardRenderer::SetFrameBuffer(phi::FrameBuffer &target) {
    CheckedCall(phi::glBindFramebuffer, GL_FRAMEBUFFER, target.GetId());
}

const phi::Rect2D &ForwardRenderer::GetViewport() const {
    return m_viewport;
}

const phi::Rect2D &ForwardRenderer::GetScissor() const {
    return m_scissor;
}

void ForwardRenderer::ClearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::SetZWrite(bool enabled) {
    m_zwrite = enabled;
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}

bool ForwardRenderer::GetZWrite() const {
    return m_zwrite;
}

} // namespace phi
