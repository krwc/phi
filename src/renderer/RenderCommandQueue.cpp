#include "RenderCommandQueue.h"

namespace phi {

RenderCommandQueue::RenderCommandQueue() : m_commands() {}

void RenderCommandQueue::Insert(RenderCommand &command) {
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
