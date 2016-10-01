#ifndef PHI_RENDERER_BUFFER_H
#define PHI_RENDERER_BUFFER_H
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

namespace phi {

enum class BufferType {
    Vertex = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER
};

enum class BufferUsage {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW
};

struct BufferDesc {
    BufferType type;
    BufferUsage usage;
    const void *data;
    size_t size;
};

class Buffer {
    BufferType m_type;
    GLuint m_bind;
    size_t m_size;

public:
    Buffer(const BufferDesc &desc);
    virtual ~Buffer();
    void UpdateData(const void *data, size_t size, size_t offset = 0);

    inline BufferType GetType() const {
        return m_type;
    }

    GLuint GetId() const {
        return m_bind;
    }
};

} // namespace phi

#endif
