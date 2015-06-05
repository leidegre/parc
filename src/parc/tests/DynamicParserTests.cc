
#include "TestHarness.h"

#include "..\DynamicParser.h"
#include "..\Emit.h"
#include "..\Program.h"
#include "..\Utility.h"

namespace {
using namespace parc;
enum {
  kTokenNumber,
  kTokenOperator,
  kTokenLeftParenthesis,
  kTokenRightParenthesis
};
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
}

BEGIN_TEST_CASE("DynamicParser_Test") {
  using namespace parc;
  auto parser = GetParser();
  ByteCodeGenerator byte_code_generator;
  byte_code_generator.EmitMetadataToken(kTokenNumber, "kTokenNumber");
  byte_code_generator.EmitMetadataToken(kTokenOperator, "kTokenOperator");
  byte_code_generator.EmitMetadataToken(kTokenLeftParenthesis,
                                        "kTokenLeftParenthesis");
  byte_code_generator.EmitMetadataToken(kTokenRightParenthesis,
                                        "kTokenRightParenthesis");
  parser->Emit(&byte_code_generator);
  std::string s;
  byte_code_generator.DebugString(&s);
  TEST_OUTPUT(s);
}
END_TEST_CASE

BEGIN_TEST_CASE("ProgramLoadFromTest") {
  using namespace parc;
  auto parser = GetParser();
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

  std::string s;
  p.Decompile(&s);
  TEST_OUTPUT(s);

  std::string bin;
  p.Save(&bin);
  std::string hex;
  GetHexDump(bin, &hex);
  TEST_OUTPUT(hex);
}
END_TEST_CASE
