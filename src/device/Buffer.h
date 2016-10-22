#ifndef PHI_DEVICE_BUFFER_H
#define PHI_DEVICE_BUFFER_H
#include "Prototypes.h"
#include "Resource.h"

namespace phi {

enum class BufferType {
    Vertex = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER,
};

enum class BufferHint {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW
};

class Buffer : virtual public Resource {
    void Destroy();

    BufferType m_type;
    GLuint m_bind;
    size_t m_size;

public:
    Buffer &operator=(const Buffer &) = delete;
    Buffer(const Buffer &) = delete;

    Buffer &operator=(Buffer &&);
    Buffer(Buffer &&);
    Buffer(BufferType type, BufferHint usage, const void *data, size_t size);
    virtual ~Buffer();
    void UpdateData(const void *data, size_t size, size_t offset = 0);

    inline BufferType GetType() const {
        return m_type;
    }

    virtual GLuint GetId() const {
        return m_bind;
    }
};

} // namespace phi

#endif
