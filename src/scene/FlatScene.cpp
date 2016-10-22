#include "FlatScene.h"
#include "Entity.h"

#include "renderer/CommandQueue.h"

namespace phi {

void FlatScene::SetCamera(Camera *camera) {
    m_camera = camera;
}

void FlatScene::AddEntity(Entity *entity) {
    m_entities.push_back(entity);
}

void FlatScene::AddLight(DirectionalLight *light) {
    m_directional_lights.push_back(light);
}

void FlatScene::AddLight(PointLight *light) {
    m_point_lights.push_back(light);
}

void FlatScene::Render(CommandQueue *q) {
    for (auto &entity : m_entities) {
        Command command{};
        command.directional_lights = m_directional_lights;
        command.point_lights = m_point_lights;
        entity->Render(&command);
        q->Insert(command);
    }
}

} // namespace phi

