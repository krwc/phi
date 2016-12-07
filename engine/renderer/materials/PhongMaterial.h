#ifndef PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#define PHI_RENDERER_MATERIALS_PHONG_MATERIAL_H
#include <memory>
#include <string>
#include <vector>

#include "math/Math.h"

#include "device/Program.h"
#include "device/Texture.h"

#include "renderer/Material.h"

#include "utils/ShaderPreprocessor.h"

namespace phi {

class PhongMaterial final : public phi::Material {
public:
    struct Config {
        /* Solid diffuse color -- used if diffuse texture not specified */
        glm::vec4 diffuse_color;
        /* Solid specular color -- used if specular texture not specified */
        glm::vec4 specular_color;
        const phi::Texture2D *diffuse;
        const phi::Texture2D *specular;
        const phi::Texture2D *normal;
        float diffuse_uv_scale;
        float specular_uv_scale;
        float normal_uv_scale;
    };

    PhongMaterial(const phi::PhongMaterial::Config &config);

    void Compile();
    void Setup(const phi::PhongMaterial::Config &config);

    phi::AnyRange<phi::TextureBinding> GetTextureBindings() const;

    phi::Program *GetProgram() {
        return &m_program;
    }

    phi::MaterialId GetId() const {
        return phi::MaterialId::Phong;
    }

    std::string GetName() const {
        return "mat_phong";
    }

private:
    phi::Program m_program;
    PhongMaterial::Config m_config;
    bool m_dirty;
    std::vector<phi::TextureBinding> m_textures;

};

} // namespace phi

#endif
