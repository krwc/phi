#ifndef PHI_RENDERER_FRAME_BUFFER_H
#define PHI_RENDERER_FRAME_BUFFER_H
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <map>

#include "Texture.h"

namespace phi {

class FrameBuffer {
    GLuint m_bind;
    int m_width;
    int m_height;

    std::map<int, Texture2D *> m_color_attachments;
    Texture2D *m_depth_attachment;

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
    virtual void SetColorAttachment(int index, Texture2D *color_texture);

    /**
     * Sets @p depth_texture as a depth attachment of id @p index.
     */
    virtual void SetDepthAttachment(Texture2D *depth_texture);

    virtual bool IsReady() const;

    virtual const std::map<int, Texture2D *> &GetColorAttachments() const {
        return m_color_attachments;
    }

    virtual const Texture2D *GetDepthAttachment() const {
        return m_depth_attachment;
    }

    virtual GLuint GetId() const {
        return m_bind;
    }
};

} // namespace phi

#endif
