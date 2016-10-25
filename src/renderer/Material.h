#ifndef PHI_RENDERER_MATERIAL_H
#define PHI_RENDERER_MATERIAL_H
#include <vector>

#include "DataBinding.h"

#include "utils/ProgramCache.h"

namespace phi {
class Renderer;

enum class MaterialId {
    Basic
};

class Material {
protected:
    static phi::ProgramCache g_cache;

public:
    virtual ~Material() {}

    /**
     * This method is called by the Renderer to obtain information about textures
     * that need to be bound in order to properly render this material.
     */
    virtual void
    OnPrepareTextureBindings(std::vector<phi::TextureBinding> &) const = 0;

    /**
     * This method is called by the Renderer to obtain information about program
     * that is used by the material.
     */
    virtual void OnPrepareProgramBinding(phi::ProgramBinding &) const = 0;

    /**
     * Returns identifier of this material. This information is then used by
     * the Renderer to be able to properly sort rendering commands by materials.
     */
    virtual phi::MaterialId GetId() const = 0;

    /**
     * Returns human-readable material name.
     */
    virtual std::string GetName() const = 0;
};

} // namespace phi

#endif
