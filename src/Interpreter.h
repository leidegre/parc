#pragma once

#include "Slice.h"
#include "Token.h"
#include "Emit.h"
#include "MsgPack.h"

#include <vector>
#include <stack>

namespace parc {
class SyntaxTree;

class ByteCodeInterpreter {
 public:
  ByteCodeInterpreter(const Slice& byte_code)
      : byte_code_(byte_code), reader_(byte_code), stack_() {}

  // gets/sets the current instruction pointer
  int32_t GetAddress() const {
    return (int32_t)(reader_.GetPosition() - byte_code_.GetData());
  }

  void SetAddress(const int32_t new_address) {
    assert(((size_t)new_address <= byte_code_.GetSize()));
    reader_.SetPosition(byte_code_.GetData() + new_address);
  }

  bool Read();

  // REQUIRES: Read
  uint32_t GetOpCode() const { return op_code_[0].uint32_; }

  // REQUIRES: Read
  uint32_t GetOperandCount() const { return op_code_[0].uint32_ & 3; }

  // REQUIRES: Read
  const msgpack::Value& GetOperand(uint32_t index) const {
    assert(index < GetOperandCount());
    return op_code_[1 + index];
  }

  void Push(const int32_t v) { stack_.push(v); }

  int32_t Pop() {
    assert(!stack_.empty() && "the evaluation stack is empty");
    int32_t v = stack_.top();
    stack_.pop();
    return v;
  }

  void BranchOnEqual(const int32_t address) {
    int32_t equal = Pop();
    if (equal != 0) {
      Branch(address);
    }
  }

  void Branch(const int32_t address) { SetAddress(address); }

  void Call(const int32_t address) {
    Push(GetAddress());
    Branch(address);
  }

  void Return() {
    int32_t return_address = Pop();
    Branch(return_address);
  }

 private:
  Slice byte_code_;
  msgpack::Reader reader_;
  msgpack::Value op_code_[4];
  std::stack<int32_t> stack_;
};

class ParserByteCodeInterpreter : public ByteCodeInterpreter {
 public:
  ParserByteCodeInterpreter(const Slice& byte_code)
      : ByteCodeInterpreter(byte_code), inp_(nullptr), syntax_stack_() {}

  TokenInputStream* GetInput() const { return inp_; }
  void SetInput(TokenInputStream* inp) { inp_ = inp; }

  // REQUIRES: SetInput
  void Accept(const int32_t token);

  void Reduce(const int32_t node_count, const Slice& node_name);

  SyntaxTree* GetSyntaxTree() const {
    return syntax_stack_.empty() ? nullptr : syntax_stack_.back();
  }

 private:
  TokenInputStream* inp_;
  std::vector<SyntaxTree*> syntax_stack_;
};
}
