#ifndef PHI_SCENE_ORTHO_CAMERA_H
#define PHI_SCENE_ORTHO_CAMERA_H

#include "scene/Camera.h"

#include "math/AABB.h"

namespace phi {

class OrthoCamera : public Camera {
    glm::mat4 m_proj;
    glm::mat4 m_view;

public:
    /**
     * Orthograpic projection camera.
     *
     * @param position  Position of the camera.
     * @param direction Direction where it is looking.
     * @param extents   Viewing frustum extents (ignoring rotation, as it will
     *                  be applied automatically according to @p direction)
     *                  as AABB.
     */
    OrthoCamera(const glm::vec3 &position,
                const glm::vec3 &direction,
                const phi::AABB &extents);

    virtual const glm::mat4 &GetViewMatrix() const {
        return m_view;
    }
    virtual const glm::mat4 &GetProjMatrix() const {
        return m_proj;
    }
};

} // namespace

#endif
