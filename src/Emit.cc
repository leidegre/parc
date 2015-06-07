
#include <cassert>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "Emit.h"
#include "MsgPack.h"
#include "DynamicParser.h"

namespace parc {
const char* ByteCode::GetMnemonic(uint32_t op_code) {
  static const char* mnemonics[kMax];
  static bool is_initialized;
  if (!is_initialized) {
    memset(mnemonics, 0, sizeof(mnemonics));
    mnemonics[kAccept] = "accept";
    mnemonics[kBranchOnEqual] = "beq";
    mnemonics[kBranch] = "jmp";
    mnemonics[kCall] = "call";
    mnemonics[kReturn] = "ret";
    mnemonics[kError] = "err";
    mnemonics[kLabel] = "lbl";
    mnemonics[kReduce] = "reduce";
    is_initialized = true;
  }
  if (op_code < kMax) {
    return mnemonics[op_code];
  }
  return nullptr;
}

void ByteCodeGenerator::EmitAccept(int token) {
  msgpack::WriteInteger(ByteCode::kAccept, &byte_code_);
  msgpack::WriteInteger(token, &byte_code_);
}

void ByteCodeGenerator::EmitBranchOnEqual(int target) {
  msgpack::WriteInteger(ByteCode::kBranchOnEqual, &byte_code_);
  msgpack::WriteInteger(target, &byte_code_);
}

void ByteCodeGenerator::EmitBranch(int target) {
  msgpack::WriteInteger(ByteCode::kBranch, &byte_code_);
  msgpack::WriteInteger(target, &byte_code_);
}

void ByteCodeGenerator::EmitCall(int target) {
  msgpack::WriteInteger(ByteCode::kCall, &byte_code_);
  msgpack::WriteInteger(target, &byte_code_);
}

void ByteCodeGenerator::EmitReturn() {
  msgpack::WriteInteger(ByteCode::kReturn, &byte_code_);
}

void ByteCodeGenerator::EmitError() {
  msgpack::WriteInteger(ByteCode::kError, &byte_code_);
}

void ByteCodeGenerator::EmitLabel(int label) {
  msgpack::WriteInteger(ByteCode::kLabel, &byte_code_);
  msgpack::WriteInteger(label, &byte_code_);
}

void ByteCodeGenerator::EmitReduce(int32_t node_count, const Slice& node_name) {
  msgpack::WriteInteger(ByteCode::kReduce, &byte_code_);
  msgpack::WriteInteger(node_count, &byte_code_);
  msgpack::WriteString(node_name, &byte_code_);
}

void ByteCodeGenerator::EmitTrace(const Slice& msg) {
  msgpack::WriteInteger(ByteCode::kTrace, &byte_code_);
  msgpack::WriteString(msg, &byte_code_);
}

bool ByteCodeGenerator::Bind(DynamicParserNode* node, int* label) {
  auto it = tlb_.find(node);
  if (it == tlb_.end()) {
    int next_label = (int)tlb_.size() + 1;
    tlb_.insert(std::make_pair(node, next_label));
    if (node->HasLabel()) {
      EmitMetadataLabel(next_label, node->GetLabel());
    }
    *label = next_label;
    return true;
  }
  *label = it->second;
  return false;
}

////////////////////////////////

void ByteCodeGenerator::EmitMetadataLabel(int label, const Slice& name) {
  msgpack::WriteInteger(kMetadataLabel, &metadata_);
  msgpack::WriteInteger(label, &metadata_);
  msgpack::WriteString(name, &metadata_);
}

void ByteCodeGenerator::EmitMetadataToken(int token, const Slice& name) {
  msgpack::WriteInteger(kMetadataToken, &metadata_);
  msgpack::WriteInteger(token, &metadata_);
  msgpack::WriteString(name, &metadata_);
}
}
