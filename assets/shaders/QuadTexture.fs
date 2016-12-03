layout(location = 0) out vec4 out_FragColor;
layout(binding = 0) uniform sampler2D Image;

in vec2 UV;

void main() {
    out_FragColor = texture(Image, UV);
}
