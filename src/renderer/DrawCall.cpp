#include "DrawCall.h"

namespace phi {

DrawCall::DrawCall()
        : primitive(PrimitiveType::Invalid),
          texture_bindings(),
          program_binding(),
          layout(nullptr),
          vbo(nullptr),
          ibo(nullptr),
          count(-1),
          offset(-1) {}

} // namespace phi
