#ifndef LIGHT_H
#define LIGHT_H

#define NUM_MAX_DIR_LIGHTS      8
#define NUM_MAX_POINT_LIGHTS    8

struct DirLight {
    vec3 Direction;
    vec3 Color;
    bool Specular;
};

struct PointLight {
    vec3 Position;
    vec3 Color;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationQuadratic;
    bool Specular;
};

struct LightInfo {
    int NumDirLights;
    int NumPointLights;
    DirLight Dir[NUM_MAX_DIR_LIGHTS];
    PointLight Point[NUM_MAX_DIR_LIGHTS];
};

#endif
