#include "VertexLayout.h"

namespace phi {
using namespace std;

VertexLayout::VertexLayout(initializer_list<VertexLayoutEntry> layout)
        : m_layout(begin(format), end(format)) {}

} // namespace phi
