#include "Transformable.h"
#include <cstdio>
namespace phi {
using namespace glm;

Transformable::Transformable() : m_diagonal(1, 1, 1) {}

const mat4 &Transformable::GetTransform() const {
    return m_transform;
}

vec3 Transformable::GetPosition() const {
    return vec3(m_transform[3]);
}

mat3 Transformable::GetRotation() const {
    return mat3(m_transform);
}

void Transformable::SetScale(const vec3 &s) {
    m_transform[0][0] = m_diagonal[0] * s[0];
    m_transform[1][1] = m_diagonal[1] * s[1];
    m_transform[2][2] = m_diagonal[1] * s[2];
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
    mat3 R = mat3_cast(x*y*z);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == j) {
                m_diagonal[i] = R[i][j];
            }
            m_transform[i][j] = R[i][j];
        }
    }
}

} // namespace phi
