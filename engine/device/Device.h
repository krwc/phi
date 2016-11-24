#ifndef PHI_DEVICE_DEVICE_H
#define PHI_DEVICE_DEVICE_H
#include <array>
#include <functional>
#include <vector>

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
struct ProcLoader {
    virtual void *operator()(const char *name) const = 0;
};

class Device {
    phi::Rect2D m_scissor;
    phi::Rect2D m_viewport;
    GLuint m_vao;

    struct State {
        bool zwrite;
        bool ztest;
        std::array<const phi::Texture *, 16u> textures;
        std::array<const phi::Sampler *, 16u> samplers;
        GLuint default_fbo;
        const phi::Layout *layout;
        const phi::Program *program;
        const phi::FrameBuffer *fbo;
        const phi::Buffer *vbo;
        const phi::Buffer *ibo;
    } m_state;

public:
    Device(const phi::ProcLoader &loader,
           int viewport_width,
           int viewport_height);

    /**
     * Blits depth buffer from a specified @p src onto Default FrameBuffer.
     *
     * @param src   Source FrameBuffer.
     */
    void BlitDepthBuffer(const phi::FrameBuffer *src);

#warning "FIXME: this is an ugly hack"
    void SetDefaultFrameBuffer(GLuint index);

    /**
     * Draws geometry from currently bound buffers.
     */
    void Draw(phi::Primitive, int start, int count);

    /**
     * Binds @p program to the device context.
     *
     * Note: @p program might be nullptr, which tells the device to unbind
     *       currently bound program.
     *
     * @param program   Pointer to the program to be bound, or nullptr.
     */
    void BindProgram(const phi::Program *program);

    /**
     * Binds vertex buffer @p buffer to the device context using currently
     * bound layout.
     *
     * Note: @p buffer might be nullptr, which tells the device to unbind
     *       currently bound vertex buffer.
     *
     * @param buffer    Pointer to the buffer to be bound, or nullptr.
     */
    void BindVbo(const phi::Buffer *buffer);

    /**
     * Binds indexx buffer @p buffer to the device context.
     *
     * Note: @p buffer might be nullptr, which tells the device to unbind
     *       currently bound index buffer.
     *
     * @param buffer    Pointer to the buffer to be bound, or nullptr.
     */
    void BindIbo(const phi::Buffer *buffer);

    /**
     * Sets layout to use by vertex buffers. It must be set before any @ref BindVbo,
     * is called.
     *
     * Note: @p layout might be nullptr, which tells the device to reset
     *       currently set layout.
     *
     * @param layout    Pointer to the layout to be bound, or nullptr.
     */
    void BindLayout(const phi::Layout *layout);

    /**
     * Binds texture to a given texture unit.
     *
     * Note: @p texture MUST NOT be nullptr.
     *
     * @param texture_unit  Texture unit to which texture will be bound.
     * @param texture       Texture to be bound.
     */
    void BindTexture(int texture_unit, const phi::Texture *texture);

    /**
     * Binds texture sampler to a given texture unit.
     *
     * Note: @p sampler MUST NOT be nullptr.
     *
     * @param texture_unit  Texture unit to which texture will be bound.
     * @param sampler       Sampler to be bound.
     */
    void BindSampler(int texture_unit, const phi::Sampler *sampler);

    /**
     * Sets current viewport. It works equivalently to @ref glViewport.
     */
    void SetViewport(const phi::Rect2D &viewport);

    /**
     * Sets current scissor box. It works equivalently to @ref glScissor
     */
    void SetScissor(const phi::Rect2D &scissor);

    /**
     * Sets current render target. If the @p target is nullptr, the default
     * FrameBuffer will be bound.
     */
    void BindFrameBuffer(phi::FrameBuffer *target);

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

    /**
     * Gets current state of depth testing.
     */
    bool GetDepthTest() const;
};

} // namespace phi

#endif
