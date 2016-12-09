#include "SsaoPass.h"
#include "Common.h"

#include "device/Sampler.h"

#include <vector>
#include <random>

namespace phi {

static const int NOISE_SIZE = 2;

void SsaoPass::InitNoiseTexture() {
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    std::vector<glm::vec3> noise(NOISE_SIZE*NOISE_SIZE);
    for (size_t i = 0; i < noise.size(); ++i) {
        noise[i].x = 2.0f * d(generator) - 1.0f;
        noise[i].y = 2.0f * d(generator) - 1.0f;
        noise[i].z = 0.0f;
        noise[i] = glm::normalize(noise[i]);
    }
    m_noise = std::make_unique<phi::Texture2D>(NOISE_SIZE, NOISE_SIZE,
                                               phi::TextureFormat::RGB_16F);
    m_noise->Write(0, 0, 0, NOISE_SIZE, NOISE_SIZE, noise.data());
}

void SsaoPass::InitProgram() {
    m_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/Quad.vs");
    m_program.SetSourceFromFile(phi::ShaderType::Fragment, "assets/shaders/SsaoPass.fs");
    m_program.Link();
}

void SsaoPass::InitFbo() {
    m_fbo = std::make_unique<phi::FrameBuffer>(m_config.fbo_width,
                                               m_config.fbo_height);
    m_ao_texture = std::make_unique<phi::Texture2D>(
            m_config.fbo_width, m_config.fbo_height, phi::TextureFormat::R_16F);
    m_fbo->SetColorAttachment(phi::ColorAttachment{ 0, m_ao_texture.get() });
    assert(m_fbo->IsReady());
}

void SsaoPass::UpdateConstants() {
    m_program.SetConstant("g_Radius", m_properties.radius);
    m_program.SetConstant("g_Power", m_properties.power);
    m_program.SetConstant("g_Bias", m_properties.bias);
    m_program.SetConstant("g_Strength", m_properties.strength);
    m_program.SetConstant("g_ScreenSize", m_screen_size);
    m_program.SetConstant("g_InvNoiseSize", 1.0f / NOISE_SIZE);
    m_dirty = false;
}

SsaoPass::SsaoPass(phi::Device &device)
        : m_device(device), m_program(), m_dirty(true) {
    InitNoiseTexture();
    InitProgram();

    /* Default values, known to be working for general scene */
    SetRadius(0.316f);
    SetPower(2.7792f);
    SetBias(0.052f);
    SetStrength(1.2);
}

void SsaoPass::Setup(const phi::SsaoPass::Config &config) {
    m_config = config;
    m_screen_size =
            glm::vec2((float) config.fbo_width, (float) config.fbo_height);
    InitFbo();

    phi::BlurPass::Config blur_config{};
    blur_config.kernel = phi::BlurPass::Kernel::Gauss9x9;
    blur_config.texture = m_ao_texture.get();
    blur_config.radius = 1.0f;
    m_blur_pass = std::make_unique<phi::BlurPass>(m_device, blur_config);
}

void SsaoPass::SetCamera(const phi::Camera &camera) {
    m_config.camera = &camera;
}

void SsaoPass::Run() {
    if (m_dirty) {
        UpdateConstants();
    }
    const bool depth_write = m_device.GetDepthWrite();
    const bool depth_test = m_device.GetDepthTest();
    const phi::Rect2D viewport = m_device.GetViewport();
    m_device.BindFrameBuffer(m_fbo.get());
    m_device.SetViewport({ 0, 0, m_config.fbo_width, m_config.fbo_height });
    m_device.BindProgram(&m_program);

    m_program.SetConstant("g_ProjMatrix", m_config.camera->GetProjMatrix());
    m_program.SetConstant("g_InvProjMatrix", glm::inverse(m_config.camera->GetProjMatrix()));

    m_device.BindLayout(&Common::QuadLayout());
    m_device.BindVbo(&Common::QuadVbo());
    m_device.BindTexture(0, m_config.normal);
    m_device.BindTexture(1, m_noise.get());
    m_device.BindTexture(2, m_config.depth);

    m_device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(1, &phi::Samplers<phi::WrapMode::Repeat>::Nearest2D());
    m_device.BindSampler(2, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.SetDepthWrite(false);
    m_device.SetDepthTest(false);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
    m_device.SetDepthWrite(depth_write);
    m_device.SetDepthTest(depth_test);
    m_device.SetViewport(viewport);
}

void SsaoPass::SetRadius(float radius) {
    m_properties.radius = radius;
    m_dirty = true;
}

void SsaoPass::SetPower(float power) {
    m_properties.power = power;
    m_dirty = true;
}

void SsaoPass::SetBias(float bias) {
    m_properties.bias = bias;
    m_dirty = true;
}

void SsaoPass::SetStrength(float strength) {
    m_properties.strength = strength;
    m_dirty = true;
}

} // namespace phi
