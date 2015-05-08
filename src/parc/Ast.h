#pragma once

#include <string>
#include <vector>

namespace parc {
// imports
using std::vector;
using std::string;

struct Token {
  enum Kind {
    kEndOfFile,
    kLiteral,
    kLeftParen,
    kRightParen,
    kSeparator,
    kRange,
    kBind,
    kArrow,
    kAssign,
    kEndOfRule,
    kLeftBrace,
    kRightBrace,
    kChoice,
    kGlobal,
    kLocal,
    kRepetition,
    kRepetitionAtLeastOnce,
    kOptional,
    kNot,
    kError = 255
  };
  string s;
  Kind kind;
  int lineNum;
  int charPos;
  Token() : s(), kind(), lineNum(), charPos() {}
  Token(const char* s, const Kind kind)
      : s(s), kind(kind), lineNum(), charPos() {}
  Token(string&& s, const Kind kind)
      : s(std::move(s)), kind(kind), lineNum(), charPos() {}
};

//
// productions
//

class Node;

// user-defined (forward declarations)
class GrammarNode;
class PackageNode;
class OptionNode;
class TokenNode;
class TokenLiteralNode;
class TokenLiteralRangeNode;
class TokenLiteralNegativeNode;
class EvalNode;
class AnyNode;  // aka switch (or choice) node
class TokenDeclarationNode;
class RepetitionNode;
class ProductionNode;

//
// visitor template
//

class IVisitor {
 public:
  virtual Node* Visit(GrammarNode*) = 0;
  virtual Node* Visit(PackageNode*) = 0;
  virtual Node* Visit(OptionNode*) = 0;
  virtual Node* Visit(TokenNode*) = 0;
  virtual Node* Visit(TokenLiteralNode*) = 0;
  virtual Node* Visit(TokenLiteralRangeNode*) = 0;
  virtual Node* Visit(TokenLiteralNegativeNode*) = 0;
  virtual Node* Visit(EvalNode*) = 0;
  virtual Node* Visit(AnyNode*) = 0;
  virtual Node* Visit(RepetitionNode*) = 0;
  virtual Node* Visit(TokenDeclarationNode*) = 0;
  virtual Node* Visit(ProductionNode*) = 0;
};

//
// user-defined (definitions)
//

class Node {
 public:
  virtual ~Node() {}
  virtual void Accept(IVisitor* visitor) = 0;
};

class GrammarNode : public Node {
 public:
  virtual ~GrammarNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  vector<PackageNode*> Packages;
};

class PackageNode : public Node {
 public:
  virtual ~PackageNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token Name;
  vector<Node*> m_Definitions;
};

class OptionNode : public Node {
 public:
  virtual ~OptionNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token Name;
  Token Value;
};

class TokenNode : public Node {
 public:
  virtual ~TokenNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token m_Id;
  vector<Node*> m_Expressions;
};

class TokenLiteralNode : public Node {
 public:
  virtual ~TokenLiteralNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token m_Value;
};

class TokenLiteralRangeNode : public Node {
 public:
  virtual ~TokenLiteralRangeNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token m_Begin;
  Token m_End;
};

class TokenLiteralNegativeNode : public Node {
 public:
  virtual ~TokenLiteralNegativeNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Node* m_Expression;
};

class EvalNode : public Node {
 public:
  virtual ~EvalNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Node* m_Expression;
};

class AnyNode : public Node {
 public:
  virtual ~AnyNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  vector<Node*> m_Expressions;
};

class RepetitionNode : public Node {
 public:
  virtual ~RepetitionNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Node* m_Expression;
  Token m_Cardinality;
};

class TokenDeclarationNode : public Node {
 public:
  virtual ~TokenDeclarationNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
  Token m_Local;
  Node* m_Expression;
};

class ProductionNode : public Node {
 public:
  virtual ~ProductionNode() {}
  virtual void Accept(IVisitor* visitor) { visitor->Visit(this); }
};
}