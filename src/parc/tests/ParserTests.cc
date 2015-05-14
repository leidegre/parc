
#include "TestHarness.h"

#include "../Parser.h"
#include "../SyntaxTree.h"

BEGIN_TEST_CASE("Parser_TokenTest") {
  auto s = "\"parc\" { token = \"a\" ; }";

  parc::Lexer lexer;
  lexer.SetInput(parc::Utf8Decoder(s));

  parc::Parser parser;
  parser.SetInput(&lexer);

  auto syntax = parser.MainRule();

  ASSERT_TRUE(syntax != nullptr);

  std::string s2;
  syntax->DebugString(&s2);
  TEST_OUTPUT(s2);
}
END_TEST_CASE
