#ifndef MATERIAL_H
#define MATERIAL_H

#define GBUFFER_OUT_DECLARATION() \
    layout(location = 0) out vec4 GBufferNormal; \
    layout(location = 1) out vec4 GBufferDiffuse; \
    layout(location = 2) out vec4 GBufferSpecular;

#define GBUFFER_OUT_NORMAL(Value) GBufferNormal = (Value);
#define GBUFFER_OUT_DIFFUSE(Value) GBufferDiffuse = (Value);
#define GBUFFER_OUT_SPECULAR(Value) GBufferSpecular = (Value)

#endif
