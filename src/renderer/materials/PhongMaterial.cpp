#include "PhongMaterial.h"

#include <stdexcept>

namespace phi {
namespace {
const char *vertex_shader = R"(
    #version 430
    layout(location=0) in vec4 in_position;
    layout(location=1) in vec3 in_normal;

    uniform mat4 g_ProjViewModelMatrix;
    uniform mat3 g_NormalMatrix;

    out vec3 Normal;

    void main() {
        gl_Position = g_ProjViewModelMatrix * in_position;
        Normal = normalize(g_NormalMatrix * in_normal);
    }
)";

const char *fragment_shader = R"(
    #version 430
    layout(location=0) out vec4 out_Normal;
    layout(location=1) out vec4 out_Diffuse;
    layout(location=2) out vec4 out_Specular;
    uniform vec4 Diffuse;
    uniform vec4 Specular;

    in vec3 Normal;

    void main() {
        out_Normal = Normal;
        out_Diffuse = Diffuse;
        out_Specular = Specular;
    }
)";
}

PhongMaterial::PhongMaterial() : m_diffuse(1, 1, 1, 1), m_specular(0, 0, 0, 1) {
    try {
        m_program = Material::g_cache.Get(GetName());
    } catch (std::out_of_range &) {
        m_program = std::make_shared<phi::Program>();
        m_program->SetSource(phi::ShaderType::Vertex, vertex_shader);
        m_program->SetSource(phi::ShaderType::Fragment, fragment_shader);
        m_program->Link();
    }
}

void PhongMaterial::SetDiffuse(const glm::vec4 &diffuse) {
    m_diffuse = diffuse;
}

void PhongMaterial::SetSpecular(const glm::vec4 &specular) {
    m_specular = specular;
}

void PhongMaterial::FillTextureBindings(std::vector<phi::TextureBinding> &) const {
}

void PhongMaterial::FillProgramConstants(std::vector<phi::ProgramConstant> &constants) const {
    constants.emplace_back("Diffuse", &m_diffuse);
    constants.emplace_back("Specular", &m_specular);
}

} // namespace phi
