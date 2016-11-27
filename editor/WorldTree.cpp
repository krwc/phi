#include "WorldTree.h"
#include "ui_WorldTree.h"

#include "engine/scene/Entity.h"

#include <QStringList>

namespace phi {
namespace editor {

WorldTree::WorldTree(QWidget *parent)
        : QWidget(parent), ui(new Ui::WorldTree), m_world(nullptr) {
    ui->setupUi(this);
}

WorldTree::~WorldTree() {
    delete ui;
}

void WorldTree::AddElement(const QString &name, const QString &type) {
    assert(m_world);
    m_world->addChild(new QTreeWidgetItem(QStringList{name, type}));
}

void WorldTree::Rebuild() {
    const phi::Scene &scene = m_context->GetScene();
    m_world = new QTreeWidgetItem({"World", "World"});

    ui->treeWidget->clear();
    for (const auto& entity : scene.GetEntities()) {
        AddElement(entity->GetName().c_str(), "Entity");
    }
    size_t lightIndex = 0u;
    for (const auto& light : scene.GetDirLights()) {
        AddElement(tr("Light%1").arg(lightIndex++), "DirectionalLight");
    }
    for (const auto& light : scene.GetPointLights()) {
        AddElement(tr("Light%1").arg(lightIndex++), "PointLight");
    }

    ui->treeWidget->addTopLevelItem(m_world);
}

void WorldTree::OnRenderingContextInitialized(phi::editor::RenderingContext *context) {
    m_context = context;
    Rebuild();
}

} // namespace editor
} // namespace phi
