#ifndef PHI_SCENE_FLAT_SCENE_H
#define PHI_SCENE_FLAT_SCENE_H
#include "Scene.h"

#include <vector>

namespace phi {

/**
 * Very basic implementation of the Scene that keeps everything in the lists.
 */
class FlatScene : public Scene {
    std::vector<phi::Entity *> m_entities;
    std::vector<phi::PointLight *> m_point_lights;
    std::vector<phi::DirectionalLight *> m_directional_lights;
    const phi::Camera *m_camera;

public:
    virtual void SetCamera(phi::Camera *);
    virtual void AddEntity(phi::Entity *);
    virtual void AddLight(phi::DirectionalLight *);
    virtual void AddLight(phi::PointLight *);
    virtual void Render(phi::CommandQueue *);
    virtual const phi::Camera *GetCamera() const {
        return m_camera;
    }
};

} // namespace phi

#endif
