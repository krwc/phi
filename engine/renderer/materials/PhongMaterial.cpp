#include "PhongMaterial.h"
#include "renderer/Common.h"
#include "utils/ShaderPreprocessor.h"

#include <stdexcept>
#include <iostream>

namespace phi {

PhongMaterial::PhongMaterial(const phi::PhongMaterial::Config &config)
        : m_program(), m_config(), m_dirty(true) {
    Setup(config);
}

enum TextureUnit {
    TEXTURE_UNIT_DIFFUSE = 0u,
    TEXTURE_UNIT_SPECULAR = 1u,
    TEXTURE_UNIT_NORMALMAP = 2u
};

void PhongMaterial::Compile() {
    if (!m_dirty) {
        return;
    }
    m_constants.clear();
    m_textures.clear();

    phi::ShaderPreprocessor spp("assets/shaders/PhongMaterial.fs");
    if (m_config.diffuse) {
        EnableTexture(TEXTURE_UNIT_DIFFUSE, m_config.diffuse, &m_config.diffuse_uv_scale, spp);
    } else {
        m_constants.push_back({ "Diffuse", &m_config.diffuse_color });
    }

    if (m_config.specular) {
        EnableTexture(TEXTURE_UNIT_SPECULAR, m_config.diffuse, &m_config.specular_uv_scale, spp);
    } else {
        m_constants.push_back({ "Specular", &m_config.specular_color });
    }

    if (m_config.normal) {
        EnableTexture(TEXTURE_UNIT_NORMALMAP, m_config.normal, &m_config.normal_uv_scale, spp);
    }
    m_program = phi::Program{};
    m_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/PhongMaterial.vs");
    m_program.SetSource(phi::ShaderType::Fragment, spp.Preprocess());
    m_program.Link();
    m_dirty = false;
}

void PhongMaterial::Setup(const phi::PhongMaterial::Config &config) {
    m_config = config;
    m_dirty = true;
}

namespace {

struct ParamMapping {
    TextureUnit unit;
    std::string feature_name;
    std::string uv_scale_name;
} material_parameters[] = {
    { TEXTURE_UNIT_DIFFUSE, "ENABLE_DIFFUSE_TEXTURE", "DiffuseUvScale" },
    { TEXTURE_UNIT_SPECULAR, "ENABLE_SPECULAR_TEXTURE", "SpecularUvScale" },
    { TEXTURE_UNIT_NORMALMAP, "ENABLE_NORMALMAP_TEXTURE", "NormalmapUvScale" }
};

}

void PhongMaterial::EnableTexture(int texture_unit,
                                  const phi::Texture2D *texture,
                                  const float *scale,
                                  phi::ShaderPreprocessor &preprocessor) {
    const phi::Sampler *sampler;
    if (texture->HasMipmaps()) {
        sampler = &phi::Samplers<phi::WrapMode::Repeat>::Trilinear2D();
    } else {
        sampler = &phi::Samplers<phi::WrapMode::Repeat>::Bilinear2D();
    }
    m_textures.push_back({ static_cast<int>(texture_unit), texture, sampler });
    for (const ParamMapping &mapping : material_parameters) {
        if (texture_unit == mapping.unit) {
            m_constants.push_back({ mapping.uv_scale_name.c_str(), scale });
            preprocessor.Define(mapping.feature_name);
            break;
        }
    }
}

phi::AnyRange<phi::TextureBinding> PhongMaterial::GetTextureBindings() const {
    return m_textures;
}

phi::AnyRange<phi::ProgramConstant> PhongMaterial::GetProgramConstants() const {
    return m_constants;
}

} // namespace phi
