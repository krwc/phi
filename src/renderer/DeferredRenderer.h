#ifndef PHI_RENDERER_DEFERRED_RENDERER_H
#define PHI_RENDERER_DEFERRED_RENDERER_H
#include <vector>

#include "Renderer.h"

namespace phi {
class Scene;
class Camera;
class DirLight;

class DeferredRenderer : public Renderer {
    phi::Device &m_device;
    std::vector<const phi::DirLight *> m_shadow_casters;

public:
    DeferredRenderer(phi::Device &device);

    virtual void Render(phi::Scene &scene);
    virtual void Execute(const phi::DrawCall &call, const phi::Camera &camera);

    virtual phi::Device &GetDevice() {
        return m_device;
    }
};

} // namespace phi

#endif
