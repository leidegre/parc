
#include "DynamicLexer.h"
#include "Coding.h"

namespace parc {
StateMachine* StateMachineBuilder::Build() {
  auto stt_size = states_.size() * alphabet_.GetSize();
  auto stt = new int[stt_size];
  memset(stt, -1, sizeof(int) * stt_size);
  for (const auto& tran : transitions_) {
    stt[tran.m_CurrentState * alphabet_.GetSize() + (size_t)tran.m_CharNum] =
        tran.m_NextState;
  }
  auto states = new int[states_.size()];
  memcpy(states, &states_[0], sizeof(int) * states_.size());
  auto fsm = new StateMachine(alphabet_, (int)alphabet_.GetSize(), states,
                              (int)states_.size(), stt);
  return fsm;
}

const char* StateMachine::Move(const char* s) {
  Utf8Decoder utf8_decoder(s);
  for (;;) {
    int ch = utf8_decoder.Read();
    if (ch == 0) {
      break;
    }
    int next_state = MoveTransition(ch);
    if (next_state == -1) {
      if ((states_[current_state_] & kStateMachineAcceptState) != 0) {
        return s + utf8_decoder.GetByteCount();
      }
      break;
    }
    current_state_ = next_state;
  }
  return s;
}

int StateMachine::MoveTransition(int code_point) {
  int num = alphabet_.LookupCharNum(code_point);
  if (num == -1) {
    return -1;
  }
  int next_state = stt_[current_state_ * alphabet_size_ + num];
  return next_state;
}
}