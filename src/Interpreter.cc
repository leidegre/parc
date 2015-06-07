
#include "Interpreter.h"
#include "SyntaxTree.h"

namespace parc {
bool ByteCodeInterpreter::Read() {
  if (!reader_.Read(&op_code_[0])) {
    return false;
  }
  uint32_t op_code = op_code_[0].uint32_;
  auto v = &op_code_[1];
  switch (op_code & 3) {
    case 3: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
    case 2: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
    case 1: {
      if (!reader_.Read(v++)) {
        return false;
      }
      // fall through case label
    }
  }
  return true;
}

void ParserByteCodeInterpreter::Accept(const int32_t token) {
  assert(inp_ && "token input stream is null");
  if (inp_->GetCurrent().GetType() == token) {
    Push(1);
    auto token_node = new TokenNode(inp_->GetCurrent());
    syntax_stack_.push_back(token_node);
    inp_->MoveNext();  // todo: what if eof?
  } else {
    Push(0);
  }
}

void ParserByteCodeInterpreter::Reduce(const int32_t node_count,
                                       const Slice& node_name) {
  assert(node_count <= (int32_t)syntax_stack_.size());
  assert(!node_name.IsEmpty());
  auto syntax_node = new SyntaxNode(node_name);
  for (int32_t i = 0; i < node_count; i++) {
    // reverse
    int32_t node_index = (int32_t)syntax_stack_.size() - node_count + i;
    syntax_node->Add(syntax_stack_[node_index]);
  }
  for (int32_t i = 0; i < node_count; i++) {
    syntax_stack_.pop_back();
  }
  syntax_stack_.push_back(syntax_node);
}
}
