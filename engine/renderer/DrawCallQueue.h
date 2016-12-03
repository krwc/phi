#ifndef PHI_RENDERER_DRAW_CALL_QUEUE_H
#define PHI_RENDERER_DRAW_CALL_QUEUE_H
#include <vector>

#include "DrawCall.h"
#include "utils/Types.h"

namespace phi {
class Renderer;

class DrawCallQueue {
    std::vector<phi::DrawCall> m_commands;

public:
    DrawCallQueue();
    ~DrawCallQueue() {}

    /** Inserts new DrawCall to the queue. */
    void Insert(phi::DrawCall &command);

    /** Clears the queue. */
    void Flush();

    const phi::AnyRange<phi::DrawCall> GetDrawCalls() const {
        return m_commands;
    }
};

} // namespace phi

#endif
