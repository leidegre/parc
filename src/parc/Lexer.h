#pragma once

#include "Slice.h"
#include "Coding.h"

namespace parc {
class Token {
 public:
  enum {
    kError = -1,
    kEndOfFile = 0,
    kStringLiteral,
    kEqualsSign,
    kLeftParenthesis,
    kRightParenthesis,
    kAsterisk,
    kPlusSign,
    kSemicolon,
    kQuestionMark,
    kGlobal,
    kLocal,
  };

  Token(int type, const Slice& s = Slice(),
        const Slice& leading_trivia = Slice())
      : type_(type), s_(s), leading_trivia_(leading_trivia) {}

  int GetType() const { return type_; }

  const Slice& GetString() const { return s_; }

 private:
  int type_;
  Slice s_;
  Slice leading_trivia_;
};

class Lexer {
 public:
  Lexer() : inp_() {}

  void SetInput(Utf8Decoder inp) { inp_ = inp; }

  Token Next();

 private:
  Utf8Decoder inp_;
};
}
