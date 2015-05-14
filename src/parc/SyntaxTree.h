#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "Token.h"

namespace parc {
class SyntaxNode {
 public:
  virtual ~SyntaxNode() {}
  virtual void DebugString(std::string* s, size_t indent = 0) = 0;
};

class SyntaxTree : public SyntaxNode {
 public:
  SyntaxTree(const char* label) : label_(label) {}
  virtual void DebugString(std::string* s, size_t indent) override;
  void Add(SyntaxNode* syntax) {
    assert(syntax);
    tree_.push_back(syntax);
  }

 private:
  const char* label_;
  std::vector<SyntaxNode*> tree_;
};

class TokenNode : public SyntaxNode {
 public:
  TokenNode() : token_() {}
  TokenNode(const Token& token) : token_(token) {}
  virtual void DebugString(std::string* s, size_t indent) override;

  Token token_;
};

// user syntax tree
class FileSyntax : public SyntaxTree {
 public:
  FileSyntax() : SyntaxTree("file") {}
};

class PackageSyntax : public SyntaxTree {
 public:
  PackageSyntax() : SyntaxTree("package") {}
};

class TokenDefinitionSyntax : public SyntaxTree {
 public:
  TokenDefinitionSyntax() : SyntaxTree("token-definition") {}
};

class TokenLiteralSyntax : public SyntaxTree {
 public:
  TokenLiteralSyntax() : SyntaxTree("token-literal") {}
};
}
