#ifndef PHI_LIGHT_PASS_H
#define PHI_LIGHT_PASS_H
#include <vector>

#include "math/Math.h"
#include "device/Program.h"

#include "RenderPass.h"

namespace phi {
class Texture2D;
class DirLight;
class PointLight;
class Camera;
class Device;

class LightPass : public RenderPass {
public:
    struct Config {
        glm::mat4 shadow_matrix;
        const phi::Texture2D *texture_depth;
        const phi::Texture2D *texture_shadow;
        const phi::Texture2D *texture_position;
        const phi::Texture2D *texture_normal;
        const phi::Texture2D *texture_diffuse;
        const phi::Texture2D *texture_ao;
        const std::vector<phi::DirLight *> *dir_lights;
        const std::vector<phi::PointLight *> *point_lights;
        const phi::Camera *camera;
    };

    enum ShadowFiltering {
        PCF_3x3 = 0,
        PCF_5x5 = 1,
        PCF_9x9 = 2
    };

    LightPass(phi::Device &device);
    void Setup(const phi::LightPass::Config &config);
    void Run();

    /** Sets shadow filtering type */
    void SetShadowFiltering(ShadowFiltering filtering);
    /** Gets currently set shadow filtering type */
    ShadowFiltering GetShadowFiltering() const;

private:
    phi::Device &m_device;
    phi::Program m_program;
    ShadowFiltering m_shadow_filtering;
    const LightPass::Config *m_config;

    void SetupLights();
};

} // namespace phi

#endif
