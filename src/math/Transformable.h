#ifndef PHI_MATH_TRANSFORMABLE_H
#define PHI_MATH_TRANSFORMABLE_H
#include "Math.h"

namespace phi {

class Transformable {
    glm::mat4 m_transform;
    glm::vec3 m_diagonal;
    bool m_scale_changed;

public:
    Transformable();
    virtual ~Transformable() {}
    virtual const glm::mat4 &GetTransform() const;
    virtual glm::mat3 GetRotation() const;
    virtual glm::vec3 GetPosition() const;
    virtual void SetScale(const glm::vec3 &s);
    virtual void SetPosition(const glm::vec3 &p);
    virtual void SetRotation(const glm::vec3 &degrees);
};

} // namespace phi

#endif
