#ifndef PHI_RENDERER_MATERIALS_BASIC_MATERIAL_H
#define PHI_RENDERER_MATERIALS_BASIC_MATERIAL_H
#include <memory>

#include "math/Math.h"

#include "renderer/DataBinding.h"
#include "renderer/Material.h"

namespace phi {
class Renderer;

class BasicMaterial : public phi::Material {
    glm::vec4 m_specular;
    glm::vec4 m_diffuse;
    std::shared_ptr<Program> m_program;

public:
    BasicMaterial();

    void SetSpecular(const glm::vec4 &specular);
    void SetDiffuse(const glm::vec4 &diffuse);

    virtual void
    OnPrepareTextureBindings(std::vector<phi::TextureBinding> &) const;
    virtual void OnPrepareProgramBinding(phi::ProgramBinding &binding) const;

    virtual phi::MaterialId GetId() const {
        return phi::MaterialId::Basic;
    }

    virtual std::string GetName() const {
        return "mat_basic";
    }
};

} // namespace phi

#endif
