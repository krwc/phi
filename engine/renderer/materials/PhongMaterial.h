#ifndef PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#define PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#include <array>
#include <memory>
#include <string>

#include "math/Math.h"

#include "device/Program.h"
#include "device/Texture.h"

#include "renderer/Material.h"

#include <boost/container/static_vector.hpp>

namespace phi {

class PhongMaterial final : public phi::Material {
    glm::vec4 m_diffuse_color;
    glm::vec4 m_specular_color;
    phi::Program m_program;
    bool m_dirty;
    struct {
        const phi::Texture2D *diffuse;
        float diffuse_uv_scale;
        const phi::Texture2D *specular;
        float specular_uv_scale;
        const phi::Texture2D *normal;
    } m_textures;
    boost::container::static_vector<phi::ProgramConstant, 3u> m_constants;
    boost::container::static_vector<phi::TextureBinding, 3u> m_texture_bindings;

public:
    PhongMaterial();

    void SetDiffuseColor(const glm::vec4 &diffuse);
    void SetSpecularColor(const glm::vec4 &specular);

    void SetDiffuseTexture(const phi::Texture2D &texture, float scale = 1.0f);
    void SetSpecularTexture(const phi::Texture2D &texture, float scale = 1.0f);
    void SetNormalTexture(const phi::Texture2D &texture);

    void Compile();
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
