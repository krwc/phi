#include "DebugDrawer.h"
#include "DrawCall.h"
#include "Renderer.h"

#include "math/Box.h"

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
    in vec2 UV;

    void main() {
        gl_FragColor = texture(img, UV);
    }
)";

glm::vec2 quad[] = { { -1, -1 }, { 1, -1 }, { -1, 1 },
                     { -1, 1 },  { 1, -1 }, { 1, 1 } };

} // namespace


DebugDrawer::DebugDrawer(const Camera *view, Renderer *renderer)
        : m_view(view),
          m_renderer(renderer),
          m_vbo(BufferType::Vertex,
                BufferHint::Dynamic,
                nullptr,
                DebugDrawer::BUFFER_SIZE),
          m_quad() {
    m_quad.SetSource(ShaderType::Vertex, quad_vertex_shader);
    m_quad.SetSource(ShaderType::Fragment, quad_fragment_shader);
    m_quad.Link();
}

void DebugDrawer::DrawBox(const Box &box, const vec3 &color) {
    static phi::Layout layout{ { "in_position", 0, sizeof(vec4), Type::Float } };
    auto MakeVertex = [&](Box::Vertex vertex) {
        return vec4(box.GetVertex(vertex), 1);
    };
    m_material.SetColor(vec4(color, 1));
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
    DrawCall command{};
    command.primitive = PrimitiveType::Lines;
    command.model = &m_dummy;
    m_material.OnPrepareProgramBinding(command.program_binding);
    command.layout = &layout;
    command.vbo = &m_vbo;
    command.count = data.size();
    command.offset = 0;
    m_renderer->Render(*m_view, command);
}

void DebugDrawer::DrawTexture(const Texture2D *texture, int x, int y, int w, int h) {
    static phi::Layout layout{ { "in_position", 0, sizeof(vec2), Type::Float } };
    m_vbo.UpdateData(quad, sizeof(quad));
    DrawCall command{};
    command.primitive = PrimitiveType::Triangles;
    command.model = &m_dummy;
    command.texture_bindings = { { "img", texture } };
    command.program_binding.program = &m_quad;
    command.layout = &layout;
    command.vbo = &m_vbo;
    command.count = 6;
    command.offset = 0;
    m_renderer->Render(*m_view, command);
}

} // namespace phi
