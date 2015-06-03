
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
void EmitMetadata(DynamicParserNode* node,
                  DynamicParserByteCodeGenerator* byte_code_generator,
                  int* va) {
  byte_code_generator->TryAddVirtualAddress(node, va);
  if (node->HasLabel()) {
    byte_code_generator->EmitLabelMetadata(*va, node->GetLabel());
  }
}
void EmitNext(DynamicParserNode* node,
              DynamicParserByteCodeGenerator* byte_code_generator) {
  assert(node);
  if (auto next = node->GetNext()) {
    EmitTrace(node, byte_code_generator, "next");
    int next_va;
    if (byte_code_generator->TryAddVirtualAddress(next, &next_va)) {
      byte_code_generator->EmitBranch(next_va);
      next->Emit(byte_code_generator);
    } else {
      byte_code_generator->EmitBranch(next_va);
    }
  }
}
}

namespace parc {
// todo: emit should probably not be a member of the control flow graph...
void DynamicParserAcceptNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  EmitMetadata(this, byte_code_generator, &va);

  assert(GetTarget() && "Target cannot be null");
  auto target = GetTarget();
  int target_va;
  byte_code_generator->TryAddVirtualAddress(target, &target_va);
  target->Emit(byte_code_generator);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va);
  byte_code_generator->EmitAccept(GetToken());
  byte_code_generator->EmitBranchOnEqual(target_va);

  EmitNext(this, byte_code_generator);
}

void DynamicParserApplyNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  EmitMetadata(this, byte_code_generator, &va);

  assert(GetProduction() && "Production cannot be null");
  auto prod = GetProduction();
  int prod_va;
  if (byte_code_generator->TryAddVirtualAddress(prod, &prod_va)) {
    prod->Emit(byte_code_generator);
  }

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va);
  byte_code_generator->EmitCall(prod_va);

  EmitNext(this, byte_code_generator);
}

void DynamicParserSelectNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  EmitMetadata(this, byte_code_generator, &va);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va);
  byte_code_generator->EmitPop(GetPopCount());

  EmitNext(this, byte_code_generator);
}

void DynamicParserErrorNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  EmitMetadata(this, byte_code_generator, &va);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va);
  byte_code_generator->EmitError();

  EmitNext(this, byte_code_generator);
}

void DynamicParserReturnNode::Emit(
    DynamicParserByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int va;
  EmitMetadata(this, byte_code_generator, &va);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(va);
  byte_code_generator->EmitReturn();

  EmitNext(this, byte_code_generator);
}
}
