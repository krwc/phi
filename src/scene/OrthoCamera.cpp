#include "OrthoCamera.h"

namespace phi {

OrthoCamera::OrthoCamera(const glm::vec3 &position,
                         const glm::vec3 &direction,
                         const phi::AABB &extents) {
    const glm::vec3 &extents_min = extents.GetMin();
    const glm::vec3 &extents_max = extents.GetMax();
    m_proj = glm::ortho(extents_min.x, extents_max.x,
                        extents_min.y, extents_max.y,
                        extents_min.z, extents_max.z);
    m_view = glm::translate(glm::lookAtRH({0,0,0}, direction, {0,1,0}), -position);
}

} // namespace phi
