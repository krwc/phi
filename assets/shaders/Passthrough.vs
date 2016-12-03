#include "Layout.h"

layout(location=POSITION) in vec4 in_Position;
uniform mat4 ProjViewModel;

void main() {
    gl_Position = ProjViewModel * in_Position;
}
