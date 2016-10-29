#include <algorithm>

#include "FrameBuffer.h"
#include "Utils.h"

#include "utils/Logging.h"

namespace phi {
using namespace std;

phi::DefaultFrameBuffer &DefaultFrameBuffer::Instance() {
    static phi::DefaultFrameBuffer target;
    return target;
}

void FrameBuffer::Destroy() {
    if (m_bind) {
        CheckedCall(phi::glDeleteFramebuffers, 1, &m_bind);
        PHI_LOG(TRACE, "Deleted FrameBuffer (ID=%u)", m_bind);
    }
}

FrameBuffer &FrameBuffer::operator=(phi::FrameBuffer &&other) {
    if (this != &other) {
        Destroy();
        m_bind = other.m_bind;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_bind = GL_NONE;
    }
    return *this;
}

FrameBuffer::FrameBuffer(phi::FrameBuffer &&other) : m_bind(GL_NONE) {
    *this = move(other);
}

FrameBuffer::FrameBuffer(int width, int height)
        : m_bind(GL_NONE),
          m_width(width),
          m_height(height) {
    CheckedCall(phi::glCreateFramebuffers, 1, &m_bind);
    PHI_LOG(TRACE, "Created FrameBuffer (ID=%u)", m_bind);
}

FrameBuffer::~FrameBuffer() {
    Destroy();
}

void FrameBuffer::SetColorAttachment(const phi::ColorAttachment &attachment) {
    assert(attachment.color);
    phi::TextureFormat format = attachment.color->GetFormat();
    assert(format != phi::TextureFormat::DEPTH_16
           && format != phi::TextureFormat::DEPTH_24
           && format != phi::TextureFormat::DEPTH_32);
    GLenum name = GL_COLOR_ATTACHMENT0 + attachment.index;

    m_buffers.push_back(name);
    std::sort(std::begin(m_buffers), std::end(m_buffers));
    m_buffers.erase(std::unique(std::begin(m_buffers), std::end(m_buffers)),
                    std::end(m_buffers));

    CheckedCall(phi::glNamedFramebufferTexture, m_bind,
                GL_COLOR_ATTACHMENT0 + attachment.index,
                attachment.color->GetId(), 0);
    CheckedCall(phi::glNamedFramebufferDrawBuffers, m_bind,
                (GLsizei) m_buffers.size(), m_buffers.data());
}

void FrameBuffer::SetDepthAttachment(const phi::DepthAttachment &attachment) {
    assert(attachment.depth);
    phi::TextureFormat format = attachment.depth->GetFormat();
    assert(format == phi::TextureFormat::DEPTH_16
           || format == phi::TextureFormat::DEPTH_24
           || format == phi::TextureFormat::DEPTH_32);
    CheckedCall(phi::glNamedFramebufferTexture, m_bind, GL_DEPTH_ATTACHMENT,
                attachment.depth->GetId(), 0);
}

bool FrameBuffer::IsReady() const {
    return CheckedCall(phi::glCheckNamedFramebufferStatus, m_bind, GL_FRAMEBUFFER)
               == GL_FRAMEBUFFER_COMPLETE;
}

} // namespace phi
