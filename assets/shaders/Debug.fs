#version 430
layout(location=0) out vec4 out_FragColor;
uniform vec4 Color;

void main() {
    out_FragColor = Color;
}
