#include "RendererProperties.h"
#include "ui_RendererProperties.h"

namespace phi {
namespace editor {

RendererProperties::RendererProperties(QWidget *parent)
        : QWidget(parent), ui(new Ui::RendererProperties) {
    ui->setupUi(this);
}

RendererProperties::~RendererProperties() {
    delete ui;
}

/** Predefined constants that actually make sense. */
const float MAX_SSAO_RADIUS = 3.0f;
const float MAX_SSAO_POWER  = 8.0f;
const float MAX_SSAO_BIAS   = 0.2f;

void RendererProperties::OnRenderingContextInitialized(phi::editor::RenderingContext *context) {
    const phi::SsaoPass *ssao = context->GetDeferredRenderer().GetSsaoPass();
    ui->ssaoSamples->setValue(ssao->GetNumSamples());
    ui->ssaoRadius->setValue(ssao->GetRadius() / MAX_SSAO_RADIUS * ui->ssaoRadius->maximum());
    ui->ssaoPower->setValue(ssao->GetPower() / MAX_SSAO_POWER * ui->ssaoPower->maximum());
    ui->ssaoBias->setValue(ssao->GetBias() / MAX_SSAO_BIAS * ui->ssaoBias->maximum());

    QObject::connect(ui->ssaoRadius, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoPower, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoBias, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoSamples, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));

    m_context = context;
}

void RendererProperties::OnSsaoDoubleModified(double) {
    UpdateSsaoParameters();
}

void RendererProperties::OnSsaoIntModified(int) {
    UpdateSsaoParameters();
}

void RendererProperties::UpdateSsaoParameters() {
    phi::SsaoPass *ssao = m_context->GetDeferredRenderer().GetSsaoPass();
    ssao->SetNumSamples(ui->ssaoSamples->value());
    ssao->SetRadius(ui->ssaoRadius->value() * MAX_SSAO_RADIUS / ui->ssaoRadius->maximum());
    ssao->SetPower(ui->ssaoPower->value() * MAX_SSAO_POWER / ui->ssaoPower->maximum());
    ssao->SetBias(ui->ssaoBias->value() * MAX_SSAO_BIAS / ui->ssaoBias->maximum());
}
} // namespace editor
} // namespace phi
