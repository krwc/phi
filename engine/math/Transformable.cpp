#include "Transformable.h"
#include <cstdio>
namespace phi {
using namespace glm;

Transformable::Transformable()
        : m_rotation_angles(0, 0, 0), m_scale(1, 1, 1), m_position(0, 0, 0) {}

const mat4 &Transformable::GetTransform() const {
    if (m_needs_update) {
        m_needs_update = false;
        mat4 T = translate(mat4(), m_position);
        mat4 R = m_rotation;
        mat4 S = scale(mat4(), m_scale);
        m_transform = T * R * S;
    }
    return m_transform;
}

const vec3 &Transformable::GetPosition() const {
    return m_position;
}

const mat3 &Transformable::GetRotation() const {
    return m_rotation;
}

const vec3 &Transformable::GetRotationAngles() const {
    return m_rotation_angles;
}

const vec3 &Transformable::GetScale() const {
    return m_scale;
}

void Transformable::SetScale(const vec3 &s) {
    m_scale = s;
    m_needs_update = true;
}

void Transformable::SetPosition(const vec3 &p) {
    m_position = p;
    m_needs_update = true;
}

void Transformable::SetRotation(const vec3 &degrees) {
    quat x = inverse(rotate(quat(), radians(degrees.x), { 1, 0, 0 }));
    quat y = inverse(rotate(quat(), radians(degrees.y), { 0, 1, 0 }));
    quat z = inverse(rotate(quat(), radians(degrees.z), { 0, 0,-1 }));
    m_rotation = mat3_cast(x*y*z);
#warning "TODO: this can be done without storing rotation angles"
    m_rotation_angles.x = fmod(degrees.x, 360);
    if (m_rotation_angles.x < 0) {
        m_rotation_angles.x += 360;
    }
    m_rotation_angles.y = fmod(degrees.y, 360);
    if (m_rotation_angles.y < 0) {
        m_rotation_angles.y += 360;
    }
    m_rotation_angles.z = fmod(degrees.z, 360);
    if (m_rotation_angles.z < 0) {
        m_rotation_angles.z += 360;
    }
    m_needs_update = true;
}

} // namespace phi
