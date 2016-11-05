#include "DeferredRenderer.h"

#include "device/Device.h"

namespace phi {

DeferredRenderer::DeferredRenderer(phi::Device &device)
        : m_device(device), m_shadow_casters() {}

void DeferredRenderer::Render(phi::Scene &scene) {}

void DeferredRenderer::Execute(const phi::DrawCall &drawcall,
                               const phi::Camera &camera) {}

} // namespace phi
