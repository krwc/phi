#include "SsaoPass.h"
#include "Common.h"

#include "device/Sampler.h"

#include "io/File.h"

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
    m_program.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Quad.vs").c_str());
    m_program.SetSource(
            phi::ShaderType::Fragment,
            phi::io::FileContents("assets/shaders/SsaoPass.fs").c_str());
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

SsaoPass::SsaoPass(phi::Device &device)
        : m_device(device), m_program() {
    InitNoiseTexture();
    InitProgram();
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
    const bool depth_write = m_device.GetDepthWrite();
    const bool depth_test = m_device.GetDepthTest();
    const phi::Rect2D viewport = m_device.GetViewport();
    m_device.BindFrameBuffer(m_fbo.get());
    m_device.SetViewport({ 0, 0, m_config.fbo_width, m_config.fbo_height });
    m_device.BindProgram(&m_program);
    m_program.SetConstant("g_ScreenSize", m_screen_size);
    m_program.SetConstant("g_InvNoiseSize", 1.0f / NOISE_SIZE);
    m_program.SetConstant("g_ProjMatrix", m_config.camera->GetProjMatrix());
    m_program.SetConstant("g_InvProjMatrix", glm::inverse(m_config.camera->GetProjMatrix()));
    m_program.SetConstant("g_TexNormal", static_cast<int>(0));
    m_program.SetConstant("g_TexNoise", static_cast<int>(1));
    m_program.SetConstant("g_TexDepth", static_cast<int>(2));
    //m_program.SetConstant("g_TexPosition", static_cast<int>(3));

    m_device.BindLayout(&Common::QuadLayout());
    m_device.BindVbo(&Common::QuadVbo());
    m_device.BindTexture(0, m_config.normal);
    m_device.BindTexture(1, m_noise.get());
    m_device.BindTexture(2, m_config.depth);
    //m_device.BindTexture(3, m_config.position);

    m_device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.BindSampler(1, &phi::Samplers<phi::WrapMode::Repeat>::Nearest2D());
    m_device.BindSampler(2, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    //m_device.BindSampler(3, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    m_device.SetDepthWrite(false);
    m_device.SetDepthTest(false);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
//    m_blur_pass->Run();
//    m_blur_pass->Run();
    m_device.SetDepthWrite(depth_write);
    m_device.SetDepthTest(depth_test);
    m_device.SetViewport(viewport);
}

} // namespace phi
