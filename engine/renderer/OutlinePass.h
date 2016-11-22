#ifndef PHI_RENDERER_OUTLINE_PASS_H
#define PHI_RENDERER_OUTLINE_PASS_H
#include "device/Device.h"
#include "device/Program.h"

#include "scene/Camera.h"
#include "scene/Entity.h"

#include "DeferredRenderer.h"
#include "RenderPass.h"

namespace phi {

/**
 * Forward pass that renders an outline of the picked entity. Should be
 * used after deferred renderer finished its rendering.
 */
class OutlinePass : public RenderPass {
    phi::DeferredRenderer &m_renderer;
    std::unique_ptr<phi::FrameBuffer> m_fbo;
    std::unique_ptr<phi::Texture2D> m_mask;
    const phi::Entity *m_entity;
    const phi::Camera *m_camera;
    phi::Program m_prepass;
    phi::Program m_outline;

public:
    OutlinePass(phi::DeferredRenderer &renderer);

    /** Sets an entity for which outline shall be drawn or nullptr if none. */
    void SetEntity(const phi::Entity *entity);

    /** Sets camera */
    void SetCamera(const phi::Camera *camera);

    /** Draws an outline of the picked entity */
    void Run();
};

} // namespace phi

#endif
