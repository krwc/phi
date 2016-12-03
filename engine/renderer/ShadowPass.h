#ifndef PHI_RENDERER_SHADOW_PASS_H
#define PHI_RENDERER_SHADOW_PASS_H
#include <memory>
#include <vector>

#include "device/Device.h"
#include "device/FrameBuffer.h"
#include "device/Program.h"
#include "device/Sampler.h"
#include "device/Texture.h"

#include "scene/Light.h"
#include "scene/OrthoCamera.h"

#include "utils/Types.h"

#include "math/AABB.h"

#include "RenderPass.h"
#include "DrawCall.h"

namespace phi {
class Renderer;

template <typename LightType>
class ShadowPassBase : public RenderPass {
public:
    virtual ~ShadowPassBase() {}
    /** Returns texture reference representing shadow map */
    virtual const phi::Texture &GetShadowMap() const = 0;
    /** Returns light camera */
    virtual const phi::Camera &GetLightCamera() const = 0;
};

template <typename LightType>
class ShadowPass : public ShadowPassBase<LightType> {
    ShadowPass() = delete;
};

template <>
class ShadowPass<phi::DirLight>
        : public phi::ShadowPassBase<phi::DirLight> {
public:
    struct Config {
        const phi::AABB *aabb;
        const phi::DirLight *light;
        phi::AnyRange<phi::DrawCall> draw_calls;
    };

    ShadowPass(phi::Device &device, uint32_t resolution);
    void Setup(const phi::ShadowPass<phi::DirLight>::Config &config);
    void Run();

    virtual const phi::Texture2D &GetShadowMap() const {
        return m_depth;
    }

    glm::mat4 GetShadowMatrix() const;

    virtual const phi::Camera &GetLightCamera() const;

private:
    phi::Device &m_device;
    phi::OrthoCamera m_light_camera;

    uint32_t m_resolution;
    phi::FrameBuffer m_fbo;
    phi::Program m_depth_program;
    phi::Texture2D m_depth;
    const ShadowPass<phi::DirLight>::Config *m_config;

    void RecomputeLightFrustum();
};

} // namespace phi

#endif
