#ifndef PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#define PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#include <array>
#include <memory>
#include <string>

#include "math/Math.h"

#include "device/Program.h"
#include "device/Texture.h"

#include "renderer/Material.h"

namespace phi {

class PhongMaterial final : public phi::Material {
    glm::vec4 m_diffuse_color;
    glm::vec4 m_specular_color;
    phi::Program m_program;
    bool m_dirty;
    struct {
        const phi::Texture2D *diffuse;
        const phi::Texture2D *specular;
    } m_textures;
    std::vector<phi::ProgramConstant> m_constants;
    std::vector<phi::TextureBinding> m_texture_bindings;

    void CompileProgram();

public:
    PhongMaterial();

    void SetDiffuseColor(const glm::vec4 &diffuse);
    void SetSpecularColor(const glm::vec4 &specular);

    void SetDiffuseTexture(const phi::Texture2D &texture);
    void SetSpecularTexture(const phi::Texture2D &texture);

    void OnRender();
    phi::AnyRange<phi::TextureBinding> GetTextureBindings() const;
    phi::AnyRange<phi::ProgramConstant> GetProgramConstants() const;

    virtual phi::Program *GetProgram() {
        return &m_program;
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
