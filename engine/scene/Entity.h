#ifndef PHI_SCENE_ENTITY_H
#define PHI_SCENE_ENTITY_H
#include <string>

#include "math/AABB.h"
#include "math/Math.h"
#include "math/Ray.h"
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
     * Tests whether @p ray hits this entity.
     *
     * @param out_t Closest hit distance from the Ray#origin .
     * @return true on hit, false otherwise
     */
    virtual bool Hit(const phi::Ray &ray, double *out_t = nullptr) const = 0;

    /**
     * Returns current world-space bounding box.
     */
    virtual phi::AABB GetAABB() const = 0;

    /**
     * @returns human readable name of the entity.
     */
    virtual const std::string &GetName() const = 0;
};

} // namespace phi

#endif
