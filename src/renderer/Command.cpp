#include "Command.h"

namespace phi {

Command::Command()
        : primitive(PrimitiveType::Invalid),
          material(nullptr),
          layout(nullptr),
          vbo(nullptr),
          ibo(nullptr),
          count(-1),
          offset(-1) {}

} // namespace phi
