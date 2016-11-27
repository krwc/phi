#ifndef PHI_EDITOR_WORLD_TREE_H
#define PHI_EDITOR_WORLD_TREE_H
#include "RenderingContext.h"

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class WorldTree;
}

namespace phi {
namespace editor {

class WorldTree : public QWidget {
    Q_OBJECT

public:
    explicit WorldTree(QWidget *parent = 0);
    ~WorldTree();

public slots:
    void OnRenderingContextInitialized(phi::editor::RenderingContext *);

private:
    Ui::WorldTree *ui;
    phi::editor::RenderingContext *m_context;
    QTreeWidgetItem *m_world;

    void AddElement(const QString& name, const QString &type);
    void Rebuild();
};

} // namespace editor
} // namespace phi

#endif // PHI_EDITOR_WORLD_TREE_H
