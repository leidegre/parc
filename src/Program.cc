
#include "Program.h"
#include "Emit.h"
#include "MsgPack.h"
#include "Utility.h"

#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace {
using namespace parc;
// todo: move to Interpreter.h?
bool ReadInstruction(msgpack::Reader* reader, msgpack::Value inst[4]) {
  auto v = &inst[0];
  if (!reader->Read(v++)) {
    return false;
  }
  switch (inst[0].int32_ & 3) {
    case 3: {
      if (!reader->Read(v++)) {
        return false;
      }
    }
    // fall-through case label
    case 2: {
      if (!reader->Read(v++)) {
        return false;
      }
    }
    // fall-through case label
    case 1: {
      if (!reader->Read(v++)) {
        return false;
      }
    }
  }
  return true;
}

void WriteInstruction(msgpack::Value inst[4], std::string* buf) {
  auto v = &inst[0];
  WriteValue(*v++, buf);
  switch (inst[0].int32_ & 3) {
    case 3: {
      WriteValue(*v++, buf);
    }
    // fall-through case label
    case 2: {
      WriteValue(*v++, buf);
    }
    // fall-through case label
    case 1: {
      WriteValue(*v++, buf);
    }
  }
}

void MemorizeTarget(msgpack::Value inst[4],
                    std::vector<std::pair<size_t, int>>* fixme,
                    std::string* byte_code) {
  msgpack::WriteInteger(inst[0].int32_, byte_code);
  int target = inst[1].int32_;
  fixme->push_back(std::make_pair(byte_code->size(), target));
  msgpack::WriteInt32(target, byte_code);
}
}

namespace parc {
void Program::LoadFrom(const ByteCodeGenerator& byte_code_generator) {
  byte_code_ = byte_code_generator.GetByteCodeStream();
  metadata_ = byte_code_generator.GetMetadataStream();
}

void Program::Optimize() {
  // OK, the format of the program in the byte code generator is using virtual
  // labels and a crazy amount of jump instructions. The primary purpose of this
  // function is to replace the virtual labels with byte offsets and minimize
  // (remove) unecessary jump instructions.

  // Note about virtual labels to byte code offsets transformation
  //
  // Take a look at this byte code
  //
  // 3:
  //   return
  // PrimaryExpression(2):
  //   accept   kTokenNumber(0)
  //   beq      3
  //   jmp      4
  // 5:
  //   ...
  // 4:
  //   accept   kTokenLeftParenthesis(2)
  //   beq      5
  //   jmp      9
  //
  // My typical strategy here would be to run a pass over the byte code under
  // the assumption that we will know the final offset of the label we wish to
  // jump to at the time we need it. This is sound but won't work. For example,
  // the production PrimaryExpression has a reference to label 3 and 4. We know
  // the offset of label 3 but we have not yet seen label 4. We will eventually
  // get to label 4 and we could then go back and patch the jump to label 4 but
  // since we use varible length encodings the bytes that we would have to
  // reserve would be variable so we can't predict the changes in offsets.
  //
  // Basically what we need to do is to write the offsets after we've identified
  // all the locations and as we patch the byte code with the actual offsets we
  // need to adjust the offsets after the current offset.
  //
  // The algorithm works like this.
  //
  // First run through the byte code and memorize the all the offsets of all the
  // labels, branch targets and calls. Emit the byte code but not the label, the
  // offset we want to memorize is where to insert our byte offset.
  //
  // As we work our way through this list (as we patch our byte code stream) we
  // simply maintain a relative offset for all the remaining patches that we
  // need to make.
  //
  // The bad part about this is that if we do inserts we force the buffer to
  // reorganize itself. This will have a very undersierable avalanche effect
  // (we're lots of copies of the remainder gets pushed back in the buffer). We
  // could ammortize this cost by over allocating space for offsets and then
  // reclaim that space as a last shrink-copy operation. The easiest solution is
  // to use a fixed space integer designation. Such as int32 for offsets (we can
  // always optimize this particular case later).

  // a parc program is defined as a metadata section that is read first
  // followed by the runnable byte code (but in a single stream)
  std::unordered_map<int, size_t> offsets;
  std::vector<std::pair<size_t, int>> fixme;

  std::string byte_code;
  msgpack::Value inst[4];
  msgpack::Reader byte_code_reader(byte_code_);
  while (ReadInstruction(&byte_code_reader, inst)) {
    switch (inst[0].int32_) {
      case ByteCode::kBranchOnEqual:
      case ByteCode::kBranch:
      case ByteCode::kCall: {
        MemorizeTarget(inst, &fixme, &byte_code);
        break;
      }
      case ByteCode::kLabel: {
        int label = inst[1].int32_;
        offsets.insert(std::make_pair(label, byte_code.size()));
        break;
      }
      default: {
        WriteInstruction(inst, &byte_code);
        break;
      }
    }
  }

  // fix offsets
  for (auto item : fixme) {
    auto it = offsets.find(item.second);
    assert(it != offsets.end());
    msgpack::WriteInt32(it->second, &byte_code[item.first]);
  }

  std::string metadata;
  msgpack::Reader metadata_reader(metadata_);
  while (ReadInstruction(&metadata_reader, inst)) {
    switch (inst[0].int32_) {
      case ByteCodeGenerator::kMetadataLabel: {
        int label = inst[1].int32_;
        Slice name = inst[2].s_;
        auto it = offsets.find(label);
        assert(it != offsets.end() &&
               "cannot load program: cannot find label metadata");
        msgpack::WriteInteger(ByteCodeGenerator::kMetadataLabelOffset,
                              &metadata);
        msgpack::WriteInteger(it->second, &metadata);
        msgpack::WriteString(name, &metadata);
        break;
      }
      default: { WriteInstruction(inst, &metadata); }
    }
  }

  byte_code_.swap(byte_code);
  metadata_.swap(metadata);
}

void Program::LoadFrom(const Slice& data) {
  msgpack::Reader reader(data);
  msgpack::Value v;
  if (!reader.Read(&v) ||
      !((v.type_ & msgpack::Value::kMapFamily) == msgpack::Value::kMapFamily)) {
    assert(false && "Cannot load program: bad header");
    return;
  }
  std::string magic_number;
  size_t count = v.uint32_;
  for (size_t i = 0; i < count && reader.Read(&v); i++) {
    switch (v.uint32_) {
      case 0: {
        reader.Read(&v);
        magic_number = v.s_.ToString();
        break;
      }
      case 1: {
        reader.Read(&v);
        metadata_ = v.s_.ToString();
        break;
      }
      case 2: {
        reader.Read(&v);
        byte_code_ = v.s_.ToString();
        break;
      }
      default: {
        assert(false && "Cannot load program: bad data segment");
        break;
      }
    }
  }
  if (magic_number != "PARCv1") {
    assert(false && "Cannot load program: bad magic number");
  }
}

int32_t Program::GetAddress(const Slice& label) {
  msgpack::Value metadata[4];
  msgpack::Reader reader(metadata_);
  while (reader.Read(&metadata[0])) {
    int count = metadata[0].int32_ & 3;
    for (int i = 0; i < count; i++) {
      reader.Read(&metadata[1 + i]);
    }
    switch (metadata[0].int32_) {
      case ByteCodeGenerator::kMetadataLabelOffset: {
        if (label.CompareTo(metadata[2].s_) == 0) {
          return metadata[1].int32_;
        }
        break;
      }
    }
  }
  return -1;
}

void Program::Disassemble(std::string* s) const {
  assert(s);
  std::stringstream ss;

  std::unordered_map<int, Slice> label_metadata;
  std::unordered_map<int, Slice> offset_metadata;

  {
    msgpack::Value metadata[4];
    msgpack::Reader reader(metadata_);
    while (reader.Read(&metadata[0])) {
      int count = metadata[0].int32_ & 3;
      for (int i = 0; i < count; i++) {
        reader.Read(&metadata[1 + i]);
      }
      switch (metadata[0].int32_) {
        case ByteCodeGenerator::kMetadataLabel: {
          label_metadata.insert(
              std::make_pair(metadata[1].int32_, metadata[2].s_));
          break;
        }
        case ByteCodeGenerator::kMetadataLabelOffset: {
          offset_metadata.insert(
              std::make_pair(metadata[1].int32_, metadata[2].s_));
          break;
        }
      }
    }
  }

  msgpack::Value inst[4];
  msgpack::Reader reader(byte_code_);
  for (const char* p0 = reader.GetPosition();;) {
    int offset = (int)(reader.GetPosition() - p0);
    if (!reader.Read(&inst[0])) {
      break;
    }
    int op_code = inst[0].int32_;
    ss << std::setw(4) << offset << " ";

    ss << std::setw(8);
    if (op_code < ByteCode::kMax) {
      const char* mn = ByteCode::GetMnemonic(op_code);
      if (mn) {
        ss << mn << " ";
      } else {
        ss << op_code << " ";
      }
    } else {
      ss << op_code << " ";
    }

    for (int i = 0, arg_count = op_code & 3; i < arg_count; i++) {
      if (i > 0) {
        ss << ", ";
      }
      if (!reader.Read(&inst[1 + i])) {
        fprintf(stderr, "cannot decompile: \n%s", ss.str().c_str());
        assert(false && "cannot decompile: unexpected end of stream");
        break;
      }
      const auto& v = inst[1 + i];
      switch (v.type_ & 0xf0) {
        case msgpack::Value::kIntFamily: {
          ss << std::setw(4);
          if ((v.type_ & 1) == 1) {
            ss << v.int64_;
          } else {
            ss << v.uint64_;
          }
          break;
        }
        case msgpack::Value::kStrFamily: {
          ss << v.s_.ToString();
          break;
        }
        default: {
          fprintf(stderr, "cannot decompile: \n%s", ss.str().c_str());
          assert(false && "cannot decompile: unrecognized type family");
          break;
        }
      }
    }

    if (op_code == ByteCode::kLabel) {
      auto it = label_metadata.find(inst[1].int32_);
      if (it != label_metadata.end()) {
        ss << "  ; " << it->second.ToString();
      }
    }

    auto it = offset_metadata.find((int)offset);
    if (it != offset_metadata.end()) {
      ss << "  ; " << it->second.ToString();
    }

    ss << std::endl;
  }

  s->swap(ss.str());
}

void Program::Save(std::string* s) const {
  msgpack::WriteMap(3, s);
  msgpack::WriteInteger(0, s);
  msgpack::WriteString("PARCv1", s);
  msgpack::WriteInteger(1, s);
  msgpack::WriteByteArray(metadata_, s);
  msgpack::WriteInteger(2, s);
  msgpack::WriteByteArray(byte_code_, s);
}
}
