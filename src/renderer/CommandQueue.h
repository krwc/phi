#ifndef PHI_RENDERER_COMMAND_QUEUE_H
#define PHI_RENDERER_COMMAND_QUEUE_H
#include <vector>

#include "Command.h"

namespace phi {
class Renderer;

class CommandQueue {
    std::vector<phi::Command> m_commands;

public:
    CommandQueue();
    virtual ~CommandQueue() {}

    /** Inserts new Command to the queue. */
    virtual void Insert(phi::Command &command);

    /** Clears the queue. */
    virtual void Flush();

    // TODO: After implementing custom iterator interface this must be changed.
    virtual const std::vector<phi::Command> &GetCommands() const {
        return m_commands;
    }
};

} // namespace phi

#endif
