#include "ListScene.h"
#include "Entity.h"

#include "renderer/CommandQueue.h"

namespace phi {

void ListScene::SetCamera(Camera *camera) {
    m_camera = camera;
}

void ListScene::AddEntity(Entity *entity) {
    m_entities.push_back(entity);
}

void ListScene::AddLight(Light *light) {
    m_lights.push_back(light);
}

void ListScene::Render(CommandQueue *q) {
    for (auto &entity : m_entities) {
        entity->Render(q);
    }
}

} // namespace phi

