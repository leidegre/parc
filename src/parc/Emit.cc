
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

////////////////////////////////

void ByteCodeGenerator::DebugString(std::string* s) const {
  assert(s);
  std::unordered_map<int, Slice> labels;
  std::unordered_map<int, Slice> tokens;

  {
    msgpack::Reader reader(metadata_);
    msgpack::Value v;
    while (reader.Read(&v)) {
      switch (v.int32_) {
        case kMetadataLabel: {
          msgpack::Value label;
          reader.Read(&label);
          msgpack::Value name;
          reader.Read(&name);
          labels.insert(std::make_pair(label.int32_, name.s_));
          break;
        }
        case kMetadataToken: {
          msgpack::Value label;
          reader.Read(&label);
          msgpack::Value name;
          reader.Read(&name);
          tokens.insert(std::make_pair(label.int32_, name.s_));
          break;
        }
      }
    }
  }

  static const int kOpCodeMaxWidth = 8;
  std::stringstream ss;
  ss << std::left;  // left justify
  {
    msgpack::Reader reader(byte_code_);
    msgpack::Value v;
    while (reader.Read(&v)) {
      switch (v.int32_) {
        case ByteCode::kAccept: {
          msgpack::Value token;
          reader.Read(&token);
          ss << "  " << std::setw(kOpCodeMaxWidth) << "accept"
             << " ";
          auto it = tokens.find(token.int32_);
          if (it != tokens.end()) {
            ss << it->second.ToString() << "(" << token.int32_ << ")";
          } else {
            ss << token.int32_;
          }
          ss << std::endl;
          break;
        }
        case ByteCode::kBranchOnEqual: {
          msgpack::Value target;
          reader.Read(&target);
          ss << "  " << std::setw(kOpCodeMaxWidth) << "beq"
             << " " << target.int32_ << std::endl;
          break;
        }
        case ByteCode::kBranch: {
          msgpack::Value target;
          reader.Read(&target);
          ss << "  " << std::setw(kOpCodeMaxWidth) << "jmp"
             << " " << target.int32_ << std::endl;
          break;
        }
        case ByteCode::kCall: {
          msgpack::Value address;
          reader.Read(&address);
          ss << "  " << std::setw(kOpCodeMaxWidth) << "call";
          auto it = labels.find(address.int32_);
          if (it != labels.end()) {
            ss << " " << it->second.ToString() << "(" << address.int32_ << ")";
          } else {
            ss << " " << address.int32_;
          }
          ss << std::endl;
          break;
        }
        case ByteCode::kReturn: {
          ss << "  " << std::setw(kOpCodeMaxWidth) << "return" << std::endl;
          break;
        }
        case ByteCode::kError: {
          ss << "  " << std::setw(kOpCodeMaxWidth) << "error" << std::endl;
          break;
        }
        case ByteCode::kLabel: {
          msgpack::Value va;
          reader.Read(&va);
          auto it = labels.find(va.int32_);
          if (it != labels.end()) {
            ss << it->second.ToString() << "(" << va.int32_ << ")";
          } else {
            ss << va.int32_;
          }
          ss << ":" << std::endl;
          break;
        }
        case ByteCode::kTrace: {
          msgpack::Value str;
          reader.Read(&str);
          ss << str.s_.ToString() << std::endl;
          break;
        }
        default: {
          ss << "!unk_byte_code" << std::endl;
          break;
        }
      }
    }
  }
  s->swap(ss.str());
}
}
