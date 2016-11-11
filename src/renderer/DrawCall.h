#ifndef PHI_RENDERER_DRAW_CALL_H
#define PHI_RENDERER_DRAW_CALL_H
#include <vector>

#include "DataBinding.h"

#include "device/Primitive.h"
#include "device/Prototypes.h"

#include "math/Math.h"

namespace phi {
class Material;
class Layout;
class Buffer;

struct DrawCall {
    phi::Primitive primitive;
    glm::mat4 transform;
    phi::Program *program;
    const phi::Layout *layout;
    const phi::Buffer *vbo;
    const phi::Buffer *ibo;
    std::vector<phi::ProgramConstant> program_constants;
    std::vector<phi::TextureBinding> texture_bindings;
    /* Number of primitives to draw. */
    int count;
    /* Byte offset in buffer where geometry starts. */
    int offset;

    /** Creates an invalid draw call that needs to be filled by the user. */
    DrawCall();
};

} // namespace phi

#endif
