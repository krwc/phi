#ifndef PHI_RENDERER_RESOURCE_H
#define PHI_RENDERER_RESOURCE_H
#include <GL/gl.h>

namespace phi {

struct Resource {
    virtual GLuint GetId() const = 0;
};

} // namespace phi

#endif
