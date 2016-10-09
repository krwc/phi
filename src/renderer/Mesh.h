#ifndef PHI_RENDERER_MESH_H
#define PHI_RENDERER_MESH_H
#include "Buffer.h"
#include "Material.h"
#include "Layout.h"

#include "math/Math.h"

#include <memory>
#include <vector>

namespace phi {

class Mesh {
public:
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) = default;
    Mesh &operator=(Mesh &&) = default;

    Mesh() {}
    virtual ~Mesh() {}
    virtual const phi::Layout *GetLayout() const = 0;
    virtual const phi::Material *GetMaterial() const = 0;
    virtual const phi::Buffer *GetVertexBuffer() {
        return nullptr;
    }
    virtual const phi::Buffer *GetIndexBuffer() {
        return nullptr;
    }
};

class SimpleMesh : virtual public Mesh {
public:
    struct Vertex {
        glm::vec4 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    SimpleMesh(std::unique_ptr<const phi::Material> &&material);
    virtual ~SimpleMesh();
    virtual const phi::Material *GetMaterial() const;
    virtual const phi::Layout *GetLayout() const;
    virtual const phi::Buffer *GetVertexBuffer();
    void AppendVertex(const SimpleMesh::Vertex &);

private:
    bool m_dirty;
    std::unique_ptr<const phi::Material> m_material;
    std::unique_ptr<phi::Buffer> m_vbo;
    std::vector<Vertex> m_vertices;
};

} // namespace phi

#endif
