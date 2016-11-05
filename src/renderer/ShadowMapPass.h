#ifndef PHI_RENDERER_SHADOW_MAP_PASS_H
#define PHI_RENDERER_SHADOW_MAP_PASS_H
#include <memory>
#include <vector>

#include "device/FrameBuffer.h"
#include "device/Shader.h"
#include "device/Texture.h"

#include "scene/Light.h"
#include "scene/OrthoCamera.h"

#include "math/AABB.h"

#include "DrawCall.h"

namespace phi {
class Renderer;

template <typename LightType>
class ShadowMapPassBase {
public:
    virtual ~ShadowMapPassBase() {}
    /** Draws to a shadow map. */
    virtual void Draw() = 0;
    /** Returns texture reference representing shadow map */
    virtual const phi::Texture &GetShadowMap() const = 0;
    /** Returns light camera */
    virtual const phi::Camera &GetLightCamera() const = 0;
};

template <typename LightType>
class ShadowMapPass : public ShadowMapPassBase<LightType> {
    ShadowMapPass() = delete;
};

template <>
class ShadowMapPass<phi::DirLight>
        : public phi::ShadowMapPassBase<phi::DirLight> {
    uint32_t m_resolution;
    std::vector<phi::DrawCall> m_drawcalls;
    mutable bool m_needs_update;
    mutable phi::OrthoCamera m_light_camera;
    phi::DirLight m_light;
    phi::AABB m_aabb;
    phi::Renderer &m_renderer;

    phi::FrameBuffer m_fbo;
    phi::Program m_depth_program;
    phi::Texture2D m_depth;

    void RecomputeLightFrustum() const;

public:
    ShadowMapPass(phi::Renderer &renderer, uint32_t resolution);

    /**
     * @param light Directional light for which shadow map shall be computed.
     */
    void SetLight(const phi::DirLight &light);

    /**
     * @param drawcalls Drawcalls of objects that should be rendered to a shadow
     *                  map.
     */
    void SetDrawCalls(const std::vector<phi::DrawCall> &drawcalls);

    /**
     * @param aabb  AABB of objects being drawn. Used to compute light frustum.
     */
    void SetObjectsAABB(const phi::AABB &aabb);

    virtual void Draw();

    virtual const phi::Texture2D &GetShadowMap() const {
        return m_depth;
    }

    glm::mat4 GetShadowMatrix() const;

    virtual const phi::Camera &GetLightCamera() const;
};

} // namespace phi

#endif
