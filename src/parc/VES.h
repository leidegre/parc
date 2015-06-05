#pragma once

#include "Token.h"

#include <stack>

namespace parc {
class StackMachine {
 public:
  bool Exec(const Slice& byte_code, TokenInputStream* inp);

 private:
  std::stack<int> es_;  // evaluation stack
};
}
