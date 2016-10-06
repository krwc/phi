#include "Mesh.h"
#include <utility>

namespace phi {
using namespace std;

namespace {

const VertexLayout SimpleMeshLayout = {
    { "position", offsetof(SimpleMesh::Vertex, position), sizeof(SimpleMesh::Vertex), Type::Float },
    { "normal"  , offsetof(SimpleMesh::Vertex, normal)  , sizeof(SimpleMesh::Vertex), Type::Float },
    { "uv"      , offsetof(SimpleMesh::Vertex, uv)      , sizeof(SimpleMesh::Vertex), Type::Float }
};

} // namespace

SimpleMesh::SimpleMesh(const Material &material)
        : m_dirty(true), m_material(material), m_vertices() {}

SimpleMesh::~SimpleMesh() {}

const VertexLayout *SimpleMesh::GetVertexLayout() const {
    return &SimpleMeshLayout;
}

const Material *SimpleMesh::GetMaterial() const {
    return &m_material;
}

void SimpleMesh::AppendVertex(const SimpleMesh::Vertex &vertex) {
    m_vertices.push_back(vertex);
    m_dirty = true;
}

const Buffer *SimpleMesh::GetVertexBuffer() {
    if (m_dirty) {
        m_vbo = make_unique<Buffer>(BufferType::Vertex, BufferUsage::Static,
                                    m_vertices.data(),
                                    m_vertices.size() * sizeof(Vertex));
        m_dirty = false;
    }
    return m_vbo.get();
}
} // namespace phi
