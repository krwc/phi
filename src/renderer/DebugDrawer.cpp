#include "DebugDrawer.h"
#include "DrawCall.h"
#include "Renderer.h"

#include "math/AABB.h"
#include "math/Rect2D.h"

#include "device/Sampler.h"

#include "scene/Camera.h"
#include "scene/DummyCamera.h"

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

DebugDrawer::DebugDrawer(phi::Device &device)
        : m_device(device),
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

void DebugDrawer::DrawAABB(const phi::Camera &view,
                           const phi::AABB &box,
                           const vec3 &color) {
    auto MakeVertex = [&](AABB::Vertex vertex) {
        return vec4(box.GetVertex(vertex), 1);
    };
    std::vector<vec4> data = {
        MakeVertex(AABB::Vertex::MinMinMin), MakeVertex(AABB::Vertex::MaxMinMin),
        MakeVertex(AABB::Vertex::MaxMinMin), MakeVertex(AABB::Vertex::MaxMaxMin),
        MakeVertex(AABB::Vertex::MaxMaxMin), MakeVertex(AABB::Vertex::MinMaxMin),
        MakeVertex(AABB::Vertex::MinMaxMin), MakeVertex(AABB::Vertex::MinMinMin),

        MakeVertex(AABB::Vertex::MinMinMax), MakeVertex(AABB::Vertex::MaxMinMax),
        MakeVertex(AABB::Vertex::MaxMinMax), MakeVertex(AABB::Vertex::MaxMaxMax),
        MakeVertex(AABB::Vertex::MaxMaxMax), MakeVertex(AABB::Vertex::MinMaxMax),
        MakeVertex(AABB::Vertex::MinMaxMax), MakeVertex(AABB::Vertex::MinMinMax),

        MakeVertex(AABB::Vertex::MinMinMin), MakeVertex(AABB::Vertex::MinMinMax),
        MakeVertex(AABB::Vertex::MaxMinMin), MakeVertex(AABB::Vertex::MaxMinMax),
        MakeVertex(AABB::Vertex::MaxMaxMin), MakeVertex(AABB::Vertex::MaxMaxMax),
        MakeVertex(AABB::Vertex::MinMaxMin), MakeVertex(AABB::Vertex::MinMaxMax),
    };
    m_vbo.UpdateData(data.data(), sizeof(data) * sizeof(vec4));
    m_device.BindProgram(m_debug_program);
    m_device.BindVbo(m_vbo);
    m_device.BindLayout(debug_layout);
    m_debug_program.SetConstant("g_ProjViewModelMatrix",
                                view.GetProjMatrix() * view.GetViewMatrix());
    m_debug_program.SetConstant("Color", glm::vec4(color, 1.0f));
    m_device.Draw(phi::Primitive::Lines, 0, 6);
}

void DebugDrawer::DrawTexture(const Texture2D &texture, int x, int y, int w, int h) {
    m_vbo.UpdateData(quad, sizeof(quad));
    m_device.BindProgram(m_quad_program);
    m_device.BindVbo(m_vbo);
    m_device.BindLayout(quad_layout);
    m_device.BindTexture(0, texture);
    m_device.BindSampler(0, phi::Sampler::Bilinear2D(phi::WrapMode::Clamp));
    auto viewport = m_device.GetViewport();
    auto scissor = m_device.GetScissor();
    m_device.SetViewport({ x, y, w, h });
    m_device.SetScissor({ x, y, w, h });
    m_device.Draw(phi::Primitive::Triangles, 0, 6);
    m_device.SetViewport(viewport);
    m_device.SetScissor(scissor);
}

} // namespace phi
