#include "Light.h"

layout(location=0) out vec4 FragColor;

uniform LightInfo g_LightInfo;
uniform mat4 g_ShadowMatrix;
uniform mat4 g_InvProjMatrix;
uniform float g_DepthTexelSize = 1.0f/2048;
layout(binding = 0) uniform sampler2D g_TexDepth;
layout(binding = 1) uniform sampler2D g_TexNormal;
layout(binding = 2) uniform sampler2D g_TexDiffuse;
layout(binding = 3) uniform sampler2D g_TexAo;
layout(binding = 4) uniform sampler2DShadow g_TexShadow;

in vec2 UV;

float ComputeSpecularIntensity(in vec3 Incidence,
                               in vec3 Normal,
                               in vec3 ViewRay,
                               in float Power) {
    vec3 Result = vec3(0,0,0);
    vec3 R = reflect(Incidence, Normal);
    float DotRV = max(dot(R, -ViewRay), 0.0f);
    return pow(DotRV, Power);
}

vec3 ComputeDirLightIntensity(in LightInfo Info,
                              in vec3 Normal,
                              in vec3 ViewRay,
                              in float SpecularPower) {
    vec3 Result = vec3(0,0,0);
    for (uint i = 0; i < Info.NumDirLights; ++i) {
        float I = max(0.0f, dot(Normal, -Info.Dir[i].Direction));
        float S = 0.0f;
        if (Info.Dir[i].Specular && SpecularPower >= 1.0f) {
            S = ComputeSpecularIntensity(Info.Dir[i].Direction, Normal, ViewRay,
                                         SpecularPower);
        }
        Result += (I + S) * Info.Dir[i].Color;
    }
    return Result;
}

vec3 ComputePointLightIntensity(in LightInfo Info,
                                in vec3 Normal,
                                in vec3 Point,
                                in vec3 ViewRay,
                                in float SpecularPower) {
    vec3 Result = vec3(0,0,0);
    for (uint i = 0; i < Info.NumPointLights; ++i) {
        vec3 L = Info.Point[i].Position - Point;
        float D = length(L);
        if (D > 1.0f / Info.Point[i].AttenuationQuadratic) {
            continue;
        }
        float A = 1.0f / (Info.Point[i].AttenuationConstant
                            + Info.Point[i].AttenuationLinear * D
                            + Info.Point[i].AttenuationQuadratic * D * D);
        float I = max(0.0, dot(Normal, L / D));
        float S = 0.0f;
        if (Info.Point[i].Specular && SpecularPower >= 1.0f) {
            S = ComputeSpecularIntensity(L, Normal, ViewRay, SpecularPower);
        }
        Result += A * (I + S) * Info.Point[i].Color;
    }
    return Result;
}

#define PCF_IMPL(R, Size)                                                 \
    float I = 0.0f;                                                       \
    const float S = 2.0f / (Size - 1.0f);                                 \
    for (float i = -R + Offset.x; i <= R + Offset.x; i += R * S) {        \
        for (float j = -R + Offset.y; j <= R + Offset.y; j += R * S) {    \
            I += texture(g_TexShadow,                                     \
                         vec3(ProjCoords + g_DepthTexelSize * vec2(i, j), \
                              CurrentDepth))                              \
                         .r;                                              \
        }                                                                 \
    }                                                                     \
    return I / (Size * Size);

float PCF3x3(in float CurrentDepth, in vec2 ProjCoords, in vec2 Offset) {
    PCF_IMPL(1.0f, 3.0f)
}

float PCF5x5(in float CurrentDepth, in vec2 ProjCoords, in vec2 Offset) {
    PCF_IMPL(2.0f, 5.0f)
}

float PCF9x9(in float CurrentDepth, in vec2 ProjCoords, in vec2 Offset) {
    PCF_IMPL(2.5f, 9.0f)
}

float ShadowIntensity(in vec4 Coord, in vec3 N, in vec3 L) {
    const float Bias = max(0.002 * abs(dot(N, L)), 0.004);
    const vec3 ProjCoords = 0.5 * Coord.xyz / Coord.w + 0.5;
    const float CurrentDepth = ProjCoords.z - Bias;

#if defined(SHADOW_FILTERING_PCF_3x3)
    return PCF3x3(CurrentDepth, ProjCoords.xy, vec2(0, 0));
#elif defined(SHADOW_FILTERING_PCF_5x5)
    return PCF5x5(CurrentDepth, ProjCoords.xy, vec2(0, 0));
#elif defined(SHADOW_FILTERING_PCF_9x9)
    return PCF9x9(CurrentDepth, ProjCoords.xy, vec2(0, 0));
#endif
}

vec4 GetPosition(in vec2 uv) {
    float z = 2*texture(g_TexDepth, uv).r - 1;
    float x = uv.x*2 - 1;
    float y = uv.y*2 - 1;
    vec4 P = vec4(x, y, z, 1.0f);
    P = g_InvProjMatrix * P;
    return P /= P.w;
}

vec4 GetNormal(in vec2 uv) {
    return texture(g_TexNormal, uv);
}

vec4 GetDiffuse(in vec2 uv) {
    return texture(g_TexDiffuse, uv);
}

float GetAo(in vec2 uv) {
    const vec2 Offset = 1.0f / textureSize(g_TexAo, 0).xy;
    float Result = 0.0f;
    for (float i = -0.5; i <= 0.5; i += 1.0) {
        for (float j = -0.5; j <= 0.5; j += 1.0) {
            Result += texture(g_TexAo, uv + vec2(i, j)*Offset).r;
        }
    }
    return Result / 4.0f;
}

#undef DEBUG_SSAO

void main() {
    vec4 N = GetNormal(UV);
    if (N == vec4(0,0,0,0)) {
        // NOTE: Fix this with sky color.
        FragColor = vec4(0.3,0.3,0.3,1);
    } else {
        float Ao = GetAo(UV);
        vec4 P = GetPosition(UV);
        vec3 ViewRay = normalize(P.xyz);
        vec4 DiffuseColor = GetDiffuse(UV);
        float SpecularPower = DiffuseColor.w;
        vec3 DirLightI = ComputeDirLightIntensity(g_LightInfo, N.xyz, ViewRay,
                                                  SpecularPower);
        vec3 PointLightI = ComputePointLightIntensity(g_LightInfo, N.xyz, P.xyz,
                                                      ViewRay, SpecularPower);

        vec4 ShadowCoord = g_ShadowMatrix * P;
        vec3 I = (1.0f - ShadowIntensity(g_ShadowMatrix * P, N.xyz,
                                         g_LightInfo.Dir[0].Direction))
                         * DirLightI
                 + PointLightI;

        vec3 Color = clamp((1-Ao) * I * DiffuseColor.xyz, 0, 1);

#if defined(DEBUG_SSAO)
        FragColor = vec4(min(Color, vec3(0)) + vec3(1 - Ao), 1);
#else
        FragColor = vec4(Color, 1);
#endif
    }
}
