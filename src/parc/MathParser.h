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
};

// a simple example of a recursive decent parser for basic arithmetic
class MathParser : public ParserBase {
 public:
  SyntaxTree* Parse();
  SyntaxTree* ParseExpression();
  SyntaxTree* ParsePrimaryExpression();
  SyntaxTree* ParseNumber();
};
}
