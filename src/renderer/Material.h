#ifndef PHI_RENDERER_MATERIAL_H
#define PHI_RENDERER_MATERIAL_H
#include "Texture.h"

#include "math/Math.hpp"

namespace phi {

struct Material {
    phi::Texture2D *texture_diffuse;
    phi::Texture2D *texture_normal;
    glm::vec4 color_diffuse;
    glm::vec4 color_specular;

    Material()
            : texture_diffuse(nullptr),
              texture_normal(nullptr),
              color_diffuse(1, 1, 1, 1),
              color_specular(0, 0, 0, 1) {}
};

} // namespace phi

#endif
