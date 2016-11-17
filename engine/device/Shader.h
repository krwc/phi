#ifndef PHI_DEVICE_SHADER_H
#define PHI_DEVICE_SHADER_H
#include "Prototypes.h"
#include "Resource.h"

namespace phi {

enum class ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
};

class Shader : virtual public Resource {
    void Destroy();

    GLuint m_id;
    ShaderType m_type;
    const char *m_source;

public:
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader &operator=(Shader &&);
    Shader(Shader &&);
    Shader(ShaderType type, const char *source) noexcept;
    virtual ~Shader() noexcept;
    virtual void Compile();

    virtual ShaderType GetType() const {
        return m_type;
    }

    virtual GLuint GetId() const {
        return m_id;
    }
};

} // namespace phi

#endif
