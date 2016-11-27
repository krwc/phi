#ifndef PHI_EDITOR_PHI_WIDGET_H
#define PHI_EDITOR_PHI_WIDGET_H
#include "RenderingContext.h"

#include <QOpenGLWidget>
#include <QSet>
#include <memory>

#include "engine/renderer/OutlinePass.h"
#include "engine/scene/FreeLookCamera.h"
#include "engine/utils/Timer.h"

namespace phi {
namespace editor {

class PhiWidget : public QOpenGLWidget {
    Q_OBJECT

    std::unique_ptr<phi::editor::RenderingContext> m_context;
    std::unique_ptr<phi::FreeLookCamera> m_camera;
    std::unique_ptr<phi::OutlinePass> m_outline_pass;
    phi::Entity *m_selected_entity;
    phi::Timer m_timer;
    double m_dt;

    QSet<int> m_pressed_keys;
    // True if rotation via mouse movement should be performed.
    bool m_rotate;
    // Valid if mouse position is being tracked by the widget.
    struct {
        int x, y;
    } m_mouse_position;

    void handleInput();

signals:
    void EntityPicked(phi::Entity *);
    void RenderingContextInitialized(phi::editor::RenderingContext *);

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
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
};

} // namespace editor
} // namespace phi

#endif
