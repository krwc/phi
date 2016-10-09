#include "RenderCommandQueue.h"

#include <cassert>

namespace phi {

RenderCommandQueue::RenderCommandQueue() : m_commands() {}

void RenderCommandQueue::Insert(RenderCommand &command) {
    assert(command.primitive != PrimitiveType::Invalid);
    assert(command.count > 0);
    assert(command.offset >= 0);
    m_commands.push_back(command);
}

void RenderCommandQueue::Flush() {
    m_commands.clear();
}

void RenderCommandQueue::Execute(Renderer &renderer) const {
    // TODO: Sorting.
    for (const RenderCommand &command : m_commands) {
        // renderer.Render(command);
    }
}

} // namespace phi
