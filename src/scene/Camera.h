#ifndef PHI_SCENE_CAMERA_H
#define PHI_SCENE_CAMERA_H

#include "math/Math.h"

namespace phi {

class Camera {
public:
    virtual ~Camera() {}
    virtual const glm::mat4 &GetViewMatrix() const = 0;
};

} // namespace phi

#endif
