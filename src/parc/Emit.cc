
#include <cassert>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "Emit.h"
#include "MsgPack.h"

namespace parc {
void DynamicParserByteCodeGenerator::EmitNone() {
  msgpack::WriteInteger(kByteCodeNone, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitAccept(int token) {
  msgpack::WriteInteger(kByteCodeAccept, &byte_code_stream_);
  msgpack::WriteInteger(token, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitBranchOnEqual(int target) {
  msgpack::WriteInteger(kByteCodeBranchOnEqual, &byte_code_stream_);
  msgpack::WriteInteger(target, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitBranch(int target) {
  msgpack::WriteInteger(kByteCodeBranch, &byte_code_stream_);
  msgpack::WriteInteger(target, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitCall(int target) {
  msgpack::WriteInteger(kByteCodeCall, &byte_code_stream_);
  msgpack::WriteInteger(target, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitReturn() {
  msgpack::WriteInteger(kByteCodeReturn, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitError() {
  msgpack::WriteInteger(kByteCodeError, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitLabel(int va) {
  msgpack::WriteInteger(kByteCodeLabel, &byte_code_stream_);
  msgpack::WriteInteger(va, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitLabelMetadata(int va,
                                                       const Slice& label) {
  msgpack::WriteInteger(kByteCodeLabelMetadata, &byte_code_stream_);
  msgpack::WriteInteger(va, &byte_code_stream_);
  msgpack::WriteString(label, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitNew(const Slice& type_name) {
  msgpack::WriteInteger(kByteCodeLdstr, &byte_code_stream_);
  msgpack::WriteString(type_name, &byte_code_stream_);
  msgpack::WriteInteger(kByteCodeNewobj, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::EmitPop(int pop_count) {
  msgpack::WriteInteger(kByteCodePop, &byte_code_stream_);
  msgpack::WriteInteger(pop_count, &byte_code_stream_);
}

// void DynamicParserByteCodeGenerator::EmitVirtualAddress(int virtual_address)
// {
//   msgpack::WriteInteger(kByteCodeVirtualAddress, &byte_code_stream_);
//   msgpack::WriteInteger(virtual_address, &byte_code_stream_);
// }

void DynamicParserByteCodeGenerator::EmitTrace(const Slice& msg) {
  msgpack::WriteInteger(kByteCodeTrace, &byte_code_stream_);
  msgpack::WriteString(msg, &byte_code_stream_);
}

void DynamicParserByteCodeGenerator::DebugString(
    std::string* s, DynamicParserByteCodeMetadata* metadata) const {
  assert(s);
  std::unordered_map<int, Slice> labels;
  static const int kOpCodeMaxWidth = 8;
  std::stringstream ss;
  ss << std::left;  // left justify
  msgpack::Reader reader(byte_code_stream_);
  msgpack::Value v;
  while (reader.Read(&v)) {
    switch (v.int32_) {
      case kByteCodeNone: {
        ss << "  " << std::setw(kOpCodeMaxWidth) << "nop" << std::endl;
        break;
      }
      case kByteCodeAccept: {
        msgpack::Value operand;
        reader.Read(&operand);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "accept"
           << " ";
        if (metadata) {
          auto it = metadata->tokens_.find(operand.int32_);
          if (it != metadata->tokens_.end()) {
            ss << it->second.ToString();
          } else {
            ss << operand.int32_;
          }
        } else {
          ss << operand.int32_;
        }
        ss << std::endl;
        break;
      }
      case kByteCodeBranchOnEqual: {
        msgpack::Value operand;
        reader.Read(&operand);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "beq"
           << " " << operand.int32_ << std::endl;
        break;
      }
      case kByteCodeBranch: {
        msgpack::Value operand;
        reader.Read(&operand);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "jmp"
           << " " << operand.int32_ << std::endl;
        break;
      }
      case kByteCodeCall: {
        msgpack::Value operand;
        reader.Read(&operand);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "call";
        auto it = labels.find(operand.int32_);
        if (it != labels.end()) {
          ss << " " << it->second.ToString();
        } else {
          ss << " " << operand.int32_;
        }
        ss << std::endl;
        break;
      }
      case kByteCodeReturn: {
        ss << "  " << std::setw(kOpCodeMaxWidth) << "return" << std::endl;
        break;
      }
      case kByteCodeError: {
        ss << "  " << std::setw(kOpCodeMaxWidth) << "error" << std::endl;
        break;
      }
      case kByteCodeLabelMetadata: {
        msgpack::Value va;
        reader.Read(&va);
        msgpack::Value label;
        reader.Read(&label);
        labels.insert(std::make_pair(va.int32_, label.s_));  // remember this
        break;
      }
      case kByteCodeLabel: {
        msgpack::Value va;
        reader.Read(&va);
        auto it = labels.find(va.int32_);
        if (it != labels.end()) {
          ss << it->second.ToString();
        } else {
          ss << va.int32_;
        }
        ss << ":" << std::endl;
        break;
      }
      case kByteCodeNewobj: {
        ss << "  " << std::setw(kOpCodeMaxWidth) << "newobj" << std::endl;
        break;
      }
      case kByteCodeLdstr: {
        msgpack::Value str;
        reader.Read(&str);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "ldstr"
           << " " << str.s_.ToString() << std::endl;
        break;
      }
      case kByteCodePop: {
        msgpack::Value pop_count;
        reader.Read(&pop_count);
        ss << "  " << std::setw(kOpCodeMaxWidth) << "pop"
           << " " << pop_count.int32_ << std::endl;
        break;
      }
      // case kByteCodeVirtualAddress: {
      //   msgpack::Value va;
      //   reader.Read(&va);
      //   ss << va.int32_ << ":";
      //   break;
      // }
      case kByteCodeTrace: {
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
    ss;
  }
  s->append(ss.str());
}
}
