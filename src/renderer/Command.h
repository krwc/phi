#ifndef PHI_RENDERER_COMMAND_H
#define PHI_RENDERER_COMMAND_H
#include "device/Prototypes.h"

namespace phi {
class Material;
class Layout;
class Buffer;

enum class PrimitiveType : unsigned {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
    Lines = GL_LINES,
    Invalid
};

struct Command {
    PrimitiveType primitive;
    phi::Material *material;
    phi::Layout *layout;
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
