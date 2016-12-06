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

#if defined(ENABLE_NORMAL_MAPPING)
layout(binding = 2) uniform sampler2D NormalTexture;
#endif

in vec3 Normal;
in vec4 Position;
in vec2 UV;
uniform mat4 g_ViewModelMatrix;

void main() {
#if defined(ENABLE_NORMAL_MAPPING)
    vec3 N = normalize(2.0f * texture(NormalTexture, DiffuseUvScale * UV).xyz - 1.0f);
    vec3 Q1 = dFdx(Position.xyz);
    vec3 Q2 = dFdy(Position.xyz);
    vec2 tc1 = dFdx(DiffuseUvScale * UV);
    vec2 tc2 = dFdy(DiffuseUvScale * UV);
    vec3 T = normalize(tc2.y * Q1 - tc1.y * Q2);
    vec3 B = normalize(tc2.x * Q1 - tc1.x * Q2);
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
