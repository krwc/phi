#include "Layout.h"
#include "Utils.h"

#include <cassert>
#include <stdexcept>

namespace phi {
using namespace std;

int LayoutTypeUnits(phi::Layout::Type type) {
    switch (type) {
    case Layout::Float:
        return 1;
    case Layout::Float2:
        return 2;
    case Layout::Float3:
        return 3;
    case Layout::Float4:
        return 4;
    default:
        throw std::invalid_argument("Unsupported layout type");
    }
}

Layout::Layout(size_t stride, initializer_list<Layout::Entry> layout)
        : m_id(GL_NONE), m_stride(stride) {
    CheckedCall(phi::glCreateVertexArrays, 1, &m_id);
    for (const phi::Layout::Entry &entry: layout) {
        CheckedCall(phi::glEnableVertexArrayAttrib, m_id, entry.location);
        CheckedCall(phi::glVertexArrayAttribFormat, m_id, entry.location,
                    LayoutTypeUnits(entry.type), GL_FLOAT, GL_FALSE,
                    entry.offset);
        CheckedCall(phi::glVertexArrayAttribBinding, m_id, entry.location, 0u);
    }
}

Layout::~Layout() {
    CheckedCall(phi::glDeleteVertexArrays, 1, &m_id);
}

GLuint Layout::GetId() const {
    return m_id;
}

size_t Layout::GetStride() const {
    return m_stride;
}

} // namespace phi
