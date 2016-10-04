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
