#include <QKeyEvent>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QCursor>
#include "PhiWidget.h"

#include "engine/renderer/materials/PhongMaterial.h"
#include "engine/renderer/DebugDrawer.h"

#include "engine/utils/MeshImporter.h"
#include "engine/utils/TextureImporter.h"

#include "engine/math/Ray.h"

namespace phi {
namespace editor {

// TODO: Remove this after some scene serialization is done.
static void SetupSampleScene(phi::Scene &scene) {
    static auto box = phi::MeshImporter::FromFile("assets/model/box.obj", "box");
    static auto plane_h = phi::MeshImporter::FromFile("assets/model/plane.obj", "plane_h");
    static auto plane_v = phi::MeshImporter::FromFile("assets/model/plane.obj", "plane_v");
    static auto wall_l = phi::MeshImporter::FromFile("assets/model/plane.obj", "wall_l");
    static auto torus = phi::MeshImporter::FromFile("assets/model/torus.obj", "torus");
    static auto bunny = phi::MeshImporter::FromFile("assets/model/bunny.obj", "bunny");

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

    static std::unique_ptr<phi::Texture2D> brick1 =
            phi::TextureImporter::FromFile("assets/textures/brick1.jpg");

    torus->SetScale({4, 4, 4});

    static auto green_material = std::make_unique<phi::PhongMaterial>();
    green_material->SetDiffuseColor({1, 1, 1, 1});
    static auto red_material = std::make_unique<phi::PhongMaterial>();
    red_material->SetDiffuseColor({1, 0, 0, 1});
    static auto pink_material = std::make_unique<phi::PhongMaterial>();
    pink_material->SetDiffuseColor({0.6, 0.2, 0.4, 1});
    static auto blue_material = std::make_unique<phi::PhongMaterial>();
    blue_material->SetDiffuseTexture(*brick1.get());
    static auto violet_material = std::make_unique<phi::PhongMaterial>();
    violet_material->SetDiffuseColor({0.4, 0., 0.6, 1});
    static auto bunny_material = std::make_unique<phi::PhongMaterial>();
    bunny_material->SetDiffuseColor({0.9, 0.9, 0.9, 1});

    box->SetMaterial(red_material.get());
    plane_h->SetMaterial(pink_material.get());
    plane_v->SetMaterial(blue_material.get());
    wall_l->SetMaterial(green_material.get());
    torus->SetMaterial(violet_material.get());
    bunny->SetMaterial(bunny_material.get());

    scene.AddEntity(plane_h.get());
    scene.AddEntity(plane_v.get());
    scene.AddEntity(box.get());
    scene.AddEntity(torus.get());
    scene.AddEntity(bunny.get());
    scene.AddEntity(wall_l.get());

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

    scene.AddLight(sun.get());
    scene.AddLight(red_bulb.get());
    scene.AddLight(violet_bulb.get());
}

PhiWidget::PhiWidget(QWidget *widget)
        : QOpenGLWidget(widget),
          m_camera(std::make_unique<phi::FreeLookCamera>()),
          m_selected_entity(nullptr),
          m_rotate(false),
          m_rotate_light(false) {
    setFocusPolicy(Qt::StrongFocus);
}

void PhiWidget::initializeGL() {
    const QSize widget_size = size();
    m_context = std::make_unique<phi::editor::RenderingContext>(
            context(), context()->surface(), widget_size.width(), widget_size.height());

    m_context->GetScene().SetCamera(m_camera.get());
    SetupSampleScene(m_context->GetScene());
    m_camera->Move({0,16,20});
    m_camera->RotateX(40);

    emit RenderingContextInitialized(m_context.get());
}

void PhiWidget::paintGL() {
    m_timer.Begin();
    handleInput();
    m_context->RenderFrame();
    m_outline_pass->SetEntity(m_selected_entity);
    m_outline_pass->SetCamera(m_camera.get());
    m_outline_pass->Run();
    update();
    m_timer.End();
    m_dt = m_timer.ElapsedTimeUs<double>() / 1000.0;
}

void PhiWidget::resizeGL(int w, int h) {
    m_context->GetDeferredRenderer().Resize(w, h);
    m_camera->SetAspectRatio(float(w) / float(h));
    m_outline_pass = std::make_unique<phi::OutlinePass>(m_context->GetDeferredRenderer());
}

void PhiWidget::keyPressEvent(QKeyEvent *event) {
    m_pressed_keys.insert(event->key());
}

void PhiWidget::keyReleaseEvent(QKeyEvent *event) {
    m_pressed_keys.remove(event->key());
}

void PhiWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // NDC
        float x = 2.0f * (float(event->x()) / width()) - 1.0f;
        float y = 1.0f - 2.0f * (float(event->y()) / height());
        float z = -1.0f;
        // View
        glm::vec4 dir = glm::inverse(m_camera->GetProjMatrix()) * glm::vec4(x, y, z, 1.0f);
        dir = glm::vec4(dir.x, dir.y, -1.0f, 0.0f);
        // World
        dir = glm::inverse(m_camera->GetViewMatrix()) * dir;

        phi::Ray r;
        r.origin = m_camera->GetPosition();
        r.direction = glm::normalize(glm::vec3(dir));
        m_selected_entity = m_context->GetScene().Pick(r);
        emit EntityPicked(m_selected_entity);
    } else if (event->button() == Qt::RightButton) {
        m_rotate = true;
    } else if (event->button() == Qt::MiddleButton) {
        m_rotate_light = true;
    }
}

void PhiWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_rotate = false;
    } else if (event->button() == Qt::MiddleButton) {
        m_rotate_light = false;
    }
}

void PhiWidget::mouseMoveEvent(QMouseEvent *) {
}

void PhiWidget::handleInput() {
    const double c = m_dt / 16.0;
    float accel = c;
    if (m_pressed_keys.contains(Qt::Key_Shift)) {
        accel *= 4.0f;
    }

    if (m_pressed_keys.contains(Qt::Key_W)) {
        m_camera->Move(accel * glm::vec3{ 0, 0, -0.1 });
    }
    if (m_pressed_keys.contains(Qt::Key_S)) {
        m_camera->Move(accel * glm::vec3{ 0, 0, 0.1 });
    }
    if (m_pressed_keys.contains(Qt::Key_A)) {
        m_camera->Move(accel * glm::vec3{ -0.1, 0, 0 });
    }
    if (m_pressed_keys.contains(Qt::Key_D)) {
        m_camera->Move(accel * glm::vec3{ 0.1, 0, 0 });
    }

    auto cursor = QCursor::pos();
    int x = cursor.x();
    int y = cursor.y();
    int dx = m_mouse_position.x - x;
    int dy = m_mouse_position.y - y;

    if (m_rotate) {
        m_camera->RotateX(-dy * 0.2);
        m_camera->RotateY(-dx * 0.2);
    } else if (m_rotate_light) {
        phi::DirLight *sun = *m_context->GetScene().GetDirLights().begin();
        glm::vec3 P = sun->GetPosition();
        glm::quat rotx = glm::rotate(glm::quat(), glm::radians(-dy * 0.2f), glm::vec3(1, 0, 0));
        glm::quat roty = glm::rotate(glm::quat(), glm::radians(-dx * 0.2f), glm::vec3(0, 1, 0));
        P = glm::mat3_cast(rotx * roty) * P;
        sun->SetPosition(P);
    }

    m_mouse_position.x = x;
    m_mouse_position.y = y;
}

} // namespace editor
} // namespace phi
