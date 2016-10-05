#ifndef PHI_RENDERER_VERTEX_LAYOUT_H
#define PHI_RENDERER_VERTEX_LAYOUT_H
#include <GL/gl.h>

#include <initializer_list>
#include <string>
#include <vector>

namespace phi {

enum class Type {
    Float = GL_FLOAT
};

/**
 * This structure is used by renderer to determine which shader input variable
 * this particular data entry shall be bound to. For example, let's consider
 * following GLSL shader:
 * @code:
 *  layout(location=0) in vec4 position;
 *  layout(location=1) in vec4 color;
 *  ...
 *
 * Now, let's assume we have a Buffer object filled with vertex data:
 * @code:
 *  struct Vertex { vec4 position; vec4 color };
 * in this case we'd have to setup two VertexLayoutEntry instances, in order to
 * bind data properly:
 * @code:
 *  {
 *      { "position", offsetof(Vertex, position), sizeof(Vertex), Type::Float },
 *      { "color"   , offsetof(Vertex, color)   , sizeof(Vertex), Type::Float }
 *  }
 */
struct VertexLayoutEntry {
    std::string name;
    int offset;
    int stride;
    Type type;
};

class VertexLayout {
    std::vector<VertexLayoutEntry> m_layout;

public:
    VertexLayout(std::initializer_list<VertexLayoutEntry> layout);

    const std::vector<VertexLayoutEntry> &GetEntries() const {
        return m_layout;
    }
};

} // namespace phi

#endif
