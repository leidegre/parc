
#include "TestHarness.h"

#include "../DynamicLexer.h"
#include "../SyntaxTree.h"
#include "../MathParser.h"

namespace {
parc::DynamicLexerNode* GetLexerTree() {
  using namespace parc;

  auto root = new DynamicLexerNode();

  auto number2 = new DynamicLexerNode('0', '9');
  number2->SetToken(kMathNumber);
  number2->SetNextState(number2);

  auto number = root->Define('0', '9');
  number->SetToken(kMathNumber);
  number->SetNextState(number2);

  auto plus_sign = root->Define('+');
  plus_sign->SetToken(kMathPlusSign);

  auto hypen_minus = root->Define('-');
  hypen_minus->SetToken(kMathHypenMinus);

  auto asterisk = root->Define('*');
  asterisk->SetToken(kMathAsterisk);

  auto solidus = root->Define('/');
  solidus->SetToken(kMathSolidus);

  auto left_paren = root->Define('(');
  left_paren->SetToken(kMathLeftParenthesis);

  auto right_paren = root->Define(')');
  right_paren->SetToken(kMathRightParenthesis);

  auto left_bracket = root->Define('[');
  left_bracket->SetToken(kMathLeftBracket);

  auto right_bracket = root->Define(']');
  right_bracket->SetToken(kMathRightBracket);

  auto comma = root->Define(',');
  comma->SetToken(kMathComma);

  return root;
}
}

BEGIN_TEST_CASE("RecursiveDescentParserTest") {
  using namespace parc;
  DynamicLexer lexer(GetLexerTree());
  lexer.SetInput(Slice("1-(2+3)"));
  MathParser parser;
  parser.SetInput(&lexer);
  auto syntax = parser.Parse();
  std::string s;
  syntax->DebugString(&s);
  TEST_OUTPUT(s);
  ASSERT_EQ("(- 1 (+ 2 3))", s);
}
END_TEST_CASE

BEGIN_TEST_CASE("RecursiveDescentParserTest") {
  using namespace parc;
  DynamicLexer lexer(GetLexerTree());
  lexer.SetInput(Slice("(1-2)+3"));
  MathParser parser;
  parser.SetInput(&lexer);
  auto syntax = parser.Parse();
  std::string s;
  syntax->DebugString(&s);
  TEST_OUTPUT(s);
  ASSERT_EQ("(+ (- 1 2) 3)", s);
}
END_TEST_CASE

BEGIN_TEST_CASE("RecursiveDescentParserTest") {
  using namespace parc;
  DynamicLexer lexer(GetLexerTree());
  lexer.SetInput(Slice("[1+2,3-4]"));
  MathParser parser;
  parser.SetInput(&lexer);
  auto syntax = parser.Parse();
  std::string s;
  syntax->DebugString(&s);
  TEST_OUTPUT(s);
  ASSERT_EQ("(list (+ 1 2) (- 3 4))", s);
}
END_TEST_CASE
