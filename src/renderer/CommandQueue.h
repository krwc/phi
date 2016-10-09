#ifndef PHI_RENDERER_COMMAND_QUEUE_H
#define PHI_RENDERER_COMMAND_QUEUE_H
#include <vector>

#include "Command.h"

namespace phi {
class Renderer;

class CommandQueue {
    std::vector<Command> m_commands;

public:
    CommandQueue();
    virtual ~CommandQueue() {}

    /** Inserts new Command to the queue. */
    virtual void Insert(Command &command);

    /** Clears the queue. */
    virtual void Flush();

    /** Executes all rendering commands from the queue. */
    virtual void Execute(Renderer &) const;
};

} // namespace phi

#endif
