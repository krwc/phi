#ifndef PHI_RENDERER_COMMON_H
#define PHI_RENDERER_COMMON_H
#include "device/Buffer.h"
#include "device/Layout.h"
#include "device/Sampler.h"

namespace phi {

struct Common {
    static const phi::Buffer &QuadVbo();
    static const phi::Layout &QuadLayout();
};

template<phi::WrapMode mode>
struct Samplers {
    static const phi::Sampler &Shadow2D();
    static const phi::Sampler &Nearest2D();
    static const phi::Sampler &Bilinear2D();
    static const phi::Sampler &Trilinear2D();
};
template struct Samplers<phi::WrapMode::Repeat>;
template struct Samplers<phi::WrapMode::ClampToEdge>;
template struct Samplers<phi::WrapMode::ClampToBorder>;

} // namespace phi

#endif
