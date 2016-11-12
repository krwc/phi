#include "BlurPass.h"

#include "device/Layout.h"
#include "device/Buffer.h"
#include "device/Sampler.h"

#include "io/File.h"

namespace phi {
namespace {
const phi::Layout quad_layout{{ "in_Position", 0u, sizeof(glm::vec2), phi::Type::Float }};

const glm::vec2 quad[] = { { -1, -1 }, { 1, -1 }, { -1, 1 },
                           { -1, 1 },  { 1, -1 }, { 1, 1 } };
}

BlurPass::BlurPass(phi::BlurPass::Kernel kernel,
                   phi::Device &device,
                   phi::Texture2D &texture)
        : m_device(device),
          m_texture(texture),
          m_width(texture.GetWidth()),
          m_height(texture.GetHeight()),
          m_radius(1.0f),
          m_fbo1(m_width, m_height),
          m_fbo2(m_width, m_height),
          m_tmp(m_width, m_height, texture.GetFormat()) {
    m_fbo1.SetColorAttachment(phi::ColorAttachment{ 0, &m_tmp });
    m_fbo2.SetColorAttachment(phi::ColorAttachment{ 0, &texture });
    assert(m_fbo1.IsReady());
    assert(m_fbo2.IsReady());
    assert(kernel == phi::BlurPass::Kernel::Gauss9x9);
    m_program.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Quad.vs").c_str());
    m_program.SetSource(
            phi::ShaderType::Fragment,
            phi::io::FileContents("assets/shaders/BlurGauss9x9.fs").c_str());
    m_program.Link();
}

void BlurPass::SetRadius(float radius) {
    m_radius = radius;
}

void BlurPass::Run() {
    static phi::Buffer quad_vbo(phi::BufferType::Vertex,
                                phi::BufferHint::Static, quad, sizeof(quad));
    const glm::vec2 blur_x = glm::vec2(m_radius, 0) * (1.0f / m_width);
    const glm::vec2 blur_y = glm::vec2(0, m_radius) * (1.0f / m_height);
    m_device.BindProgram(m_program);
    m_device.BindVbo(quad_vbo);
    m_device.BindLayout(quad_layout);
    m_device.BindSampler(0, phi::Sampler::Bilinear2D(phi::WrapMode::Clamp));

    m_program.SetConstant("InvSize",
                          glm::vec2(1.0f / m_width, 1.0f / m_height));
    m_device.SetFrameBuffer(m_fbo1);
    m_device.BindTexture(0, m_texture);
    m_program.SetConstant("Step", blur_x);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);

    m_device.SetFrameBuffer(m_fbo2);
    m_device.BindTexture(0, m_tmp);
    m_program.SetConstant("Step", blur_y);
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
}

} // namespace phi
