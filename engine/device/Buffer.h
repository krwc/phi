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
    BufferHint m_hint;
    GLuint m_id;
    size_t m_size;

public:
    Buffer &operator=(const Buffer &) = delete;
    Buffer(const Buffer &) = delete;

    Buffer &operator=(Buffer &&);
    Buffer(Buffer &&);
    /**
     * Creates buffer of specified @p type and initializes it with @p data.
     *
     * @param type  Type of the buffer.
     * @param usage Usage hint for the driver.
     * @param data  Pointer to the data to write into the buffer or nullptr.
     * @param size  Size of the data to write. MUST NOT be nullptr.
     */
    Buffer(BufferType type, BufferHint usage, const void *data, size_t size);
    virtual ~Buffer();

    /**
     * Writes data to the buffer.
     *
     * If the buffer is too small to hold @p size bytes it is trashed and resized.
     * Otherwise only @p size bytes are updated and the remaining bytes are untouched.
     *
     * @param data  Data to write. MUST NOT be nullptr.
     * @param size  Size of the data. MUST NOT be 0.
     */
    void Fill(const void *data, size_t size);

    /**
     * Updates part of the contents of the buffer. Note that @ref size + @ref offset
     * can AT MOST be equal to the size of the buffer, or otherwise an exception will
     * be thrown.
     *
     * @param data      Data to write. MUST NOT be nullptr.
     * @param size      Size of the data. MUST NOT be 0.
     * @param offset    Offset where writing should start.
     */
    void UpdateData(const void *data, size_t size, size_t offset = 0);

    inline BufferType GetType() const {
        return m_type;
    }

    virtual GLuint GetId() const {
        return m_id;
    }
};

} // namespace phi

#endif
