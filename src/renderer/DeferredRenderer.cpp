#include "DeferredRenderer.h"
#include "DataBinding.h"
#include "DrawCall.h"
#include "DrawCallQueue.h"

#include "scene/Camera.h"
#include "scene/Scene.h"

#include "device/Device.h"
#include "device/Shader.h"

#include "utils/Timer.h"

namespace phi {

DeferredRenderer::DeferredRenderer(phi::Device &device, int w, int h)
        : m_device(device),
          m_shadow_casters(),
          m_light_pass(device),
          m_shadow_pass(device, 2048) {
    Resize(w, h);
}

void DeferredRenderer::BindGlobals(phi::Program &program,
                                   const glm::mat4 &proj,
                                   const glm::mat4 &view,
                                   const glm::mat4 &model) {
    if (program.FindConstant("g_ProjViewModelMatrix")) {
        program.SetConstant("g_ProjViewModelMatrix", proj * view * model);
    }
    if (program.FindConstant("g_ViewModelMatrix")) {
        program.SetConstant("g_ViewModelMatrix", view * model);
    }
    if (program.FindConstant("g_NormalMatrix")) {
        program.SetConstant("g_NormalMatrix",
                            glm::mat3(glm::transpose(glm::inverse(model))));
    }
    if (program.FindConstant("g_ModelMatrix")) {
        program.SetConstant("g_ModelMatrix", model);
    }
}

#define PERF_STATS

void DeferredRenderer::Render(phi::Scene &scene) {
#ifdef PERF_STATS
    static phi::Timer T;
    static unsigned counter;
    T.Begin();
#endif

    phi::DrawCallQueue Q{};
    scene.Render(&Q);
    const auto &draw_calls = Q.GetDrawCalls();
    const auto camera = scene.GetCamera();
    for (const phi::DirLight *light : scene.GetDirLights()) {
        if (light->IsCastingShadows()) {
            m_shadow_casters.push_back(light);
        }
    }

    m_device.BindFrameBuffer(m_gbuffer.get());
    m_device.ClearDepth();
    m_device.ClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    for (const phi::DrawCall &draw_call : draw_calls) {
        Execute(draw_call, *camera);
    }

    {
        phi::ShadowPass<phi::DirLight>::Config config{};
        config.aabb = &scene.GetAABB();
        config.light = m_shadow_casters[0];
        config.draw_calls = &draw_calls;
        m_shadow_pass.Setup(config);
        m_shadow_pass.Run();
    }
    m_device.BindFrameBuffer(nullptr);

    {
        phi::LightPass::Config config{};
        config.shadow_matrix = m_shadow_pass.GetShadowMatrix();
        config.texture_shadow = &m_shadow_pass.GetShadowMap();
        config.texture_normal = m_normal.get();
        config.texture_diffuse = m_diffuse.get();
        config.texture_position = m_position.get();
        config.point_lights = &scene.GetPointLights();
        config.dir_lights = &scene.GetDirLights();
        config.camera = camera;
        m_light_pass.Setup(config);
        m_device.SetDepthWrite(false);
        m_device.SetDepthTest(false);
        m_light_pass.Run();
        m_device.SetDepthTest(true);
        m_device.SetDepthWrite(true);
    }
    m_shadow_casters = {};

#ifdef PERF_STATS
    glFinish();
    T.End();
    ++counter;
    if (counter % 300 == 0) {
        auto dt = T.ElapsedTimeUs<float>() / 1000;
        auto avg = T.AvgUs<float>() / 1000;
        auto max = T.MaxUs<float>() / 1000;
        printf("dt=%.4fms, avg=%.4fms, max=%.4fms\n", dt, avg, max);
    }
#endif
}

void DeferredRenderer::Execute(const phi::DrawCall &draw_call,
                               const phi::Camera &camera) {
    m_device.BindProgram(draw_call.program);
    BindGlobals(*draw_call.program, camera.GetProjMatrix(),
                camera.GetViewMatrix(), draw_call.transform);
    for (const phi::ProgramConstant &constant : draw_call.program_constants) {
        draw_call.program->SetConstant(constant.name, constant.value);
    }
    uint32_t texture_unit = 0;
    for (const phi::TextureBinding &binding : draw_call.texture_bindings) {
        assert(binding.sampler);
        assert(binding.texture);
        m_device.BindTexture(texture_unit, binding.texture);
        m_device.BindSampler(texture_unit, binding.sampler);
        draw_call.program->SetConstant(binding.name, texture_unit++);
    }
    m_device.BindVbo(draw_call.vbo);
    m_device.BindLayout(draw_call.layout);
    m_device.BindIbo(draw_call.ibo);
    m_device.Draw(draw_call.primitive, draw_call.offset, draw_call.count);
}

void DeferredRenderer::Resize(int w, int h) {
    m_device.SetViewport({ 0, 0, w, h });
    m_device.SetScissor({ 0, 0, w, h });

    m_gbuffer = std::make_unique<phi::FrameBuffer>(w, h);
    m_depth = std::make_unique<phi::Texture2D>(w, h,
                                               phi::TextureFormat::DEPTH_24);
    m_position = std::make_unique<phi::Texture2D>(w, h,
                                                  phi::TextureFormat::RGBA_32F);
    m_normal = std::make_unique<phi::Texture2D>(w, h,
                                                phi::TextureFormat::RGBA_16F);
    m_specular = std::make_unique<phi::Texture2D>(w, h,
                                                  phi::TextureFormat::RGBA_16F);
    m_diffuse = std::make_unique<phi::Texture2D>(w, h,
                                                 phi::TextureFormat::RGBA_16F);
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 0, m_normal.get() });
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 1, m_position.get() });
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 2, m_diffuse.get() });
    m_gbuffer->SetColorAttachment(phi::ColorAttachment{ 3, m_specular.get() });
    m_gbuffer->SetDepthAttachment(phi::DepthAttachment{ m_depth.get() });
    assert(m_gbuffer->IsReady());
}

} // namespace phi
