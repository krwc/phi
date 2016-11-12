#ifndef PHI_RENDERER_BLUR_PASS_H
#define PHI_RENDERER_BLUR_PASS_H
#include "RenderPass.h"

#include "device/Device.h"
#include "device/FrameBuffer.h"
#include "device/Program.h"
#include "device/Texture.h"

namespace phi {

class BlurPass : public RenderPass {
public:
    enum class Kernel {
        Gauss9x9
    };

    struct Config {
        phi::BlurPass::Kernel kernel;
        phi::Texture2D *texture;
        float radius = 1.0f;
    };

    BlurPass(phi::Device &device, const phi::BlurPass::Config &config);
    void SetRadius(float radius);
    /**
     * Runs specified Gaussian blur kernel.
     *
     * WARNING: this function changes current FrameBuffer and DOES NOT restore
     * it.
     */
    void Run();

private:
    phi::Device &m_device;
    phi::Texture2D *m_texture;
    int m_width;
    int m_height;

    phi::FrameBuffer m_fbo1;
    phi::FrameBuffer m_fbo2;
    phi::Texture2D m_tmp;

    glm::vec2 m_inv_size;
    glm::vec2 m_blur_x;
    glm::vec2 m_blur_y;
    phi::Program m_program;
};

} // namespace phi

#endif
