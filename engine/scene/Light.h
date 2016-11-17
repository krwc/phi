#ifndef PHI_SCENE_LIGHT_H
#define PHI_SCENE_LIGHT_H
#include "math/Math.h"

namespace phi {

class Light {
    glm::vec3 m_position;
    glm::vec3 m_color;
    bool m_casting_shadows;

public:
    Light(const glm::vec3 &position,
          const glm::vec3 &color,
          bool casting_shadows);
    Light();
    virtual ~Light() {}
    virtual void SetColor(const glm::vec3 &color);
    virtual void SetPosition(const glm::vec3 &position);
    virtual const glm::vec3 &GetColor() const;
    virtual const glm::vec3 &GetPosition() const;
    virtual void SetShadowCasting(bool enabled);
    virtual bool IsCastingShadows() const;
};

class DirLight : public Light {};

class PointLight : public Light {
    float m_attenuation_constant;
    float m_attenuation_linear;
    float m_attenuation_quadratic;

public:
    PointLight();

    void SetConstantAttenuation(float value) {
        m_attenuation_constant = value;
    }
    void SetLinearAttenuation(float value) {
        m_attenuation_linear = value;
    }
    void SetQuadraticAttenuation(float value) {
        m_attenuation_quadratic = value;
    }
    float GetConstantAttenuation() const {
        return m_attenuation_constant;
    }
    float GetLinearAttenuation() const {
        return m_attenuation_linear;
    }
    float GetQuadraticAttenuation() const {
        return m_attenuation_quadratic;
    }
};

} // namespace phi

#endif
