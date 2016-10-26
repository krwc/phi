#include "BasicMaterial.h"

#include "renderer/Renderer.h"

#include "device/Shader.h"

namespace phi {
using namespace glm;
using namespace std;

namespace {

const char *vertex_shader = R"(
    #version 430
    layout(location=0) in vec4 in_position;
    layout(location=1) in vec3 in_normal;
    uniform mat4 g_ProjViewModelMatrix;
    uniform mat4 g_ViewModelMatrix;
    uniform mat4 g_ModelMatrix;
    uniform mat3 g_NormalMatrix;

    out vec3 N;
    out vec3 P;

    void main() {
        vec4 Transformed = g_ProjViewModelMatrix * in_position;
        gl_Position = Transformed;
        N = normalize(g_NormalMatrix * in_normal);
        P = (g_ModelMatrix * in_position).xyz;
    }
)";

const char *fragment_shader = R"(
    #version 430
    uniform vec3 diffuse;
    out vec4 FragColor;
    in vec3 N;
    in vec3 P;

    struct DirectionalLight {
        vec3 Position;
        vec3 Color;
    };

    struct PointLight {
        vec3 Position;
        vec3 Color;
        float ConstantAttenuation;
        float LinearAttenuation;
        float QuadraticAttenuation;
    };

    struct Light {
        int NumDirectionalLights;
        DirectionalLight Directional[8];
        int NumPointLights;
        PointLight Point[8];
    };

    uniform Light g_Light;

    vec3 ComputeDirLightIntensity(vec3 Normal) {
        vec3 Intensity = vec3(0,0,0);
        for (int i = 0; i < g_Light.NumDirectionalLights; ++i) {
            float I = max(0.0, dot(Normal, normalize(g_Light.Directional[i].Position)));
            Intensity += I * g_Light.Directional[i].Color;
        }
        return Intensity;
    }

    vec3 ComputePointLightIntensity(vec3 Normal, vec3 Point) {
        vec3 Intensity = vec3(0,0,0);
        for (int i = 0; i < g_Light.NumPointLights; ++i) {
            vec3 L = g_Light.Point[i].Position - Point;
            float D = length(L);
            float I = max(0.0, dot(N, L / D));
            float A = 1 / (g_Light.Point[i].ConstantAttenuation
                            + g_Light.Point[i].LinearAttenuation*D
                            + g_Light.Point[i].QuadraticAttenuation*D*D);
            if (D > 1/g_Light.Point[i].QuadraticAttenuation) {
                I = 0;
            }
            Intensity += I * A * g_Light.Point[i].Color;
        }
        return Intensity;
    }

    void main() {
        vec3 DirLightI = ComputeDirLightIntensity(N);
        vec3 PointLightI = ComputePointLightIntensity(N, P);

        FragColor = vec4(clamp((DirLightI + PointLightI) * diffuse, 0, 1), 1.0f);
    }
)";

} // namespace

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

void BasicMaterial::FillTextureBindings(std::vector<phi::TextureBinding> &) const {
    return;
}

void BasicMaterial::FillProgramConstants(
        std::vector<phi::ProgramConstant> &constants) const {
    constants = { { "diffuse", glm::value_ptr(m_diffuse) } };
}

} // namespace phi
