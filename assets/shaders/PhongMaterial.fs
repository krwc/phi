#include "Material.h"

out GBufferBlock GBuffer;

uniform vec4 Diffuse;
uniform vec4 Specular;

in vec3 Normal;
in vec4 Position;

void main() {
    GBuffer.Normal = vec4(Normal, 1);
    GBuffer.Diffuse = Diffuse;
    GBuffer.Specular = Specular;
}
