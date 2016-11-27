#include "LightPass.h"
#include "Common.h"

#include "device/Buffer.h"
#include "device/Device.h"
#include "device/Layout.h"
#include "device/Texture.h"
#include "device/Sampler.h"

#include "scene/Camera.h"
#include "scene/Light.h"

#include "io/File.h"

namespace phi {

LightPass::LightPass(phi::Device &device)
        : m_device(device),
          m_program(),
          m_config(),
          m_shadow_filtering(LightPass::PCF_5x5) {
    m_program.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Quad.vs").c_str());
    m_program.SetSource(
            phi::ShaderType::Fragment,
            phi::io::FileContents("assets/shaders/LightPass.fs").c_str());
    m_program.Link();
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
    m_program.SetConstant("g_LightInfo.NumDirLights",
                          static_cast<int>(m_config->dir_lights->size()));
    const glm::mat4 view = m_config->camera->GetViewMatrix();

    for (uint32_t i = 0; i < m_config->dir_lights->size(); ++i) {
        auto &&item = ArrayMember("g_LightInfo.Dir", i);
        // Transform light direction to view space.
        const glm::vec4 position =
                glm::vec4((*m_config->dir_lights)[i]->GetPosition(), 0.0f);
        const glm::vec3 direction = glm::normalize(glm::vec3(view * position));
        m_program.SetConstant(item + LIGHT_PARAM_DIRECTION, -direction);
        m_program.SetConstant(item + LIGHT_PARAM_COLOR,
                              (*m_config->dir_lights)[i]->GetColor());
    }

    m_program.SetConstant("g_LightInfo.NumPointLights",
                          int(m_config->point_lights->size()));
    for (uint32_t i = 0; i < m_config->point_lights->size(); ++i) {
        auto &&item = ArrayMember("g_LightInfo.Point", i);
        const glm::vec4 position =
                glm::vec4((*m_config->point_lights)[i]->GetPosition(), 1.0f);
        const glm::vec3 view_position = glm::vec3(view * position);
        m_program.SetConstant(item + LIGHT_PARAM_POSITION, view_position);
        m_program.SetConstant(item + LIGHT_PARAM_COLOR,
                              (*m_config->point_lights)[i]->GetColor());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_CONSTANT,
                              (*m_config->point_lights)[i]->GetConstantAttenuation());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_LINEAR,
                              (*m_config->point_lights)[i]->GetLinearAttenuation());
        m_program.SetConstant(item + POINT_LIGHT_PARAM_ATT_QUADRATIC,
                              (*m_config->point_lights)[i]->GetQuadraticAttenuation());
    }
}

void LightPass::Run() {
    const float texel_size = 1.0f / m_config->texture_shadow->GetWidth();
    m_device.BindProgram(&m_program);
    m_device.BindTexture(0, m_config->texture_depth);
    m_device.BindTexture(1, m_config->texture_normal);
    m_device.BindTexture(2, m_config->texture_diffuse);
    m_device.BindTexture(3, m_config->texture_shadow);
    m_device.BindTexture(4, m_config->texture_ao);

    m_device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(1, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(2, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(3, &phi::Samplers<phi::WrapMode::ClampToEdge>::Shadow2D());
    m_device.BindSampler(4, &phi::Samplers<phi::WrapMode::ClampToEdge>::Bilinear2D());

    glm::mat4 inv_view = glm::inverse(m_config->camera->GetViewMatrix());
    m_program.SetConstant("g_TexDepth", static_cast<int>(0));
    m_program.SetConstant("g_TexNormal", static_cast<int>(1));
    m_program.SetConstant("g_TexDiffuse", static_cast<int>(2));
    m_program.SetConstant("g_TexShadow", static_cast<int>(3));
    m_program.SetConstant("g_TexAo", static_cast<int>(4));
    m_program.SetConstant("g_ShadowMatrix", m_config->shadow_matrix * inv_view);
    m_program.SetConstant("g_InvProjMatrix", glm::inverse(m_config->camera->GetProjMatrix()));
    m_program.SetConstant("g_DepthTexelSize", texel_size);
    m_program.SetConstant("g_ShadowFiltering", static_cast<int>(m_shadow_filtering));
    SetupLights();
    m_device.BindLayout(&Common::QuadLayout());
    m_device.BindVbo(&Common::QuadVbo());
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
}

void LightPass::SetShadowFiltering(phi::LightPass::ShadowFiltering filtering) {
    m_shadow_filtering = filtering;
}

phi::LightPass::ShadowFiltering LightPass::GetShadowFiltering() const {
    return m_shadow_filtering;
}

} // namespace phi
