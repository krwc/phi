#ifndef PHI_SCENE_SCENE_H
#define PHI_SCENE_SCENE_H

#include "utils/Types.h"

namespace phi {
class DrawCallQueue;
class Camera;
class AABB;
struct Ray;
class Entity;
class PointLight;
class DirLight;

class Scene {
public:
    virtual ~Scene() {}
    virtual void SetCamera(phi::Camera *) = 0;
    virtual void AddEntity(phi::Entity *) = 0;
    virtual void AddLight(phi::DirLight *) = 0;
    virtual void AddLight(phi::PointLight *) = 0;
    virtual void Render(phi::DrawCallQueue *) = 0;
    virtual const phi::Camera *GetCamera() const = 0;
    virtual const phi::AABB &GetAABB() const = 0;

    virtual const phi::AnyRange<phi::Entity *> GetEntities() const = 0;
    virtual const phi::AnyRange<phi::DirLight *> GetDirLights() const = 0;
    virtual const phi::AnyRange<phi::PointLight *> GetPointLights() const = 0;

    /**
     * @returns Closest entity being hit in the scene or nullptr if
     *          no entity were hit.
     */
    virtual phi::Entity *Pick(const phi::Ray &ray) = 0;
};

} // namespace phi

#endif
