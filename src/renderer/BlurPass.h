#ifndef PHI_RENDERER_BLUR_PASS_H
#define PHI_RENDERER_BLUR_PASS_H
#include "RenderPass.h"

#include "device/Device.h"
#include "device/FrameBuffer.h"
#include "device/Program.h"
#include "device/Texture.h"

namespace phi {

class BlurPass : public RenderPass {
    phi::Device &m_device;
    phi::Texture2D &m_texture;
    int m_width;
    int m_height;
    float m_radius;
    phi::FrameBuffer m_fbo1;
    phi::FrameBuffer m_fbo2;
    phi::Texture2D m_tmp;
    phi::Program m_program;

public:
    enum class Kernel {
        Gauss9x9
    };

    BlurPass(BlurPass::Kernel kernel,
             phi::Device &device,
             phi::Texture2D &texture);

    /**
     * @param radius    Blur radius (default 1.0f)
     */
    void SetRadius(float radius);

    /**
     * Runs specified Gaussian blur kernel.
     *
     * WARNING: this function changes current FrameBuffer and DOES NOT restore
     * it.
     */
    void Run();
};

} // namespace phi

#endif
