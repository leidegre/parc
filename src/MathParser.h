#pragma once

#include "ParserBase.h"

namespace parc {
class SyntaxTree;

enum {
  kMathLeftParenthesis = 1,
  kMathRightParenthesis,
  kMathNumber,
  kMathPlusSign,
  kMathHypenMinus,
  kMathAsterisk,
  kMathSolidus,
  kMathLeftBracket,
  kMathRightBracket,
  kMathComma,
};

// a simple example of a recursive decent parser for basic arithmetic
// we've added lists of expressions to exemplify how a recursive decent
// parser would treat repetition or lists
class MathParser : public ParserBase {
 public:
  SyntaxTree* Parse();
  SyntaxTree* ParseExpression();
  SyntaxTree* ParsePrimaryExpression();
  SyntaxTree* ParseNumber();
};
}
