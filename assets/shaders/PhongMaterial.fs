#version 430
layout(location = 0) out vec4 out_Normal;
layout(location = 1) out vec4 out_Position;
layout(location = 2) out vec4 out_Diffuse;
layout(location = 3) out vec4 out_Specular;
uniform vec4 Diffuse;
uniform vec4 Specular;

in vec3 Normal;
in vec4 Position;

void main() {
    out_Normal = vec4(Normal, 1);
    out_Position = Position;
    out_Diffuse = Diffuse;
    out_Specular = Specular;
}
