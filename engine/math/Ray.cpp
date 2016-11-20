#include "Ray.h"
#include <limits>

namespace phi {

bool Ray::Hits(const phi::AABB &aabb, double *out_near, double *out_far) const {
    double tmin = std::numeric_limits<double>::min();
    double tmax = std::numeric_limits<double>::max();
    const glm::vec3 min = aabb.GetMin();
    const glm::vec3 max = aabb.GetMax();
    for (int i = 0; i < 3; ++i) {
        if (std::abs(direction[i]) > 1e-5) {
            double t1 = ((double) min[i] - origin[i]) / direction[i];
            double t2 = ((double) max[i] - origin[i]) / direction[i];
            tmin = glm::max(tmin, glm::min(t1, t2));
            tmax = glm::min(tmax, glm::max(t1, t2));
        }
    }

    if (tmax >= tmin) {
        if (out_near) {
            *out_near = tmin;
        }
        if (out_far) {
            *out_far = tmax;
        }
        return true;
    }
    return false;
}

bool Ray::Hits(const glm::vec3 &v0,
               const glm::vec3 &v1,
               const glm::vec3 &v2,
               double *out_near) const {
    static const double epsilon = 1e-5;
    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;
    glm::vec3 P, Q, T;
    const glm::vec3 &D = direction;
    const glm::vec3 &O = origin;
    float det, inv_det, u, v, t;

    P = glm::cross(D, e2);
    det = glm::dot(e1, P);

    if (fabs(det) < epsilon) {
        return false;
    }

    inv_det = 1.0f / det;
    T = O - v0;
    u = glm::dot(T, P) * inv_det;

    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Q = glm::cross(T, e1);
    v = glm::dot(D, Q) * inv_det;

    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    t = glm::dot(e2, Q) * inv_det;

    if (t > epsilon) {
        if (out_near) {
            *out_near = t;
        }
        return true;
    }
    return false;
}

} // namespace phi
