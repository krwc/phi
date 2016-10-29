#ifndef PHI_RENDERER_FORWARD_RENDERER_H
#define PHI_RENDERER_FORWARD_RENDERER_H
#include <memory>

#include "DrawCallQueue.h"
#include "Renderer.h"

#include "math/Box.h"
#include "math/Math.h"

#include "device/FrameBuffer.h"
#include "device/Shader.h"
#include "device/Texture.h"

namespace phi {
class Layout;
class Scene;
class Light;

class ForwardRenderer : public Renderer {
    friend class DebugDrawer;

    std::vector<const phi::DirLight *> m_shadow_casters;
    glm::mat4 m_proj;
    int m_width;
    int m_height;
    GLuint m_vao;

    struct State {
        std::vector<int> arrays;
        phi::Program *program;
        const phi::Buffer *vbo;
        const phi::Buffer *ibo;
    } m_last;

    void BindGlobals(const glm::mat4 &view, const glm::mat4 &model);
    void BindLights(const std::vector<phi::DirLight *> &,
                    const std::vector<phi::PointLight *> &);
    void BindProgram(phi::Program *program);
    void BindLayout(const phi::Layout *layout);
    void BindVbo(const phi::Buffer *buffer);
    void BindIbo(const phi::Buffer *buffer);
    void Draw(phi::Primitive, int start, int count);
    void Execute(const glm::mat4 &view, const phi::DrawCall &);

public:
    ForwardRenderer(int width, int height);
    virtual void Render(phi::Scene &);
    virtual void Execute(const phi::DrawCall &);
    virtual void Resize(int width, int height);
    virtual void SetViewport(int x, int y, int w, int h);
    virtual void SetScissor(int x, int y, int w, int h);
    virtual void SetFrameBuffer(phi::FrameBuffer &target);
};

} // namespace phi

#endif
