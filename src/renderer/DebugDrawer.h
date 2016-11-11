#ifndef PHI_RENDERER_DEBUG_H
#define PHI_RENDERER_DEBUG_H
#include "math/Math.h"

#include "device/Buffer.h"
#include "device/Device.h"
#include "device/Layout.h"
#include "device/Program.h"
#include "device/Texture.h"

namespace phi {
class Camera;
class AABB;

class DebugDrawer {
    // 4K should be enough to draw single primitive
    static const size_t BUFFER_SIZE = 4096;

    phi::Device &m_device;
    phi::Buffer m_vbo;
    phi::Program m_debug_program;
    phi::Program m_quad_program;
    glm::mat4 m_dummy;

public:
    DebugDrawer(phi::Device &device);
    void DrawAABB(const phi::Camera &m_view,
                  const phi::AABB &box,
                  const glm::vec3 &color = { 1, 1, 1 });
    void DrawTexture(const phi::Texture2D &texture, int x, int y, int w, int h);
};

} // namespace phi
#endif
