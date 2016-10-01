#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

#include "utils/Logging.h"

#include "renderer/Buffer.h"
#include "renderer/Texture.h"

using namespace std;

namespace phi {

struct AppData {
    int width;
    int height;
    GLFWwindow *window;
};

static void InitWindow(AppData &data, const string &title = "Phi Renderer") {
    if (!glfwInit()) {
        throw runtime_error("Cannot initialize GLFW");
    }
    // Request OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    data.window = glfwCreateWindow(data.width, data.height, title.c_str(),
                                   nullptr, nullptr);
    if (!data.window) {
        throw runtime_error("Cannot create GLFW window");
    }
    glfwMakeContextCurrent(data.window);
    glfwSwapInterval(1);
    PHI_LOG(TRACE, "Initialized OpenGL 4.5 window (`%s`)", title.c_str());
}

} // namespace phi

int main() {
    phi::AppData app {};
    app.width = 1024;
    app.height = 768;
    phi::InitWindow(app);
    phi::BufferDesc buffer_desc;
    buffer_desc.type = phi::BufferType::Vertex;
    buffer_desc.usage = phi::BufferUsage::Static;
    buffer_desc.data = nullptr;
    buffer_desc.size = 64666;
    phi::Buffer vbo(buffer_desc);

    bool running = true;
    while (running) {
        if (glfwWindowShouldClose(app.window)) {
            running = false;
        }
        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }
    glfwDestroyWindow(app.window);
    return 0;
}
