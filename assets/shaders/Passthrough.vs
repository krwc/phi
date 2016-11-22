#version 430
layout(location=0) in vec4 in_Position;
uniform mat4 ProjViewModel;

void main() {
    gl_Position = ProjViewModel * in_Position;
}
