#pragma once

#include <map>
#include <vector>

// the follow regular expression represents three deterministic finite
// automatons (DFAs)
// [A-Z] [0-9A-Za-z]*
// [a-z] [0-9A-Za-z]*
// "\"" [^"]* "\""

namespace parc {
class StateMachineBuilder;

class Alphabet {
 public:
  int DefineCharNum(int code_point) {
    auto it = char_map_.find(code_point);
    if (it != char_map_.end()) return it->second;
    int char_num = (int)char_map_.size();
    char_map_[code_point] = char_num;
    return char_num;
  }

  int LookupCharNum(int code_point) const {
    auto it = char_map_.find(code_point);
    if (it != char_map_.end()) return it->second;
    return -1;
  }

  size_t GetSize() const { return char_map_.size(); }

 private:
  // to later be able to create a directly addressable state transition table
  // we map a possibly non-continuous range to a continuous range
  std::map<int, int> char_map_;
};

struct StateMachineTransition {
  int m_CurrentState;
  int m_CharNum;
  int m_NextState;
};

class StateMachine {
 public:
  StateMachine(Alphabet alphabet, int alphabet_size, int* states,
               int state_count, int* stt)
      : alphabet_(alphabet),
        alphabet_size_(alphabet_size),
        states_(states),
        state_count_(state_count),
        stt_(stt),
        current_state_(0) {}

  void Reset(int state = 0) { current_state_ = state; }

  const char* Move(const char* s);

  int MoveTransition(int code_point);

 private:
  Alphabet alphabet_;
  int alphabet_size_;
  int* states_;
  int state_count_;
  int* stt_;
  int current_state_;
};

class StateMachineState {
 public:
  StateMachineState(StateMachineBuilder* smb, int state)
      : smb_(smb), state_(state) {}

  int GetState() const { return state_; }

  void DefineTransition(int code_point, const StateMachineState& next_state);

 private:
  StateMachineBuilder* smb_;
  int state_;
};

enum { kStateMachineNormalState = 0, kStateMachineAcceptState = 1 };

class StateMachineBuilder {
 public:
  StateMachineState DefineState(int flags = 0) {
    int state = (int)states_.size();
    states_.push_back(flags);
    return StateMachineState(this, state);
  }

  void DefineTransition(const StateMachineState& current_state, int code_point,
                        const StateMachineState& next_state) {
    transitions_.push_back(StateMachineTransition{
        current_state.GetState(), alphabet_.DefineCharNum(code_point),
        next_state.GetState()});
  }

  StateMachine* Build();

 private:
  Alphabet alphabet_;
  std::vector<int> states_;
  std::vector<StateMachineTransition> transitions_;
};

inline void StateMachineState::DefineTransition(
    int code_point, const StateMachineState& next_state) {
  smb_->DefineTransition(*this, code_point, next_state);
}
}
