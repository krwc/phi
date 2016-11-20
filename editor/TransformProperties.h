#ifndef PHI_EDITOR_TRANSFORM_PROPERTIES_H
#define PHI_EDITOR_TRANSFORM_PROPERTIES_H
#include <QGroupBox>

#include "engine/scene/Entity.h"

namespace Ui {
class TransformProperties;
}

namespace phi {
namespace editor {

class TransformProperties : public QGroupBox {
    Q_OBJECT

    phi::Entity *m_entity;
    bool m_supress_update;

    Ui::TransformProperties *ui;

public:
    explicit TransformProperties(QWidget *parent = 0);
    ~TransformProperties();

private slots:
    void OnTransformChanged(double);
    void OnRotationEditFinished();

public slots:
    void OnEntitySelected(phi::Entity *);
};

} // namespace editor
} // namespace phi

#endif // PHI_EDITOR_TRANSFORM_PROPERTIES_H
