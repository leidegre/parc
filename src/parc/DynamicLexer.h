#pragma once

#include <cassert>
#include <map>
#include <vector>

#include "Coding.h"
#include "Token.h"

namespace parc {
class DynamicLexerNode {
 public:
  enum {
    kNone,
    kComplement = (1 << 0),
    kAccept = (1 << 1),
  };

  DynamicLexerNode()
      : lower_(0),
        upper_(0),
        flags_(0),
        token_(0),
        next_state_(nullptr),
        next_(nullptr) {}

  DynamicLexerNode(int lower, int upper, int flags = 0)
      : lower_(lower),
        upper_(upper),
        flags_(flags),
        token_(0),
        next_state_(nullptr),
        next_(nullptr) {
    assert(lower_ <= upper_);
  }

  bool CanRead(int code_point) const {
    if ((lower_ <= code_point) & (code_point <= upper_)) {
      return (flags_ & kComplement) == 0;
    }
    return (flags_ & kComplement) == kComplement;
  }

  int GetLower() const { return lower_; }

  int GetUpper() const { return upper_; }

  int GetFlags() const { return flags_; }

  int GetToken() const { return token_; }

  void SetToken(int token) {
    token_ = token;
    flags_ |= kAccept;
  }

  DynamicLexerNode* Define(int character) {
    return Define(character, character);
  }

  DynamicLexerNode* Define(int lower, int upper, int flags = 0);

  const DynamicLexerNode* GetNextState() const { return next_state_; }

  DynamicLexerNode* SetNextState(DynamicLexerNode* next_state) {
    auto temp = next_state_;
    next_state_ = next_state;
    return temp;
  }

  const DynamicLexerNode* GetNext() const { return next_; }

 private:
  int lower_;
  int upper_;
  int flags_;
  int token_;
  DynamicLexerNode* next_state_;
  DynamicLexerNode* next_;  // intrusive linked list ptr
};

class DynamicLexer : public TokenInputStream {
 public:
  DynamicLexer(const DynamicLexerNode* root) : root_(root) {}

  void SetInput(Utf8Decoder inp) { inp_ = inp; }

  virtual Token Next() override;

 private:
  const DynamicLexerNode* root_;
  Utf8Decoder inp_;
};
}
