#include "DeferredRenderer.h"
#include "DataBinding.h"
#include "DrawCall.h"
#include "DrawCallQueue.h"

#include "scene/Camera.h"
#include "scene/Scene.h"

#include "device/Device.h"
#include "device/Shader.h"

namespace phi {

DeferredRenderer::DeferredRenderer(phi::Device &device, int w, int h)
        : m_device(device), m_shadow_casters(), m_shadow_pass(*this, 1024) {
    Resize(w, h);
}

void DeferredRenderer::BindGlobals(phi::Program &program,
                                   const glm::mat4 &proj,
                                   const glm::mat4 &view,
                                   const glm::mat4 &model) {
    if (!program.FindConstant("g_ProjViewModelMatrix")) {
        program.SetConstant("g_ProjViewModelMatrix", proj * view * model);
    }
    if (!program.FindConstant("g_ViewModelMatrix")) {
        program.SetConstant("g_ViewModelMatrix", view * model);
    }
    if (!program.FindConstant("g_NormalMatrix")) {
        program.SetConstant("g_NormalMatrix",
                            glm::mat3(glm::transpose(glm::inverse(model))));
    }
    if (!program.FindConstant("g_ModelMatrix")) {
        program.SetConstant("g_ModelMatrix", model);
    }
}

void DeferredRenderer::Render(phi::Scene &scene) {
    phi::DrawCallQueue Q{};
    scene.Render(&Q);
    const auto &draw_calls = Q.GetDrawCalls();
    for (const phi::DirLight *light : scene.GetDirLights()) {
        if (light->IsCastingShadows()) {
            m_shadow_casters.push_back(light);
        }
    }

    m_device.SetFrameBuffer(*m_gbuffer.get());
    m_device.ClearDepth();
    m_device.ClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    for (const phi::DrawCall &draw_call : draw_calls) {
        Execute(draw_call, *scene.GetCamera());
    }
    m_device.SetFrameBuffer(phi::DefaultFrameBuffer::Instance());
    m_shadow_pass.SetLight(*m_shadow_casters[0]);
    m_shadow_pass.SetDrawCalls(draw_calls);
    m_shadow_pass.SetObjectsAABB(scene.GetAABB());
    m_shadow_pass.Run();

    {
        phi::LightPass::Config config{};
        config.shadow_matrix = m_shadow_pass.GetShadowMatrix();
        config.shadow_map = &m_shadow_pass.GetShadowMap();
        config.depth_map = m_depth.get();
        config.diffuse_map = m_diffuse.get();
        config.specular_map = m_diffuse.get();
        config.point_lights = &scene.GetPointLights();
        config.dir_lights = &scene.GetDirLights();
        m_light_pass.Setup(config);
        m_light_pass.Run();
    }
    m_shadow_casters = {};
}

void DeferredRenderer::Execute(const phi::DrawCall &draw_call,
                               const phi::Camera &camera) {
    m_device.BindProgram(*draw_call.program);
    BindGlobals(*draw_call.program, camera.GetProjMatrix(),
                camera.GetViewMatrix(), draw_call.transform);
    for (const phi::ProgramConstant &constant : draw_call.program_constants) {
        draw_call.program->SetConstant(constant.name, constant.value);
    }
    uint32_t texture_unit = 0;
    for (const phi::TextureBinding &binding : draw_call.texture_bindings) {
        assert(binding.sampler);
        assert(binding.texture);
        m_device.BindTexture(texture_unit, *binding.texture);
        m_device.BindSampler(texture_unit, *binding.sampler);
        draw_call.program->SetConstant(binding.name, texture_unit++);
    }
    m_device.BindVbo(*draw_call.vbo);
    m_device.BindLayout(*draw_call.layout);
    m_device.BindIbo(*draw_call.ibo);
    m_device.Draw(draw_call.primitive, draw_call.offset, draw_call.count);
}

void DeferredRenderer::Resize(int w, int h) {
    m_device.SetViewport({ 0, 0, w, h });
    m_device.SetScissor({ 0, 0, w, h });

    m_gbuffer = std::make_unique<phi::FrameBuffer>(w, h);
    m_depth = std::make_unique<phi::Texture2D>(w, h,
                                               phi::TextureFormat::DEPTH_24);
    m_normal = std::make_unique<phi::Texture2D>(w, h,
                                                phi::TextureFormat::RGBA_16F);
    m_specular = std::make_unique<phi::Texture2D>(w, h,
                                                  phi::TextureFormat::RGBA_16F);
    m_diffuse = std::make_unique<phi::Texture2D>(w, h,
                                                 phi::TextureFormat::RGBA_16F);
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 0, m_normal.get() });
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 1, m_diffuse.get() });
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 2, m_specular.get() });
    m_gbuffer->SetDepthAttachment(phi::DepthAttachment{ m_depth.get() });
    assert(m_gbuffer->IsReady());
}

} // namespace phi
