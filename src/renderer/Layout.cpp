#include "Layout.h"

#include <cassert>

namespace phi {
using namespace std;

Layout::Layout(initializer_list<LayoutEntry> layout)
        : m_layout(begin(layout), end(layout)) {
    assert(m_layout.size() > 0 && "Empty layouts make no sense");
}

} // namespace phi
