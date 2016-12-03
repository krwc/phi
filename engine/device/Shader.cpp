#include "Shader.h"
#include "Utils.h"

#include "io/File.h"

#include <stdexcept>

namespace phi {
using namespace std;

void Shader::Destroy() {
    if (m_id) {
        PHI_LOG(INFO, "Shader: deleted shader (ID=%u)", m_id);
        CheckedCall(phi::glDeleteShader, m_id);
    }
}

Shader &Shader::operator=(Shader &&other) {
    if (&other != this) {
        Destroy();
        m_id = other.m_id;
        m_type = other.m_type;
        m_source = other.m_source;
        other.m_id = GL_NONE;
    }
    return *this;
}

Shader::Shader(Shader &&other) : m_id(GL_NONE) {
    *this = move(other);
}

Shader::Shader(ShaderType type, const std::string &source) noexcept
        : m_id(CheckedCall(phi::glCreateShader, (GLenum) type)),
          m_type(type),
          m_source(source) {
    PHI_LOG(TRACE, "Shader: created shader (ID=%u)", m_id);
}

Shader::~Shader() noexcept {
    Destroy();
}

void Shader::Compile() {
    GLint length = static_cast<GLint>(m_source.length());
    const char *source = m_source.c_str();
    CheckedCall(phi::glShaderSource, m_id, 1, &source, &length);
    int status, log_length;
    CheckedCall(phi::glCompileShader, m_id);
    CheckedCall(phi::glGetShaderiv, m_id, GL_COMPILE_STATUS, &status);
    CheckedCall(phi::glGetShaderiv, m_id, GL_INFO_LOG_LENGTH, &log_length);
    string compilation_log;
    if (status == GL_FALSE) {
        compilation_log.resize(log_length - 1, '\0');
        CheckedCall(phi::glGetShaderInfoLog, m_id, log_length - 1, &log_length,
                    &compilation_log[0]);
        throw invalid_argument(compilation_log);
    }
}

} // namespace phi
