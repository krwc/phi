#include "ListScene.h"

#include "renderer/CommandQueue.h"

namespace phi {

void ListScene::SetCamera(const Camera *camera) {
    m_camera = camera;
}

void ListScene::AddEntity(const Entity *entity) {
    m_entities.push_back(entity);
}

void ListScene::AddLight(const Light *light) {
    m_lights.push_back(light);
}

void ListScene::Render(CommandQueue *q) {
    (void) q;
    return;
}

} // namespace phi

