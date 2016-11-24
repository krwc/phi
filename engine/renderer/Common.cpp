#include "Common.h"

#include "math/Math.h"

namespace phi {

const phi::Buffer &Common::QuadVbo() {
    static const glm::vec2 quad[] = { { -1, -1 }, { 1, -1 }, { -1, 1 },
                                      { -1, 1 },  { 1, -1 }, { 1, 1 } };
    static const phi::Buffer vbo(phi::BufferType::Vertex,
                                 phi::BufferHint::Static, quad, sizeof(quad));
    return vbo;
}

const phi::Layout &Common::QuadLayout() {
    static const phi::Layout layout(sizeof(glm::vec2), {
        { phi::Layout::Position, phi::Layout::Float2, 0u }
    });
    return layout;
}

template <phi::WrapMode mode>
const phi::Sampler &Samplers<mode>::Shadow2D() {
    static const phi::Sampler sampler(phi::Sampler::Config2D(
            mode, mode,
            phi::Filter::Linear,
            phi::Filter::Linear,
            phi::CompareMode::RefToTexture,
            phi::CompareFunc::Gequal));
    return sampler;
}

template <phi::WrapMode mode>
const phi::Sampler &Samplers<mode>::Nearest2D() {
    static const phi::Sampler sampler(phi::Sampler::Config2D(
            mode, mode,
            phi::Filter::Nearest,
            phi::Filter::Nearest,
            phi::CompareMode::None));
    return sampler;
}

template <phi::WrapMode mode>
const phi::Sampler &Samplers<mode>::Bilinear2D() {
    static const phi::Sampler sampler(phi::Sampler::Config2D(
            mode, mode,
            phi::Filter::Linear,
            phi::Filter::Linear,
            phi::CompareMode::None));
    return sampler;
}

template <phi::WrapMode mode>
const phi::Sampler &Samplers<mode>::Trilinear2D() {
    static const phi::Sampler sampler(phi::Sampler::Config2D(
            mode, mode,
            phi::Filter::BilinearMipmap,
            phi::Filter::Linear,
            phi::CompareMode::None));
    return sampler;
}

} // namespace phi
