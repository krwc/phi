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
    int num_active_uniforms = 0;
    phi::glGetProgramInterfaceiv(bind, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_active_uniforms);
    enum {
        PROPERTY_BLOCK_INDEX = 0,
        PROPERTY_NAME_LENGTH = 1,
        PROPERTY_LOCATION = 2,
        PROPERTY_TYPE = 3
    };
    const GLenum properties[4] = {
        GL_BLOCK_INDEX,
        GL_NAME_LENGTH,
        GL_LOCATION,
        GL_TYPE
    };

    for (int index = 0; index < num_active_uniforms; ++index) {
        GLint results[4];
        phi::glGetProgramResourceiv(bind, GL_UNIFORM, index, 4, properties, 4, nullptr,
                                    results);

        /* Ignore uniforms located in Uniform Blocks */
        if (results[PROPERTY_BLOCK_INDEX] != -1) {
            continue;
        }

        std::string uniform_name(results[PROPERTY_NAME_LENGTH] - 1, '\0');
        phi::glGetProgramResourceName(bind, GL_UNIFORM, index, results[PROPERTY_NAME_LENGTH],
                                      nullptr, &uniform_name[0]);
        result[uniform_name] = Program::ParamInfo {
            (GLenum) results[PROPERTY_TYPE],
            (GLint) results[PROPERTY_LOCATION]
        };

        PHI_LOG(TRACE, "Program: found constant %s", uniform_name.c_str());
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
