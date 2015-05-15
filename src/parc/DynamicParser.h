#pragma once

#include <stack>

namespace parc {
class TokenInputStream;
class SyntaxTree;

class DynamicParserNode {
 public:
  DynamicParserNode() : next_(nullptr), tran_(nullptr), token_(0), flags_(0) {}

  enum {
    kNone,
    kAccept = (1 << 0),
    kExpect = (1 << 1),
  };

  void SetAccept(int token) {
    token_ = token;
    flags_ |= kAccept;
  }

  void SetExpect(int token) {
    token_ = token;
    flags_ |= kExpect;
  }

  int GetToken() const { return token_; }

  int GetFlags() const { return flags_; }

  const DynamicParserNode* GetNext() const { return next_; }

  DynamicParserNode* SetNext(DynamicParserNode* next) { next_ = next; }

  const DynamicParserNode* GetTransition() const { return tran_; }

  void SetTransition(DynamicParserNode* transition) { tran_ = transition; }

  void Append(DynamicParserNode* next) {
    auto node = this;
    while (node->next_ != nullptr) {
      node = node->next_;
    }
    node->next_ = next;
  }

 private:
  DynamicParserNode* next_;
  DynamicParserNode* tran_;
  int token_;
  int flags_;
};

class DynamicParser {
 public:
  DynamicParser(const DynamicParserNode* root) : parser_(), symbols_() {
    parser_.push(root);
  }

  SyntaxTree* Parse(TokenInputStream* inp);

 private:
  std::stack<const DynamicParserNode*> parser_;
  std::stack<SyntaxTree*> symbols_;
};
}
