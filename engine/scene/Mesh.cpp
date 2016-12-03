#include "Mesh.h"

#include "renderer/DrawCallQueue.h"

#include <utility>

namespace phi {
using namespace std;

SimpleMesh::SimpleMesh(const std::string &name)
        : m_dirty(true), m_material(nullptr), m_name(name), m_vertices() {}

SimpleMesh::~SimpleMesh() {}

const Layout *SimpleMesh::GetLayout() const {
    static const Layout SimpleMeshLayout(sizeof(SimpleMesh::Vertex), {
        { phi::Layout::Position, phi::Layout::Float4, offsetof(SimpleMesh::Vertex, position) },
        { phi::Layout::Normal  , phi::Layout::Float3, offsetof(SimpleMesh::Vertex, normal)   }
    });
    return &SimpleMeshLayout;
}

Material *SimpleMesh::GetMaterial() const {
    return m_material;
}

Buffer *SimpleMesh::GetVertexBuffer() {
    if (m_dirty) {
        m_vbo = make_unique<Buffer>(BufferType::Vertex, BufferHint::Static,
                                    m_vertices.data(),
                                    m_vertices.size() * sizeof(Vertex));
        m_dirty = false;
    }
    return m_vbo.get();
}

AABB SimpleMesh::GetAABB() const {
    return GetTransform() * m_box;
}

void SimpleMesh::AppendVertex(const SimpleMesh::Vertex &vertex) {
    m_vertices.push_back(vertex);
    m_box.Cover(vertex.position);
    m_dirty = true;
}

void SimpleMesh::SetMaterial(phi::Material *material) {
    m_material = material;
}

bool SimpleMesh::Hit(const phi::Ray &ray, double *out_t) const {
    double closest = std::numeric_limits<double>::max();
    double current;
    bool hit = false;
    for (size_t i = 0; i < m_vertices.size(); i += 3) {
        const glm::vec4 v0 = GetTransform() * m_vertices[i + 0].position;
        const glm::vec4 v1 = GetTransform() * m_vertices[i + 1].position;
        const glm::vec4 v2 = GetTransform() * m_vertices[i + 2].position;
        if (ray.Hits(v0, v1, v2, &current)) {
            if (!out_t) {
                // No need to fill out_t, so let's just return that hit occured
                return true;
            } else {
                hit = true;
                closest = glm::min(closest, current);
            }
        }
    }
    if (hit && out_t) {
        *out_t = closest;
    }
    return hit;
}

void SimpleMesh::Render(phi::DrawCall &draw) {
    draw.primitive = Primitive::Triangles;
    draw.transform = GetTransform();
    draw.program = m_material->GetProgram();
    draw.layout = GetLayout();
    draw.vbo = GetVertexBuffer();
    draw.count = int(m_vertices.size());
    draw.offset = 0;
    draw.program_constants = m_material->GetProgramConstants();
    draw.texture_bindings = m_material->GetTextureBindings();
}

} // namespace phi
