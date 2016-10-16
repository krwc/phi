#ifndef PHI_DEVICE_RESOURCE_H
#define PHI_DEVICE_RESOURCE_H
#include "Prototypes.h"

namespace phi {

struct Resource {
    virtual GLuint GetId() const = 0;
};

} // namespace phi

#endif
