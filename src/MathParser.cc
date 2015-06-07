
#include "MathParser.h"
#include "SyntaxTree.h"

// number = ("0" - "9") + ;
// op = ("+" | "-" | "*" | "/") + ;
// Expression = PrimaryExpression
//  | PrimaryExpression op Expression
//  ;
// PrimaryExpression = number
//  | "(" Expression ")"
//  | "[" a : Expression ("," b : Expression)* "]" => a + b
//  ;

namespace parc {
SyntaxTree* MathParser::Parse() { return ParseExpression(); }

SyntaxTree* MathParser::ParseExpression() {
  auto expr = ParsePrimaryExpression();
  if (expr != nullptr) {
    if (Accept(kMathPlusSign)) {
      auto expr2 = ParseExpression();
      assert(expr2);
      auto node = new SyntaxNode("+");
      node->Add(expr);
      node->Add(expr2);
      return node;
    }
    if (Accept(kMathHypenMinus)) {
      auto expr2 = ParseExpression();
      assert(expr2);
      auto node = new SyntaxNode("-");
      node->Add(expr);
      node->Add(expr2);
      return node;
    }
    if (Accept(kMathAsterisk)) {
      auto expr2 = ParseExpression();
      assert(expr2);
      auto node = new SyntaxNode("*");
      node->Add(expr);
      node->Add(expr2);
      return node;
    }
    if (Accept(kMathSolidus)) {
      auto expr2 = ParseExpression();
      assert(expr2);
      auto node = new SyntaxNode("/");
      node->Add(expr);
      node->Add(expr2);
      return node;
    }
    return expr;
  }
  return nullptr;
}

SyntaxTree* MathParser::ParsePrimaryExpression() {
  if (auto number = ParseNumber()) {
    return number;
  }
  if (Accept(kMathLeftParenthesis)) {
    auto expr = ParseExpression();
    PARC_EXPECT(kMathRightParenthesis);
    return expr;
  }
  if (Accept(kMathLeftBracket)) {
    auto list = new SyntaxNode("list");
    int i = 0;
    while (!Accept(kMathRightBracket)) {
      if (i > 0) {
        PARC_EXPECT(kMathComma);
      }
      auto expr = ParseExpression();
      list->Add(expr);
      i++;
    }
    return list;
  }
  return nullptr;
}

SyntaxTree* MathParser::ParseNumber() {
  auto number = GetToken();
  if (Accept(kMathNumber)) {
    return new TokenNode(number);
  }
  return nullptr;
}
}
