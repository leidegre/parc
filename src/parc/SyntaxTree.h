#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "Token.h"

namespace parc {
class SyntaxTree {
 public:
  enum {
    kNone,
    kToken,
    kSyntax,
  };
  SyntaxTree(int type) : type_(type) {}
  int GetType() const { return type_; }
  virtual void DebugString(std::string* s, size_t indent = (size_t)-1) = 0;
  virtual ~SyntaxTree() {}

 private:
  int type_;
};

class TokenNode : public SyntaxTree {
 public:
  TokenNode() : SyntaxTree(kToken), token_() {}
  TokenNode(const Token& token) : SyntaxTree(kToken), token_(token) {}
  virtual void DebugString(std::string* s, size_t indent) override;
  const Token& GetToken() const { return token_; }

 private:
  Token token_;
};

class SyntaxNode : public SyntaxTree {
 public:
  SyntaxNode(const char* label) : SyntaxTree(kSyntax), label_(label) {}
  virtual void DebugString(std::string* s, size_t indent) override;
  void Add(SyntaxTree* syntax) {
    assert(syntax);
    tree_.push_back(syntax);
  }

 private:
  const char* label_;
  std::vector<SyntaxTree*> tree_;
};

// user syntax tree
class FileSyntax : public SyntaxNode {
 public:
  FileSyntax() : SyntaxNode("file") {}
};

class PackageSyntax : public SyntaxNode {
 public:
  PackageSyntax() : SyntaxNode("package") {}
};

class TokenDefinitionSyntax : public SyntaxNode {
 public:
  TokenDefinitionSyntax() : SyntaxNode("token-definition") {}
};

class TokenLiteralSyntax : public SyntaxNode {
 public:
  TokenLiteralSyntax() : SyntaxNode("token-literal") {}
};
}
