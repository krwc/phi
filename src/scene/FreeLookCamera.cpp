#include "FreeLookCamera.h"

namespace phi {
using namespace glm;

static const vec3 CAMERA_UP = { 0, 1, 0 };
static const vec3 CAMERA_Z  = { 0, 0,-1 };

FreeLookCamera::FreeLookCamera(const vec3 &position,
                               float aspect_ratio,
                               float fov,
                               float near,
                               float far)
        : m_needs_view_update(true),
          m_needs_proj_update(true),
          m_aspect_ratio(aspect_ratio),
          m_fov(fov),
          m_near(near),
          m_far(far),
          m_position(position) {}

void FreeLookCamera::SetAspectRatio(float aspect_ratio) {
    m_needs_proj_update = true;
    m_aspect_ratio = aspect_ratio;
}

void FreeLookCamera::SetNear(float near) {
    m_needs_proj_update = true;
    m_near = near;
}

void FreeLookCamera::SetFar(float far) {
    m_needs_proj_update = true;
    m_far = far;
}

void FreeLookCamera::SetFov(float degrees) {
    m_needs_proj_update = true;
    m_fov = degrees;
}

void FreeLookCamera::Move(const vec3 &translation) {
    m_needs_view_update = true;
    m_position += inverse(m_rotx * m_roty) * translation;
}

void FreeLookCamera::RotateX(float degrees) {
    m_needs_view_update = true;
    m_rotx = rotate(quat(), radians(degrees), cross(CAMERA_Z, CAMERA_UP)) * m_rotx;
}

void FreeLookCamera::RotateY(float degrees) {
    m_needs_view_update = true;
    m_roty = rotate(quat(), radians(degrees), CAMERA_UP) * m_roty;
}

const mat4 &FreeLookCamera::GetViewMatrix() const {
    if (m_needs_view_update) {
        m_needs_view_update = false;
        mat4 T = translate(mat4(), -m_position);
        mat4 R = mat4_cast(m_rotx * m_roty);
        m_view = R * T;
    }
    return m_view;
}

const mat4 &FreeLookCamera::GetProjMatrix() const {
    if (m_needs_proj_update) {
        m_needs_proj_update = false;
        m_proj = perspective(m_fov, m_aspect_ratio, m_near, m_far);
    }
    return m_proj;
}

} // namespace phi
