#pragma once

#include "Coding.h"
#include "Token.h"

namespace parc {
class Lexer {
 public:
  Lexer() : inp_() {}

  void SetInput(Utf8Decoder inp) { inp_ = inp; }

  Token Next();

 private:
  Utf8Decoder inp_;
};
}
