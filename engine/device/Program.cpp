#include "Program.h"
#include "Utils.h"

namespace phi {
using namespace std;

void Program::Destroy() {
    if (m_id) {
        PHI_LOG(TRACE, "Program: deleted (ID=%u)", m_id);
        CheckedCall(phi::glDeleteProgram, m_id);
    }
}

Program &Program::operator=(Program &&other) {
    if (&other != this) {
        Destroy();
        m_id = other.m_id;
        m_constants = move(other.m_constants);
        m_shaders = move(other.m_shaders);
        other.m_id = 0;
    }
    return *this;
}

Program::Program(Program &&other) : m_id(GL_NONE) {
    *this = move(other);
}

Program::Program()
        : m_id(CheckedCall(phi::glCreateProgram)), m_constants() {
    PHI_LOG(TRACE, "Program: created (ID=%u)", m_id);
}

Program::~Program() {
    Destroy();
}

const Program::ParamInfo *Program::FindConstant(const string &name) const try {
    return &m_constants.at(name);
} catch (out_of_range &) {
    return nullptr;
}

void Program::SetConstant(GLint location, GLenum type, const void *data) {
    switch (type) {
    case GL_INT:
    case GL_BOOL:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_SHADOW:
        CheckedCall(phi::glProgramUniform1i, m_id, location,
                    *((const GLint *) data));
        break;
    case GL_FLOAT:
        CheckedCall(phi::glProgramUniform1fv, m_id, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC2:
        CheckedCall(phi::glProgramUniform2fv, m_id, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC3:
        CheckedCall(phi::glProgramUniform3fv, m_id, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_VEC4:
        CheckedCall(phi::glProgramUniform4fv, m_id, location, 1,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_MAT3:
        CheckedCall(phi::glProgramUniformMatrix3fv, m_id, location, 1, GL_FALSE,
                    (const GLfloat *) data);
        break;
    case GL_FLOAT_MAT4:
        CheckedCall(phi::glProgramUniformMatrix4fv, m_id, location, 1, GL_FALSE,
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
        throw logic_error("Compilation failure");
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

} // namespace

void Program::Link() {
    for (const auto &shader : m_shaders) {
        CheckedCall(phi::glAttachShader, m_id, shader.GetId());
    }
    CheckedCall(phi::glLinkProgram, m_id);
    for (const auto &shader : m_shaders) {
        CheckedCall(phi::glDetachShader, m_id, shader.GetId());
    }

    int status;
    CheckedCall(phi::glGetProgramiv, m_id, GL_LINK_STATUS, &status);
    int length;
    CheckedCall(phi::glGetProgramiv, m_id, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        string log(length - 1, 0);
        CheckedCall(phi::glGetProgramInfoLog, m_id, length - 1, &length, &log[0]);
        PHI_LOG(DEBUG, "Program link log:\n%s\n", log.c_str());
    }
    if (status == GL_FALSE) {
        throw logic_error("Cannot link program");
    }
    m_constants = DiscoverProgramConstants(m_id);
    PHI_LOG(TRACE, "Program: linked program (ID=%u)", m_id);
}

} // namespace phi
