#ifndef PHI_DEVICE_FRAME_BUFFER_H
#define PHI_DEVICE_FRAME_BUFFER_H
#include <map>

#include "Prototypes.h"
#include "Resource.h"
#include "Texture.h"

namespace phi {

class FrameBuffer : virtual public Resource {
    void Destroy();

    GLuint m_bind;
    int m_width;
    int m_height;

    std::map<int, phi::Texture2D *> m_color_attachments;
    phi::Texture2D *m_depth_attachment;

public:
    FrameBuffer &operator=(const FrameBuffer &) = delete;
    FrameBuffer(const FrameBuffer&) = delete;

    FrameBuffer &operator=(FrameBuffer &&);
    FrameBuffer(FrameBuffer &&);
    FrameBuffer(int width, int height);
    virtual ~FrameBuffer();

    /**
     * Sets @p color_texture as a color attachment of id @p index.
     */
    virtual void SetColorAttachment(int index, phi::Texture2D *color_texture);

    /**
     * Sets @p depth_texture as a depth attachment of id @p index.
     */
    virtual void SetDepthAttachment(phi::Texture2D *depth_texture);

    virtual bool IsReady() const;

    virtual const std::map<int, phi::Texture2D *> &GetColorAttachments() const {
        return m_color_attachments;
    }

    virtual const phi::Texture2D *GetDepthAttachment() const {
        return m_depth_attachment;
    }

    virtual GLuint GetId() const {
        return m_bind;
    }
};

} // namespace phi

#endif
