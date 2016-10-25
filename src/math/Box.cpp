#include "Box.h"

#include <numeric>
#include <cassert>
#include <stdexcept>

namespace phi {
using namespace std;
using namespace glm;

Box::Box(const vec3 &min, const vec3 &max) : m_min(min), m_max(max) {}

const float MAX = numeric_limits<float>::max();
const float MIN = -MAX;

Box::Box()
        : Box({ MAX, MAX, MAX }, { MIN, MIN, MIN }) {}

void Box::Cover(const vec3& point) {
    m_min.x = std::min(m_min.x, point.x);
    m_min.y = std::min(m_min.y, point.y);
    m_min.z = std::min(m_min.z, point.z);

    m_max.x = std::max(m_max.x, point.x);
    m_max.y = std::max(m_max.y, point.y);
    m_max.z = std::max(m_max.z, point.z);
}

void Box::Cover(const Box& box) {
    Cover(box.GetMin());
    Cover(box.GetMax());
}

vec3 Box::GetCenter() const {
    return 0.5f * (m_min + m_max);
}

vec3 Box::GetVertex(Box::Vertex vertex) const {
    switch (vertex) {
    case Box::Vertex::MinMinMin:
        return m_min;
    case Box::Vertex::MaxMaxMax:
        return m_max;
    case Box::Vertex::MinMinMax:
        return vec3(m_min.x, m_min.y, m_max.z);
    case Box::Vertex::MinMaxMin:
        return vec3(m_min.x, m_max.y, m_min.z);
    case Box::Vertex::MinMaxMax:
        return vec3(m_min.x, m_max.y, m_max.z);
    case Box::Vertex::MaxMinMin:
        return vec3(m_max.x, m_min.y, m_min.z);
    case Box::Vertex::MaxMinMax:
        return vec3(m_max.x, m_min.y, m_max.z);
    case Box::Vertex::MaxMaxMin:
        return vec3(m_max.x, m_max.y, m_min.z);
    default:
        throw runtime_error("Invalid vertex");
    }
}

Box operator*(const mat4 &T, const Box &box) {
    Box result{};
    for (int i = 0; i < 8; ++i) {
        auto v = vec3(T * vec4(box.GetVertex(static_cast<Box::Vertex>(i)), 1));
        result.Cover(v);
    }
    return result;
}

} // namespace phi
