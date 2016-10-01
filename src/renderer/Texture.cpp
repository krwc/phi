#include "Texture.h"
#include "Utils.h"
#include "utils/Logging.h"
#include <stdexcept>

namespace phi {
using namespace std;

Texture::Texture(const TextureDesc &desc)
        : m_bind(0),
          m_type(desc.type),
          m_format(desc.format),
          m_depth(desc.depth),
          m_width(desc.width),
          m_height(desc.height) {
    CheckedCall(glGenTextures, 1, &m_bind);
}

Texture::~Texture() {
    CheckedCall(glDeleteTextures, 1, &m_bind);
}

namespace {

GLenum TextureInternalFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::R_8:
    case TextureFormat::R_16F:
    case TextureFormat::R_32F:
        return GL_RED;
    case TextureFormat::RG_16F:
    case TextureFormat::RG_32F:
        return GL_RG;
    case TextureFormat::RGB_888:
        return GL_RGB;
    case TextureFormat::RGBA_8888:
        return GL_RGBA;
    case TextureFormat::DEPTH_24:
    case TextureFormat::DEPTH_32:
        return GL_DEPTH_COMPONENT;
    default:
        throw runtime_error("Invalid format");
    }
}

GLenum TexturePixelType(TextureFormat format) {
    switch (format) {
    case TextureFormat::R_8:
    case TextureFormat::RGB_888:
    case TextureFormat::RGBA_8888:
        return GL_UNSIGNED_BYTE;
    case TextureFormat::R_16F:
    case TextureFormat::R_32F:
    case TextureFormat::RG_16F:
    case TextureFormat::RG_32F:
        return GL_FLOAT;
    case TextureFormat::DEPTH_24:
    case TextureFormat::DEPTH_32:
        return GL_FLOAT;
    default:
        throw runtime_error("Invalid format");
    }
}

} // namespace

Texture2D::Texture2D(int width, int height, TextureFormat format)
        : Texture({ TextureType::Texture2D, format, 1, width, height }) {
    GLint current = GL_NONE;
    CheckedCall(glGetIntegerv, GL_TEXTURE_BINDING_2D, &current);
    CheckedCall(glBindTexture, GL_TEXTURE_2D, m_bind);
    CheckedCall(glTexImage2D, GL_TEXTURE_2D, 0, (GLenum) format, width, height,
                0, TextureInternalFormat(format), TexturePixelType(format),
                nullptr);
    CheckedCall(glBindTexture, GL_TEXTURE_2D, current);
    PHI_LOG(TRACE, "Texture2D: created texture (ID=%u) %dx%d", m_bind, width, height);
}

Texture2D::~Texture2D() {
    PHI_LOG(TRACE, "Texture2D: deleted texture (ID=%u)", m_bind);
}

} // namespace phi
