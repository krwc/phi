#version 430
layout(location=0) out vec4 out_FragColor;
uniform sampler2D Image;

in vec2 UV;

void main() {
    out_FragColor = texelFetch(Image, ivec2(UV), 0);
}