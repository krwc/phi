#ifndef PHI_EDITOR_PHI_WIDGET_H
#define PHI_EDITOR_PHI_WIDGET_H
#include "engine/device/Device.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <memory>

#include "engine/scene/FreeLookCamera.h"
#include "engine/scene/FlatScene.h"

#include "engine/renderer/DeferredRenderer.h"

namespace phi {
namespace editor {

class PhiWidget : public QOpenGLWidget {
    std::unique_ptr<phi::Device> m_device;
    std::unique_ptr<phi::Renderer> m_renderer;
    std::unique_ptr<phi::FreeLookCamera> m_camera;
    std::unique_ptr<phi::Scene> m_scene;

public:
    explicit PhiWidget(QWidget *parent = nullptr);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
};

} // namespace editor
} // namespace phi

#endif
