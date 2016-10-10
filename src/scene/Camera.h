#ifndef PHI_SCENE_CAMERA_H
#define PHI_SCENE_CAMERA_H

#include "math/Math.h"

namespace phi {

class Camera {
public:
    virtual ~Camera() {}
    virtual const glm::mat4 &GetViewMatrix() const = 0;
};

class FreeLookCamera : public Camera {
    mutable bool m_needs_update;
    mutable glm::mat4 m_view;
    glm::vec3 m_position;
    glm::quat m_rotx;
    glm::quat m_roty;

public:
    FreeLookCamera(const glm::vec3 &position = { 0, 0, 0 });
    /** Moves camera relatively to its rotation */
    void Move(const glm::vec3 &translation);
    /** Rotates around X axis */
    void RotateX(float degrees);
    /** Rotates around Y axis */
    void RotateY(float degrees);

    const glm::vec3 &GetPosition() const {
        return m_position;
    }

    virtual const glm::mat4 &GetViewMatrix() const;
};

} // namespace phi

#endif
