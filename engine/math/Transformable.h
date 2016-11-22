#ifndef PHI_MATH_TRANSFORMABLE_H
#define PHI_MATH_TRANSFORMABLE_H
#include "Math.h"

namespace phi {

class Transformable {
    mutable glm::mat4 m_transform;
    mutable bool m_needs_update;
    glm::vec3 m_rotation_angles;
    glm::vec3 m_scale;
    glm::vec3 m_position;
    glm::mat3 m_rotation;

public:
    Transformable();
    virtual ~Transformable() {}
    virtual const glm::mat4 &GetTransform() const;
    virtual const glm::mat3 &GetRotation() const;
    virtual const glm::vec3 &GetRotationAngles() const;
    virtual const glm::vec3 &GetScale() const;
    virtual const glm::vec3 &GetPosition() const;
    virtual void SetScale(const glm::vec3 &s);
    virtual void SetPosition(const glm::vec3 &p);
    virtual void SetRotation(const glm::vec3 &degrees);
};

} // namespace phi

#endif
