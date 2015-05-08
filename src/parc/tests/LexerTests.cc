
#include "TestHarness.h"

#include "..\Lexer.h"

using parc::Utf8Decoder;
using parc::Token;
using parc::Lexer;

BEGIN_TEST_CASE("Lexer_LocalTokenTest") {
  Lexer lex;
  lex.SetInput(Utf8Decoder("local"));
  auto tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kLocal);
}
END_TEST_CASE

BEGIN_TEST_CASE("Lexer_GlobalTokenTest") {
  Lexer lex;
  lex.SetInput(Utf8Decoder("Global"));
  auto tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kGlobal);
}
END_TEST_CASE

BEGIN_TEST_CASE("Lexer_LocalTokenDefinitionTest") {
  Lexer lex;
  lex.SetInput(Utf8Decoder("local = \"A\" ;"));
  auto tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kLocal);
  tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kEqualsSign);
  tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kStringLiteral);
  tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kSemicolon);
  tok = lex.Next();
  ASSERT_TRUE(tok.GetType() == Token::kEndOfFile);
}
END_TEST_CASE
