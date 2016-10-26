#ifndef PHI_SCENE_ENTITY_H
#define PHI_SCENE_ENTITY_H

#include "math/Box.h"
#include "math/Math.h"
#include "math/Transformable.h"

namespace phi {
struct DrawCall;

class Entity : public Transformable {
public:
    virtual ~Entity() {}

    /**
     * Fills in render command describing what needs to be done to render
     * entity.
     */
    virtual void Render(phi::DrawCall &) = 0;

    /**
     * Returns current world-space bounding box.
     */
    virtual phi::Box GetBox() const = 0;
};

} // namespace phi

#endif
