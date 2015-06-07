
#include "TestHarness.h"

#include "..\DynamicLexer.h"

BEGIN_TEST_CASE("DynamicLexer_TreeConstructionTest") {
  parc::DynamicLexerNode root;

  auto digit =
      root.Define('0', '9');  // note: define can only be used on root node
  digit->SetToken(1);

  auto digit2 = new parc::DynamicLexerNode('0', '9');
  digit2->SetToken(1);
  digit2->SetNextState(digit2);  // repetition

  digit->SetNextState(digit2);

  auto left_paren = root.Define('(', '(');
  left_paren->SetToken(2);

  auto right_paren = root.Define(')', ')');
  right_paren->SetToken(3);

  auto plus_sign = root.Define('+', '+');
  plus_sign->SetToken(4);

  auto minus_sign = root.Define('-', '-');
  minus_sign->SetToken(5);

  parc::DynamicLexer lex(&root);
  lex.SetInput(parc::Utf8Decoder("1-(2+345)"));
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 1);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 5);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 2);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 1);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 4);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 1);
  lex.MoveNext();
  ASSERT_TRUE(lex.GetCurrent().GetType() == 3);
  ASSERT_TRUE(!lex.MoveNext());  // end of file
}
END_TEST_CASE
