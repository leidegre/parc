#pragma once

#include "Slice.h"

#include <vector>

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

class TokenInputStream {
 public:
  virtual Token Next() = 0;
  virtual ~TokenInputStream() {}
};

class TokenList : public TokenInputStream {
 public:
  // non-copyable
  TokenList(const TokenList&) = delete;
  TokenList& operator=(const TokenList&) = delete;

  TokenList() : position_(0), token_stream_() {}

  virtual Token Next() override {
    if (position_ < token_stream_.size()) {
      return token_stream_[position_++];
    }
    return Token(Token::kEndOfFile);
  }

  void Add(const Token& token) { token_stream_.push_back(token); }

  bool IsEndOfFile() { return !(position_ < token_stream_.size()); }

 private:
  size_t position_;
  std::vector<Token> token_stream_;
};
}
