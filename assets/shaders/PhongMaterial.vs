#line 1 //PhongMaterial.vs
layout(location = POSITION) in vec4 in_Position;
layout(location = NORMAL) in vec3 in_Normal;

uniform mat4 g_ProjViewModelMatrix;
uniform mat4 g_ModelMatrix;
uniform mat4 g_InvModelMatrix;

uniform mat4 g_ViewModelMatrix;
uniform mat4 g_InvViewModelMatrix;

uniform mat4 g_InvViewMatrix;
uniform mat4 g_ViewMatrix;

out vec3 Normal;
out vec4 Position;

void main() {
    gl_Position = g_ProjViewModelMatrix * in_Position;
    Normal = normalize(transpose(mat3(g_InvViewModelMatrix)) * in_Normal);
    Position = g_ViewModelMatrix * in_Position;
}
