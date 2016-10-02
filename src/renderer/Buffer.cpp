#include "Buffer.h"
#include "Utils.h"
#include "utils/Logging.h"
#include <stdexcept>

namespace phi {
using namespace std;

Buffer::Buffer(BufferType type,
               BufferUsage usage,
               const void *data,
               size_t size)
        : m_type(type), m_bind(GL_NONE), m_size(size) {
    CheckedCall(glCreateBuffers, 1, &m_bind);
    CheckedCall(glNamedBufferData, m_bind, size, data, (GLenum) usage);
    PHI_LOG(TRACE, "Buffer: created (ID=%u) of size %u", m_bind, size);
}

Buffer::~Buffer() {
    CheckedCall(glDeleteBuffers, 1, &m_bind);
    PHI_LOG(TRACE, "Buffer: deleted (ID=%u)", m_bind);
}

void Buffer::UpdateData(const void *data, size_t size, size_t offset) {
    if (offset + size > m_size) {
        throw invalid_argument("Data size exceeds buffer capacity");
    }
    CheckedCall(glNamedBufferSubData, m_bind, offset, size, data);
}

} // namespace phi
