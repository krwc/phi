#ifndef PHI_RENDERER_RENDER_PASS_H
#define PHI_RENDERER_RENDER_PASS_H

namespace phi {

struct RenderPass {
    virtual void Run() = 0;
};

} // namespace phi

#endif
