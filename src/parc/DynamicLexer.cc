
#include "DynamicLexer.h"
#include "Coding.h"

namespace {
const parc::DynamicLexerNode* FindFirst(const parc::DynamicLexerNode* state,
                                        int code_point) {
  while (state) {
    if (state->CanRead(code_point)) {
      return state;
    }
    state = state->GetNext();
  }
  return nullptr;
}
}

namespace parc {
DynamicLexerNode* DynamicLexerNode::Define(int lower, int upper, int flags) {
  auto lexer_node = new DynamicLexerNode(lower, upper, flags);
  auto node = this;
  while (node->next_ != nullptr) {  // todo: split
    node = node->next_;
  }
  node->next_ = lexer_node;
  return lexer_node;
}

Token DynamicLexer::Next() {
  assert(root_);

  if (!inp_.Load()) {
    if (inp_.IsEmpty()) {
      return Token(0);  // end of file
    } else {
      return Token(-1);  // error: encoding
    }
  }
  // skip whitespace
  while (((inp_.Peek() == '\t') | (inp_.Peek() == '\n') |
          (inp_.Peek() == '\r') | (inp_.Peek() == ' ')) &&
         inp_.Read()) {
    ;
  }
  auto leading_trivia = inp_.Accept();
  // todo: headache (need pen and paper and to draw stuff)
  // need to support non-ambigous greedy matches, such as
  // - (minus), -- (comment)
  // GetNext represents the next non-overlapping interval
  // for each state we need to check if there's a transition
  // anywhere
  auto current_state = root_->GetNext();
  while (current_state) {
    auto first = FindFirst(current_state, inp_.Peek());
    printf(">> %c %x -> %x\n", (char)inp_.Peek(), current_state, first);
    if (first) {
      int l = first->GetLower();
      int u = first->GetUpper();
      int t = first->GetToken();
      printf("[%c, %c]: %i %c\n", (char)l, (char)u, t, (char)inp_.Peek());
      if (first->GetNextState()) {
        current_state = first->GetNextState();
        if (inp_.Read()) {
          continue;  // greedy
        }
      } else {
        current_state = first;
        inp_.Read();
      }
    }
    break;
  }
  if (current_state &&
      (current_state->GetFlags() & DynamicLexerNode::kAccept) ==
          DynamicLexerNode::kAccept) {
    auto t = inp_.Accept();
    std::string s(t.GetData(), t.GetSize());
    printf("<< %i %s\n", current_state->GetToken(), s.c_str());
    return Token(current_state->GetToken(), t);
  }
  return Token(-2);  // error: token
}
}
