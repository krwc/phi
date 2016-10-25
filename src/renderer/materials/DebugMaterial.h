#ifndef PHI_RENDERER_MATERIALS_DEBUG_MATERIAL_H
#define PHI_RENDERER_MATERIALS_DEBUG_MATERIAL_H
#include <memory>

#include "math/Math.h"

#include "renderer/DataBinding.h"
#include "renderer/Material.h"

namespace phi {

class DebugMaterial : public phi::Material {
    glm::vec4 m_color;
    std::shared_ptr<Program> m_program;

public:
    DebugMaterial();

    void SetColor(const glm::vec4 &color);
    virtual void
    OnPrepareTextureBindings(std::vector<phi::TextureBinding> &) const;
    virtual void OnPrepareProgramBinding(phi::ProgramBinding &binding) const;

    virtual phi::MaterialId GetId() const {
        return phi::MaterialId::Debug;
    }

    virtual std::string GetName() const {
        return "mat_debug";
    }
};

} // namespace phi

#endif
