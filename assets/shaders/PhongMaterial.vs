#version 430
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 g_ProjViewModelMatrix;
uniform mat4 g_ModelMatrix;
uniform mat3 g_NormalMatrix;

out vec3 Normal;
out vec4 Position;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_Position;
    Normal = normalize(g_NormalMatrix * in_Normal);
    Position = g_ModelMatrix * in_Position;
}
