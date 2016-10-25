#ifndef PHI_RENDERER_DEBUG_H
#define PHI_RENDERER_DEBUG_H
#include "math/Math.h"

#include "device/Buffer.h"
#include "device/Layout.h"
#include "device/Shader.h"
#include "device/Texture.h"

namespace phi {
class Renderer;
class Camera;
class Box;

class DebugDrawer {
    // 4K should be enough to draw single primitive
    static const size_t BUFFER_SIZE = 4096;

    const phi::Camera &m_view;
    phi::Renderer &m_renderer;
    phi::Buffer m_vbo;
    phi::Program m_debug_program;
    phi::Program m_quad_program;
    glm::mat4 m_dummy;

public:
    DebugDrawer(const phi::Camera &view, phi::Renderer &renderer);
    void DrawBox(const phi::Box &box, const glm::vec3 &color = { 1, 1, 1 });
    void DrawTexture(const phi::Texture2D *texture, int x, int y, int w, int h);
};

} // namespace phi
#endif
