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
class DirectionalLight;

enum class PrimitiveType {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Invalid = GL_NONE
};

struct DrawCall {
    std::vector<phi::PointLight *> point_lights;
    std::vector<phi::DirectionalLight *> directional_lights;
    PrimitiveType primitive;
    const glm::mat4 *model;
    std::vector<phi::TextureBinding> texture_bindings;
    phi::ProgramBinding program_binding;
    const phi::Layout *layout;
    const phi::Buffer *vbo;
    const phi::Buffer *ibo;
    /** Number of primitives to draw */
    int count;
    /** Byte offset in buffer where geometry starts */
    int offset;

    /** Creates an invalid render command that needs to be filled by the user */
    DrawCall();
};

} // namespace phi

#endif
