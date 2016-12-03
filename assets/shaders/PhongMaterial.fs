#include "Material.h"

out GBufferBlock GBuffer;

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
    GBuffer.Normal = vec4(Normal, 1);
#if !defined(ENABLE_DIFFUSE_TEXTURE)
    GBuffer.Diffuse = Diffuse;
#else
    GBuffer.Diffuse = texture(DiffuseTexture, UV);
#endif

#if !defined(ENABLE_SPECULAR_TEXTURE)
    GBuffer.Specular = Specular;
#else
    GBuffer.Specular = texture(SpecularTexture, UV);
#endif
}
