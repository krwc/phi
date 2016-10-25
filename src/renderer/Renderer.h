#ifndef PHI_RENDERER_RENDERER_H
#define PHI_RENDERER_RENDERER_H

#include "math/Math.h"

namespace phi {
class Scene;
class Box;
class Camera;
class Layout;
class Program;
class Buffer;
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
     * rendering (without packing data into the scene). The idea is just to execute
     * draw call encapsulated in @p call.
     *
     * @param camera    Camera whose view will be used for rendering.
     * @param call      Draw call to execute.
     */
    virtual void Render(const phi::Camera &camera, const phi::DrawCall &call) = 0;

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
    virtual void SetViewport(int x, int y, int w, int h) = 0;
};

} // namespace phi

#endif
