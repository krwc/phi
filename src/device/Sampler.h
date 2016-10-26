#ifndef PHI_DEVICE_SAMPLER_H
#define PHI_DEVICE_SAMPLER_H
#include "Prototypes.h"

#include "Texture.h"

namespace phi {

enum class WrapMode {
    Clamp = GL_CLAMP_TO_EDGE,
    Repeat = GL_REPEAT
};

class Sampler : public Resource {
    GLuint m_bind;

    Sampler(GLenum min_filter,
            GLenum mag_filter,
            phi::WrapMode mode,
            phi::TextureType type);
    ~Sampler();

public:
    virtual GLuint GetId() const {
        return m_bind;
    }

    static const phi::Sampler *Nearest2D(phi::WrapMode mode);
    static const phi::Sampler *Bilinear2D(phi::WrapMode mode);
    static const phi::Sampler *Trilinear2D(phi::WrapMode mode);
};

} // namespace phi

#endif
