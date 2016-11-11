#ifndef PHI_DEVICE_PRIMITIVE_H
#define PHI_DEVICE_PRIMITIVE_H

#include "Prototypes.h"

namespace phi {

enum class Primitive {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Invalid = GL_NONE
};

} // namespace phi

#endif
