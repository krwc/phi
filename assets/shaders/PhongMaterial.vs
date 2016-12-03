#include "Layout.h"

layout(location = POSITION) in vec4 in_Position;
layout(location = NORMAL) in vec3 in_Normal;

uniform mat4 g_ProjViewModelMatrix;
uniform mat4 g_ViewModelMatrix;
uniform mat3 g_InvViewModelMatrix;

out vec3 Normal;
out vec4 Position;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_Position;
    Normal = normalize(g_InvViewModelMatrix * in_Normal);
    Position = g_ViewModelMatrix * in_Position;
}
