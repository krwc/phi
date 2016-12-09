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
        /**
         * Solid diffuse color -- used if diffuse texture not specified
         */
        glm::vec3 diffuse_color = glm::vec3(1,1,1);
        /**
         * Specular power exponent. Should be >= 1.0f, or otherwise specular
         * will not be applied.
         */
        float specular_power = 0.0f;
        /**
         * Diffuse texture. If set, Config#diffuse_color will not have any effect.
         */
        const phi::Texture2D *diffuse = nullptr;
        /**
         * Normalmap texture. If set, bumpmapping will be used to perturb surface
         * normals.
         */
        const phi::Texture2D *normal = nullptr;
        /** Repetitiveness factor of diffuse texture */
        float diffuse_uv_scale = 1.0f;
        /** Repetitiveness factor of normalmap texture */
        float normal_uv_scale = 1.0f;
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
