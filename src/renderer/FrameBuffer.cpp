#include "FrameBuffer.h"
#include "Utils.h"

#include "utils/Logging.h"

namespace phi {
using namespace std;

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) {
    if (this != &other) {
        this->~FrameBuffer();
        m_bind = other.m_bind;
        m_width = other.m_width;
        m_height = other.m_height;
        m_color_attachments = move(other.m_color_attachments);
        m_depth_attachment = move(other.m_depth_attachment);
    }
    return *this;
}

FrameBuffer::FrameBuffer(FrameBuffer &&other) : m_bind(GL_NONE) {
    *this = move(other);
}

FrameBuffer::FrameBuffer(int width, int height)
        : m_bind(GL_NONE),
          m_width(width),
          m_height(height),
          m_color_attachments(),
          m_depth_attachment(nullptr) {
    CheckedCall(glCreateFramebuffers, 1, &m_bind);
    PHI_LOG(TRACE, "Created FrameBuffer (ID=%u)", m_bind);
}

FrameBuffer::~FrameBuffer() {
    if (m_bind) {
        CheckedCall(glDeleteFramebuffers, 1, &m_bind);
        PHI_LOG(TRACE, "Deleted FrameBuffer (ID=%u)", m_bind);
    }
}

void FrameBuffer::SetColorAttachment(int index, Texture2D *color_texture) {
    assert(color_texture);
    TextureFormat format = color_texture->GetFormat();
    assert(format != TextureFormat::DEPTH_24
           && format != TextureFormat::DEPTH_32);
    CheckedCall(glNamedFramebufferTexture, m_bind, GL_COLOR_ATTACHMENT0 + index,
                color_texture->GetId(), 0);
    m_color_attachments[index] = color_texture;
}

void FrameBuffer::SetDepthAttachment(Texture2D *depth_texture) {
    assert(depth_texture);
    TextureFormat format = depth_texture->GetFormat();
    assert(format == TextureFormat::DEPTH_24
           || format == TextureFormat::DEPTH_32);
    CheckedCall(glNamedFramebufferTexture, m_bind, GL_DEPTH_ATTACHMENT,
                depth_texture->GetId(), 0);
}

bool FrameBuffer::IsReady() const {
    return CheckedCall(glCheckNamedFramebufferStatus, m_bind, GL_FRAMEBUFFER)
           == GL_FRAMEBUFFER_COMPLETE;
}

} // namespace phi
