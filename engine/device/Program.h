#ifndef PHI_DEVICE_PROGRAM_H
#define PHI_DEVICE_PROGRAM_H
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Prototypes.h"
#include "Resource.h"
#include "Shader.h"

#include "utils/Logging.h"

namespace phi {

class Program : public virtual Resource {
    void Destroy();

    struct ParamInfo {
        GLenum type;
        GLint location;
    };

public:
    typedef Program::ParamInfo *ConstantRef;

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    Program &operator=(Program &&);
    Program(Program &&);
    Program();
    virtual ~Program();

    template <typename T>
    void SetConstant(ConstantRef parameter, const T &value) {
        SetConstant(parameter->location, parameter->type, (const void *) &value);
    }

    template <typename T>
    void SetConstant(const std::string &name, const T &value) {
        if (auto &&info = FindConstant(name)) {
            SetConstant(info, value);
        } else {
            PHI_LOG(WARNING, "Shader: cannot set constant %s: not found", name);
        }
    }

    void SetSource(ShaderType type, const std::string &source);
    void SetSourceFromFile(ShaderType type, const std::string &filename);

    void Link();

    virtual GLuint GetId() const {
        return m_id;
    }

    Program::ConstantRef FindConstant(const std::string &name);

private:
    GLuint m_id;

    std::map<std::string, struct ParamInfo> m_constants;
    std::vector<Shader> m_shaders;

    void DiscoverConstants();
    void SetConstant(GLint location, GLenum type, const void *value);

};

} // namespace phi

#endif
