#include "Light.h"

namespace phi {
using namespace glm;

Light::Light(const vec3 &position,
             const vec3 &color,
             bool casting_shadows)
        : m_position(position),
          m_color(color),
          m_casting_shadows(casting_shadows) {}

Light::Light() : Light({0,0,0}, {1,1,1}, false) {}

void Light::SetColor(const glm::vec3 &color) {
    m_color = color;
}

void Light::SetPosition(const glm::vec3 &position) {
    m_position = position;
}

const glm::vec3 &Light::GetColor() const {
    return m_color;
}

const glm::vec3 &Light::GetPosition() const {
    return m_position;
}

void Light::SetShadowCasting(bool enabled) {
    m_casting_shadows = enabled;
}

bool Light::IsCastingShadows() const {
    return m_casting_shadows;
}

PointLight::PointLight()
        : m_attenuation_constant(1.0f),
          m_attenuation_linear(0.1f),
          m_attenuation_quadratic(0.03f) {}

} // namespace phi

