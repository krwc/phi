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
        draw.point_lights = m_point_lights;
        draw.dir_lights = m_dir_lights;
        queue->Insert(draw);
    }
}

const phi::Box &FlatScene::GetBox() const {
    m_box = {};
    for (const auto &entity : m_entities) {
        m_box.Cover(entity->GetBox());
    }
    return m_box;
}

} // namespace phi

