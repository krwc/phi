#line 1 "PhongMaterial.h"
layout(location = 0) out vec4 out_Normal;
layout(location = 1) out vec4 out_Diffuse;
layout(location = 2) out vec4 out_Specular;
//layout(location = 3) out vec4 out_Position;
uniform vec4 Diffuse;
uniform vec4 Specular;

in vec3 Normal;
in vec4 Position;

void main() {
    out_Normal = vec4(Normal, 1);
    out_Diffuse = Diffuse;
    out_Specular = Specular;
//    out_Position = Position;
}
