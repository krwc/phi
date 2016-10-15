#ifndef PHI_SCENE_ENTITY_H
#define PHI_SCENE_ENTITY_H

#include "math/Math.h"
#include "math/Box.h"

namespace phi {

class Entity {
protected:
    glm::mat4 m_transform;
    phi::Box m_box;

public:
    virtual ~Entity() {}
    /**
     * Returns absolute transformation of entity.
     */
    virtual const glm::mat4 &GetTransform() const {
        return m_transform;
    }

    /**
     * Returns axis aligned bounding box of entity.
     */
    virtual const phi::Box &GetBound() const {
        return m_box;
    }

    /**
     * Pushes render commands to the queue needed to properly render entity.
     */
    virtual void Render(phi::CommandQueue &) const = 0;
};

} // namespace phi

#endif
