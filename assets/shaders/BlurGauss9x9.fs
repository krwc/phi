#line 1 //BlurGauss9x9.fs
layout(location=0) out vec4 FragColor;
uniform sampler2D Texture;
uniform vec2 InvSize;
uniform vec2 Step;

// Magic taken from: http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
void main() {
    const vec2 UV = vec2(gl_FragCoord) * InvSize;
    vec4 Result = vec4(0,0,0,0);
    Result += texture(Texture, UV + Step * 1.3846153846) * 0.3162162162;
    Result += texture(Texture, UV + Step * 3.2307692308) * 0.0702702703;
    Result += texture(Texture, UV) * 0.2270270270;
    Result += texture(Texture, UV - Step * 1.3846153846) * 0.3162162162;
    Result += texture(Texture, UV - Step * 3.2307692308) * 0.0702702703;
    FragColor = Result;
}
