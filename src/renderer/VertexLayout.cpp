#include "VertexLayout.h"

#include <cassert>

namespace phi {
using namespace std;

VertexLayout::VertexLayout(initializer_list<VertexLayoutEntry> layout)
        : m_layout(begin(layout), end(layout)) {
    assert(m_layout.size() > 0 && "Empty layouts make no sense");
}

} // namespace phi
