#ifndef PHI_RENDERER_RENDERER_H
#define PHI_RENDERER_RENDERER_H

namespace phi {
class Scene;

class Renderer {
public:
    virtual ~Renderer() {}
    virtual void Render(phi::Scene *) = 0;
    virtual void Resize(int width, int height) = 0;
};

} // namespace phi

#endif
