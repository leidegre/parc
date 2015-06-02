
#include "DynamicParser.h"
#include "Emit.h"

namespace {
using namespace parc;
#if 0
void EmitTrace(DynamicParserNode* node,
               DynamicParserByteCodeGenerator* byte_code_generator,
               const char* comment = nullptr) {
  char temp[1024];
  if (node->HasLabel()) {
    sprintf(temp, comment == nullptr ? "%x %s" : "%x %s -- %s", node,
            node->GetLabel().ToString().c_str(), comment);
  } else {
    sprintf(temp, comment == nullptr ? "%x [%s]" : "%x [%s] -- %s", node,
            typeid(*node).name(), comment);
  }
  byte_code_generator->EmitTrace(temp);
}
#else
void EmitTrace(DynamicParserNode* node,
               DynamicParserByteCodeGenerator* byte_code_generator,
               const char* comment = nullptr) {}
#endif
void EmitNext(DynamicParserNode* node,
              DynamicParserByteCodeGenerator* byte_code_generator) {
  assert(node);
  if (auto next = node->GetNext()) {
    EmitTrace(node, byte_code_generator, "next");
    int node_va;
    byte_code_generator->TryAddVirtualAddress(node, &node_va);
    int next_va;
    if (byte_code_generator->TryAddVirtualAddress(next, &next_va)) {
      if (node_va != next_va) {
        byte_code_generator->EmitBranch(next_va);
      }
      next->Emit(byte_code_generator);
    } else {
      byte_code_generator->EmitBranch(next_va);
    }
  } else {
    EmitTrace(node, byte_code_generator, "return");
    byte_code_generator->EmitReturn();
  }
}
}

namespace parc {
// todo: emit should probably not be a member of the control flow graph...
void DynamicParserAcceptNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  byte_code_generator->TryAddVirtualAddress(this, &va);

  assert(GetTarget() && "Target cannot be null");
  auto target = GetTarget();
  int target_va;
  byte_code_generator->TryAddVirtualAddress(target, &target_va);
  target->Emit(byte_code_generator);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va, GetLabel());
  byte_code_generator->EmitAccept(GetToken());
  byte_code_generator->EmitBranchOnEqual(target_va);

  EmitNext(this, byte_code_generator);
}

void DynamicParserApplyNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  byte_code_generator->TryAddVirtualAddress(this, &va);

  assert(GetProduction() && "Production cannot be null");
  auto prod = GetProduction();
  int prod_va;
  if (byte_code_generator->TryAddVirtualAddress(prod, &prod_va)) {
    prod->Emit(byte_code_generator);
  }

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va, GetLabel());
  byte_code_generator->EmitCall(prod_va);

  EmitNext(this, byte_code_generator);
}

void DynamicParserSelectNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  byte_code_generator->TryAddVirtualAddress(this, &va);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va, GetLabel());
  byte_code_generator->EmitPop(GetPopCount());

  EmitNext(this, byte_code_generator);
}
}
