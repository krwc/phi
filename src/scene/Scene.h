#ifndef PHI_SCENE_SCENE_H
#define PHI_SCENE_SCENE_H

namespace phi {
class CommandQueue;
class Camera;
class Entity;
class PointLight;
class DirectionalLight;

class Scene {
public:
    virtual ~Scene() {}
    virtual void SetCamera(phi::Camera *) = 0;
    virtual void AddEntity(phi::Entity *) = 0;
    virtual void AddLight(phi::DirectionalLight *) = 0;
    virtual void AddLight(phi::PointLight *) = 0;
    virtual void Render(phi::CommandQueue *) = 0;
    virtual const phi::Camera *GetCamera() const = 0;
};

} // namespace phi

#endif
