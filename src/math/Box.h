#ifndef PHI_MATH_BOX_H
#define PHI_MATH_BOX_H
#include "Math.h"

namespace phi {

class Box {
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
    Box(const glm::vec3 &min, const glm::vec3 &max);
    Box();

    /** Resizes this box to cover point @p point. */
    void Cover(const glm::vec3 &point);

    /** Resizes this box to cover passed @p box. */
    void Cover(const phi::Box &box);

    glm::vec3 GetCenter() const;

    const glm::vec3 &GetMin() const {
        return m_min;
    }
    const glm::vec3 &GetMax() const {
        return m_max;
    }
    glm::vec3 GetVertex(Box::Vertex vertex) const;
};

phi::Box operator*(const glm::mat4 &transformation, const Box &box);

} // namespace phi

#endif
