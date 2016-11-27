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
const float MAX_SSAO_RADIUS     = 3.0f;
const float MAX_SSAO_POWER      = 8.0f;
const float MAX_SSAO_BIAS       = 0.2f;
const float MAX_SSAO_STRENGTH   = 8.0f;

void RendererProperties::OnRenderingContextInitialized(phi::editor::RenderingContext *context) {
    const phi::SsaoPass *ssao = context->GetDeferredRenderer().GetSsaoPass();
    const phi::LightPass *light = context->GetDeferredRenderer().GetLightPass();
    ui->ssaoStrength->setValue(ssao->GetStrength() / MAX_SSAO_STRENGTH * ui->ssaoStrength->maximum());
    ui->ssaoRadius->setValue(ssao->GetRadius() / MAX_SSAO_RADIUS * ui->ssaoRadius->maximum());
    ui->ssaoPower->setValue(ssao->GetPower() / MAX_SSAO_POWER * ui->ssaoPower->maximum());
    ui->ssaoBias->setValue(ssao->GetBias() / MAX_SSAO_BIAS * ui->ssaoBias->maximum());
    ui->shadowFilter->setCurrentIndex((int) light->GetShadowFiltering());

    QObject::connect(ui->ssaoRadius, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoPower, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoBias, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));
    QObject::connect(ui->ssaoStrength, SIGNAL(valueChanged(int)), this, SLOT(OnSsaoIntModified(int)));

    QObject::connect(ui->shadowFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(OnShadowModified(int)));

    m_context = context;
}

void RendererProperties::OnSsaoIntModified(int) {
    UpdateSsaoParameters();
}

void RendererProperties::OnShadowModified(int index) {
    phi::LightPass *light = m_context->GetDeferredRenderer().GetLightPass();
    switch ((LightPass::ShadowFiltering) index) {
    case LightPass::PCF_3x3:
    case LightPass::PCF_5x5:
    case LightPass::PCF_9x9:
        light->SetShadowFiltering((LightPass::ShadowFiltering) index);
        break;
    default:
        light->SetShadowFiltering(LightPass::PCF_5x5);
        break;
    }
}

void RendererProperties::UpdateSsaoParameters() {
    phi::SsaoPass *ssao = m_context->GetDeferredRenderer().GetSsaoPass();
    ssao->SetStrength(ui->ssaoStrength->value() * MAX_SSAO_STRENGTH / ui->ssaoStrength->maximum());
    ssao->SetRadius(ui->ssaoRadius->value() * MAX_SSAO_RADIUS / ui->ssaoRadius->maximum());
    ssao->SetPower(ui->ssaoPower->value() * MAX_SSAO_POWER / ui->ssaoPower->maximum());
    ssao->SetBias(ui->ssaoBias->value() * MAX_SSAO_BIAS / ui->ssaoBias->maximum());
}
} // namespace editor
} // namespace phi
