
#include "DynamicParser.h"
#include "SyntaxTree.h"

namespace {
const parc::DynamicParserNode* FindFirst(const parc::DynamicParserNode* state,
                                         int token) {
  while (state) {
    if (state->GetToken() == token) {
      return state;
    }
    state = state->GetNext();
  }
  return nullptr;
}
}

namespace parc {
SyntaxTree* DynamicParser::Parse(TokenInputStream* inp) {
  for (;;) {
    const auto top = parser_.top();

    // auto token = new TokenNode(inp->Next());
    // stack_.push(token);

    // auto next = FindFirst(top, token->GetToken().GetType());
    // if (next) {
    // state = next->GetTransition();
    //} else {
    // unrecognized token
    // break;
    //}
  }
  return nullptr;
}
}
