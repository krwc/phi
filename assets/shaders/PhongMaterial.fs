#include "Material.h"

GBUFFER_OUT_DECLARATION()

#if defined(ENABLE_DIFFUSE_TEXTURE)
layout(binding = 0) uniform sampler2D DiffuseTexture;
uniform float DiffuseUvScale = 1.0f;
#else
uniform vec4 Diffuse;
#endif

#if defined(ENABLE_SPECULAR_TEXTURE)
layout(binding = 1) uniform sampler2D SpecularTexture;
uniform float SpecularUvScale = 1.0f;
#else
uniform vec4 Specular;
#endif

#if defined(ENABLE_NORMALMAP_TEXTURE)
layout(binding = 2) uniform sampler2D NormalTexture;
uniform float NormalmapUvScale = 1.0f;
#endif

in vec3 Normal;
in vec4 Position;
in vec2 UV;
uniform mat4 g_ViewModelMatrix;

void main() {
#if defined(ENABLE_NORMALMAP_TEXTURE)
    vec3 N = normalize(2.0f * texture(NormalTexture, NormalmapUvScale * UV).xyz - 1.0f);
    // TODO: Meh, probably slow, geometry shaders for the rescue!
    vec3 dP_dx = dFdx(Position.xyz);
    vec3 dP_dy = dFdy(Position.xyz);
    vec2 dx = dFdx(NormalmapUvScale * UV);
    vec2 dy = dFdy(NormalmapUvScale * UV);
    vec3 T = normalize(dy.y * dP_dx - dx.y * dP_dy);
    vec3 B = normalize(dy.x * dP_dx - dx.x * dP_dy);
    mat3 TBN = mat3(T, B, Normal);
    GBUFFER_OUT_NORMAL(vec4(TBN * N, 1));
#else
    GBUFFER_OUT_NORMAL(vec4(Normal, 1));
#endif

#if defined(ENABLE_DIFFUSE_TEXTURE)
    GBUFFER_OUT_DIFFUSE(texture(DiffuseTexture, DiffuseUvScale * UV));
#else
    GBUFFER_OUT_DIFFUSE(Diffuse);
#endif

#if defined(ENABLE_SPECULAR_TEXTURE)
    GBUFFER_OUT_SPECULAR(texture(SpecularTexture, SpecularUvScale * UV));
#else
    GBUFFER_OUT_SPECULAR(Specular);
#endif
}
