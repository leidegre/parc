
#include "Interpreter.h"

namespace parc {
bool ByteCodeInterpreter::Read() {
  if (!reader_.Read(&op_code_[0])) {
    return false;
  }
  uint32_t op_code = op_code_[0].uint32_;
  auto v = &op_code_[1];
  switch (op_code & 3) {
    case 3: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
    case 2: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
    case 1: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
  }
  return true;
}
}
