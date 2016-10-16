#ifndef PHI_RENDERER_FORWARD_RENDERER_H
#define PHI_RENDERER_FORWARD_RENDERER_H
#include "CommandQueue.h"
#include "Renderer.h"

#include "math/Math.h"

namespace phi {
class Program;
class Layout;
class Scene;

class ForwardRenderer : public Renderer {
    glm::mat4 m_proj;
    GLuint m_vao;

    struct State {
        std::vector<int> arrays;
        const phi::Layout *layout;
        phi::Program *program;
        phi::Buffer *vbo;
        phi::Buffer *ibo;
    } m_last;

    void
    BindGlobals(phi::Program *, const glm::mat4 &view, const glm::mat4 &model);
    void BindProgram(phi::Program *);
    void BindLayout(phi::Program *, const phi::Layout *);
    void BindVbo(phi::Buffer *);
    void BindIbo(phi::Buffer *);
    void Draw(phi::PrimitiveType, int start, int count);

public:
    ForwardRenderer(int width, int height);
    virtual void Render(phi::Scene *);
    virtual void Resize(int width, int height);
};

} // namespace phi

#endif
