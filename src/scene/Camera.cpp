#include "Camera.h"

namespace phi {
using namespace glm;

static const vec3 CAMERA_UP = { 0, 1, 0 };
static const vec3 CAMERA_Z  = { 0, 0,-1 };

FreeLookCamera::FreeLookCamera(const vec3 &position)
        : m_needs_update(true), m_position(position) {}

void FreeLookCamera::Move(const vec3 &translation) {
    m_needs_update = true;
    m_position += inverse(m_rotx * m_roty) * translation;
}

void FreeLookCamera::RotateX(float degrees) {
    m_needs_update = true;
    m_rotx = rotate(quat(), radians(degrees), cross(CAMERA_Z, CAMERA_UP)) * m_rotx;
}

void FreeLookCamera::RotateY(float degrees) {
    m_needs_update = true;
    m_roty = rotate(quat(), radians(degrees), CAMERA_UP) * m_roty;
}

const mat4 &FreeLookCamera::GetViewMatrix() const {
    if (m_needs_update) {
        m_needs_update = false;
        mat4 T = translate(mat4(), -m_position);
        mat4 R = mat4_cast(m_rotx * m_roty);
        m_view = R * T;
    }
    return m_view;
}

} // namespace phi
