#include "FlatScene.h"
#include "Entity.h"

#include <limits>

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

phi::Entity *FlatScene::Pick(const phi::Ray &ray) {
    double current = std::numeric_limits<double>::max();
    phi::Entity *result = nullptr;

    for (phi::Entity *entity : m_entities) {
        if (ray.Hits(entity->GetAABB())) {
            double temp;
            if (entity->Hit(ray, &temp) && temp < current) {
                current = temp;
                result = entity;
            }
        }
    }
    return result;
}

const phi::AABB &FlatScene::GetAABB() const {
    m_box = {};
    for (const auto &entity : m_entities) {
        m_box.Cover(entity->GetAABB());
    }
    return m_box;
}

} // namespace phi

