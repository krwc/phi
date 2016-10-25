#include "DebugMaterial.h"

#include "renderer/Renderer.h"

#include "device/Shader.h"

namespace phi {
using namespace glm;
using namespace std;

namespace {

const char *vertex_shader = R"(
    #version 430
    layout(location=0) in vec4 in_position;
    uniform mat4 g_ProjViewModelMatrix;

    void main() {
        gl_Position = g_ProjViewModelMatrix * in_position;
    }
)";

static const char *fragment_shader = R"(
    #version 430
    uniform vec4 Color;
    out vec4 FragColor;

    void main() {
        FragColor = Color;
    }
)";

} // namespace

DebugMaterial::DebugMaterial()
        : m_color(1, 1, 1, 1), m_program() {
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

void DebugMaterial::SetColor(const vec4 &color) {
    m_color = color;
}

void DebugMaterial::OnPrepareTextureBindings(
        std::vector<phi::TextureBinding> &) const {
    return;
}

void DebugMaterial::OnPrepareProgramBinding(phi::ProgramBinding &binding) const {
    binding.program = m_program.get();
    binding.constants = { { "Color", glm::value_ptr(m_color) } };
}

} // namespace phi
