#line 1 "LightPass.fs"
#define NUM_MAX_DIR_LIGHTS      8
#define NUM_MAX_POINT_LIGHTS    8

struct DirLight {
    vec3 Direction;
    vec3 Color;
};

struct PointLight {
    vec3 Position;
    vec3 Color;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationQuadratic;
};

struct LightInfo {
    int NumDirLights;
    int NumPointLights;
    DirLight Dir[NUM_MAX_DIR_LIGHTS];
    PointLight Point[NUM_MAX_DIR_LIGHTS];
};

vec3 ComputeDirLightIntensity(in LightInfo Info, in vec3 Normal) {
    vec3 Result = vec3(0,0,0);
    for (uint i = 0; i < Info.NumDirLights; ++i) {
        float I = max(0.0f, dot(Normal, -Info.Dir[i].Direction));
        Result += I * Info.Dir[i].Color;
    }
    return Result;
}

vec3 ComputePointLightIntensity(in LightInfo Info,
                                in vec3 Normal,
                                in vec3 Point) {
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
        Result += I * A * Info.Point[i].Color;
    }
    return Result;
}

layout(location=0) out vec4 FragColor;

uniform LightInfo g_LightInfo;
uniform mat4 g_ShadowMatrix;
uniform mat4 g_InvProjMatrix;
uniform float g_DepthTexelSize = 1.0f/2048;
uniform sampler2DShadow g_TexShadow;
uniform sampler2D g_TexDepth;
uniform sampler2D g_TexDiffuse;
uniform sampler2D g_TexSpecular;
uniform sampler2D g_TexNormal;
uniform sampler2D g_TexAo;

in vec2 UV;

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
    const float Bias = max(0.002*abs(dot(N, L)), 0.005);
    const vec3 ProjCoords = 0.5 * Coord.xyz / Coord.w + 0.5;
    const float CurrentDepth = ProjCoords.z - Bias;
    return PCF5x5(CurrentDepth, ProjCoords.xy, vec2(0, 0));
}

float LinearDepth(in vec2 uv) {
    float z = texture(g_TexDepth, uv).r;
    float g_Near = 0.1f;
    float g_Far = 1000.0f;
    return 2.0 * g_Near * g_Far / (g_Far + g_Near - z * (g_Far - g_Near));
}


vec4 GetPosition(in vec2 uv) {
    float z = 2*texture(g_TexDepth, uv).r - 1;
    float x = uv.x*2 - 1;
    float y = uv.y*2 - 1;
    vec4 P = vec4(x, y, z, 1.0f);
    P = g_InvProjMatrix * P;
    return P /= P.w;
}

vec4 GetNormal(vec2 uv) {
    return texture(g_TexNormal, uv);
}

vec4 GetDiffuse(vec2 uv) {
    return texture(g_TexDiffuse, uv);
}

float GetAo(vec2 uv) {
//    return texture(g_TexAo, uv).r;
    const vec2 Offset = 1.0f / textureSize(g_TexAo, 0).xy;
    float Result = 0.0f;
    for (float i = -1.0; i <= 1.0; i += 1.0) {
        for (float j = -1.0; j <= 1.0; j += 1.0) {
            Result += texture(g_TexAo, uv + vec2(i, j)*Offset).r;
        }
    }
    return Result / 4.0f;
}

void main() {
    vec2 Pixel = UV;//ivec2(gl_FragCoord.xy);
    vec4 N = GetNormal(Pixel);
    if (N == vec4(0,0,0,0)) {
        FragColor = vec4(0.3,0.3,0.3,1);
    } else {
        float Ao = GetAo(Pixel);
        vec4 P = GetPosition(UV);
        vec4 DiffuseColor = GetDiffuse(Pixel);
        vec3 DirLightI = ComputeDirLightIntensity(g_LightInfo, N.xyz);
        vec3 PointLightI = ComputePointLightIntensity(g_LightInfo, N.xyz, P.xyz);

        vec4 ShadowCoord = g_ShadowMatrix * P;
        vec3 I = (1.0f - ShadowIntensity(g_ShadowMatrix * P, N.xyz,
                                         g_LightInfo.Dir[0].Direction))
                         * DirLightI
                 + PointLightI;
        vec3 Color = clamp((1-Ao) * I * DiffuseColor.xyz, 0, 1);

    //    FragColor = vec4(mix(Color, vec3(1-Ao,0,0), 0.5), 1.0);
//        FragColor = vec4(vec3(1 - Ao),1);
        FragColor = vec4(Color, 1);
//        FragColor = P;
//         FragColor = vec4(min(Color, vec3(0)) + vec3(1 - Ao), 1);
        //        FragColor = P;
    }
}
