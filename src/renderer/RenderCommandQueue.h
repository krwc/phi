#ifndef PHI_RENDERER_RENDER_COMMAND_QUEUE_H
#define PHI_RENDERER_RENDER_COMMAND_QUEUE_H
#include <vector>

#include "RenderCommand.h"

namespace phi {
class Renderer;

class RenderCommandQueue {
    std::vector<RenderCommand> m_commands;

public:
    RenderCommandQueue();
    virtual ~RenderCommandQueue() {}

    /** Inserts new RenderCommand to the queue. */
    virtual void Insert(RenderCommand &command);

    /** Clears the queue. */
    virtual void Flush();

    /** Executes all rendering commands from the queue. */
    virtual void Execute(Renderer &) const;
};

} // namespace phi

#endif
