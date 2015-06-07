#pragma once

#include "Token.h"

#define PARC_EXPECT(token) \
  if (!Accept(token)) assert(false && "parser error: expected " #token);

namespace parc {
class ParserBase {
 public:
  ParserBase() : inp_(nullptr), token_() {}

  void SetInput(TokenInputStream* inp) {
    assert(inp);
    inp_ = inp;
    Read();  // initialize
  }

 protected:
  // REQUIRES: SetInput
  bool Read() {
    assert(inp_);
    if (inp_->MoveNext()) {
      token_ = inp_->GetCurrent();
      return true;
    }
    token_ = Token(Token::kEndOfFile);
    return false;
  }

  // REQUIRES: Read
  const Token& GetToken() const { return token_; }

  // REQUIRES: Read
  bool Accept(int kind) {
    if (GetToken().GetType() == kind) {
      Read();
      return true;
    }
    return false;
  }

  // REQUIRES: Read
  void Expect(int kind) {
    if (GetToken().GetType() == kind) {
      Read();
    }
    assert(false && "parser error");  // todo: how do we manage parser error?
  }

 private:
  TokenInputStream* inp_;
  Token token_;
};
}
