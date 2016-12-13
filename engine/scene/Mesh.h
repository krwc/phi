#ifndef PHI_SCENE_MESH_H
#define PHI_SCENE_MESH_H
#include "device/Buffer.h"
#include "device/Layout.h"
#include "renderer/Material.h"

#include "math/AABB.h"
#include "math/Math.h"
#include "math/Ray.h"

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
        glm::vec2 uv;
    };

    SimpleMesh(const std::string &name);
    virtual ~SimpleMesh();
    virtual const phi::Layout *GetLayout() const;
    virtual phi::Material *GetMaterial() const;
    virtual phi::Buffer *GetVertexBuffer();
    virtual phi::AABB GetAABB() const;
    virtual bool Hit(const phi::Ray &ray, double *out_t) const;

    virtual void SetMaterial(phi::Material *);
    void AppendVertex(const SimpleMesh::Vertex &);
    /** Calculates normals of currently appended vertices */
    void ComputeNormals();
    /** Resets contents of the mesh removing all of its vertices. */
    void Reset();
    virtual void Render(phi::DrawCall &);

    virtual const std::string &GetName() const {
        return m_name;
    }

private:
    bool m_dirty;
    phi::AABB m_box;
    phi::Material *m_material;
    std::string m_name;
    std::unique_ptr<phi::Buffer> m_vbo;
    std::vector<Vertex> m_vertices;
};

} // namespace phi

#endif
