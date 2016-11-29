#line 1 //Outline.fs
layout(location=0) out vec4 FragColor;

uniform sampler2D ObjectMask;
uniform sampler2D SceneDepth;
uniform vec3 OutlineColor = vec3(1, 0, 0);

float GetMask(in ivec2 Coord) {
    return texelFetch(ObjectMask, Coord, 0).r;
}

void main() {
    const int Radius = 2;
    const ivec2 Base = ivec2(gl_FragCoord.xy);
    const float BaseMask = GetMask(Base);
    bool FoundEdge = false;

    for (int i = -Radius; i <= Radius; i++) {
        for (int j = -Radius; j <= Radius; j++) {
            if (abs(GetMask(Base + ivec2(i, j)) - BaseMask) > 0) {
                const float PixelDepth = texelFetch(SceneDepth, Base, 0).r;
                // Outline behind will be more transculent
                const float Intensity = PixelDepth < gl_FragCoord.z ? 0.1 : 1.0;
                FragColor = vec4(Intensity * OutlineColor, Intensity);
                return;
            }
        }
    }
    discard;
}
