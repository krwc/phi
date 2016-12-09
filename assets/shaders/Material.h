#ifndef MATERIAL_H
#define MATERIAL_H

#define GBUFFER_OUT_DECLARATION() \
    layout(location = 0) out vec4 GBufferNormal; \
    layout(location = 1) out vec4 GBufferColor; \

#define GBUFFER_OUT_NORMAL(Value) GBufferNormal = (Value)
#define GBUFFER_OUT_DIFFUSE(Value) GBufferColor.xyz = (Value).xyz
#define GBUFFER_OUT_SPECULAR_POWER(Value) GBufferColor.w = (Value)

#endif
