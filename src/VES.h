#pragma once

#include "Token.h"

namespace parc {
class Program;
class SyntaxTree;

class StackMachine {
 public:
  StackMachine() : p_(nullptr), inp_(nullptr) {}

  Program* GetProgram(Program* p) const { return p_; }
  void SetProgram(Program* p) { p_ = p; }

  TokenInputStream* GetInput() const { return inp_; }
  void SetInput(TokenInputStream* inp) { inp_ = inp; }

  // REQUIRES: SetProgram, SetInput
  SyntaxTree* Exec(const size_t entry_point);

 private:
  Program* p_;
  TokenInputStream* inp_;
};
}
