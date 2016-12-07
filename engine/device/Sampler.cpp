#include "Sampler.h"
#include "Utils.h"

#include <stdexcept>

namespace phi {

Sampler::Sampler(const phi::Sampler::Config2D &config) : m_id(GL_NONE) {
    CheckedCall(phi::glCreateSamplers, 1, &m_id);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_MIN_FILTER,
                (GLenum) config.min_filter);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_MAG_FILTER,
                (GLenum) config.mag_filter);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_WRAP_S,
                (GLenum) config.wrap_u);
    CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_WRAP_T,
                (GLenum) config.wrap_v);

    // Shadow sampler specific parameters
    if (config.compare_mode != phi::CompareMode::None) {
        CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_COMPARE_MODE,
                    (GLenum) config.compare_mode);
        CheckedCall(phi::glSamplerParameteri, m_id, GL_TEXTURE_COMPARE_FUNC,
                    (GLenum) config.compare_func);
    }
    if (config.anisotropy > 1.0f) {
        CheckedCall(phi::glSamplerParameterf, m_id, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    config.anisotropy);
    }
    PHI_LOG(TRACE, "Created sampler object (ID=%u)", m_id);
}

Sampler::~Sampler() {
    CheckedCall(phi::glDeleteSamplers, 1, &m_id);
    PHI_LOG(TRACE, "Deleted sampler object (ID=%u)", m_id);
}


} // namespace phi
