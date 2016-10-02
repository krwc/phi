#ifndef PHI_RENDERER_SHADER_H
#define PHI_RENDERER_SHADER_H
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/Logging.h"

namespace phi {

enum class ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Compute = GL_COMPUTE_SHADER
};

class Shader {
    GLuint m_bind;
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
        return m_bind;
    }
};

class Program {
public:
    struct ParamInfo {
        GLenum type;
        GLint location;
    };

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    Program &operator=(Program &&);
    Program(Program &&);
    Program();
    virtual ~Program();

    template <typename T>
    void SetConstant(const std::string &name, const T &value) {
        if (!m_bind) {
            throw std::logic_error("Shader not linked yet");
        } else if (auto &&info = FindConstant(name)) {
            SetConstant(info->location, info->type, (const void *) &value);
        } else {
            PHI_LOG(WARNING, "Shader: cannot set constant %s: not found",
                    name.c_str());
        }
    }

    virtual void SetSource(ShaderType type, const char *source);
    virtual void Link();

    virtual GLuint GetId() const {
        return m_bind;
    }

private:
    GLuint m_bind;

    std::map<std::string, struct ParamInfo> m_constants;
    std::map<std::string, struct ParamInfo> m_attributes;
    std::vector<Shader> m_shaders;

    const ParamInfo *FindConstant(const std::string &name) const;
    const ParamInfo *FindAttribute(const std::string &name) const;
    void SetConstant(GLint location, GLenum type, const void *value);

};

} // namespace phi

#endif
