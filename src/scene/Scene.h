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
    virtual void SetCamera(const phi::Camera *) = 0;
    virtual void AddEntity(const phi::Entity *) = 0;
    virtual void AddLight(const phi::Light *) = 0;
    virtual void Render(phi::CommandQueue *) = 0;
};

} // namespace phi

#endif
