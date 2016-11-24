#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QSurfaceFormat format;
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    phi::editor::MainWindow w;
    w.show();
    return app.exec();
}
