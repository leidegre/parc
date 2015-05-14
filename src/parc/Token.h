#pragma once

#include "Slice.h"

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
    // left curly bracket
    kLeftBrace,
    // vertical line
    kPipe,
    // right curly bracket
    kRightBrace,
  };

  Token() : type_(-1), s_(), leading_trivia_() {}

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
}
