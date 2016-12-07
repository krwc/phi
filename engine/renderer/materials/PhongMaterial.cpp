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

namespace {

enum TextureUnit {
    TEXTURE_UNIT_DIFFUSE = 0u,
    TEXTURE_UNIT_SPECULAR = 1u,
    TEXTURE_UNIT_NORMALMAP = 2u
};

void EnableTexture(TextureUnit texture_unit,
                   const phi::Texture2D *texture,
                   std::vector<phi::TextureBinding> &texture_bindings) {
    const phi::Sampler *sampler;
    if (texture->HasMipmaps()) {
        sampler = &phi::Samplers<phi::WrapMode::Repeat>::Trilinear2D();
    } else {
        sampler = &phi::Samplers<phi::WrapMode::Repeat>::Bilinear2D();
    }
    texture_bindings.push_back(phi::TextureBinding{
            static_cast<int>(texture_unit), texture, sampler });
}

} // namespace

void PhongMaterial::Compile() {
    if (!m_dirty) {
        return;
    }
    m_textures.clear();

    phi::ShaderPreprocessor spp("assets/shaders/PhongMaterial.fs");
    if (m_config.diffuse) {
        spp.Define("ENABLE_DIFFUSE_TEXTURE");
        EnableTexture(TEXTURE_UNIT_DIFFUSE, m_config.diffuse, m_textures);
    }
    if (m_config.specular) {
        spp.Define("ENABLE_SPECULAR_TEXTURE");
        EnableTexture(TEXTURE_UNIT_SPECULAR, m_config.specular, m_textures);
    }
    if (m_config.normal) {
        spp.Define("ENABLE_NORMALMAP_TEXTURE");
        EnableTexture(TEXTURE_UNIT_NORMALMAP, m_config.normal, m_textures);
    }
    m_program = phi::Program{};
    m_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/PhongMaterial.vs");
    m_program.SetSource(phi::ShaderType::Fragment, spp.Preprocess());
    m_program.Link();

    if (m_config.diffuse) {
        m_program.SetConstant("DiffuseUvScale", m_config.diffuse_uv_scale);
    } else {
        m_program.SetConstant("Diffuse", m_config.diffuse_color);
    }
    if (m_config.specular) {
        m_program.SetConstant("SpecularUvScale", m_config.specular_uv_scale);
    } else {
        m_program.SetConstant("Specular", m_config.specular_color);
    }
    if (m_config.normal) {
        m_program.SetConstant("NormalmapUvScale", m_config.normal_uv_scale);
    }

    m_dirty = false;
}

void PhongMaterial::Setup(const phi::PhongMaterial::Config &config) {
    m_config = config;
    m_dirty = true;
}

phi::AnyRange<phi::TextureBinding> PhongMaterial::GetTextureBindings() const {
    return m_textures;
}

} // namespace phi
