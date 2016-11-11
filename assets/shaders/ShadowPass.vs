#version 430
layout(location=0) in vec4 in_Position;
uniform mat4 g_ProjViewModelMatrix;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_Position;
}
