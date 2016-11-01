#ifndef PHI_RENDERER_RENDERER_H
#define PHI_RENDERER_RENDERER_H

#include "math/Math.h"

namespace phi {
class Scene;
class Camera;
class FrameBuffer;
struct Rect2D;
struct DrawCall;

class Renderer {
public:
    virtual ~Renderer() {}
    /**
     * Queries scene for renderable objects and renders them on the screen
     *
     * @param scene     Scene to render.
     */
    virtual void Render(phi::Scene &scene) = 0;

    /**
     * This is a lower level method that might be used to perform "manual"
     * rendering (without packing data into the scene). The idea is just to
     * execute draw call encapsulated in @p call.
     *
     * @param call      Draw call to execute.
     * @param camera    Camera that will be queried for view & proj matrices.
     */
    virtual void Execute(const phi::DrawCall &call,
                         const phi::Camera &camera) = 0;

    /**
     * This method should be called whenever view changes, to be able to properly
     * adjust to a new resolution.
     *
     * @param width     New screen width.
     * @param height    New screen height.
     */
    virtual void Resize(int width, int height) = 0;

    /**
     * Sets current viewport. It works equivalently to @ref glViewport.
     */
    virtual void SetViewport(const phi::Rect2D &viewport) = 0;

    /**
     * Sets current scissor box. It works equivalently to @ref glScissor
     */
    virtual void SetScissor(const phi::Rect2D &scissor) = 0;

    /**
     * Sets current render target to a FrameBuffer. To return to a default
     * framebuffer one one can set @p target to DefaultFrameBuffer::Instance().
     */
    virtual void SetFrameBuffer(phi::FrameBuffer &target) = 0;

    /**
     * @returns Current viewport
     */
    virtual const phi::Rect2D &GetViewport() const = 0;

    /**
     * @returns Current scissor area.
     */
    virtual const phi::Rect2D &GetScissor() const = 0;

    /**
     * Clears current depth buffer.
     */
    virtual void ClearDepth() = 0;

    /**
     * Enables / disables writing to the depth buffer.
     */
    virtual void SetZWrite(bool enabled) = 0;

    /**
     * Gets current state of depth writing.
     */
    virtual bool GetZWrite() const = 0;
};

} // namespace phi

#endif
