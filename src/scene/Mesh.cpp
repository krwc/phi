#include "Mesh.h"

#include "renderer/DrawCallQueue.h"

#include <utility>

namespace phi {
using namespace std;

namespace {

const Layout SimpleMeshLayout = {
    { "in_position", offsetof(SimpleMesh::Vertex, position), sizeof(SimpleMesh::Vertex), Type::Float },
    { "in_normal"  , offsetof(SimpleMesh::Vertex, normal)  , sizeof(SimpleMesh::Vertex), Type::Float }
};

} // namespace

SimpleMesh::SimpleMesh()
        : m_dirty(true), m_material(nullptr), m_vertices() {}

SimpleMesh::~SimpleMesh() {}

const Layout *SimpleMesh::GetLayout() const {
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

Box SimpleMesh::GetBox() const {
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

void SimpleMesh::Render(phi::DrawCall &draw) {
    draw.primitive = Primitive::Triangles;
    draw.transform = GetTransform();
    draw.program = m_material->GetProgram();
    draw.layout = &SimpleMeshLayout;
    draw.vbo = GetVertexBuffer();
    draw.count = int(m_vertices.size());
    draw.offset = 0;
    m_material->FillTextureBindings(draw.texture_bindings);
    m_material->FillProgramConstants(draw.program_constants);
}

} // namespace phi
