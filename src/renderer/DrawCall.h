#ifndef PHI_RENDERER_DRAW_CALL_H
#define PHI_RENDERER_DRAW_CALL_H
#include <vector>

#include "DataBinding.h"

#include "device/Prototypes.h"

#include "math/Math.h"

namespace phi {
class Material;
class Layout;
class Buffer;
class PointLight;
class DirLight;

enum class Primitive {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Invalid = GL_NONE
};

struct DrawCall {
    phi::Primitive primitive;
    glm::mat4 transform;
    phi::Program *program;
    const phi::Layout *layout;
    const phi::Buffer *vbo;
    const phi::Buffer *ibo;
    std::vector<phi::ProgramConstant> program_constants;
    std::vector<phi::TextureBinding> texture_bindings;
    std::vector<phi::PointLight *> point_lights;
    std::vector<phi::DirLight *> dir_lights;
    /* Number of primitives to draw. */
    int count;
    /* Byte offset in buffer where geometry starts. */
    int offset;

    /** Creates an invalid draw call that needs to be filled by the user. */
    DrawCall();
};

} // namespace phi

#endif
