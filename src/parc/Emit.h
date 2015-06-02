#pragma once

#include <string>
#include <unordered_map>
#include "Slice.h"

namespace parc {
// the byte code emitter is a two pass algorithm
struct DynamicParserByteCodeMetadata;
class DynamicParserByteCodeGenerator {
 public:
  // non-copyable
  DynamicParserByteCodeGenerator(const DynamicParserByteCodeGenerator&) =
      delete;
  DynamicParserByteCodeGenerator& operator=(
      const DynamicParserByteCodeGenerator&) = delete;

  DynamicParserByteCodeGenerator() : byte_code_stream_(), tlb_() {}

  // todo: struct?
  struct ByteCode {
    enum {
      kNone,
      kAccept,
      kBranchOnEqual,
      kBranch,
    };
    int op_code_;
    // op_code metadata
    int operand_count;
  };
  enum {
    // lsb bits 0-1 (#operands), 2-6 (op code)
    kByteCodeNone = 0x0,
    kByteCodeAccept = 0x01,
    kByteCodeBranchOnEqual = 0x02,
    kByteCodeBranch = 0x03,
    kByteCodeCall = 0x04,
    kByteCodeReturn = 0x05,
    kByteCodeError = 0x06,
    kByteCodeLabel = 0x07,
    kByteCodeNewobj = 0x08,
    kByteCodeLdstr = 0x09,
    kByteCodePop = 0x0a,
    // kByteCodeVirtualAddress = 0x0b,
    kByteCodeTrace = 0x0c,
  };

  void EmitNone();
  void EmitAccept(int token);
  void EmitBranchOnEqual(int target);
  void EmitBranch(int target);
  void EmitCall(int target);
  void EmitReturn();
  void EmitError();  // todo: how do we recover from error?
  void EmitLabel(int va, const Slice& label);
  void EmitNew(const Slice& type_name);
  void EmitPop(int pop_count);
  // void EmitVirtualAddress(int virtual_address);
  void EmitTrace(const Slice& msg);

  int GetSize() const { return (int)byte_code_stream_.size(); }

  bool TryAddVirtualAddress(void* node, int* virtual_address) {
    auto it = tlb_.find(node);
    if (it == tlb_.end()) {
      int va = (int)tlb_.size() + 1;
      tlb_.insert(std::make_pair(node, va));
      *virtual_address = va;
      return true;
    }
    *virtual_address = it->second;
    return false;
  }

  const std::string& GetByteCodeStream() const { return byte_code_stream_; }

  void DebugString(std::string* s,
                   DynamicParserByteCodeMetadata* metadata = nullptr) const;

 private:
  std::string byte_code_stream_;
  std::unordered_map<void*, int> tlb_;
};

struct DynamicParserByteCodeMetadata {
  std::unordered_map<int, Slice> tokens_;
};
}
