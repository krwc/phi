#include "AABB.h"

#include <numeric>
#include <cassert>
#include <stdexcept>

namespace phi {
using namespace std;
using namespace glm;

AABB::AABB(const vec3 &min, const vec3 &max) : m_min(min), m_max(max) {}

const float MAX = numeric_limits<float>::max();
const float MIN = -MAX;

AABB::AABB()
        : AABB({ MAX, MAX, MAX }, { MIN, MIN, MIN }) {}

void AABB::Cover(const vec3& point) {
    m_min.x = std::min(m_min.x, point.x);
    m_min.y = std::min(m_min.y, point.y);
    m_min.z = std::min(m_min.z, point.z);

    m_max.x = std::max(m_max.x, point.x);
    m_max.y = std::max(m_max.y, point.y);
    m_max.z = std::max(m_max.z, point.z);
}

void AABB::Cover(const AABB& box) {
    Cover(box.GetMin());
    Cover(box.GetMax());
}

vec3 AABB::GetCenter() const {
    return 0.5f * (m_min + m_max);
}

vec3 AABB::GetVertex(AABB::Vertex vertex) const {
    switch (vertex) {
    case AABB::Vertex::MinMinMin:
        return m_min;
    case AABB::Vertex::MaxMaxMax:
        return m_max;
    case AABB::Vertex::MinMinMax:
        return vec3(m_min.x, m_min.y, m_max.z);
    case AABB::Vertex::MinMaxMin:
        return vec3(m_min.x, m_max.y, m_min.z);
    case AABB::Vertex::MinMaxMax:
        return vec3(m_min.x, m_max.y, m_max.z);
    case AABB::Vertex::MaxMinMin:
        return vec3(m_max.x, m_min.y, m_min.z);
    case AABB::Vertex::MaxMinMax:
        return vec3(m_max.x, m_min.y, m_max.z);
    case AABB::Vertex::MaxMaxMin:
        return vec3(m_max.x, m_max.y, m_min.z);
    default:
        throw runtime_error("Invalid vertex");
    }
}

AABB operator*(const mat4 &T, const AABB &box) {
    AABB result{};
    for (int i = 0; i < 8; ++i) {
        auto v = vec3(T * vec4(box.GetVertex(static_cast<AABB::Vertex>(i)), 1));
        result.Cover(v);
    }
    return result;
}

} // namespace phi
