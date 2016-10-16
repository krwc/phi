#include "CommandQueue.h"

#include <cassert>

namespace phi {

CommandQueue::CommandQueue() : m_commands() {}

void CommandQueue::Insert(Command &command) {
    assert(command.primitive != PrimitiveType::Invalid);
    assert(command.count > 0);
    assert(command.offset >= 0);
    assert(command.model);
    m_commands.push_back(command);
}

void CommandQueue::Flush() {
    m_commands.clear();
}

} // namespace phi
