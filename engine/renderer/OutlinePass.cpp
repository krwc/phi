#include "OutlinePass.h"
#include "Common.h"

#include "io/File.h"

namespace phi {
namespace {
const char *prepass_contour_shader = R"(
    #line 1 // "OutlineContourShader"
    layout(location=0) out vec4 FragColor;

    void main() {
        FragColor = vec4(1);
    }
)";
}

OutlinePass::OutlinePass(phi::DeferredRenderer &renderer)
        : m_renderer(renderer), m_entity(nullptr) {
    m_prepass.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Passthrough.vs").c_str());
    m_prepass.SetSource(phi::ShaderType::Fragment, prepass_contour_shader);
    m_prepass.Link();

    m_outline.SetSource(
            phi::ShaderType::Vertex,
            phi::io::FileContents("assets/shaders/Passthrough.vs").c_str());
    m_outline.SetSource(
            phi::ShaderType::Fragment,
            phi::io::FileContents("assets/shaders/Outline.fs").c_str());
    m_outline.Link();



    const int w = renderer.GetDevice().GetViewport().w;
    const int h = renderer.GetDevice().GetViewport().h;

    m_fbo = std::make_unique<phi::FrameBuffer>(w, h);
    m_mask = std::make_unique<phi::Texture2D>(w, h, phi::TextureFormat::R_8);
    m_fbo->SetColorAttachment(phi::ColorAttachment{ 0, m_mask.get() });
    assert(m_fbo->IsReady());
}

void OutlinePass::SetEntity(const phi::Entity *entity) {
    m_entity = entity;
}

void OutlinePass::SetCamera(const phi::Camera *camera) {
    m_camera = camera;
}

void OutlinePass::Run() {
    if (!m_entity) {
        return;
    }
    assert(m_camera);
    phi::Device &device = m_renderer.GetDevice();
    glm::mat4 proj_view_model = m_camera->GetProjMatrix()
                                * m_camera->GetViewMatrix()
                                * m_entity->GetTransform();
    phi::DrawCall draw_call{};
#warning "FIXME: This is hack"
    const_cast<phi::Entity *>(m_entity)->Render(draw_call);

    device.BindFrameBuffer(m_fbo.get());
    device.ClearColor(0, 0, 0, 0);

    device.BindProgram(&m_prepass);
    device.BindLayout(draw_call.layout);
    device.BindVbo(draw_call.vbo);
    device.BindIbo(draw_call.ibo);

    device.BindTexture(0, m_mask.get());
    device.BindTexture(1, m_renderer.GetDepth());
    device.BindSampler(0, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());
    device.BindSampler(1, &phi::Samplers<phi::WrapMode::ClampToEdge>::Nearest2D());

    m_prepass.SetConstant("ProjViewModel", proj_view_model);

    /* Render object color mask first. */
    device.Draw(draw_call.primitive, draw_call.offset, draw_call.count);

    /* Now detect edges and render the outline. */
    device.BindFrameBuffer(nullptr);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    device.BindProgram(&m_outline);
    m_outline.SetConstant("ProjViewModel", proj_view_model);
    m_outline.SetConstant("ObjectMask", static_cast<int>(0));
    m_outline.SetConstant("SceneDepth", static_cast<int>(1));
    device.Draw(draw_call.primitive, draw_call.offset, draw_call.count);
    glDisable(GL_BLEND);
}
}
