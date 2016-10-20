#include "BasicMaterial.h"

#include "device/Shader.h"

namespace phi {
using namespace glm;
using namespace std;

static const char *vertex_shader = R"(
#version 430
layout(location=0) in vec4 in_position;
layout(location=1) in vec3 in_normal;
uniform mat4 g_ProjViewModelMatrix;
uniform mat4 g_ViewModelMatrix;
uniform mat4 g_ModelMatrix;
uniform mat3 g_NormalMatrix;

out vec3 N;
out vec3 P;
out vec3 DirLightPos;

void main() {
    vec4 Transformed = g_ProjViewModelMatrix * in_position;
    gl_Position = Transformed;
    N = normalize(g_NormalMatrix * in_normal);
    P = (g_ModelMatrix * in_position).xyz;
}
)";

static const char *fragment_shader = R"(
#version 430
uniform vec3 diffuse;
out vec4 FragColor;
in vec3 N;
in vec3 P;

const vec3 DirLightPos = vec3(0, 30, 100);
const vec3 PointLightPos = vec3(0, 4, 8);

void main() {
    float DirLightI = max(0.0, dot(N, normalize(DirLightPos)));
    vec3 L = (PointLightPos - P);
    float PointLightI = max(0.0, dot(N, normalize(L)));
    float d = length(L);
    float Attenuation = 1/(1.0f + 0.1*d + 0.03*d*d);
    if (d > 1/0.03) {
        PointLightI = 0;
    }

    FragColor = vec4(clamp(DirLightI + PointLightI * Attenuation, 0, 1) * diffuse, 1.0f);
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
}

} // namespace phi
