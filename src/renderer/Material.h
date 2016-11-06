#ifndef PHI_RENDERER_MATERIAL_H
#define PHI_RENDERER_MATERIAL_H
#include <vector>

#include "DataBinding.h"

#include "utils/ProgramCache.h"

namespace phi {
class Renderer;

enum class MaterialId {
    Basic,
    Phong
};

class Material {
protected:
    static phi::ProgramCache g_cache;

public:
    virtual ~Material() {}

    /**
     * Fills passed vector with textures used by this material.
     */
    virtual void
    FillTextureBindings(std::vector<phi::TextureBinding> &) const = 0;

    /**
     * Fills passed vector with constant names and pointers to the data where
     * their values are stored.
     */
    virtual void
    FillProgramConstants(std::vector<phi::ProgramConstant> &) const = 0;

    /**
     * @returns Pointer to the program being utilized by this material.
     */
    virtual phi::Program *GetProgram() = 0;

    /**
     * @returns Identifier of this material.
     */
    virtual phi::MaterialId GetId() const = 0;

    /**
     * @returns Human-readable material name.
     */
    virtual std::string GetName() const = 0;
};

} // namespace phi

#endif
