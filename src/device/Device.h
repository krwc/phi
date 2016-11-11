#ifndef PHI_DEVICE_DEVICE_H
#define PHI_DEVICE_DEVICE_H
#include <vector>
#include <array>

#include "math/Math.h"
#include "math/Rect2D.h"

#include "Primitive.h"
#include "Prototypes.h"

namespace phi {
class Buffer;
class Layout;
class Program;
class Texture;
class Sampler;
class FrameBuffer;

// Callback used to initialize OpenGL procedures
typedef void *ProcLoader(const char *name);

class Device {
    phi::Rect2D m_scissor;
    phi::Rect2D m_viewport;
    GLuint m_vao;

    struct State {
        bool zwrite;
        std::vector<int> arrays;
        std::array<const phi::Texture *, 16u> textures;
        std::array<const phi::Sampler *, 16u> samplers;
        const phi::Program *program;
        const phi::FrameBuffer *fbo;
        const phi::Buffer *vbo;
        const phi::Buffer *ibo;
    } m_state;

public:
    Device(phi::ProcLoader *loader,
           int viewport_width,
           int viewport_height);

    /**
     * Draws geometry from currently bound buffers.
     */
    void Draw(phi::Primitive, int start, int count);

    void BindProgram(const phi::Program &program);
    void BindVbo(const phi::Buffer &buffer);
    void BindIbo(const phi::Buffer &buffer);
    void BindLayout(const phi::Layout &layout);
    void BindTexture(int texture_unit, const phi::Texture &texture);
    void BindSampler(int texture_unit, const phi::Sampler &sampler);

    /**
     * Sets current viewport. It works equivalently to @ref glViewport.
     */
    void SetViewport(const phi::Rect2D &viewport);

    /**
     * Sets current scissor box. It works equivalently to @ref glScissor
     */
    void SetScissor(const phi::Rect2D &scissor);

    /**
     * Sets current render target to a FrameBuffer. To return to a default
     * framebuffer one one can set @p target to DefaultFrameBuffer::Instance().
     */
    void SetFrameBuffer(phi::FrameBuffer &target);

    /**
     * @returns Current viewport
     */
    const phi::Rect2D &GetViewport() const;

    /**
     * @returns Current scissor area.
     */
    const phi::Rect2D &GetScissor() const;

    /**
     * Clears current depth buffer.
     */
    void ClearDepth();

    /**
     * Clears current color buffer.
     */
    void ClearColor(float r, float g, float b, float a);

    /**
     * Enables / disables writing to the depth buffer.
     */
    void SetDepthWrite(bool enabled);

    /**
     * Enables / disables depth testing.
     */
    void SetDepthTest(bool enabled);

    /**
     * Gets current state of depth writing.
     */
    bool GetDepthWrite() const;
};

} // namespace phi

#endif
