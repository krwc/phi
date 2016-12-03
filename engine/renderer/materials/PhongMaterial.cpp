#include "PhongMaterial.h"
#include "renderer/Common.h"
#include "utils/ShaderPreprocessor.h"

#include <stdexcept>
#include <iostream>

namespace phi {

PhongMaterial::PhongMaterial()
        : m_diffuse_color(1, 1, 1, 1),
          m_specular_color(0, 0, 0, 1),
          m_program(),
          m_dirty(true),
          m_textures(),
          m_constants() {}

enum {
    DIFFUSE_TEXTURE_UNIT = 0,
    SPECULAR_TEXTURE_UNIT = 1
};

void PhongMaterial::Compile() {
    if (!m_dirty) {
        return;
    }
    m_texture_bindings.clear();
    m_constants.clear();
    phi::ShaderPreprocessor spp("assets/shaders/PhongMaterial.fs");
    if (m_textures.diffuse) {
        spp.Define("ENABLE_DIFFUSE_TEXTURE");
        m_texture_bindings.push_back({
            DIFFUSE_TEXTURE_UNIT, m_textures.diffuse, &phi::Samplers<phi::WrapMode::ClampToEdge>::Bilinear2D()
        });
    } else {
        m_constants.push_back({ "Diffuse", &m_diffuse_color });
    }
    if (m_textures.specular) {
        spp.Define("ENABLE_SPECULAR_TEXTURE");
        m_texture_bindings.push_back({
            SPECULAR_TEXTURE_UNIT, m_textures.specular, &phi::Samplers<phi::WrapMode::ClampToEdge>::Bilinear2D()
        });
    } else {
        m_constants.push_back({ "Specular", &m_specular_color });
    }
    m_program = phi::Program{};
    m_program.SetSourceFromFile(phi::ShaderType::Vertex, "assets/shaders/PhongMaterial.vs");
    m_program.SetSource(phi::ShaderType::Fragment, spp.Preprocess());
    m_program.Link();
    m_dirty = false;
}

void PhongMaterial::SetDiffuseColor(const glm::vec4 &diffuse) {
    m_diffuse_color = diffuse;
    m_textures.diffuse = nullptr;
    m_dirty = true;
}

void PhongMaterial::SetSpecularColor(const glm::vec4 &specular) {
    m_specular_color = specular;
    m_textures.specular = nullptr;
    m_dirty = true;
}

void PhongMaterial::SetDiffuseTexture(const phi::Texture2D &texture) {
    m_textures.diffuse = &texture;
    m_dirty = true;
}

void PhongMaterial::SetSpecularTexture(const phi::Texture2D &texture) {
    m_textures.specular = &texture;
    m_dirty = true;
}

phi::AnyRange<phi::TextureBinding> PhongMaterial::GetTextureBindings() const {
    return m_texture_bindings;
}

phi::AnyRange<phi::ProgramConstant> PhongMaterial::GetProgramConstants() const {
    return m_constants;
}

} // namespace phi
