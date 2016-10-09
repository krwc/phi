#ifndef PHI_RENDERER_MATERIALS_BASIC_MATERIAL_H
#define PHI_RENDERER_MATERIALS_BASIC_MATERIAL_H
#include "math/Math.h"

#include "renderer/Material.h"

namespace phi {

class BasicMaterial : public phi::Material {
    glm::vec4 m_specular;
    glm::vec4 m_diffuse;
    std::shared_ptr<Program> m_program;

public:
    BasicMaterial();

    void SetSpecular(const glm::vec4 &specular);
    void SetDiffuse(const glm::vec4 &diffuse);
    virtual void PrepareForRendering(phi::Renderer &) const;

    virtual phi::MaterialId GetId() const {
        return phi::MaterialId::Basic;
    }

    virtual std::string GetName() const {
        return "mat_basic";
    }
};

} // namespace phi

#endif
