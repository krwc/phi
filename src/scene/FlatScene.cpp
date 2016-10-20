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

void FlatScene::AddLight(Light *light) {
    m_lights.push_back(light);
}

void FlatScene::Render(CommandQueue *q) {
    for (auto &entity : m_entities) {
        entity->Render(q);
    }
}

} // namespace phi

