#ifndef PHI_LIGHT_PASS_H
#define PHI_LIGHT_PASS_H
#include <vector>
#include <array>

#include "math/Math.h"
#include "utils/Types.h"
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
        const phi::Texture2D *texture_normal;
        const phi::Texture2D *texture_diffuse;
        const phi::Texture2D *texture_shadow;
        const phi::Texture2D *texture_ao;
        const phi::Camera *camera;
        phi::AnyRange<phi::DirLight *> dir_lights;
        phi::AnyRange<phi::PointLight *> point_lights;
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
    const LightPass::Config *m_config;
    ShadowFiltering m_shadow_filtering;

    struct PointLightConstants {
        phi::Program::ConstantRef color;
        phi::Program::ConstantRef position;
        phi::Program::ConstantRef specular;
        phi::Program::ConstantRef attenuation_constant;
        phi::Program::ConstantRef attenuation_linear;
        phi::Program::ConstantRef attenuation_quadratic;
    };

    struct DirLightConstants {
        phi::Program::ConstantRef color;
        phi::Program::ConstantRef specular;
        phi::Program::ConstantRef direction;
    };

    std::array<PointLightConstants, 8u> m_point_light_constants;
    std::array<DirLightConstants, 8u> m_dir_light_constants;

    void CompileProgram();
    void SetupLights();
};

} // namespace phi

#endif
