#line 1 //Quad.vs
layout(location=POSITION) in vec2 in_Position;
out vec2 UV;

void main() {
    UV = (in_Position + 1.0f) * 0.5f;
    gl_Position = vec4(in_Position, 0.0f, 1.0f);
}
