
#include "TestHarness.h"

#include "..\DynamicParser.h"
#include "..\Emit.h"

namespace {
using namespace parc;
enum {
  kTokenNumber,
  kTokenOperator,
  kTokenLeftParenthesis,
  kTokenRightParenthesis
};
DynamicParserNode* GetParser() {
  // PrimaryExpression:
  auto pexp = new DynamicParserAcceptNode(kTokenNumber);
  pexp->SetLabel("PrimaryExpression");
  pexp->SetTarget(new DynamicParserSelectNode("PrimitiveExpression", 1));

  // Expression:
  auto expr = new DynamicParserApplyNode(pexp);
  expr->SetLabel("Expression");

  // Expression:
  auto binary = new DynamicParserApplyNode(expr);
  binary->SetNext(new DynamicParserSelectNode("BinaryExpression", 3));
  expr->SetNext(new DynamicParserAcceptNode(kTokenOperator, binary));

  // PrimaryExpression:
  auto eval2 = new DynamicParserApplyNode(expr);
  eval2->SetNext(new DynamicParserAcceptNode(
      kTokenRightParenthesis,
      new DynamicParserSelectNode("EvalExpression", 3)));
  auto eval = new DynamicParserAcceptNode(kTokenLeftParenthesis, eval2);
  pexp->SetNext(eval);

  return expr;
}

char GetNibble(unsigned char v) { return *("0123456789abcdef" + (v & 0xf)); }

std::string GetHexDumpString(const std::string& buf) {
  std::string hex;
  std::string str;
  size_t i = 0;
  for (i = 0; i < buf.size(); i++) {
    if ((i > 0) & ((i & 15) == 0)) {
      hex.append(1, ' ');
      hex.append(str);
      str.clear();
      hex.append(1, '\n');
    }
    if ((i & 15) == 0) {
      hex.append(1, GetNibble(i >> 12));
      hex.append(1, GetNibble(i >> 8));
      hex.append(1, GetNibble(i >> 4));
      hex.append(1, GetNibble(i));
      hex.append(1, ' ');
    }
    if (((i & 15) != 0) & ((i & 7) == 0)) {
      hex.append(1, ' ');
    }
    unsigned char v = (unsigned char)buf[i];
    hex.append(1, GetNibble(v >> 4));
    hex.append(1, GetNibble(v));
    if ((0x20 <= v) & (v <= 0x7f)) {
      str.append(1, buf[i]);
    } else {
      str.append(1, '.');
    }
  }
  if ((i > 0) & ((i & 15) != 0)) {
    hex.append(2 * (16 - (i & 15)) + (16 - (i & 15)) / 8, ' ');
    hex.append(1, ' ');
    hex.append(str);
    str.clear();
    hex.append(1, '\n');
  }
  return hex;
}
}

BEGIN_TEST_CASE("DynamicParser_Test") {
  using namespace parc;
  auto parser = GetParser();
  DynamicParserByteCodeGenerator byte_code_generator;
  parser->Emit(&byte_code_generator);
  // TEST_OUTPUT(GetHexDumpString(byte_code_generator.GetByteCodeStream()));
  std::string s;
  DynamicParserByteCodeMetadata metadata;
  metadata.tokens_.insert(std::make_pair(kTokenNumber, "kTokenNumber"));
  metadata.tokens_.insert(std::make_pair(kTokenOperator, "kTokenOperator"));
  metadata.tokens_.insert(
      std::make_pair(kTokenLeftParenthesis, "kTokenLeftParenthesis"));
  metadata.tokens_.insert(
      std::make_pair(kTokenRightParenthesis, "kTokenRightParenthesis"));
  byte_code_generator.DebugString(&s, &metadata);
  TEST_OUTPUT(s);
}
END_TEST_CASE
