
#include "Parser.h"
#include "SyntaxTree.h"

namespace parc {
SyntaxTree* Parser::MainRule() { return PackageRule(); }

SyntaxTree* Parser::PackageRule() {
  auto package_name = GetToken();
  if (Accept(Token::kStringLiteral)) {
    auto package_syntax = new PackageSyntax();
    package_syntax->Add(new TokenNode(package_name));
    Expect(Token::kLeftBrace);
    for (;;) {
      if (auto token_rule = TokenRule()) {
        package_syntax->Add(token_rule);
        continue;
      }
      break;
    }
    Expect(Token::kRightBrace);
    return package_syntax;
  }
  return nullptr;
}

SyntaxTree* Parser::TokenRule() {
  auto token_name = GetToken();
  if (Accept(Token::kLocal)) {
    auto token_definition_syntax = new TokenDefinitionSyntax();
    token_definition_syntax->Add(new TokenNode(token_name));
    Expect(Token::kEqualsSign);
    while (auto token_definition_rule = TokenDefinitionRule()) {
      token_definition_syntax->Add(token_definition_rule);
    }
    Expect(Token::kSemicolon);
    return token_definition_syntax;
  }
  return nullptr;
}

SyntaxTree* Parser::TokenDefinitionRule() {
  if (auto token_primary_syntax = TokenPrimaryRule()) {
    return token_primary_syntax;
  }
  return nullptr;
}

SyntaxTree* Parser::TokenPrimaryRule() {
  auto tok = GetToken();
  if (Accept(Token::kStringLiteral)) {
    auto token_literal = new TokenLiteralSyntax();
    token_literal->Add(new TokenNode(tok));
    return token_literal;
  }
  if (Accept(Token::kLeftParenthesis)) {
    auto syntax = TokenDefinitionRule();
    Expect(Token::kRightParenthesis);
    return syntax;
  }
  return nullptr;
}
}
