
#include "TestHarness.h"

#include "..\DynamicParser.h"
#include "..\Emit.h"
#include "..\Program.h"
#include "..\VES.h"
#include "..\Utility.h"
#include "..\SyntaxTree.h"

namespace {
using namespace parc;
enum {
  kTokenNumber,
  kTokenOperator,
  kTokenLeftParenthesis,
  kTokenRightParenthesis
};
// parser without tree construction
DynamicParserNode* GetParser() {
  auto primary = new DynamicParserAcceptNode(kTokenNumber);
  auto expr = new DynamicParserApplyNode(primary);
  auto lparen = new DynamicParserAcceptNode(kTokenLeftParenthesis);
  auto eval = new DynamicParserApplyNode(expr);
  auto rparen = new DynamicParserAcceptNode(kTokenRightParenthesis);
  auto binary = new DynamicParserAcceptNode(kTokenOperator);
  auto expr2 = new DynamicParserApplyNode(expr);

  primary->SetLabel("PrimaryExpression");
  primary->SetTarget(new DynamicParserReturnNode());
  primary->SetNext(lparen);

  rparen->SetTarget(new DynamicParserReturnNode());
  rparen->SetNext(new DynamicParserErrorNode());

  eval->SetNext(rparen);

  lparen->SetTarget(eval);
  lparen->SetNext(new DynamicParserReturnNode());

  expr2->SetNext(new DynamicParserReturnNode());

  binary->SetTarget(expr2);
  binary->SetNext(new DynamicParserReturnNode());

  expr->SetLabel("Expression");
  expr->SetNext(binary);

  return expr;
}

// parser with tree construction
DynamicParserNode* GetParser2() {
  auto primary = new DynamicParserAcceptNode(kTokenNumber);
  auto literal_number = new DynamicParserSelectNode(1, "NumberLiteral");
  auto expr = new DynamicParserApplyNode(primary);
  auto lparen = new DynamicParserAcceptNode(kTokenLeftParenthesis);
  auto eval = new DynamicParserApplyNode(expr);
  auto rparen = new DynamicParserAcceptNode(kTokenRightParenthesis);
  auto eval_expression = new DynamicParserSelectNode(3, "EvalExpression");
  auto binary = new DynamicParserAcceptNode(kTokenOperator);
  auto expr2 = new DynamicParserApplyNode(expr);
  auto binary_expression = new DynamicParserSelectNode(3, "BinaryExpression");

  literal_number->SetNext(new DynamicParserReturnNode());

  primary->SetLabel("PrimaryExpression");
  primary->SetTarget(literal_number);
  primary->SetNext(lparen);

  eval_expression->SetNext(new DynamicParserReturnNode());

  rparen->SetTarget(eval_expression);
  rparen->SetNext(new DynamicParserErrorNode());

  eval->SetNext(rparen);

  lparen->SetTarget(eval);
  lparen->SetNext(new DynamicParserReturnNode());

  expr2->SetNext(new DynamicParserReturnNode());

  binary_expression->SetNext(new DynamicParserReturnNode());

  binary->SetTarget(expr2);
  binary->SetNext(binary_expression);

  expr->SetLabel("Expression");
  expr->SetNext(binary);

  return expr;
}
}

BEGIN_TEST_CASE("ParserCodeGenerationTest") {
  using namespace parc;
  auto parser = GetParser2();
  ByteCodeGenerator byte_code_generator;
  parser->Emit(&byte_code_generator);

  Program p;
  p.LoadFrom(byte_code_generator);

  std::string s;
  p.Disassemble(&s);
  TEST_OUTPUT(s);
}
END_TEST_CASE

BEGIN_TEST_CASE("ParserCodeGenerationWithOptimizationsTest") {
  using namespace parc;
  auto parser = GetParser2();
  ByteCodeGenerator byte_code_generator;
  parser->Emit(&byte_code_generator);

  Program p;
  p.LoadFrom(byte_code_generator);
  p.Optimize();

  std::string s;
  p.Disassemble(&s);
  TEST_OUTPUT(s);
}
END_TEST_CASE

BEGIN_TEST_CASE("ProgramTest") {
  using namespace parc;
  auto parser = GetParser2();
  ByteCodeGenerator byte_code_generator;
  byte_code_generator.EmitMetadataToken(kTokenNumber, "kTokenNumber");
  byte_code_generator.EmitMetadataToken(kTokenOperator, "kTokenOperator");
  byte_code_generator.EmitMetadataToken(kTokenLeftParenthesis,
                                        "kTokenLeftParenthesis");
  byte_code_generator.EmitMetadataToken(kTokenRightParenthesis,
                                        "kTokenRightParenthesis");
  parser->Emit(&byte_code_generator);

  Program p;
  p.LoadFrom(byte_code_generator);
  p.Optimize();

  TokenList token_stream;
  token_stream.Add(Token(kTokenNumber, "1"));
  token_stream.Add(Token(kTokenOperator, "+"));
  token_stream.Add(Token(kTokenNumber, "2"));

  StackMachine sm;
  sm.SetProgram(&p);
  sm.SetInput(&token_stream);

  auto entry_point = p.GetAddress("Expression");

  ASSERT_TRUE(entry_point != -1);

  auto syntax_tree = sm.Exec(entry_point);

  ASSERT_TRUE(syntax_tree != nullptr);

  std::string s;
  syntax_tree->DebugString(&s);
  TEST_OUTPUT(s);
}
END_TEST_CASE
