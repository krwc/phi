#ifndef PHI_RENDERER_DEFERRED_RENDERER_H
#define PHI_RENDERER_DEFERRED_RENDERER_H
#include <vector>
#include <memory>

#include "LightPass.h"
#include "Renderer.h"
#include "ShadowPass.h"
#include "SsaoPass.h"

#include "math/Math.h"

#include "device/FrameBuffer.h"
#include "device/Texture.h"

namespace phi {
class Scene;
class Camera;
class Program;
class DirLight;

class DeferredRenderer final : public Renderer {
    phi::Device &m_device;
    std::unique_ptr<phi::FrameBuffer> m_gbuffer;
    std::unique_ptr<phi::Texture2D> m_depth;
    std::unique_ptr<phi::Texture2D> m_position;
    std::unique_ptr<phi::Texture2D> m_normal;
    std::unique_ptr<phi::Texture2D> m_diffuse;
    std::unique_ptr<phi::Texture2D> m_specular;
    std::vector<const phi::DirLight *> m_shadow_casters;
    std::unique_ptr<phi::SsaoPass> m_ssao_pass;
    phi::LightPass m_light_pass;
    phi::ShadowPass<phi::DirLight> m_shadow_pass;

    void BindGlobals(phi::Program &program,
                     const glm::mat4 &proj,
                     const glm::mat4 &view,
                     const glm::mat4 &model);

public:
    DeferredRenderer(phi::Device &device);

    virtual void Render(phi::Scene &scene);
    virtual void Execute(const phi::DrawCall &call, const phi::Camera &camera);
    virtual void Resize(int w, int h);

    virtual phi::Device &GetDevice() {
        return m_device;
    }

};

} // namespace phi

#endif
