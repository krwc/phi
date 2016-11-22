#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QPalette>
#include <QString>
#include <QStyleFactory>

#include "engine/scene/Entity.h"

namespace phi {
namespace editor {

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QPalette style;
    style.setColor(QPalette::Window, QColor("#252b30"));
    style.setColor(QPalette::WindowText, QColor("#c6c6c6"));
    style.setColor(QPalette::Base, QColor("#1e2227"));
    style.setColor(QPalette::Text, QColor("#c6c6c6"));
    style.setColor(QPalette::Button, QColor("#182631"));
    style.setColor(QPalette::ButtonText, QColor("#d0d0d0"));
    style.setColor(QPalette::Highlight, QColor("#4f8186"));
    style.setColor(QPalette::HighlightedText, QColor("#f0f0f0"));
    qApp->setPalette(style);

    QObject::connect(ui->phiWidget, &phi::editor::PhiWidget::EntityPicked, this,
                     &phi::editor::MainWindow::OnEntityPicked);

    // By default no object is selected.
    ui->transformProperties->setVisible(false);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::OnEntityPicked(phi::Entity *entity) {
    ui->transformProperties->setVisible(!!entity);
    ui->transformProperties->OnEntitySelected(entity);
    if (entity) {
        ui->statusbar->showMessage(
                QString("Picked %0").arg(entity->GetName().c_str()), 5000);
    }
}

} // namespace editor
} // namespace phi
