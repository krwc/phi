#include "LightPass.h"
#include "device/Buffer.h"
#include "device/Device.h"
#include "device/Layout.h"
#include "device/Texture.h"
#include "device/Sampler.h"

#include "scene/Light.h"

#include "io/File.h"

namespace phi {
namespace {

const phi::Layout quad_layout{{ "in_Position", 0u, sizeof(glm::vec2), phi::Type::Float }};

const glm::vec2 quad[] = { { -1, -1 }, { 1, -1 }, { -1, 1 },
                           { -1, 1 },  { 1, -1 }, { 1, 1 } };


} // namespace

LightPass::LightPass(phi::Device &device) : m_device(device), m_program(), m_config() {
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
                          int(m_config->dir_lights->size()));
    for (uint32_t i = 0; i < m_config->dir_lights->size(); ++i) {
        auto &&item = ArrayMember("g_LightInfo.Dir", i);
        m_program.SetConstant(item + LIGHT_PARAM_DIRECTION,
                              -normalize((*m_config->dir_lights)[i]->GetPosition()));
        m_program.SetConstant(item + LIGHT_PARAM_COLOR,
                              (*m_config->dir_lights)[i]->GetColor());
    }

    m_program.SetConstant("g_LightInfo.NumPointLights",
                          int(m_config->point_lights->size()));
    for (uint32_t i = 0; i < m_config->point_lights->size(); ++i) {
        auto &&item = ArrayMember("g_LightInfo.Point", i);
        m_program.SetConstant(item + LIGHT_PARAM_POSITION,
                              (*m_config->point_lights)[i]->GetPosition());
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
    static phi::Buffer quad_vbo(phi::BufferType::Vertex,
                                phi::BufferHint::Static, quad, sizeof(quad));
    const float texel_size = 1.0f / m_config->texture_shadow->GetWidth();
    m_device.BindProgram(&m_program);
    m_device.BindTexture(0, m_config->texture_position);
    m_device.BindTexture(1, m_config->texture_normal);
    m_device.BindTexture(2, m_config->texture_diffuse);
    m_device.BindTexture(3, m_config->texture_shadow);

    m_device.BindSampler(0, &phi::Sampler::Nearest2D(phi::WrapMode::Clamp));
    m_device.BindSampler(1, &phi::Sampler::Nearest2D(phi::WrapMode::Clamp));
    m_device.BindSampler(2, &phi::Sampler::Nearest2D(phi::WrapMode::Clamp));
    m_device.BindSampler(3, &phi::Sampler::Nearest2D(phi::WrapMode::Clamp));

    m_program.SetConstant("g_TexPosition", (int) 0);
    m_program.SetConstant("g_TexNormal", (int) 1);
    m_program.SetConstant("g_TexDiffuse", (int) 2);
    m_program.SetConstant("g_TexShadow", (int) 3);
    m_program.SetConstant("g_ShadowMatrix", m_config->shadow_matrix);
    m_program.SetConstant("g_DepthTexelSize", texel_size);
    SetupLights();
    m_device.BindVbo(&quad_vbo);
    m_device.BindLayout(&quad_layout);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
}

} // namespace phi
