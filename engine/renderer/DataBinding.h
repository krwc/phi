#ifndef PHI_RENDERER_DATA_BINDING_H
#define PHI_RENDERER_DATA_BINDING_H
#include <vector>
#include <string>

namespace phi {
class Texture;
class Program;
class Sampler;

struct TextureBinding {
    /** Texture unit to which texture should be bound */
    const int unit;
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

    ProgramConstant(const char *name, const void *value)
            : name(name), value(value) {}
};

} // namespace phi

#endif
