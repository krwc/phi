#include "OrthoCamera.h"

namespace phi {

OrthoCamera::OrthoCamera(const glm::mat4 &view,
                         const glm::vec3 &extents_min,
                         const glm::vec3 &extents_max)
        : m_view(view) {
    m_proj = glm::ortho(extents_min.x, extents_max.x,
                        extents_min.y, extents_max.y,
                        extents_min.z, extents_max.z);
}

} // namespace phi
