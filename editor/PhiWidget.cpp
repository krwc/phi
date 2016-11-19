#include <QSurfaceFormat>
#include "PhiWidget.h"

#include "engine/renderer/materials/PhongMaterial.h"

#include "engine/utils/MeshImporter.h"

namespace phi {
namespace editor {

struct QtProcLoader : phi::ProcLoader {
    QOpenGLContext *context;
    QtProcLoader(QOpenGLContext *context) : context(context) {}
    void *operator()(const char *name) const {
        return (void *) context->getProcAddress(name);
    }
};

PhiWidget::PhiWidget(QWidget *widget)
        : QOpenGLWidget(widget),
          m_camera(std::make_unique<phi::FreeLookCamera>()),
          m_scene(std::make_unique<phi::FlatScene>()) {
    m_scene->SetCamera(m_camera.get());
    QSurfaceFormat format;
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(format);
}

void PhiWidget::initializeGL() {
    const QSize widget_size = size();
    m_device = std::make_unique<phi::Device>(QtProcLoader(this->context()),
                                             widget_size.width(),
                                             widget_size.height());
    m_renderer = std::make_unique<phi::DeferredRenderer>(*m_device.get());

    static auto box = phi::MeshImporter::FromFile("assets/model/box.obj");
    static auto plane_h = phi::MeshImporter::FromFile("assets/model/plane.obj");
    static auto plane_v = phi::MeshImporter::FromFile("assets/model/plane.obj");
    static auto wall_l = phi::MeshImporter::FromFile("assets/model/plane.obj");
    static auto torus = phi::MeshImporter::FromFile("assets/model/torus.obj");
    static auto bunny = phi::MeshImporter::FromFile("assets/model/bunny.obj");

    bunny->SetScale({80, 80, 80});
    bunny->SetPosition({16, -8, 0});
    box->SetScale({2,2,2});
    plane_h->SetPosition({0,-5,0});
    plane_h->SetScale({6,1,6});

    plane_v->SetRotation({90,0,0});
    plane_v->SetScale({6, 2, 3});
    plane_v->SetPosition({0,10,-30});

    wall_l->SetRotation({0,0,90});
    wall_l->SetScale({4, 1, 8});
    wall_l->SetPosition({-30, 6, 0});

    torus->SetScale({4, 4, 4});

    static auto green_material = std::make_unique<phi::PhongMaterial>();
    green_material->SetDiffuse({0, 1, 0, 1});
    static auto red_material = std::make_unique<phi::PhongMaterial>();
    red_material->SetDiffuse({1, 0, 0, 1});
    static auto pink_material = std::make_unique<phi::PhongMaterial>();
    pink_material->SetDiffuse({0.6, 0.2, 0.4, 1});
    static auto blue_material = std::make_unique<phi::PhongMaterial>();
    blue_material->SetDiffuse({0.2, 0.2, 0.3, 1});
    static auto violet_material = std::make_unique<phi::PhongMaterial>();
    violet_material->SetDiffuse({0.4, 0., 0.6, 1});
    static auto bunny_material = std::make_unique<phi::PhongMaterial>();
    bunny_material->SetDiffuse({0.9, 0.9, 0.9, 1});

    box->SetMaterial(red_material.get());
    plane_h->SetMaterial(pink_material.get());
    plane_v->SetMaterial(blue_material.get());
    wall_l->SetMaterial(green_material.get());
    torus->SetMaterial(violet_material.get());
    bunny->SetMaterial(bunny_material.get());

    m_scene->AddEntity(plane_h.get());
    m_scene->AddEntity(plane_v.get());
    m_scene->AddEntity(box.get());
    m_scene->AddEntity(torus.get());
    m_scene->AddEntity(bunny.get());
    m_scene->AddEntity(wall_l.get());
    m_camera->Move({0,16,20});
    m_camera->RotateX(40);

    static auto sun = std::make_unique<phi::DirLight>();
    sun->SetPosition({10,15,-10});
    sun->SetColor({1,1,1});
    sun->SetShadowCasting(true);

    static auto red_bulb = std::make_unique<phi::PointLight>();
    red_bulb->SetPosition({0,4,8});
    red_bulb->SetColor({1,0,0});
    red_bulb->SetLinearAttenuation(0.01);
    red_bulb->SetQuadraticAttenuation(0.001);

    static auto violet_bulb = std::make_unique<phi::PointLight>();
    violet_bulb->SetPosition(torus->GetPosition());
    violet_bulb->SetColor({0.4, 0., 0.6});
    violet_bulb->SetConstantAttenuation(0.4);
    violet_bulb->SetLinearAttenuation(0.05);
    violet_bulb->SetQuadraticAttenuation(0.001);

    m_scene->AddLight(sun.get());
    m_scene->AddLight(red_bulb.get());
    m_scene->AddLight(violet_bulb.get());
}


void PhiWidget::paintGL() {
    m_device->SetDefaultFrameBuffer(context()->defaultFramebufferObject());
    m_renderer->Render(*m_scene.get());
    update();
}

void PhiWidget::resizeGL(int w, int h) {
    m_renderer->Resize(w, h);
    m_camera->SetAspectRatio(float(w) / float(h));
}

} // namespace editor
} // namespace phi
