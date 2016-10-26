#include "DrawCallQueue.h"

#include <cassert>

namespace phi {

DrawCallQueue::DrawCallQueue() : m_commands() {}

void DrawCallQueue::Insert(DrawCall &command) {
    assert(command.primitive != Primitive::Invalid);
    assert(command.count > 0);
    assert(command.offset >= 0);
    m_commands.push_back(command);
}

void DrawCallQueue::Flush() {
    m_commands.clear();
}

} // namespace phi
