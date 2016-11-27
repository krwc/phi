#line 1 "SsaoPass.fs"
layout(location=0) out vec4 FragColor;

uniform sampler2D g_TexNormal;
uniform sampler2D g_TexNoise;
uniform sampler2D g_TexDepth;
uniform sampler2D g_TexPosition;

uniform float g_Radius = 0.9f;
uniform float g_Power = 1.5f;
uniform float g_Strength = 1.0f;
uniform float g_Bias = 0.05f;

uniform float g_InvNoiseSize;
uniform vec2 g_ScreenSize;
uniform mat4 g_ProjMatrix;
uniform mat4 g_InvProjMatrix;

in vec2 UV;

const vec3 HemisphereSamples[] = vec3[](
vec3(0.010068,-0.019681,0.003757),
vec3(0.004099,0.009315,0.008651),
vec3(-0.065169,-0.075670,0.021531),
vec3(0.035915,0.061937,0.006561),
vec3(0.006783,0.025953,0.005791),
vec3(-0.052369,0.052545,0.038605),
vec3(-0.077930,-0.032624,0.081116),
vec3(-0.059003,0.070086,0.068791),
vec3(-0.082775,0.021815,0.007837),
vec3(0.051644,0.057534,0.061185),
vec3(-0.063532,-0.075335,0.053093),
vec3(-0.095477,0.000253,0.100952),
vec3(0.007282,-0.002652,0.000098),
vec3(0.042741,0.012587,0.038328),
vec3(0.020103,-0.020156,0.078469),
vec3(-0.075937,-0.099930,0.037710),
vec3(-0.042329,0.027630,0.059053),
vec3(-0.053277,-0.050361,0.020484),
vec3(-0.036426,-0.006079,0.019615),
vec3(-0.139285,0.097837,0.034578),
vec3(-0.011554,-0.013270,0.002057),
vec3(-0.054462,-0.017751,0.014811),
vec3(0.012470,-0.132571,0.152307),
vec3(-0.028008,-0.026417,0.032417),
vec3(-0.020591,-0.071447,0.057890),
vec3(0.104200,0.056550,0.119734),
vec3(0.061505,-0.013602,0.046343),
vec3(0.102590,0.036717,0.124970),
vec3(-0.150586,0.014460,0.019260),
vec3(-0.039143,-0.157138,0.116483),
vec3(-0.015380,0.252771,0.003692),
vec3(0.146616,-0.081548,0.108336)
);

vec3 GetNoise(in vec2 uv) {
    return texture(g_TexNoise, uv * g_ScreenSize * g_InvNoiseSize).xyz;
}

vec3 GetNormal(in vec2 uv) {
    return normalize(texture(g_TexNormal, uv).xyz);
}

vec4 GetPosition(in vec2 uv) {
    float z = 2.0 * texture(g_TexDepth, uv).r - 1.;
    float x = uv.x * 2. - 1.;
    float y = uv.y * 2. - 1.;
    vec4 P = vec4(x, y, z, 1.0f);
    P = g_InvProjMatrix * P;
    return P /= P.w;
}

void main() {
    const vec3 N = GetNormal(UV);
    const vec4 P = GetPosition(UV);
    const vec3 R = GetNoise(UV);
    vec3 T = normalize(R - N * dot(R, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    float Ao = 0.0f;

    const int SAMPLES = 32;
    for (int i = 0; i < SAMPLES; ++i) {
        vec3 Sample = TBN * HemisphereSamples[i];
        if (abs(dot(N, Sample)) <= g_Bias) {
            continue;
        }

        Sample = Sample * g_Radius * log(exp(1) + abs(P.z)) + P.xyz;

        vec4 Offset = vec4(Sample, 1.);
        Offset = g_ProjMatrix * Offset;
        Offset.xy /= Offset.w;
        Offset.xy = Offset.xy * 0.5 + 0.5;

        float SampleDepth = GetPosition(Offset.xy).z;
        float RangeCheck = abs(P.z - SampleDepth) < g_Radius ? 1.0f : 0.0f;
        Ao += RangeCheck * (Sample.z < SampleDepth ? 1.0 : 0.0);
    }
    Ao /= SAMPLES;
    Ao = clamp(Ao * g_Strength, 0, 1);

    FragColor = vec4(clamp(1 - pow(1-Ao, g_Power), 0, 1));
}
