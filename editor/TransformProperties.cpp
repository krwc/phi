#include "TransformProperties.h"
#include "ui_TransformProperties.h"

namespace phi {
namespace editor {

TransformProperties::TransformProperties(QWidget *parent)
        : QGroupBox(parent),
          m_entity(nullptr),
          m_supress_update(false),
          ui(new Ui::TransformProperties) {
    ui->setupUi(this);
    QObject::connect(ui->posX, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->posY, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->posZ, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->spinRotX, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->spinRotY, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->spinRotZ, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));

    QObject::connect(ui->spinRotX, SIGNAL(editingFinished()), this, SLOT(OnRotationEditFinished()));
    QObject::connect(ui->spinRotY, SIGNAL(editingFinished()), this, SLOT(OnRotationEditFinished()));
    QObject::connect(ui->spinRotZ, SIGNAL(editingFinished()), this, SLOT(OnRotationEditFinished()));

    QObject::connect(ui->rotX, &QSlider::valueChanged, [this]() {
        if (m_supress_update) {
            return;
        }
        ui->spinRotX->setValue((360.0 * ui->rotX->value()) / ui->rotX->maximum());
    });
    QObject::connect(ui->rotY, &QSlider::valueChanged, [this]() {
        if (m_supress_update) {
            return;
        }
        ui->spinRotY->setValue((360.0 * ui->rotY->value()) / ui->rotY->maximum());
    });
    QObject::connect(ui->rotZ, &QSlider::valueChanged, [this]() {
        if (m_supress_update) {
            return;
        }
        ui->spinRotZ->setValue((360.0 * ui->rotZ->value()) / ui->rotZ->maximum());
    });
    QObject::connect(ui->scaleX, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->scaleY, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
    QObject::connect(ui->scaleZ, SIGNAL(valueChanged(double)), this, SLOT(OnTransformChanged(double)));
}

TransformProperties::~TransformProperties() {
    delete ui;
}

void TransformProperties::OnTransformChanged(double) {
    m_entity->SetRotation({ ui->spinRotX->value(),
                            ui->spinRotY->value(),
                            ui->spinRotZ->value() });
    m_entity->SetPosition({ ui->posX->value(),
                            ui->posY->value(),
                            ui->posZ->value() });
    m_entity->SetScale({ ui->scaleX->value(),
                         ui->scaleY->value(),
                         ui->scaleZ->value() });
}

void TransformProperties::OnRotationEditFinished() {
    // Set to avoid rotation slider update causing rotation spinbox update
    // during value synchronization.
    m_supress_update = true;
    ui->rotX->setValue(ui->spinRotX->value() / 360.0 * ui->rotX->maximum());
    ui->rotY->setValue(ui->spinRotY->value() / 360.0 * ui->rotY->maximum());
    ui->rotZ->setValue(ui->spinRotZ->value() / 360.0 * ui->rotZ->maximum());
    m_supress_update = false;
}

void TransformProperties::OnEntitySelected(phi::Entity *entity) {
    if (!entity) {
        return;
    }
    m_entity = entity;

    const glm::vec3 P = entity->GetPosition();
    const glm::vec3 R = entity->GetRotationAngles();
    const glm::vec3 S = entity->GetScale();
    ui->posX->setValue(P.x);
    ui->posY->setValue(P.y);
    ui->posZ->setValue(P.z);
    ui->spinRotX->setValue(R.x);
    ui->spinRotY->setValue(R.y);
    ui->spinRotZ->setValue(R.z);
    ui->scaleX->setValue(S.x);
    ui->scaleY->setValue(S.y);
    ui->scaleZ->setValue(S.z);
}

}
}
