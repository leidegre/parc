
#include "DynamicParser.h"
#include "Emit.h"

namespace {
using namespace parc;
#if 0
void EmitTrace(DynamicParserNode* node,
               ByteCodeGenerator* byte_code_generator,
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
void EmitTrace(DynamicParserNode* node, ByteCodeGenerator* byte_code_generator,
               const char* comment = nullptr) {}
#endif
void EmitNext(DynamicParserNode* node, ByteCodeGenerator* byte_code_generator) {
  assert(node);
  if (auto next = node->GetNext()) {
    EmitTrace(node, byte_code_generator, "next");
    int next_label;
    if (byte_code_generator->Bind(next, &next_label)) {
      byte_code_generator->EmitBranch(next_label);
      next->Emit(byte_code_generator);
    } else {
      byte_code_generator->EmitBranch(next_label);
    }
  }
}
}

namespace parc {
// todo: emit should probably not be a member of the control flow graph...
void DynamicParserAcceptNode::Emit(ByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int label;
  byte_code_generator->Bind(this, &label);

  assert(GetTarget() && "Target cannot be null");
  auto target = GetTarget();
  int target_label;
  byte_code_generator->Bind(target, &target_label);
  target->Emit(byte_code_generator);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(label);
  byte_code_generator->EmitAccept(GetToken());
  byte_code_generator->EmitBranchOnEqual(target_label);

  EmitNext(this, byte_code_generator);
}

void DynamicParserApplyNode::Emit(ByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int label;
  byte_code_generator->Bind(this, &label);

  assert(GetProduction() && "Production cannot be null");
  auto prod = GetProduction();
  int prod_label;
  if (byte_code_generator->Bind(prod, &prod_label)) {
    prod->Emit(byte_code_generator);
  }

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(label);
  byte_code_generator->EmitCall(prod_label);

  EmitNext(this, byte_code_generator);
}

void DynamicParserSelectNode::Emit(ByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int label;
  byte_code_generator->Bind(this, &label);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(label);
  assert(false && "not implemented yet");

  EmitNext(this, byte_code_generator);
}

void DynamicParserErrorNode::Emit(ByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int label;
  byte_code_generator->Bind(this, &label);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(label);
  byte_code_generator->EmitError();

  EmitNext(this, byte_code_generator);
}

void DynamicParserReturnNode::Emit(ByteCodeGenerator* byte_code_generator) {
  EmitTrace(this, byte_code_generator, "enter");
  int label;
  byte_code_generator->Bind(this, &label);

  EmitTrace(this, byte_code_generator, "emit");

  byte_code_generator->EmitLabel(label);
  byte_code_generator->EmitReturn();

  EmitNext(this, byte_code_generator);
}
}
