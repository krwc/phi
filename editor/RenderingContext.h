#ifndef PHI_EDITOR_RENDERING_CONTEXT_H
#define PHI_EDITOR_RENDERING_CONTEXT_H
#include "engine/device/Device.h"

#include "engine/scene/FlatScene.h"

#include "engine/renderer/DeferredRenderer.h"

#include <QOpenGLContext>
#include <memory>

namespace phi {
namespace editor {

class RenderingContext : public QObject {
    Q_OBJECT

    QOpenGLContext *m_context;
    QSurface *m_surface;
    std::unique_ptr<phi::Device> m_device;
    std::unique_ptr<phi::DeferredRenderer> m_renderer;
    std::unique_ptr<phi::Scene> m_scene;

public:
    explicit RenderingContext(QOpenGLContext *context,
                              QSurface *surface,
                              int width,
                              int height,
                              QObject *parent = 0);

    void RenderFrame();

    phi::Device &GetDevice() {
        return *m_device.get();
    }
    phi::Scene &GetScene() {
        return *m_scene.get();
    }
    phi::DeferredRenderer &GetDeferredRenderer() {
        return *m_renderer.get();
    }
};

} // namespace editor
} // namespace phi

#endif
