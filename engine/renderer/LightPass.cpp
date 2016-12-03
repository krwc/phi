#include "LightPass.h"
#include "Common.h"

#include "device/Buffer.h"
#include "device/Device.h"
#include "device/Layout.h"
#include "device/Texture.h"
#include "device/Sampler.h"

#include "scene/Camera.h"
#include "scene/Light.h"

#include "utils/ShaderPreprocessor.h"

namespace phi {

LightPass::LightPass(phi::Device &device)
        : m_device(device),
          m_program(),
          m_config(),
          m_shadow_filtering(LightPass::PCF_5x5) {
    CompileProgram();
}

void LightPass::Setup(const phi::LightPass::Config &config) {
    m_config = &config;
}

namespace {

const std::string POINT_LIGHT_PARAM_ATT_CONSTANT = ".AttenuationConstant";
const std::string POINT_LIGHT_PARAM_ATT_LINEAR = ".AttenuationLinear";
const std::string POINT_LIGHT_PARAM_ATT_QUADRATIC = ".AttenuationQuadratic";

const std::string LIGHT_PARAM_DIRECTION = ".Direction";
const std::string LIGHT_PARAM_POSITION = ".Position";
const std::string LIGHT_PARAM_COLOR = ".Color";

std::string ArrayMember(const std::string &name, uint32_t index) {
    return name + "[" + std::to_string(index) + "]";
}

}

void LightPass::SetupLights() {
    const glm::mat4 view = m_config->camera->GetViewMatrix();

    uint32_t dir_light_idx = 0u;
    for (const phi::DirLight *light : m_config->dir_lights) {
        auto &&item = ArrayMember("g_LightInfo.Dir", dir_light_idx);
        // Transform light direction to view space.
        const glm::vec4 position = glm::vec4(light->GetPosition(), 0.0f);
        const glm::vec3 direction = glm::normalize(glm::vec3(view * position));
        m_program.SetConstant(item + LIGHT_PARAM_DIRECTION, -direction);
        m_program.SetConstant(item + LIGHT_PARAM_COLOR, light->GetColor());
        ++dir_light_idx;
    }

    uint32_t point_light_idx = 0u;
    for (const phi::PointLight *light : m_config->point_lights) {
        auto &&item = ArrayMember("g_LightInfo.Point", point_light_idx);
        const glm::vec4 position = glm::vec4(light->GetPosition(), 1.0f);
        const glm::vec3 view_position = glm::vec3(view * position);
        m_program.SetConstant(item + LIGHT_PARAM_POSITION, view_position);
        m_program.SetConstant(item + LIGHT_PARAM_COLOR,
                              light->GetColor());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_CONSTANT,
                              light->GetConstantAttenuation());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_LINEAR,
                              light->GetLinearAttenuation());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_QUADRATIC,
                              light->GetQuadraticAttenuation());
        ++point_light_idx;
    }
    m_program.SetConstant("g_LightInfo.NumDirLights", dir_light_idx);
    m_program.SetConstant("g_LightInfo.NumPointLights", point_light_idx);
}

void LightPass::CompileProgram() {
    phi::ShaderPreprocessor spp("assets/shaders/LightPass.fs");
    if (m_shadow_filtering == LightPass::PCF_5x5) {
        spp.Define("SHADOW_FILTERING_PCF_5x5");
    } else if (m_shadow_filtering == LightPass::PCF_9x9) {
        spp.Define("SHADOW_FILTERING_PCF_9x9");
    } else {
        spp.Define("SHADOW_FILTERING_PCF_3x3");
    }
    m_program = phi::Program{};
    m_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/Quad.vs");
    m_program.SetSource(phi::ShaderType::Fragment, spp.Preprocess());
    m_program.Link();
}

void LightPass::Run() {
    const float texel_size = 1.0f / m_config->texture_shadow->GetWidth();
    m_device.BindProgram(&m_program);
    m_device.BindTexture(0, m_config->texture_depth);
    m_device.BindTexture(1, m_config->texture_normal);
    m_device.BindTexture(2, m_config->texture_diffuse);
    m_device.BindTexture(3, m_config->texture_ao);
    m_device.BindTexture(4, m_config->texture_shadow);

    m_device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(1, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(2, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(3, &phi::Samplers<phi::WrapMode::ClampToEdge>::Bilinear2D());
    m_device.BindSampler(4, &phi::Samplers<phi::WrapMode::ClampToEdge>::Shadow2D());

    glm::mat4 inv_view = glm::inverse(m_config->camera->GetViewMatrix());
    m_program.SetConstant("g_ShadowMatrix", m_config->shadow_matrix * inv_view);
    m_program.SetConstant("g_InvProjMatrix", glm::inverse(m_config->camera->GetProjMatrix()));
    m_program.SetConstant("g_DepthTexelSize", texel_size);
    SetupLights();
    m_device.BindLayout(&Common::QuadLayout());
    m_device.BindVbo(&Common::QuadVbo());
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
}

void LightPass::SetShadowFiltering(phi::LightPass::ShadowFiltering filtering) {
    m_shadow_filtering = filtering;
    CompileProgram();
}

phi::LightPass::ShadowFiltering LightPass::GetShadowFiltering() const {
    return m_shadow_filtering;
}

} // namespace phi
