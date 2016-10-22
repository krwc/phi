#include "Buffer.h"
#include "Utils.h"
#include "utils/Logging.h"
#include <stdexcept>

namespace phi {
using namespace std;

namespace {

const char *BufferTypeString(BufferType type) {
    switch (type) {
    case BufferType::Vertex:
        return "Vertex";
    case BufferType::Index:
        return "Index";
    default:
        return "Unknown";
    }
}

}

void Buffer::Destroy() {
    if (m_bind) {
        CheckedCall(phi::glDeleteBuffers, 1, &m_bind);
        PHI_LOG(TRACE, "%s buffer: deleted (ID=%u)", BufferTypeString(m_type),
                m_bind);
    }
}

Buffer &Buffer::operator=(Buffer &&other) {
    if (&other != this) {
        Destroy();
        m_type = other.m_type;
        m_bind = other.m_bind;
        m_size = other.m_size;
        other.m_bind = GL_NONE;
    }
    return *this;
}

Buffer::Buffer(Buffer &&other) : m_bind(GL_NONE) {
    *this = move(other);
}

Buffer::Buffer(BufferType type,
               BufferHint usage,
               const void *data,
               size_t size)
        : m_type(type), m_bind(GL_NONE), m_size(size) {
    CheckedCall(phi::glCreateBuffers, 1, &m_bind);
    CheckedCall(phi::glNamedBufferData, m_bind, size, data, (GLenum) usage);
    PHI_LOG(TRACE, "%s buffer: created (ID=%u) of size %u",
            BufferTypeString(m_type), m_bind, size);
}

Buffer::~Buffer() {
    Destroy();
}

void Buffer::UpdateData(const void *data, size_t size, size_t offset) {
    if (offset + size > m_size) {
        throw invalid_argument("Data size exceeds buffer capacity");
    }
    CheckedCall(phi::glNamedBufferSubData, m_bind, offset, size, data);
}

} // namespace phi
