#ifndef PHI_DEVICE_SAMPLER_H
#define PHI_DEVICE_SAMPLER_H
#include "Prototypes.h"

#include "Texture.h"

namespace phi {

enum class WrapMode {
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    Repeat = GL_REPEAT
};

enum class Filter {
    Linear = GL_LINEAR,
    Nearest = GL_NEAREST,
    BilinearMipmap = GL_LINEAR_MIPMAP_LINEAR
};

enum class CompareFunc {
    Less = GL_LESS,
    Lequal = GL_LEQUAL,
    Greater = GL_GREATER,
    Gequal = GL_GEQUAL
};

enum class CompareMode {
    RefToTexture = GL_COMPARE_REF_TO_TEXTURE,
    None = GL_NONE
};

class Sampler : public Resource {
    GLuint m_id;

public:
    /**
     * Configuration structure used to create sampler for a 2 dimensional
     * arbitrary texture object.
     */
    struct Config2D {
        WrapMode wrap_u;
        WrapMode wrap_v;
        Filter min_filter;
        Filter mag_filter;
        CompareFunc compare_func;
        CompareMode compare_mode;
        float anisotropy;

        Config2D(WrapMode wrap_u = WrapMode::ClampToEdge,
                 WrapMode wrap_v = WrapMode::ClampToEdge,
                 Filter min_filter = Filter::Nearest,
                 Filter mag_filter = Filter::Nearest,
                 CompareMode compare_mode = CompareMode::None,
                 CompareFunc compare_func = CompareFunc::Less,
                 float anisotropy = 1.0f)
                : wrap_u(wrap_u),
                  wrap_v(wrap_v),
                  min_filter(min_filter),
                  mag_filter(mag_filter),
                  compare_func(compare_func),
                  compare_mode(compare_mode),
                  anisotropy(anisotropy) {}
    };

    /**
     * Creates sampler for a two dimensional texture object.
     *
     * @param config    Sampler configuration.
     */
    Sampler(const phi::Sampler::Config2D &config);
    ~Sampler();

    virtual GLuint GetId() const {
        return m_id;
    }
};

} // namespace phi

#endif
