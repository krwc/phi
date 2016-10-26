#include "DebugDrawer.h"
#include "DrawCall.h"
#include "Renderer.h"

#include "math/Box.h"

#include "device/Sampler.h"

#include "scene/Camera.h"

#include <vector>

namespace phi {
using namespace glm;

namespace {

const char *quad_vertex_shader = R"(
    #version 430
    layout(location=0) in vec2 in_position;
    out vec2 UV;

    void main() {
        UV = (in_position + 1.0f) * 0.5f;
        gl_Position = vec4(in_position, 0, 1);
    }
)";

const char *quad_fragment_shader = R"(
    #version 430
    uniform sampler2D img;
    out vec4 FragColor;
    in vec2 UV;

    void main() {
        FragColor = texture(img, UV);
    }
)";

const phi::Layout quad_layout{ { "in_position", 0, sizeof(vec2),
                                 Type::Float } };

glm::vec2 quad[] = { { -1, -1 }, { 1, -1 }, { -1, 1 },
                     { -1, 1 },  { 1, -1 }, { 1, 1 } };


const char *debug_vertex_shader = R"(
    #version 430
    layout(location=0) in vec4 in_position;
    uniform mat4 g_ProjViewModelMatrix;

    void main() {
        gl_Position = g_ProjViewModelMatrix * in_position;
    }
)";

static const char *debug_fragment_shader = R"(
    #version 430
    uniform vec4 Color;
    out vec4 FragColor;

    void main() {
        FragColor = Color;
    }
)";

const phi::Layout debug_layout{ { "in_position", 0, sizeof(vec4),
                                  Type::Float } };
} // namespace

DebugDrawer::DebugDrawer(const Camera &view, Renderer &renderer)
        : m_view(view),
          m_renderer(renderer),
          m_vbo(BufferType::Vertex,
                BufferHint::Dynamic,
                nullptr,
                DebugDrawer::BUFFER_SIZE),
          m_debug_program(),
          m_quad_program() {
    m_debug_program.SetSource(ShaderType::Vertex, debug_vertex_shader);
    m_debug_program.SetSource(ShaderType::Fragment, debug_fragment_shader);
    m_debug_program.Link();

    m_quad_program.SetSource(ShaderType::Vertex, quad_vertex_shader);
    m_quad_program.SetSource(ShaderType::Fragment, quad_fragment_shader);
    m_quad_program.Link();
}

void DebugDrawer::DrawBox(const Box &box, const vec3 &color) {
    auto MakeVertex = [&](Box::Vertex vertex) {
        return vec4(box.GetVertex(vertex), 1);
    };
    std::vector<vec4> data = {
        MakeVertex(Box::Vertex::MinMinMin), MakeVertex(Box::Vertex::MaxMinMin),
        MakeVertex(Box::Vertex::MaxMinMin), MakeVertex(Box::Vertex::MaxMaxMin),
        MakeVertex(Box::Vertex::MaxMaxMin), MakeVertex(Box::Vertex::MinMaxMin),
        MakeVertex(Box::Vertex::MinMaxMin), MakeVertex(Box::Vertex::MinMinMin),

        MakeVertex(Box::Vertex::MinMinMax), MakeVertex(Box::Vertex::MaxMinMax),
        MakeVertex(Box::Vertex::MaxMinMax), MakeVertex(Box::Vertex::MaxMaxMax),
        MakeVertex(Box::Vertex::MaxMaxMax), MakeVertex(Box::Vertex::MinMaxMax),
        MakeVertex(Box::Vertex::MinMaxMax), MakeVertex(Box::Vertex::MinMinMax),

        MakeVertex(Box::Vertex::MinMinMin), MakeVertex(Box::Vertex::MinMinMax),
        MakeVertex(Box::Vertex::MaxMinMin), MakeVertex(Box::Vertex::MaxMinMax),
        MakeVertex(Box::Vertex::MaxMaxMin), MakeVertex(Box::Vertex::MaxMaxMax),
        MakeVertex(Box::Vertex::MinMaxMin), MakeVertex(Box::Vertex::MinMaxMax),
    };
    m_vbo.UpdateData(data.data(), sizeof(data) * sizeof(vec4));
    glm::vec4 color4 = glm::vec4(color, 1.0f);
    phi::DrawCall draw{};
    draw.primitive = phi::Primitive::Lines;
    draw.transform = m_view.GetViewMatrix();
    draw.program = &m_debug_program;
    draw.layout = &debug_layout;
    draw.vbo = &m_vbo;
    draw.program_constants = { { "Color", &color4 } };
    draw.texture_bindings = {};
    draw.count = data.size();
    draw.offset = 0;
    m_renderer.Execute(draw);
}

void DebugDrawer::DrawTexture(const Texture2D *texture, int x, int y, int w, int h) {
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    m_vbo.UpdateData(quad, sizeof(quad));
    DrawCall draw{};
    draw.primitive = Primitive::Triangles;
    draw.transform = m_view.GetViewMatrix();
    draw.program = &m_quad_program;
    draw.layout = &quad_layout;
    draw.vbo = &m_vbo;
    draw.texture_bindings = {
        { "img", texture, phi::Sampler::Bilinear2D(phi::WrapMode::Clamp) }
    };
    draw.count = 6;
    draw.offset = 0;
    m_renderer.Execute(draw);
}

} // namespace phi
