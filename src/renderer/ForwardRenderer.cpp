#include "ForwardRenderer.h"
#include "DataBinding.h"
#include "DebugDrawer.h"
#include "Material.h"
#include "ShadowPass.h"

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

ForwardRenderer::ForwardRenderer(phi::Device &device) : m_device(device) {}

void ForwardRenderer::BindGlobals(phi::Program *program,
                                  const glm::mat4 &proj,
                                  const glm::mat4 &view,
                                  const glm::mat4 &model) {
    if (!!program->FindConstant("g_ProjViewModelMatrix")) {
        program->SetConstant("g_ProjViewModelMatrix", proj * view * model);
    }
    if (!!program->FindConstant("g_ViewModelMatrix")) {
        program->SetConstant("g_ViewModelMatrix", view * model);
    }
    if (!!program->FindConstant("g_NormalMatrix")) {
        program->SetConstant("g_NormalMatrix", mat3(transpose(inverse(model))));
    }
    if (!!program->FindConstant("g_ModelMatrix")) {
        program->SetConstant("g_ModelMatrix", model);
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

void ForwardRenderer::BindLights(
        phi::Program *program,
        const std::vector<phi::DirLight *> &dir_lights,
        const std::vector<phi::PointLight *> &point_lights) {
    bool supports_directional_lights =
            !!program->FindConstant("g_Light.NumDirectionalLights");
    bool supports_point_lights =
            !!program->FindConstant("g_Light.NumPointLights");

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

void ForwardRenderer::Execute(const glm::mat4 &proj,
                              const glm::mat4 &view,
                              const phi::DrawCall &draw,
                              const std::vector<phi::DirLight *> &dir_lights,
                              const std::vector<phi::PointLight *> &point_lights) {
    m_device.BindProgram(*draw.program);
    BindGlobals(draw.program, proj, view, draw.transform);
    BindLights(draw.program, dir_lights, point_lights);
    for (const auto &constant : draw.program_constants) {
        draw.program->SetConstant(constant.name, constant.value);
    }
    uint32_t texture_unit = 0;
    for (const phi::TextureBinding &binding : draw.texture_bindings) {
        assert(binding.sampler);
        assert(binding.texture);
        m_device.BindTexture(texture_unit, *binding.texture);
        m_device.BindSampler(texture_unit, *binding.sampler);
        draw.program->SetConstant(binding.name, texture_unit++);
    }
    m_device.BindVbo(*draw.vbo);
    m_device.BindLayout(*draw.layout);
    m_device.BindIbo(*draw.ibo);
    m_device.Draw(draw.primitive, draw.offset, draw.count);
}

void ForwardRenderer::Render(phi::Scene &scene) {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawCallQueue Q;
    scene.Render(&Q);
    const auto &dir_lights = scene.GetDirLights();
    const auto &point_lights = scene.GetPointLights();

    for (const phi::DirLight *light : dir_lights) {
        if (light->IsCastingShadows()) {
            m_shadow_casters.push_back(light);
        }
    }
    auto camera = scene.GetCamera();
    auto proj = camera->GetProjMatrix();
    auto view = camera->GetViewMatrix();

    // Yes, I know; But keep in mind this is temporary.
    const uint32_t shadowmap_res = 2048;
    static phi::ShadowPass<phi::DirLight> shadowpass(*this, shadowmap_res);
    shadowpass.SetDrawCalls(Q.GetDrawCalls());
    shadowpass.SetObjectsAABB(scene.GetAABB());
    for (const phi::DirLight *light: m_shadow_casters) {
        shadowpass.SetLight(*light);
        shadowpass.Run();
    }

    auto shadow_matrix = shadowpass.GetShadowMatrix();
    float shadow_texel_size = 1./shadowmap_res;
    for (const phi::DrawCall &drawcall : Q.GetDrawCalls()) {
        phi::DrawCall current = drawcall;
        current.texture_bindings.push_back(
                { "DepthMap", &shadowpass.GetShadowMap(),
                  &Sampler::Bilinear2D(WrapMode::Clamp) });
        current.program_constants.push_back(
                { "g_ShadowMatrix", &shadow_matrix });
        current.program_constants.push_back(
                { "DepthTexelSize", &shadow_texel_size });
        Execute(proj, view, current, dir_lights, point_lights);
    }
    m_shadow_casters.clear();
}

void ForwardRenderer::Execute(const phi::DrawCall &drawcall,
                              const phi::Camera &camera) {
    Execute(camera.GetProjMatrix(), camera.GetViewMatrix(), drawcall, {}, {});
}

phi::Device &ForwardRenderer::GetDevice() {
    return m_device;
}

} // namespace phi
