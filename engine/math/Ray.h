#ifndef PHI_MATH_RAY_H
#define PHI_MATH_RAY_H
#include "math/Math.h"
#include "math/AABB.h"

namespace phi {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    /**
     * Tests whether the ray hits given @p aabb
     *
     * @param aabb      AABB to test interesection against.
     * @param out_near  Distance to the first (in) interesection point.
     * @param out_far   Distacen to the second (out) intersection point.
     *
     * @return true if hit occurs, false otherwise
     */
    bool Hits(const phi::AABB &aabb,
              double *out_near = nullptr,
              double *out_far = nullptr) const;

    /**
     * Tests whether the ray hits triangle v0-v1-v2.
     *
     * @param out_near  Distance to the hit.
     * @return true if hit occurs, false otherwise
     */
    bool Hits(const glm::vec3 &v0,
              const glm::vec3 &v1,
              const glm::vec3 &v2,
              double *out_near = nullptr) const;
};

} // namespace phi

#endif // PHI_MATH_RAY_H
