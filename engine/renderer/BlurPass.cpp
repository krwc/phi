#include "BlurPass.h"
#include "Common.h"

#include "device/Layout.h"
#include "device/Buffer.h"
#include "device/Sampler.h"

#include "io/File.h"

namespace phi {

BlurPass::BlurPass(phi::Device &device, const phi::BlurPass::Config &config)
        : m_device(device),
          m_texture(config.texture),
          m_width(m_texture->GetWidth()),
          m_height(m_texture->GetHeight()),
          m_fbo1(m_width, m_height),
          m_fbo2(m_width, m_height),
          m_tmp(m_width, m_height, m_texture->GetFormat()),
          m_blur_x(),
          m_blur_y(),
          m_program() {
    m_fbo1.SetColorAttachment(phi::ColorAttachment{ 0, &m_tmp });
    m_fbo2.SetColorAttachment(phi::ColorAttachment{ 0, m_texture });
    assert(m_fbo1.IsReady());
    assert(m_fbo2.IsReady());
    m_inv_size = glm::vec2(1.0f / m_width, 1.0f / m_height);
    SetRadius(config.radius);

    m_program.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Quad.vs").c_str());
    m_program.SetSource(
            phi::ShaderType::Fragment,
            phi::io::FileContents("assets/shaders/BlurGauss9x9.fs").c_str());
    m_program.Link();
}

void BlurPass::SetRadius(float radius) {
    m_blur_x = glm::vec2(radius, 0) * m_inv_size.x;
    m_blur_y = glm::vec2(0, radius) * m_inv_size.y;
}

void BlurPass::Run() {
    m_device.BindProgram(&m_program);
    m_device.BindVbo(&Common::QuadVbo());
    m_device.BindLayout(&Common::QuadLayout());
    m_device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Bilinear2D());

    m_program.SetConstant("InvSize", m_inv_size);
    m_device.BindFrameBuffer(&m_fbo1);
    m_device.BindTexture(0, m_texture);
    m_program.SetConstant("Step", m_blur_x);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);

    m_device.BindFrameBuffer(&m_fbo2);
    m_device.BindTexture(0, &m_tmp);
    m_program.SetConstant("Step", m_blur_y);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
}

} // namespace phi
