#include "Sampler.h"
#include "Utils.h"

#include <stdexcept>

namespace phi {

Sampler::Sampler(GLenum min_filter,
                 GLenum mag_filter,
                 phi::WrapMode mode,
                 phi::TextureType type)
        : m_id(GL_NONE) {
    CheckedCall(phi::glCreateSamplers, 1, &m_id);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_MIN_FILTER, min_filter);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_MAG_FILTER, mag_filter);
    if (type == phi::TextureType::Texture2D) {
        CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_WRAP_S, (GLenum) mode);
        CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_WRAP_T, (GLenum) mode);
    } else {
        throw std::runtime_error("Invalid texture type");
    }
}

Sampler::~Sampler() {
    CheckedCall(phi::glDeleteSamplers, 1, &m_id);
}

const phi::Sampler *Sampler::Nearest2D(phi::WrapMode mode) {
    static phi::Sampler clamping =
            Sampler(GL_NEAREST, GL_NEAREST, WrapMode::Clamp,
                    TextureType::Texture2D);
    static phi::Sampler repeating =
            Sampler(GL_NEAREST, GL_NEAREST, WrapMode::Repeat,
                    TextureType::Texture2D);
    if (mode == WrapMode::Clamp) {
        return &clamping;
    }
    return &repeating;
}

const phi::Sampler *Sampler::Bilinear2D(phi::WrapMode mode) {
    static phi::Sampler clamping =
            Sampler(GL_LINEAR, GL_LINEAR, WrapMode::Clamp,
                    TextureType::Texture2D);
    static phi::Sampler repeating =
            Sampler(GL_LINEAR, GL_LINEAR, WrapMode::Repeat,
                    TextureType::Texture2D);
    if (mode == WrapMode::Clamp) {
        return &clamping;
    }
    return &repeating;
}

const phi::Sampler *Sampler::Trilinear2D(phi::WrapMode mode) {
    static phi::Sampler clamping =
            Sampler(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, WrapMode::Clamp,
                    TextureType::Texture2D);
    static phi::Sampler repeating =
            Sampler(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, WrapMode::Repeat,
                    TextureType::Texture2D);
    if (mode == WrapMode::Clamp) {
        return &clamping;
    }
    return &repeating;
}

} // namespace phi
