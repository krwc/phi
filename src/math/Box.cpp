#include "Box.h"

#include <cassert>

namespace phi {
using namespace glm;

Box::Box(const vec3 &min, const vec3 &max) : m_min(min), m_max(max) {
    assert(m_min.x <= m_max.x && m_min.y <= m_max.y && m_min.z <= m_max.z
               && "min > max");
}

Box::Box() : Box({0,0,0}, {0,0,0}) { }


void Box::Cover(const vec3& point) {
    m_min.x = min(m_min.x, point.x);
    m_min.y = min(m_min.y, point.y);
    m_min.z = min(m_min.z, point.z);

    m_max.x = max(m_max.x, point.x);
    m_max.y = max(m_max.y, point.y);
    m_max.z = max(m_max.z, point.z);
}

vec3 Box::GetCenter() const {
    return 0.5f * (m_min + m_max);
}

} // namespace phi
