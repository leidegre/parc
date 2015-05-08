
#include "TestHarness.h"

#include "..\DynamicLexer.h"

// A B
// A|B
// A  *

BEGIN_TEST_CASE("DynamicLexer_StateMachineBuilderTest") {
  parc::StateMachineBuilder fsm_builder;

  auto s0 = fsm_builder.DefineState();
  auto s1 = fsm_builder.DefineState(parc::kStateMachineAcceptState);

  for (int i = 0; i <= 'Z' - 'A'; i++) {
    s0.DefineTransition('A' + i, s1);
  }

  for (int i = 0; i <= 'Z' - 'A'; i++) {
    s1.DefineTransition('A' + i, s1);
  }

  for (int i = 0; i <= 'z' - 'a'; i++) {
    s1.DefineTransition('a' + i, s1);
  }

  for (int i = 0; i <= '9' - '0'; i++) {
    s1.DefineTransition('0' + i, s1);
  }

  parc::StateMachine* fsm = fsm_builder.Build();

  auto s = fsm->Move("Az");
  auto t = fsm->Move("az");
}
END_TEST_CASE