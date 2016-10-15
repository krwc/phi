#ifndef PHI_MATH_BOX_H
#define PHI_MATH_BOX_H
#include "Math.h"

namespace phi {

class Box {
    glm::vec3 m_min;
    glm::vec3 m_max;

public:
    Box(const glm::vec3 &min, const glm::vec3 &max);
    Box();

    /** Resizes this box to cover point @p point. */
    void Cover(const glm::vec3 &point);
    glm::vec3 GetCenter() const;

    const glm::vec3 &GetMin() const {
        return m_min;
    }
    const glm::vec3 &GetMax() const {
        return m_max;
    }
};

} // namespace phi

#endif
