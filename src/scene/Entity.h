#ifndef PHI_SCENE_ENTITY_H
#define PHI_SCENE_ENTITY_H

#include "math/Box.h"
#include "math/Math.h"
#include "math/Transformable.h"

namespace phi {
class CommandQueue;

class Entity : public Transformable {
public:
    virtual ~Entity() {}

    /**
     * Pushes render commands to the queue needed to properly render entity.
     */
    virtual void Render(phi::CommandQueue *) = 0;
};

} // namespace phi

#endif
