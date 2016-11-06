#ifndef PHI_RENDERER_FORWARD_RENDERER_H
#define PHI_RENDERER_FORWARD_RENDERER_H
#include <memory>

#include "DrawCallQueue.h"
#include "Renderer.h"

#include "math/AABB.h"
#include "math/Rect2D.h"
#include "math/Math.h"

#include "device/Device.h"

namespace phi {
class Layout;
class Scene;
class DirLight;
class PointLight;

class ForwardRenderer : public Renderer {
    friend class DebugDrawer;

    phi::Device &m_device;
    glm::mat4 m_shadow_matrix;

    std::vector<const phi::DirLight *> m_shadow_casters;

    void BindGlobals(phi::Program *,
                     const glm::mat4 &proj,
                     const glm::mat4 &view,
                     const glm::mat4 &model);
    void BindLights(phi::Program *,
                    const std::vector<phi::DirLight *> &,
                    const std::vector<phi::PointLight *> &);
    void Execute(const glm::mat4 &proj,
                 const glm::mat4 &view,
                 const phi::DrawCall &,
                 const std::vector<phi::DirLight *> &,
                 const std::vector<phi::PointLight *> &);

public:
    ForwardRenderer(phi::Device &);
    virtual void Render(phi::Scene &);
    virtual void Execute(const phi::DrawCall &, const phi::Camera &);
    virtual void Resize(int w, int h) {}
    virtual phi::Device &GetDevice();
};

} // namespace phi

#endif
