#include "FlatScene.h"
#include "Entity.h"

#include "renderer/DrawCallQueue.h"

namespace phi {

void FlatScene::SetCamera(phi::Camera *camera) {
    m_camera = camera;
}

void FlatScene::AddEntity(phi::Entity *entity) {
    m_entities.push_back(entity);
}

void FlatScene::AddLight(phi::DirLight *light) {
    m_dir_lights.push_back(light);
}

void FlatScene::AddLight(phi::PointLight *light) {
    m_point_lights.push_back(light);
}

void FlatScene::Render(phi::DrawCallQueue *queue) {
    for (auto &entity : m_entities) {
        phi::DrawCall draw{};
        entity->Render(draw);
        queue->Insert(draw);
    }
}

const phi::AABB &FlatScene::GetAABB() const {
    m_box = {};
    for (const auto &entity : m_entities) {
        m_box.Cover(entity->GetAABB());
    }
    return m_box;
}

} // namespace phi

