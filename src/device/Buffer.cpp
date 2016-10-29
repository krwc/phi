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
    if (m_id) {
        CheckedCall(phi::glDeleteBuffers, 1, &m_id);
        PHI_LOG(TRACE, "%s buffer: deleted (ID=%u)", BufferTypeString(m_type),
                m_id);
    }
}

Buffer &Buffer::operator=(Buffer &&other) {
    if (&other != this) {
        Destroy();
        m_type = other.m_type;
        m_id = other.m_id;
        m_size = other.m_size;
        other.m_id = GL_NONE;
    }
    return *this;
}

Buffer::Buffer(Buffer &&other) : m_id(GL_NONE) {
    *this = move(other);
}

Buffer::Buffer(BufferType type,
               BufferHint usage,
               const void *data,
               size_t size)
        : m_type(type), m_id(GL_NONE), m_size(size) {
    CheckedCall(phi::glCreateBuffers, 1, &m_id);
    CheckedCall(phi::glNamedBufferData, m_id, size, data, (GLenum) usage);
    PHI_LOG(TRACE, "%s buffer: created (ID=%u) of size %u",
            BufferTypeString(m_type), m_id, size);
}

Buffer::~Buffer() {
    Destroy();
}

void Buffer::UpdateData(const void *data, size_t size, size_t offset) {
    if (offset + size > m_size) {
        throw invalid_argument("Data size exceeds buffer capacity");
    }
    CheckedCall(phi::glNamedBufferSubData, m_id, offset, size, data);
}

} // namespace phi
