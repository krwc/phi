#ifndef PHI_DEVICE_FRAME_BUFFER_H
#define PHI_DEVICE_FRAME_BUFFER_H
#include <vector>
#include <stdexcept>

#include "Prototypes.h"
#include "Texture.h"

namespace phi {

struct ColorAttachment {
    /** Index of the slot to which color texture is attached. */
    uint32_t index;
    /** Texture to attach. */
    phi::Texture *color;
};

struct DepthAttachment {
    phi::Texture *depth;
};

class FrameBuffer {
    void Destroy();

    GLuint m_id;
    int m_width;
    int m_height;
    std::vector<GLenum> m_buffers;

    bool IsTextureSizeValid(const phi::Texture *texture);

public:
    FrameBuffer &operator=(const FrameBuffer &) = delete;
    FrameBuffer(const FrameBuffer&) = delete;

    FrameBuffer &operator=(FrameBuffer &&);
    FrameBuffer(FrameBuffer &&);
    FrameBuffer(int width, int height);
    virtual ~FrameBuffer();

    virtual void SetColorAttachment(const phi::ColorAttachment &attachment);
    virtual void SetDepthAttachment(const phi::DepthAttachment &attachment);
    virtual bool IsReady() const;

    virtual GLuint GetId() const {
        return m_id;
    }
};

// XXX: Stop right there. You see this code? It is bad, and you can tell why
// instantly.
//
//   ...   You feel like you cannot stand it.
//      ... You feel dizzy by just looking...
//
//   ... You slowly prepare yourself to enter insert mode
//
// ... few hours later ...
//
// FUCK YEAH, you did the right thing, you refactored this piece of crap
// and now you're the master of the day! World's grateful for your work,
// beautiful teenage girls are falling themselves into your arms... they're
// moaning for more changes like that in the codebase...
//
// You have reached... you have reached a completion! Damn! Such a good
// feeling. You can now remove this comment.
class DefaultFrameBuffer : public phi::FrameBuffer {
    DefaultFrameBuffer() : phi::FrameBuffer(1, 1) {}

public:
    static DefaultFrameBuffer &Instance();

    virtual void SetColorAttachment(const phi::ColorAttachment &) {
        throw std::logic_error("Cannot modify default render target");
    }

    virtual void SetDepthAttachment(const phi::DepthAttachment &) {
        throw std::logic_error("Cannot modify default render target");
    }

    virtual bool IsReady() const {
        return true;
    }

    virtual GLuint GetId() const {
        return GL_NONE;
    }
};

} // namespace phi

#endif
