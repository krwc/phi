#include "Layout.h"

layout(location = POSITION) in vec4 in_Position;
layout(location = NORMAL) in vec3 in_Normal;
layout(location = TEXCOORD) in vec2 in_Texcoord;

uniform mat4 g_ProjViewModelMatrix;
uniform mat4 g_ViewModelMatrix;
uniform mat3 g_InvViewModelMatrix;

out vec3 Normal;
out vec4 Position;
out vec2 UV;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_Position;
    Normal = normalize(g_InvViewModelMatrix * in_Normal);
    Position = g_ViewModelMatrix * in_Position;
    UV = in_Texcoord;
}
