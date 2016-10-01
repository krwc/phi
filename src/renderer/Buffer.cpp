#include "Buffer.h"
#include "Utils.h"
#include "utils/Logging.h"
#include <stdexcept>

namespace phi {
using namespace std;

Buffer::Buffer(const BufferDesc &desc)
        : m_type(desc.type), m_bind(GL_NONE), m_size(desc.size) {
    CheckedCall(glCreateBuffers, 1, &m_bind);
    CheckedCall(glNamedBufferData, m_bind, desc.size, desc.data, (GLenum) desc.usage);
    PHI_LOG(TRACE, "Buffer: created (ID=%u) of size %u", m_bind, desc.size);
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
