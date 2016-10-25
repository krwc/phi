#include "FlatScene.h"
#include "Entity.h"

#include "renderer/DrawCallQueue.h"

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

void FlatScene::Render(DrawCallQueue *queue) {
    for (auto &entity : m_entities) {
        DrawCall command{};
        command.directional_lights = m_directional_lights;
        command.point_lights = m_point_lights;
        entity->Render(&command);
        queue->Insert(command);
    }
}

const Box &FlatScene::GetBox() const {
    m_box = {};
    for (const auto &entity : m_entities) {
        m_box.Cover(entity->GetBox());
    }
    return m_box;
}

} // namespace phi

