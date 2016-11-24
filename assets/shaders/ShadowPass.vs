#line 1 "ShadowPass.vs"
layout(location=0) in vec4 in_Position;
uniform mat4 ProjViewModelMatrix;

void main() {
    gl_Position = ProjViewModelMatrix * in_Position;
}
