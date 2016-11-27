#ifndef PHI_RENDERER_SSAO_PASS_H
#define PHI_RENDERER_SSAO_PASS_H
#include <memory>

#include "BlurPass.h"
#include "RenderPass.h"

#include "device/Device.h"
#include "device/FrameBuffer.h"
#include "device/Program.h"
#include "device/Texture.h"

#include "scene/Camera.h"

namespace phi {

class SsaoPass : public RenderPass {
public:
    struct Config {
        int fbo_width;
        int fbo_height;
        const phi::Texture2D *position;
        const phi::Texture2D *normal;
        const phi::Texture2D *depth;
        const phi::Camera *camera;
    };
    SsaoPass(phi::Device &device);
    void Setup(const phi::SsaoPass::Config &config);
    void Run();
    void SetCamera(const phi::Camera &camera);

    const phi::Texture2D &GetAoTexture() const {
        return *m_ao_texture.get();
    }

    /** Sets sampling radius around a pixel. */
    void SetRadius(float radius);
    /** Sets strength of the effect */
    void SetPower(float power);
    /** Sets angle bias. The larger the bias is, the smaller the sampling angle is. */
    void SetBias(float bias);
    /** Sets number of samples. */
    void SetNumSamples(int samples);

    /** Returns current sampling radius. */
    float GetRadius() const {
        return m_properties.radius;
    }
    float GetPower() const {
        return m_properties.power;
    }
    float GetBias() const {
        return m_properties.bias;
    }
    int GetNumSamples() const {
        return m_properties.num_samples;
    }

private:
    phi::Device &m_device;
    phi::Program m_program;
    std::unique_ptr<phi::BlurPass> m_blur_pass;
    std::unique_ptr<phi::FrameBuffer> m_fbo;
    std::unique_ptr<phi::Texture2D> m_noise;
    std::unique_ptr<phi::Texture2D> m_ao_texture;
    glm::vec2 m_screen_size;
    SsaoPass::Config m_config;

    struct {
        float radius;
        float power;
        float bias;
        int num_samples;
    } m_properties;

    void InitNoiseTexture();
    void InitProgram();
    void InitFbo();
};

} // namespace phi

#endif
