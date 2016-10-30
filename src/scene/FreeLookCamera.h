#ifndef PHI_SCENE_FREELOOK_CAMERA_H
#define PHI_SCENE_FREELOOK_CAMERA_H

#include "scene/Camera.h"

namespace phi {

class FreeLookCamera : public Camera {
    mutable bool m_needs_view_update;
    mutable bool m_needs_proj_update;
    mutable glm::mat4 m_view;
    mutable glm::mat4 m_proj;
    float m_aspect_ratio;
    float m_fov;
    float m_near;
    float m_far;
    glm::vec3 m_position;
    glm::quat m_rotx;
    glm::quat m_roty;

public:
    FreeLookCamera(const glm::vec3 &position = { 0, 0, 0 },
                   float aspect_ratio = 4.0f / 3.0f,
                   float fov = 70.0f,
                   float near = 0.1f,
                   float far = 1000.0f);

    void SetAspectRatio(float aspect_ratio);
    void SetNear(float near);
    void SetFar(float far);
    void SetFov(float degrees);
    /** Moves camera relatively to its rotation */
    void Move(const glm::vec3 &translation);
    /** Rotates around X axis */
    void RotateX(float degrees);
    /** Rotates around Y axis */
    void RotateY(float degrees);

    const glm::vec3 &GetPosition() const {
        return m_position;
    }

    float GetAspectRatio() const {
        return m_aspect_ratio;
    }

    float GetNear() const {
        return m_near;
    }

    float GetFar() const {
        return m_far;
    }

    float GetFov() const {
        return m_fov;
    }

    virtual const glm::mat4 &GetViewMatrix() const;
    virtual const glm::mat4 &GetProjMatrix() const;
};

} // namespace phi

#endif
