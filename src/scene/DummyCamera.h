#ifndef PHI_SCENE_DUMMY_CAMERA_H
#define PHI_SCENE_DUMMY_CAMERA_H

#include "scene/Camera.h"

namespace phi {

class DummyCamera : public Camera {
    glm::mat4 m_view;
    glm::mat4 m_proj;

public:
    virtual const glm::mat4 &GetViewMatrix() const {
        return m_view;
    }

    virtual const glm::mat4 &GetProjMatrix() const {
        return m_proj;
    }
};

} // namespace phi

#endif
