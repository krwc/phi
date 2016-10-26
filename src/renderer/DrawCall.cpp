#include "DrawCall.h"

namespace phi {

DrawCall::DrawCall()
        : primitive(phi::Primitive::Invalid),
          transform(),
          program(nullptr),
          layout(nullptr),
          vbo(nullptr),
          ibo(nullptr),
          program_constants(),
          texture_bindings(),
          count(-1),
          offset(-1) {}

} // namespace phi
