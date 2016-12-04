#include "Material.h"

GBUFFER_OUT_DECLARATION()

#if defined(ENABLE_DIFFUSE_TEXTURE)
layout(binding = 0) uniform sampler2D DiffuseTexture;
#else
uniform vec4 Diffuse;
#endif

#if defined(ENABLE_SPECULAR_TEXTURE)
layout(binding = 1) uniform sampler2D SpecularTexture;
#else
uniform vec4 Specular;
#endif

#if defined(ENABLE_NORMAL_MAPPING)
layout(binding = 2) uniform sampler2D NormalTexture;
#endif

in vec3 Normal;
in vec4 Position;
in vec2 UV;

void main() {
#if defined(ENABLE_NORMAL_MAPPING)
    GBUFFER_OUT_NORMAL(texture(NormalTexture, UV));
#else
    GBUFFER_OUT_NORMAL(vec4(Normal, 1));
#endif

#if defined(ENABLE_DIFFUSE_TEXTURE)
    GBUFFER_OUT_DIFFUSE(texture(DiffuseTexture, UV));
#else
    GBUFFER_OUT_DIFFUSE(Diffuse);
#endif

#if defined(ENABLE_SPECULAR_TEXTURE)
    GBUFFER_OUT_SPECULAR(texture(SpecularTexture, UV));
#else
    GBUFFER_OUT_SPECULAR(Specular);
#endif
}
