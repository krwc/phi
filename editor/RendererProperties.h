#ifndef PHI_EDITOR_RENDERER_PROPERTIES_H
#define PHI_EDITOR_RENDERER_PROPERTIES_H
#include "RenderingContext.h"

#include <QWidget>

namespace Ui {
class RendererProperties;
}

namespace phi {
namespace editor {

class RendererProperties : public QWidget {
    Q_OBJECT

public:
    explicit RendererProperties(QWidget *parent = 0);
    ~RendererProperties();

public slots:
    void OnRenderingContextInitialized(phi::editor::RenderingContext *);

private slots:
    void OnSsaoIntModified(int);
    void OnShadowModified(int);

private:
    Ui::RendererProperties *ui;
    phi::editor::RenderingContext *m_context;

    void UpdateSsaoParameters();
};

} // namespace editor
} // namespace phi

#endif
