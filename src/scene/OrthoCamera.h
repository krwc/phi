#ifndef PHI_SCENE_ORTHO_CAMERA_H
#define PHI_SCENE_ORTHO_CAMERA_H

#include "scene/Camera.h"

namespace phi {

class OrthoCamera : public Camera {
    glm::mat4 m_proj;
    glm::mat4 m_view;

public:
    /**
     * Orthograpic projection camera.
     * @param view          View matrix of this camera (containing rotations /
     *                      translations)
     * @param extents_min   Min vertex of viewing volume.
     * @param extents_max   Max vertex of viewing volume.
     */
    OrthoCamera(const glm::mat4 &view = glm::mat4(1.0f),
                const glm::vec3 &extents_min = {-1,-1,-1},
                const glm::vec3 &extents_max = {1,1,1});

    virtual const glm::mat4 &GetViewMatrix() const {
        return m_view;
    }
    virtual const glm::mat4 &GetProjMatrix() const {
        return m_proj;
    }
};

} // namespace

#endif
