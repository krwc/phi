#ifndef PHI_EDITOR_MAIN_WINDOW_H
#define PHI_EDITOR_MAIN_WINDOW_H
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace phi {
class Entity;

namespace editor {
class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow *ui;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OnEntityPicked(phi::Entity *);
};

} // namespace editor
} // namespace phi

#endif // PHI_EDITOR_MAIN_WINDOW_H
