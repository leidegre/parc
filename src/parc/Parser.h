#pragma once

#include <cassert>

#include "Token.h"
#include "Lexer.h"

namespace parc {
// forward declarations
class SyntaxNode;

template <class TToken, class TLexer>
class ParserBase {
 public:
  ParserBase() : inp_(nullptr), token_() {}

  void SetInput(TLexer* inp) {
    assert(inp);
    inp_ = inp;
    Read();  // initialize
  }

 protected:
  // REQUIRES: SetInput
  bool Read() {
    assert(inp_);
    auto tok = inp_->Next();
    token_ = tok;
    return tok.GetType() != 0;
  }

  // REQUIRES: Read
  const TToken& GetToken() const { return token_; }

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
    // todo: how do we recover from parser error?
  }

 private:
  TLexer* inp_;
  TToken token_;
};

class Parser : public ParserBase<Token, Lexer> {
 public:
  Parser() {}

  SyntaxNode* MainRule();

  SyntaxNode* FileRule();

  SyntaxNode* PackageRule();

  SyntaxNode* TokenRule();

  SyntaxNode* TokenDefinitionRule();

  SyntaxNode* TokenPrimaryRule();
};
}
