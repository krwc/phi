#ifndef PHI_RENDERER_DRAW_CALL_QUEUE_H
#define PHI_RENDERER_DRAW_CALL_QUEUE_H
#include <vector>

#include "DrawCall.h"

namespace phi {
class Renderer;

class DrawCallQueue {
    std::vector<phi::DrawCall> m_commands;

public:
    DrawCallQueue();
    virtual ~DrawCallQueue() {}

    /** Inserts new DrawCall to the queue. */
    virtual void Insert(phi::DrawCall &command);

    /** Clears the queue. */
    virtual void Flush();

    // TODO: After implementing custom iterator interface this must be changed.
    virtual const std::vector<phi::DrawCall> &GetDrawCalls() const {
        return m_commands;
    }
};

} // namespace phi

#endif
