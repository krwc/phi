#include "BasicMaterial.h"

namespace phi {
using namespace glm;

BasicMaterial::BasicMaterial()
        : m_specular(0, 0, 0, 1), m_diffuse(1, 1, 1, 1) {
    // TODO: initialize shader program.
}

void BasicMaterial::SetSpecular(const vec4 &specular) {
    m_specular = specular;
}

void BasicMaterial::SetDiffuse(const vec4 &diffuse) {
    m_diffuse = diffuse;
}

void BasicMaterial::PrepareForRendering(phi::Renderer &) const {
    // TODO: set diffuse, specular, bind shader program, it'd go somewhat like:
    // rend.BindProgram(m_program.get());
    // m_program->SetConstant("specular", m_specular);
    // m_program->SetConstant("diffuse", m_diffuse);
}

} // namespace phi
