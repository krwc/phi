#ifndef PHI_EDITOR_MAIN_WINDOW_H
#define PHI_EDITOR_MAIN_WINDOW_H
#include <QMainWindow>
#include <memory>

#include "PhiWidget.h"

namespace phi {
namespace editor {

class MainWindow : public QMainWindow {
    std::unique_ptr<phi::editor::PhiWidget> m_phi_widget;

public:
    explicit MainWindow(QWidget *parent = nullptr);
};

} // namespace editor
} // namespace phi

#endif
