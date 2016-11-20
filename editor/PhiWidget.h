#ifndef PHI_EDITOR_PHI_WIDGET_H
#define PHI_EDITOR_PHI_WIDGET_H
#include "engine/device/Device.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QSet>
#include <memory>

#include "engine/scene/FreeLookCamera.h"
#include "engine/scene/FlatScene.h"

#include "engine/renderer/DeferredRenderer.h"

namespace phi {
namespace editor {

class PhiWidget : public QOpenGLWidget {
    Q_OBJECT

    std::unique_ptr<phi::Device> m_device;
    std::unique_ptr<phi::Renderer> m_renderer;
    std::unique_ptr<phi::FreeLookCamera> m_camera;
    std::unique_ptr<phi::Scene> m_scene;

    QSet<int> m_pressed_keys;

    void handleInput();

signals:
    void EntityPicked(phi::Entity *);

public:
    explicit PhiWidget(QWidget *parent = nullptr);

    virtual QSize minimumSizeHint() const override {
        return QSize(800, 600);
    }

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *);
};

} // namespace editor
} // namespace phi

#endif
