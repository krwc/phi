#include "Mesh.h"

#include "renderer/CommandQueue.h"

#include <utility>

namespace phi {
using namespace std;

namespace {

const Layout SimpleMeshLayout = {
    { "in_position", offsetof(SimpleMesh::Vertex, position), sizeof(SimpleMesh::Vertex), Type::Float },
    { "in_normal"  , offsetof(SimpleMesh::Vertex, normal)  , sizeof(SimpleMesh::Vertex), Type::Float }
};

} // namespace

SimpleMesh::SimpleMesh(unique_ptr<Material> &&material)
        : m_dirty(true), m_material(move(material)), m_vertices() {}

SimpleMesh::~SimpleMesh() {}

const Layout *SimpleMesh::GetLayout() const {
    return &SimpleMeshLayout;
}

Material *SimpleMesh::GetMaterial() const {
    return m_material.get();
}

void SimpleMesh::AppendVertex(const SimpleMesh::Vertex &vertex) {
    m_vertices.push_back(vertex);
    m_dirty = true;
}

Buffer *SimpleMesh::GetVertexBuffer() {
    if (m_dirty) {
        m_vbo = make_unique<Buffer>(BufferType::Vertex, BufferUsage::Static,
                                    m_vertices.data(),
                                    m_vertices.size() * sizeof(Vertex));
        m_dirty = false;
    }
    return m_vbo.get();
}

void SimpleMesh::Render(CommandQueue *queue) {
    Command command{};
    command.primitive = PrimitiveType::Triangles;
    command.model = &GetTransform();
    command.material = GetMaterial();
    command.layout = &SimpleMeshLayout;
    command.vbo = GetVertexBuffer();
    command.count = int(m_vertices.size());
    command.offset = 0;
    queue->Insert(command);
}

} // namespace phi
