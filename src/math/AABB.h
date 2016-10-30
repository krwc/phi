#ifndef PHI_MATH_AABB_H
#define PHI_MATH_AABB_H
#include "Math.h"

namespace phi {

class AABB {
    glm::vec3 m_min;
    glm::vec3 m_max;

public:
    enum class Vertex {
        MinMinMin = 0,
        MinMinMax = 1,
        MinMaxMin = 2,
        MinMaxMax = 3,
        MaxMinMin = 4,
        MaxMinMax = 5,
        MaxMaxMin = 6,
        MaxMaxMax = 7
    };
    AABB(const glm::vec3 &min, const glm::vec3 &max);
    AABB();

    /** Resizes this AABB to cover point @p point. */
    void Cover(const glm::vec3 &point);

    /** Resizes this AABB to cover passed @p AABB. */
    void Cover(const phi::AABB &aabb);

    glm::vec3 GetCenter() const;

    const glm::vec3 &GetMin() const {
        return m_min;
    }
    const glm::vec3 &GetMax() const {
        return m_max;
    }
    glm::vec3 GetVertex(AABB::Vertex vertex) const;
};

phi::AABB operator*(const glm::mat4 &transformation, const phi::AABB &box);

} // namespace phi

#endif
