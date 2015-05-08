#pragma once

#include "Lexer.h"
#include "Ast.h"

#include <memory>

namespace parc {
template <class T>::std::unique_ptr<T> Alloc() {
  return ::std::unique_ptr<T>(new T);
}

class ParserError {
public:
  ParserError() {}
  ParserError(const char *) {}
};

class Parser {
public:
  Parser(Lexer &lex) : m_Lex(lex), m_Index(0) { Next(); }
  GrammarNode *GrammarRule() {
    auto g = new GrammarNode;

    while (auto package = PackageRule()) {
      g->Packages.push_back(package);
    }

    return g;
  }
  PackageNode *PackageRule() {
    if (Accept(Token::kLiteral)) {
      auto p = Alloc<PackageNode>();

      p->Name = Prev();

      Expect(Token::kLeftBrace);

      for (;;) {
        if (auto option = OptionRule()) {
          p->m_Definitions.push_back(option);
        } else if (auto token = TokenRule()) {
          p->m_Definitions.push_back(token);
        } else if (auto production = ProductionRule()) {
          p->m_Definitions.push_back(production);
        } else {
          break;
        }
      }

      Expect(Token::kRightBrace);

      return p.release();
    }

    return nullptr;
  }
  OptionNode *OptionRule() {
    if (Accept(Token::kLiteral)) {
      auto p = Alloc<OptionNode>();

      p->Name = Prev();

      Expect(Token::kLiteral);

      p->Value = Prev();

      Expect(Token::kEndOfRule);

      return p.release();
    }

    return nullptr;
  }
  TokenNode *TokenRule() {
    if (Accept(Token::kLocal)) {
      auto t = Alloc<TokenNode>();

      t->m_Id = Prev();

      Expect(Token::kAssign);

      while (auto tokDef = TokenDeclaration()) {
        t->m_Expressions.push_back(tokDef);
      }

      Expect(Token::kEndOfRule);

      return t.release();
    }

    return nullptr;
  }
  Node *TokenDeclaration() {
    if (Accept(Token::kLocal)) {
      auto a = Alloc<TokenDeclarationNode>();

      a->m_Local = Prev();

      Expect(Token::kBind);

      if (auto tokRep = TokenRepetition()) {
        a->m_Expression = tokRep;
      } else {
        throw ParserError("Expected TokenRepetition.");
      }

      return a.release();
    }

    return TokenRepetition();
  }
  Node *TokenRepetition() {
    if (auto tokDef = TokenDefinition()) {
      if (Accept(Token::kRepetition) || Accept(Token::kRepetitionAtLeastOnce) ||
          Accept(Token::kOptional)) {
        auto a = Alloc<RepetitionNode>();

        a->m_Expression = tokDef;
        a->m_Cardinality = Prev();

        return a.release();
      }

      return tokDef;
    }

    return nullptr;
  }
  Node *TokenDefinition() {
    if (auto tokenExpr = TokenPrimaryExpression()) {
      if (Accept(Token::kChoice)) {
        auto c = Alloc<AnyNode>();

        c->m_Expressions.push_back(tokenExpr);

        if (auto tokenExpr2 = TokenPrimaryExpression()) {
          c->m_Expressions.push_back(tokenExpr2);

          while (Accept(Token::kChoice)) {
            if (auto tokenExpr3 = TokenPrimaryExpression()) {
              c->m_Expressions.push_back(tokenExpr3);
            } else {
              throw ParserError("Expected TokenPrimaryExpression.");
            }
          }

          return c.release();
        } else {
          throw ParserError("Expected TokenPrimaryExpression.");
        }
      } else {
        return tokenExpr;
      }
    }

    return nullptr;
  }
  Node *TokenPrimaryExpression() {
    if (Accept(Token::kLiteral)) {
      auto a = Prev();

      if (Accept(Token::kRange)) {
        auto b = Alloc<TokenLiteralRangeNode>();

        b->m_Begin = a;

        Expect(Token::kLiteral);

        b->m_End = Prev();

        return b.release();
      } else {
        auto b = Alloc<TokenLiteralNode>();

        b->m_Value = a;

        return b.release();
      }
    } else if (Accept(Token::kNot)) {
      auto a = Alloc<TokenLiteralNegativeNode>();

      if (auto expr = TokenPrimaryExpression()) {
        a->m_Expression = expr;
      } else {
        throw ParserError("Expected TokenPrimaryExpression.");
      }

      return a.release();
    } else if (Accept(Token::kLeftParen)) {
      auto a = Alloc<EvalNode>();

      if (auto expr = TokenPrimaryExpression()) {
        a->m_Expression = expr;
      } else {
        throw ParserError("Expected TokenPrimaryExpression.");
      }

      Expect(Token::kRightParen);

      return a.release();
    }

    return nullptr;
  }
  ProductionNode *ProductionRule() { return nullptr; }

private:
  // Gets the last accepted token.
  // After a successful call to Accept() or Expect() that token can be retrived
  // by Prev()
  // until the next successful call to Accept() or Expect()
  const Token &Prev() { return m_Current[(m_Index & 1)]; }
  const Token &Curr() { return m_Current[((m_Index - 1) & 1)]; }
  void Next() { m_Current[((m_Index++) & 1)] = m_Lex.next(); }
  bool Accept(Token::Kind kind) {
    if (Curr().kind == kind) {
      Next();
      return true;
    }
    return false;
  }
  void Expect(Token::Kind kind) {
    if (Curr().kind == kind) {
      Next();
    } else {
      throw ParserError();
    }
  }
  Lexer &m_Lex;
  Token m_Current[2];
  int m_Index;
};
}
