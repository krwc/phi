#include "CommandQueue.h"

#include <cassert>

namespace phi {

CommandQueue::CommandQueue() : m_commands() {}

void CommandQueue::Insert(Command &command) {
    assert(command.primitive != PrimitiveType::Invalid);
    assert(command.count > 0);
    assert(command.offset >= 0);
    m_commands.push_back(command);
}

void CommandQueue::Flush() {
    m_commands.clear();
}

void CommandQueue::Execute(Renderer &renderer) const {
    // TODO: Sorting.
    for (const Command &command : m_commands) {
        // renderer.Render(command);
    }
}

} // namespace phi
