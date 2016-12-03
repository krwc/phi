#include "Material.h"

GBUFFER_OUT_DECLARATION()

#if !defined(ENABLE_DIFFUSE_TEXTURE)
uniform vec4 Diffuse;
#else
layout(binding = 0) uniform sampler2D DiffuseTexture;
#endif

#if !defined(ENABLE_SPECULAR_TEXTURE)
uniform vec4 Specular;
#else
layout(binding = 1) uniform sampler2D SpecularTexture;
#endif

in vec3 Normal;
in vec4 Position;
in vec2 UV;

void main() {
    GBUFFER_OUT_NORMAL(vec4(Normal, 1));
#if !defined(ENABLE_DIFFUSE_TEXTURE)
    GBUFFER_OUT_DIFFUSE(Diffuse);
#else
    GBUFFER_OUT_DIFFUSE(texture(DiffuseTexture, UV));
#endif

#if !defined(ENABLE_SPECULAR_TEXTURE)
    GBUFFER_OUT_SPECULAR(Specular);
#else
    GBUFFER_OUT_SPECULAR(texture(SpecularTexture, UV));
#endif
}
