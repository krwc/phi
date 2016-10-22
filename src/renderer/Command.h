#ifndef PHI_RENDERER_COMMAND_H
#define PHI_RENDERER_COMMAND_H
#include <vector>

#include "device/Prototypes.h"

#include "math/Math.h"

namespace phi {
class Material;
class Layout;
class Buffer;
class PointLight;
class DirectionalLight;

enum class PrimitiveType : unsigned {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
    Lines = GL_LINES,
    Invalid
};

struct Command {
    /** Lights enabled during rendering */
    std::vector<phi::PointLight *> point_lights;
    std::vector<phi::DirectionalLight *> directional_lights;
    /** Geometry primitive used for rendering */
    PrimitiveType primitive;
    /** Model transform. */
    const glm::mat4 *model;
    const phi::Layout *layout;
    phi::Material *material;
    phi::Buffer *vbo;
    phi::Buffer *ibo;
    /** Number of primitives to draw */
    int count;
    /** Byte offset in buffer where geometry starts */
    int offset;

    /** Creates an invalid render command that needs to be filled by the user */
    Command();
};

} // namespace phi

#endif
