#pragma once

#include "Token.h"

namespace parc {
class Program;

class StackMachine {
 public:
  StackMachine() : p_(nullptr), inp_(nullptr) {}

  Program* GetProgram(Program* p) const { return p_; }
  void SetProgram(Program* p) { p_ = p; }

  TokenInputStream* GetInput() const { return inp_; }
  void SetInput(TokenInputStream* inp) { inp_ = inp; }

  // REQUIRES: SetProgram, SetInput
  bool Exec(const size_t entry_point);

 private:
  bool Accept(const int token);

  Program* p_;
  TokenInputStream* inp_;
  Token token_;
};
}
