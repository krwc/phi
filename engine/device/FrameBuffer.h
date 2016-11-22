#ifndef PHI_DEVICE_FRAME_BUFFER_H
#define PHI_DEVICE_FRAME_BUFFER_H
#include <vector>
#include <stdexcept>

#include "Prototypes.h"
#include "Resource.h"
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

class FrameBuffer : public Resource {
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

    int GetWidth() const {
        return m_width;
    }

    int GetHeight() const {
        return m_height;
    }
};

} // namespace phi

#endif
