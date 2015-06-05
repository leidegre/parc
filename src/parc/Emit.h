#pragma once

#include "Slice.h"

#include <string>
#include <unordered_map>

namespace parc {
class DynamicParserNode;

#define MAKE_BYTE_CODE(op_code, num_args) ((op_code << 2) | num_args)

struct ByteCode {
  // byte code 0 (reserved)
  // byte code 1-31 (single byte instruction)
  // byte code 32-256 (two byte instruction)
  // byte code >256 (three byte instruction)
  // (a byte code can have at most 3 operands)
  enum {
    kAccept = MAKE_BYTE_CODE(1, 1),
    kBranchOnEqual = MAKE_BYTE_CODE(2, 1),
    kBranch = MAKE_BYTE_CODE(3, 1),
    kCall = MAKE_BYTE_CODE(4, 1),
    kReturn = MAKE_BYTE_CODE(5, 0),
    kError = MAKE_BYTE_CODE(6, 0),
    kLabel = MAKE_BYTE_CODE(7, 1),

    kTrace = MAKE_BYTE_CODE(32, 1),

    kMax = kTrace + 1,  // not a byte code
  };
};

class ByteCodeGenerator {
 public:
  // non-copyable
  ByteCodeGenerator(const ByteCodeGenerator&) = delete;
  ByteCodeGenerator& operator=(const ByteCodeGenerator&) = delete;

  ByteCodeGenerator() : byte_code_(), metadata_(), tlb_() {}

  // byte code
  void EmitAccept(int token);
  void EmitBranchOnEqual(int target);
  void EmitBranch(int target);
  void EmitCall(int target);
  void EmitReturn();
  void EmitError();
  void EmitLabel(int label);
  void EmitTrace(const Slice& msg);

  // Virtual labels:
  // what are these? these are virtual labels that identify where a piece of
  // code starts. Since the code is typically generated hapazardly by following
  // along the paths of the control flow graph we need an easy way to find
  // things later (regardless of when they happened).
  // todo: move DynamicParserNode away from this module
  bool Bind(DynamicParserNode* node, int* label);

  const std::string& GetByteCodeStream() const { return byte_code_; }

  // metadata
  enum {
    kMetadataLabel = 0x01,
    kMetadataToken = 0x02,
  };

  void EmitMetadataLabel(int label, const Slice& name);
  void EmitMetadataToken(int token, const Slice& name);

  const std::string& GetMetadataStream() const { return metadata_; }

  // debug
  void DebugString(std::string* s) const;

 private:
  std::string byte_code_;
  std::string metadata_;
  std::unordered_map<void*, int> tlb_;
};
}
