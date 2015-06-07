
#include "VES.h"
#include "Emit.h"
#include "Interpreter.h"
#include "Program.h"
#include "MsgPack.h"

namespace {
#if 0
void Trace(const char* s) { fprintf(stderr, "%s\n", s); }
void Trace(const char* s, const int32_t a) { fprintf(stderr, "%s %i\n", s, a); }
void Trace(size_t address, const char* s) {
  fprintf(stderr, "%u: %s\n", address, s);
}
void Trace(size_t address, const char* s, const int32_t a) {
  fprintf(stderr, "%u: %s %i\n", address, s, a);
}
void Trace(size_t address, const char* s, const int32_t a, const int32_t b) {
  fprintf(stderr, "%u: %s %i, %i\n", address, s, a, b);
}
#else
void Trace(const char*) {}
void Trace(const char*, const int32_t) {}
void Trace(size_t, const char*) {}
void Trace(size_t, const char*, const int32_t) {}
void Trace(size_t, const char*, const int32_t, const int32_t) {}
#endif
}

namespace parc {
// a program is metadata and byte code (the metadata contains the symbolic
// names for tokens and production rules)
SyntaxTree* StackMachine::Exec(const size_t entry_point) {
  assert(p_);
  assert(inp_);
  ParserByteCodeInterpreter interp(p_->GetByteCode());
  inp_->MoveNext();
  interp.SetInput(inp_);
  interp.SetAddress(p_->GetByteCode().size());
  interp.Call(entry_point);
  for (;;) {
    size_t current_address = interp.GetAddress();
    if (!interp.Read()) {
      Trace("end of program?");
      break;
    }
    uint32_t op_code = interp.GetOpCode();
    const char* mnemonic = ByteCode::GetMnemonic(op_code);
    Trace(current_address, mnemonic ? mnemonic : "?");
    switch (op_code) {
      case ByteCode::kAccept: {
        const auto& token = interp.GetOperand(0);
        interp.Accept(token.int32_);
        break;
      }
      case ByteCode::kBranchOnEqual: {
        const auto& label = interp.GetOperand(0);
        interp.BranchOnEqual(label.int32_);
        break;
      }
      case ByteCode::kBranch: {
        const auto& label = interp.GetOperand(0);
        interp.Branch(label.int32_);
        break;
      }
      case ByteCode::kCall: {
        const auto& label = interp.GetOperand(0);
        interp.Call(label.int32_);
        break;
      }
      case ByteCode::kReturn: {
        interp.Return();
        break;
      }
      case ByteCode::kError: {
        Trace("syntax error");
        return nullptr;
      }
      case ByteCode::kReduce: {
        const auto& count = interp.GetOperand(0);
        const auto& name = interp.GetOperand(1);
        interp.Reduce(count.int32_, name.s_);
        break;
      }
      default: {
        Trace("unimplemented op code", op_code);
        return nullptr;
      }
    }
  }

  return interp.GetSyntaxTree();
}
}
