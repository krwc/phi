#ifndef PHI_RENDERER_VERTEX_FORMAT_H
#define PHI_RENDERER_VERTEX_FORMAT_H
#include <GL/gl.h>

#include <initializer_list>
#include <string>
#include <vector>

namespace phi {

enum class ElementType {
    Float = GL_FLOAT
};

struct VertexLayoutEntry {
    std::string attribute;
    int offset;
    int stride;
    ElementType type;
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
