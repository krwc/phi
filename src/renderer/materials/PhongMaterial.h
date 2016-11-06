#ifndef PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#define PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#include <memory>
#include <string>

#include "math/Math.h"

#include "device/Shader.h"

#include "renderer/Material.h"

namespace phi {

class PhongMaterial final : public phi::Material {
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;
    std::shared_ptr<Program> m_program;

public:
    PhongMaterial();

    void SetDiffuse(const glm::vec4 &diffuse);
    void SetSpecular(const glm::vec4 &specular);
    virtual void FillTextureBindings(std::vector<phi::TextureBinding> &) const;
    virtual void FillProgramConstants(std::vector<phi::ProgramConstant> &) const;

    virtual phi::Program *GetProgram() {
        return m_program.get();
    }

    virtual phi::MaterialId GetId() const {
        return phi::MaterialId::Phong;
    }

    virtual std::string GetName() const {
        return "mat_phong";
    }
};

} // namespace phi

#endif
