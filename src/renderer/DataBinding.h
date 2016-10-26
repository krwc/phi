#ifndef PHI_RENDERER_DATA_BINDING_H
#define PHI_RENDERER_DATA_BINDING_H
#include <vector>
#include <string>

namespace phi {
class Texture;
class Program;
class Sampler;

struct TextureBinding {
    /** Sampler name used in the shader to access this texture */
    const char *name;
    /** Texture object to be bound */
    const phi::Texture *texture;
    /** Sampler object used to sample texture */
    const phi::Sampler *sampler;
};

struct ProgramConstant {
    /** Name of the constant in the shader */
    const char *name;
    /** Pointer to the value of the constant */
    const void *value;
};

} // namespace phi

#endif
