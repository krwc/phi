#ifndef PHI_LIGHT_PASS_H
#define PHI_LIGHT_PASS_H
#include <vector>

#include "math/Math.h"
#include "device/Shader.h"

#include "RenderPass.h"

namespace phi {
class Texture2D;
class DirLight;
class PointLight;

class LightPass : public RenderPass {
    phi::Program m_program;

public:
    struct Config {
        glm::mat4 shadow_matrix;
        const phi::Texture2D *shadow_map;
        const phi::Texture2D *depth_map;
        const phi::Texture2D *diffuse_map;
        const phi::Texture2D *specular_map;
        const std::vector<phi::DirLight *> *dir_lights;
        const std::vector<phi::PointLight *> *point_lights;
    };

    LightPass() {}
    void Setup(const phi::LightPass::Config &config) {}
    void Run() {}
};

} // namespace phi

#endif
