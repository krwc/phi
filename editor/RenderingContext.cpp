#include "RenderingContext.h"

namespace phi {
namespace editor {

struct QtProcLoader : phi::ProcLoader {
    QOpenGLContext *context;

    QtProcLoader(QOpenGLContext *context)
        : context(context) {}

    void *operator()(const char *name) const {
        return (void *) context->getProcAddress(name);
    }
};

RenderingContext::RenderingContext(QOpenGLContext *context,
                                   QSurface *surface,
                                   int width,
                                   int height,
                                   QObject *parent)
        : QObject(parent),
          m_context(context),
          m_surface(surface),
          m_scene(std::make_unique<phi::FlatScene>()) {
    m_context->makeCurrent(m_surface);
    m_device = std::make_unique<phi::Device>(QtProcLoader(m_context), width, height);
    m_renderer = std::make_unique<phi::DeferredRenderer>(*m_device.get());
}

void RenderingContext::RenderFrame() {
    m_device->SetDefaultFrameBuffer(m_context->defaultFramebufferObject());
    m_renderer->Render(*m_scene.get());
}

} // namespace editor
} // namespace phi
