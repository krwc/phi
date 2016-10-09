#ifndef PHI_RENDERER_MATERIAL_H
#define PHI_RENDERER_MATERIAL_H

namespace phi {
class Renderer;

enum class MaterialId {
    Basic
};

struct Material {
    virtual ~Material() {}
    /**
     * This method will be called by Renderer when it would decide to use this
     * material for rendering.
     *
     * It should be used to set all required Renderer parameters in order to
     * properly render an object with this material.
     */
    virtual void PrepareForRendering(Renderer &) const = 0;

    /**
     * Returns identifier of this material. This information is then used by
     * the Renderer to be able to properly sort rendering commands by materials.
     */
    virtual MaterialId GetId() const = 0;
};

} // namespace phi

#endif
