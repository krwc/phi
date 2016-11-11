#include "Texture.h"
#include "Utils.h"
#include "utils/Logging.h"
#include <stdexcept>

namespace phi {
using namespace std;

void Texture::Destroy() {
    if (m_id) {
        PHI_LOG(TRACE, "Deleting texture (ID=%u)", m_id);
        CheckedCall(phi::glDeleteTextures, 1, &m_id);
    }
}

Texture &Texture::operator=(Texture &&other) {
    if (&other != this) {
        Destroy();
        m_id = other.m_id;
        m_type = other.m_type;
        m_format = other.m_format;
        m_depth = other.m_depth;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_id = GL_NONE;
    }
    return *this;
}

Texture::Texture(Texture &&other) : m_id(GL_NONE) {
    *this = move(other);
}

Texture::Texture(const TextureDesc &desc)
        : m_id(GL_NONE),
          m_type(desc.type),
          m_format(desc.format),
          m_depth(desc.depth),
          m_width(desc.width),
          m_height(desc.height) {
    CheckedCall(phi::glCreateTextures, (GLenum) desc.type, 1, &m_id);
}

Texture::~Texture() {
    Destroy();
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
    case TextureFormat::RGBA_16F:
    case TextureFormat::RGBA_32F:
        return GL_RGBA;
    case TextureFormat::DEPTH_16:
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
    case TextureFormat::RGBA_16F:
    case TextureFormat::RGBA_32F:
        return GL_FLOAT;
    case TextureFormat::DEPTH_16:
    case TextureFormat::DEPTH_24:
    case TextureFormat::DEPTH_32:
        return GL_FLOAT;
    default:
        throw runtime_error("Invalid format");
    }
}

size_t TexturePixelSize(TextureFormat format) {
    switch (format) {
    case TextureFormat::R_8:
        return 1;
    case TextureFormat::R_16F:
    case TextureFormat::DEPTH_16:
        return 2;
    case TextureFormat::RGB_888:
    case TextureFormat::DEPTH_24:
        return 3;
    case TextureFormat::RGBA_8888:
    case TextureFormat::R_32F:
    case TextureFormat::RG_16F:
    case TextureFormat::DEPTH_32:
        return 4;
    case TextureFormat::RG_32F:
    case TextureFormat::RGBA_16F:
        return 8;
    case TextureFormat::RGBA_32F:
        return 16;
    default:
        throw runtime_error("Invalid format");
    }
}

} // namespace

Texture2D::Texture2D(int width, int height, TextureFormat format)
        : Texture({ TextureType::Texture2D, format, 1, width, height }) {
    GLint current = GL_NONE;
    CheckedCall(phi::glGetIntegerv, GL_TEXTURE_BINDING_2D, &current);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, m_id);
    CheckedCall(phi::glTexImage2D, GL_TEXTURE_2D, 0, (GLenum) format, width, height,
                0, TextureInternalFormat(format), TexturePixelType(format),
                nullptr);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, current);
    PHI_LOG(TRACE, "Texture2D: created texture (ID=%u) %dx%d", m_id, width, height);
}

Texture2D::~Texture2D() {
    if (m_id) {
        PHI_LOG(TRACE, "Texture2D: deleted texture (ID=%u)", m_id);
    }
}

void Texture2D::Write(int level, int x, int y, int w, int h, const void *data) {
    size_t size = m_width * m_height * TexturePixelType(m_format);
    assert(size >= w * h * TexturePixelType(m_format));
    GLint current = GL_NONE;
    CheckedCall(phi::glGetIntegerv, GL_TEXTURE_BINDING_2D, &current);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, m_id);
    PHI_LOG(TRACE, "Texture2D: writing %u bytes",
            w * h * TexturePixelSize(m_format));
    CheckedCall(phi::glTexSubImage2D, GL_TEXTURE_2D, level, x, y, w, h,
                TextureInternalFormat(m_format), TexturePixelType(m_format),
                data);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, current);
}

void Texture2D::GenerateMipmaps() {
    GLint current = GL_NONE;
    CheckedCall(phi::glGetIntegerv, GL_TEXTURE_BINDING_2D, &current);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, m_id);
    CheckedCall(phi::glGenerateMipmap, GL_TEXTURE_2D);
    CheckedCall(phi::glBindTexture, GL_TEXTURE_2D, current);
    PHI_LOG(TRACE, "Texture2D: generated mipmaps");
}

} // namespace phi
