#include <GLFW/glfw3.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "utils/Logging.h"

#include "device/Prototypes.h"

#include "scene/FreeLookCamera.h"
#include "scene/ListScene.h"

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
#define PHI_LOAD_PROC_HELPER(name) PHI_LOAD_PROC(name, glfwGetProcAddress)
    PHI_LOAD_PROC_HELPER(glAttachShader);
    PHI_LOAD_PROC_HELPER(glBindTexture);
    PHI_LOAD_PROC_HELPER(glCheckNamedFramebufferStatus);
    PHI_LOAD_PROC_HELPER(glCompileShader);
    PHI_LOAD_PROC_HELPER(glCreateBuffers);
    PHI_LOAD_PROC_HELPER(glCreateFramebuffers);
    PHI_LOAD_PROC_HELPER(glCreateProgram);
    PHI_LOAD_PROC_HELPER(glCreateShader);
    PHI_LOAD_PROC_HELPER(glDeleteBuffers);
    PHI_LOAD_PROC_HELPER(glDeleteFramebuffers);
    PHI_LOAD_PROC_HELPER(glDeleteProgram);
    PHI_LOAD_PROC_HELPER(glDeleteShader);
    PHI_LOAD_PROC_HELPER(glDeleteTextures);
    PHI_LOAD_PROC_HELPER(glDetachShader);
    PHI_LOAD_PROC_HELPER(glGenerateMipmap);
    PHI_LOAD_PROC_HELPER(glGenTextures);
    PHI_LOAD_PROC_HELPER(glGetActiveAttrib);
    PHI_LOAD_PROC_HELPER(glGetActiveUniform);
    PHI_LOAD_PROC_HELPER(glGetAttribLocation);
    PHI_LOAD_PROC_HELPER(glGetIntegerv);
    PHI_LOAD_PROC_HELPER(glGetProgramInfoLog);
    PHI_LOAD_PROC_HELPER(glGetProgramiv);
    PHI_LOAD_PROC_HELPER(glGetShaderInfoLog);
    PHI_LOAD_PROC_HELPER(glGetShaderiv);
    PHI_LOAD_PROC_HELPER(glGetUniformLocation);
    PHI_LOAD_PROC_HELPER(glLinkProgram);
    PHI_LOAD_PROC_HELPER(glNamedBufferData);
    PHI_LOAD_PROC_HELPER(glNamedBufferSubData);
    PHI_LOAD_PROC_HELPER(glNamedFramebufferTexture);
    PHI_LOAD_PROC_HELPER(glProgramUniform1fv);
    PHI_LOAD_PROC_HELPER(glProgramUniform1i);
    PHI_LOAD_PROC_HELPER(glProgramUniform2fv);
    PHI_LOAD_PROC_HELPER(glProgramUniform3fv);
    PHI_LOAD_PROC_HELPER(glProgramUniform4fv);
    PHI_LOAD_PROC_HELPER(glProgramUniformMatrix3fv);
    PHI_LOAD_PROC_HELPER(glProgramUniformMatrix4fv);
    PHI_LOAD_PROC_HELPER(glShaderSource);
    PHI_LOAD_PROC_HELPER(glTexImage2D);
    PHI_LOAD_PROC_HELPER(glTexSubImage2D);
#undef PHI_LOAD_PROC_HELPER
    PHI_LOG(TRACE, "Initialized window (`%s`)", title.c_str());
}

} // namespace phi

int main() {
    phi::AppData app {};
    app.width = 1024;
    app.height = 768;
    phi::InitWindow(app);
    unique_ptr<phi::Scene> scene = make_unique<phi::ListScene>();
    unique_ptr<phi::Camera> camera = make_unique<phi::FreeLookCamera>();

    scene->SetCamera(camera.get());

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
