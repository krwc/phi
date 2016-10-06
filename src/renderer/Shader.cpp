#include "Shader.h"
#include "Utils.h"
#include <stdexcept>

namespace phi {
using namespace std;

void Shader::Destroy() {
    if (m_bind) {
        PHI_LOG(INFO, "Shader: deleted shader (ID=%u)", m_bind);
        CheckedCall(phi::glDeleteShader, m_bind);
    }
}

Shader &Shader::operator=(Shader &&other) {
    if (&other != this) {
        Destroy();
        m_bind = other.m_bind;
        m_type = other.m_type;
        m_source = other.m_source;
        other.m_bind = GL_NONE;
    }
    return *this;
}

Shader::Shader(Shader &&other) : m_bind(GL_NONE) {
    *this = move(other);
}

Shader::Shader(ShaderType type, const char *source) noexcept
        : m_bind(CheckedCall(phi::glCreateShader, (GLenum) type)),
          m_type(type),
          m_source(source) {
    PHI_LOG(TRACE, "Shader: created shader (ID=%u)", m_bind);
}

Shader::~Shader() noexcept {
    Destroy();
}

void Shader::Compile() {
    GLint length = (GLint) strlen(m_source);
    CheckedCall(phi::glShaderSource, m_bind, 1, (const GLchar **) (&m_source),
                &length);
    int status, log_length;
    CheckedCall(phi::glCompileShader, m_bind);
    CheckedCall(phi::glGetShaderiv, m_bind, GL_COMPILE_STATUS, &status);
    CheckedCall(phi::glGetShaderiv, m_bind, GL_INFO_LOG_LENGTH, &log_length);
    string compilation_log;
    if (status == GL_FALSE) {
        compilation_log.resize(log_length - 1, '\0');
        CheckedCall(phi::glGetShaderInfoLog, m_bind, log_length - 1, &log_length,
                    &compilation_log[0]);
        throw invalid_argument(compilation_log);
    }
}

void Program::Destroy() {
    if (m_bind) {
        PHI_LOG(TRACE, "Program: deleted (ID=%u)", m_bind);
        CheckedCall(phi::glDeleteProgram, m_bind);
    }
}

Program &Program::operator=(Program &&other) {
    if (&other != this) {
        Destroy();
        m_bind = other.m_bind;
        m_constants = move(other.m_constants);
        m_attributes = move(other.m_attributes);
        m_shaders = move(other.m_shaders);
        other.m_bind = 0;
    }
    return *this;
}

Program::Program(Program &&other) : m_bind(GL_NONE) {
    *this = move(other);
}

Program::Program()
        : m_bind(CheckedCall(phi::glCreateProgram)), m_constants(), m_attributes() {
    PHI_LOG(TRACE, "Program: created (ID=%u)", m_bind);
}

Program::~Program() {
    Destroy();
}

const Program::ParamInfo *Program::FindConstant(const string &name) const try {
    return &m_constants.at(name);
} catch (out_of_range &) {
    return nullptr;
}

const Program::ParamInfo *Program::FindAttribute(const string &name) const try {
    return &m_attributes.at(name);
} catch (out_of_range &) {
    return nullptr;
}

void Program::SetConstant(GLint location, GLenum type, const void *data) {
    switch (type) {
    case GL_INT:
    case GL_BOOL:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_2D_ARRAY:
        CheckedCall(phi::glProgramUniform1i, m_bind, location,
                    *((const GLint *) data));
        break;
    case GL_FLOAT:
        CheckedCall(phi::glProgramUniform1fv, m_bind, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC2:
        CheckedCall(phi::glProgramUniform2fv, m_bind, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC3:
        CheckedCall(phi::glProgramUniform3fv, m_bind, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC4:
        CheckedCall(phi::glProgramUniform4fv, m_bind, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_MAT3:
        CheckedCall(phi::glProgramUniformMatrix3fv, m_bind, location, 1, GL_FALSE,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_MAT4:
        CheckedCall(phi::glProgramUniformMatrix4fv, m_bind, location, 1, GL_FALSE,
                    (const GLfloat *) data);
        break;
    default:
        throw invalid_argument("Constant type not yet supported");
    }
}

void Program::SetSource(ShaderType type, const char *source) {
    m_shaders.emplace_back(type, source);
    try {
        m_shaders.back().Compile();
    } catch (invalid_argument &compile_error) {
        PHI_LOG(ERROR,
                "Shader compilation failed:\n\n>>> Compilation log:\n%s\n",
                compile_error.what());
        m_shaders.pop_back();
    }
}

namespace {

map<string, Program::ParamInfo> DiscoverProgramConstants(GLuint bind) {
    map<string, Program::ParamInfo> result{};
    GLint num_uniforms = 0;
    GLint max_name_length = 0;
    CheckedCall(phi::glGetProgramiv, bind, GL_ACTIVE_UNIFORMS, &num_uniforms);
    CheckedCall(phi::glGetProgramiv, bind, GL_ACTIVE_UNIFORM_MAX_LENGTH,
                 &max_name_length);

    for (int id = 0; id < num_uniforms; ++id) {
        vector<char> buffer(max_name_length);
        GLint length;
        GLenum type;
        GLint size;
        CheckedCall(phi::glGetActiveUniform, bind, id, max_name_length, &length,
                     &size, &type, &buffer[0]);
        string name(buffer.begin(), buffer.begin() + length);

        result[name].type = type;
        result[name].location =
                CheckedCall(phi::glGetUniformLocation, bind, name.c_str());
        PHI_LOG(TRACE, "Shader: found constant (ID=%d) '%s'",
                result[name].location, name.c_str());
        // Ugly, hacky workaround for a bug in intel drivers (I presume),
        // which is unable to find entries in array that have index > 0
        if (name.rfind("[0]") == name.length() - 3) {
            string arr_name = name.substr(0, name.length() - 3);
            // UINT16_MAX is an arbitrary choice, but it should be enough.
            for (int i = 1; i < UINT16_MAX; ++i) {
                string name = arr_name + "[" + to_string(i) + "]";
                GLint location =
                        CheckedCall(phi::glGetUniformLocation, bind, name.c_str());
                if (location >= 0) {
                    result[name].type = type;
                    result[name].location = location;
                    PHI_LOG(TRACE, "Shader: found constant (%d) '%s'",
                            result[name].location, name.c_str());
                } else {
                    // No more locations
                    break;
                }
            }
        }
    }
    return result;
}

const char *TypeString(GLenum type) {
    switch (type) {
    case GL_INT:
        return "GL_INT";
    case GL_FLOAT:
        return "GL_FLOAT";
    case GL_FLOAT_VEC2:
        return "GL_FLOAT_VEC2";
    case GL_FLOAT_VEC3:
        return "GL_FLOAT_VEC3";
    case GL_FLOAT_VEC4:
        return "GL_FLOAT_VEC4";
    default:
        return "(unknown)";
    }
}

map<string, Program::ParamInfo> DiscoverProgramAttributes(GLuint bind) {
    map<string, Program::ParamInfo> result;
    GLint num_attribs = 0;
    GLint max_name_length = 0;
    CheckedCall(phi::glGetProgramiv, bind, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    CheckedCall(phi::glGetProgramiv, bind, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                 &max_name_length);
    for (int id = 0; id < num_attribs; ++id) {
        vector<char> buffer(max_name_length);
        GLint length;
        GLint size;
        GLenum type;
        CheckedCall(phi::glGetActiveAttrib, bind, id, max_name_length, &length,
                    &size, &type, &buffer[0]);
        string name(buffer.begin(), buffer.begin() + length);
        PHI_LOG(TRACE, "Shader: found attribute '%s' [type = %s]", name.c_str(),
                TypeString(type));
        result[name].type = type;
        result[name].location =
                CheckedCall(phi::glGetAttribLocation, bind, name.c_str());
    }
    return result;
}

} // namespace

void Program::Link() {
    for (const auto &shader : m_shaders) {
        CheckedCall(phi::glAttachShader, m_bind, shader.GetId());
    }
    CheckedCall(phi::glLinkProgram, m_bind);
    for (const auto &shader : m_shaders) {
        CheckedCall(phi::glDetachShader, m_bind, shader.GetId());
    }

    int status;
    CheckedCall(phi::glGetProgramiv, m_bind, GL_LINK_STATUS, &status);
    int length;
    CheckedCall(phi::glGetProgramiv, m_bind, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        string log(length - 1, 0);
        CheckedCall(phi::glGetProgramInfoLog, m_bind, length - 1, &length, &log[0]);
        PHI_LOG(DEBUG, "Program link log:\n%s\n", log.c_str());
    }
    if (status == GL_FALSE) {
        throw logic_error("Cannot link program");
    }
    m_constants = DiscoverProgramConstants(m_bind);
    m_attributes = DiscoverProgramAttributes(m_bind);
    PHI_LOG(TRACE, "Program: linked program (ID=%u)", m_bind);
}

} // namespace phi
