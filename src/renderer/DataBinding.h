#ifndef PHI_RENDERER_DATA_BINDING_H
#define PHI_RENDERER_DATA_BINDING_H
#include <vector>
#include <string>

namespace phi {
class Texture;
class Program;

struct TextureBinding {
    /** Sampler name used in the shader to access this texture */
    const char *name;
    /** Texture object to be bound */
    const phi::Texture *texture;
};

struct ProgramConstant {
    /** Name of the constant in the shader */
    const char *name;
    /** Pointer to the value of the constant */
    const void *value;
};

struct ProgramBinding {
    /** Program object to be bound */
    phi::Program *program;
    /** Constant specification. */
    std::vector<phi::ProgramConstant> constants;
};

} // namespace phi

#endif
