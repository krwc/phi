#include "BasicMaterial.h"

#include "renderer/Shader.h"

namespace phi {
using namespace glm;
using namespace std;

BasicMaterial::BasicMaterial()
        : m_specular(0, 0, 0, 1), m_diffuse(1, 1, 1, 1), m_program() {
    try {
        m_program = Material::g_cache.Get(GetName());
    } catch (out_of_range &) {
        m_program = make_shared<Program>();
        m_program->SetSource(ShaderType::Vertex,
                            "#version 430\nvoid main(){}\n");
        m_program->SetSource(
                ShaderType::Fragment,
                "#version 430\nvoid main(){gl_FragColor=vec4(1,1,1,1);}\n");
        m_program->Link();
        Material::g_cache.Insert(GetName(), m_program);
    }
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
