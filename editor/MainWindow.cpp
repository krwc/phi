#include "MainWindow.h"

namespace phi {
namespace editor {

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          m_phi_widget(std::make_unique<phi::editor::PhiWidget>()) {
    setCentralWidget(m_phi_widget.get());
    setWindowTitle("Phi Editor");
    resize(QSize(1024, 768));
}

} // namespace editor
} // namespace phi
