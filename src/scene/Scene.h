#ifndef PHI_SCENE_SCENE_H
#define PHI_SCENE_SCENE_H

namespace phi {
class CommandQueue;
class Camera;
class Entity;
class Light;

class Scene {
public:
    virtual ~Scene() {}
    virtual void SetCamera(phi::Camera *) = 0;
    virtual void AddEntity(phi::Entity *) = 0;
    virtual void AddLight(phi::Light *) = 0;
    virtual void Render(phi::CommandQueue *) = 0;
    virtual const phi::Camera *GetCamera() const = 0;
};

} // namespace phi

#endif
