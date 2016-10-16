#include "BasicMaterial.h"

#include "device/Shader.h"

namespace phi {
using namespace glm;
using namespace std;

static const char *vertex_shader = R"(
#version 430
layout(location=0) in vec4 in_position;
layout(location=1) in vec3 in_normal;
uniform mat4 g_MVP;

out vec3 N;

void main() {
    gl_Position = g_MVP * in_position;
    N = in_normal;
}
)";

static const char *fragment_shader = R"(
#version 430
uniform vec3 diffuse;
out vec4 FragColor;
in vec3 N;

void main() {
    FragColor = vec4(min(diffuse, N), 1.0f);
}
)";

BasicMaterial::BasicMaterial()
        : m_specular(0, 0, 0, 1), m_diffuse(1, 1, 1, 1), m_program() {
    try {
        m_program = Material::g_cache.Get(GetName());
    } catch (out_of_range &) {
        m_program = make_shared<Program>();
        m_program->SetSource(ShaderType::Vertex, vertex_shader);
        m_program->SetSource(ShaderType::Fragment, fragment_shader);
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

void BasicMaterial::PrepareForRendering() {
    m_program->SetConstant("diffuse", m_diffuse);
    // TODO: set diffuse, specular, bind shader program, it'd go somewhat like:
    // rend.BindProgram(m_program.get());
    // m_program->SetConstant("specular", m_specular);
    // m_program->SetConstant("diffuse", m_diffuse);
}

} // namespace phi
