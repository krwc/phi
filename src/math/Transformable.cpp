#include "Transformable.h"
#include <cstdio>
namespace phi {
using namespace glm;

Transformable::Transformable() {}

const mat4 &Transformable::GetTransform() const {
    if (m_needs_update) {
        m_needs_update = false;
        mat3 SR = m_rotation * m_scale;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m_transform[i][j] = SR[i][j];
            }
        }
    }
    return m_transform;
}

vec3 Transformable::GetPosition() const {
    return vec3(m_transform[3]);
}

const mat3 &Transformable::GetRotation() const {
    return m_rotation;
}

void Transformable::SetScale(const vec3 &s) {
    m_scale = mat3(scale(mat4(), s));
    m_needs_update = true;
}

void Transformable::SetPosition(const vec3 &p) {
    m_transform[3][0] = p[0];
    m_transform[3][1] = p[1];
    m_transform[3][2] = p[2];
}

void Transformable::SetRotation(const vec3 &degrees) {
    quat x = rotate(quat(), radians(degrees.x), { 1, 0, 0 });
    quat y = rotate(quat(), radians(degrees.y), { 0, 1, 0 });
    quat z = rotate(quat(), radians(degrees.z), { 0, 0,-1 });
    m_rotation = mat3_cast(x*y*z);
    m_needs_update = true;
}

} // namespace phi
