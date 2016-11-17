#ifndef PHI_SCENE_MESH_H
#define PHI_SCENE_MESH_H
#include "device/Buffer.h"
#include "device/Layout.h"
#include "renderer/Material.h"

#include "math/AABB.h"
#include "math/Math.h"

#include "Entity.h"

#include <memory>
#include <vector>

namespace phi {

class Mesh : public Entity {
public:
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) = default;
    Mesh &operator=(Mesh &&) = default;

    Mesh() {}
    virtual ~Mesh() {}
    virtual const phi::Layout *GetLayout() const = 0;
    virtual phi::Material *GetMaterial() const = 0;
    virtual phi::Buffer *GetVertexBuffer() {
        return nullptr;
    }
    virtual phi::Buffer *GetIndexBuffer() {
        return nullptr;
    }
    virtual void SetMaterial(phi::Material *) = 0;
};

class SimpleMesh : virtual public Mesh {
public:
    struct Vertex {
        glm::vec4 position;
        glm::vec3 normal;
    };

    SimpleMesh();
    virtual ~SimpleMesh();
    virtual const phi::Layout *GetLayout() const;
    virtual phi::Material *GetMaterial() const;
    virtual phi::Buffer *GetVertexBuffer();
    virtual phi::AABB GetAABB() const;

    virtual void SetMaterial(phi::Material *);
    void AppendVertex(const SimpleMesh::Vertex &);
    virtual void Render(phi::DrawCall &);

private:
    bool m_dirty;
    phi::AABB m_box;
    phi::Material *m_material;
    std::unique_ptr<phi::Buffer> m_vbo;
    std::vector<Vertex> m_vertices;
};

} // namespace phi

#endif
