#include <memory>
#include <stdexcept>
#include <string>

// This MUST be included before GLFW headers.
#include "device/Device.h"

#include <GLFW/glfw3.h>

#include "utils/Logging.h"
#include "utils/MeshImporter.h"

#include "scene/FlatScene.h"
#include "scene/FreeLookCamera.h"
#include "scene/Light.h"

#include "renderer/ForwardRenderer.h"
#include "renderer/DebugDrawer.h"
#include "renderer/materials/BasicMaterial.h"

using namespace std;
using namespace glm;
namespace phi {

struct Application {
    int width;
    int height;
    bool grab_mouse;
    GLFWwindow *window;
    glm::quat rotation;
    double last_x;
    double last_y;
    double time;
    unique_ptr<phi::Device> device;
    unique_ptr<phi::Renderer> renderer;
    unique_ptr<phi::DebugDrawer> debug;
    unique_ptr<phi::FreeLookCamera> camera;
    unique_ptr<phi::Scene> scene;

    Application(int w, int h, const string &title);
    void HandleInput();
    void HandleResize();
    void Render();
    void Swap();
};

Application::Application(int w, int h, const string &title = "Phi Renderer")
        : width(w),
          height(h),
          grab_mouse(false),
          last_x(0),
          last_y(0),
          time(0) {
    if (!glfwInit()) {
        throw runtime_error("Cannot initialize GLFW");
    }
    // Request OpenGL 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!window) {
        throw runtime_error("Cannot create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    PHI_LOG(TRACE, "Initialized window (`%s`)", title.c_str());

    device = make_unique<phi::Device>((phi::ProcLoader *) glfwGetProcAddress,
                                      width, height);
    renderer = make_unique<phi::ForwardRenderer>(*device.get());
    camera = make_unique<phi::FreeLookCamera>();
    debug = make_unique<phi::DebugDrawer>(*device.get());
    scene = make_unique<phi::FlatScene>();
    scene->SetCamera(camera.get());
}

void Application::HandleInput() {
    HandleResize();
    float accel = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        accel = 4.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->Move(accel * vec3{ 0, 0, -0.1 });
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->Move(accel * vec3{ 0, 0, 0.1 });
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->Move(accel * vec3{ -0.1, 0, 0 });
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->Move(accel * vec3{ 0.1, 0, 0 });
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        grab_mouse = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        grab_mouse = true;
        glfwGetCursorPos(window, &last_x, &last_y);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (grab_mouse) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        double dx = last_x - mx;
        double dy = last_y - my;
        last_x = mx;
        last_y = my;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glm::quat x = glm::rotate(glm::quat(), glm::radians(float(dy) * 0.2f), { 1, 0, 0 });
            glm::quat y = glm::rotate(glm::quat(), glm::radians(float(dx) * 0.2f), { 0, 1, 0 });
            rotation = x * y;
        } else {
            camera->RotateX(-dy * 0.2);
            camera->RotateY(-dx * 0.2);
        }
    }
}

void Application::Render() {
    HandleInput();
    renderer->Render(*scene.get());
}

void Application::Swap() {
    glfwSwapBuffers(window);
    glfwPollEvents();
    time += 0.1;
}

void Application::HandleResize() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    if (w != width || h != height) {
        renderer->Resize(w, h);
        camera->SetAspectRatio(float(w) / float(h));
        width = w;
        height = h;
    }
}

} // namespace phi

int main() {
    phi::Application app(1024, 768);
    auto box = phi::MeshImporter::FromFile("assets/model/box.obj");
    auto plane_h = phi::MeshImporter::FromFile("assets/model/plane.obj");
    auto plane_v = phi::MeshImporter::FromFile("assets/model/plane.obj");
    auto torus = phi::MeshImporter::FromFile("assets/model/torus.obj");
    box->SetScale({2,2,2});
    plane_h->SetPosition({0,-5,0});
    plane_h->SetScale({6,1,6});

    plane_v->SetRotation({90,0,0});
    plane_v->SetScale({6, 2, 3});
    plane_v->SetPosition({0,10,-30});

    torus->SetScale({4, 4, 4});

    auto red_material = make_unique<phi::BasicMaterial>();
    red_material->SetDiffuse({1, 0, 0, 1});
    auto pink_material = make_unique<phi::BasicMaterial>();
    pink_material->SetDiffuse({0.6, 0.2, 0.4, 1});
    auto blue_material = make_unique<phi::BasicMaterial>();
    blue_material->SetDiffuse({0.2, 0.2, 0.3, 1});
    auto violet_material = make_unique<phi::BasicMaterial>();
    violet_material->SetDiffuse({0.4, 0., 0.6, 1});

    box->SetMaterial(red_material.get());
    plane_h->SetMaterial(pink_material.get());
    plane_v->SetMaterial(blue_material.get());
    torus->SetMaterial(violet_material.get());

    app.scene->AddEntity(plane_h.get());
    app.scene->AddEntity(plane_v.get());
    app.scene->AddEntity(box.get());
    app.scene->AddEntity(torus.get());
    app.camera->Move({0,16,60});

    auto sun = make_unique<phi::DirLight>();
    sun->SetPosition({0,3,10});
    sun->SetColor({1,1,1});
    sun->SetShadowCasting(true);

    auto red_bulb = make_unique<phi::PointLight>();
    red_bulb->SetPosition({0,4,8});
    red_bulb->SetColor({1,0,0});
    red_bulb->SetLinearAttenuation(0.01);
    red_bulb->SetQuadraticAttenuation(0.001);

    auto violet_bulb = make_unique<phi::PointLight>();
    violet_bulb->SetPosition(torus->GetPosition());
    violet_bulb->SetColor({0.4, 0., 0.6});
    violet_bulb->SetConstantAttenuation(0.4);
    violet_bulb->SetLinearAttenuation(0.05);
    violet_bulb->SetQuadraticAttenuation(0.001);

    app.scene->AddLight(sun.get());
    app.scene->AddLight(red_bulb.get());
    app.scene->AddLight(violet_bulb.get());

    const float R = 20.0f;
    const float T = 0.1f;

    bool running = true;
    while (running) {
        if (glfwWindowShouldClose(app.window)) {
            running = false;
        }
        box->SetRotation({4*app.time, 3*app.time, 0});
        box->SetPosition({R*sin(T*app.time), 8, R*cos(T*app.time)});
        auto p = sun->GetPosition();
        sun->SetPosition(mat3_cast(app.rotation) * p);
        red_bulb->SetPosition(box->GetPosition());
        app.Render();
        app.Swap();
    }
    glfwDestroyWindow(app.window);
    return 0;
}
