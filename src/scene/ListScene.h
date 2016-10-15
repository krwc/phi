#ifndef PHI_SCENE_LIST_SCENE_H
#define PHI_SCENE_LIST_SCENE_H
#include "Scene.h"

#include <vector>

namespace phi {

/**
 * Very basic implementation of the Scene that keeps everything in the lists.
 */
class ListScene : public Scene {
    std::vector<const phi::Entity *> m_entities;
    std::vector<const phi::Light *> m_lights;
    const phi::Camera *m_camera;

public:
    virtual void SetCamera(const phi::Camera *);
    virtual void AddEntity(const phi::Entity *);
    virtual void AddLight(const phi::Light *);
    virtual void Render(phi::CommandQueue *);
};

} // namespace phi

#endif
