
#include "Program.h"
#include "Emit.h"
#include "MsgPack.h"
#include "Utility.h"

#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace {
using namespace parc;
void CopyByteCode(const int byte_code, msgpack::Reader* inp, std::string* buf) {
  msgpack::WriteInteger(byte_code, buf);
  for (int i = 0, arg_count = byte_code & 3; i < arg_count; i++) {
    msgpack::Value arg;
    inp->Read(&arg);
    msgpack::WriteValue(arg, buf);
  }
}

void MemorizeTarget(msgpack::Reader* inp,
                    std::vector<std::pair<size_t, int>>* fixme,
                    std::string* byte_code) {
  msgpack::Value target;
  inp->Read(&target);
  fixme->push_back(std::make_pair(byte_code->size(), target.int32_));
  msgpack::WriteInt32(target.int32_, byte_code);
}
}

namespace parc {
void Program::LoadFrom(const ByteCodeGenerator& byte_code_generator) {
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
  msgpack::Value v;
  msgpack::Reader reader(byte_code_generator.GetByteCodeStream());
  while (reader.Read(&v)) {
    switch (v.int32_) {
      case ByteCode::kBranchOnEqual:
      case ByteCode::kBranch:
      case ByteCode::kCall: {
        msgpack::WriteInteger(v.int32_, &byte_code);
        MemorizeTarget(&reader, &fixme, &byte_code);
        break;
      }
      case ByteCode::kLabel: {
        msgpack::Value label;
        reader.Read(&label);
        offsets.insert(std::make_pair(label.int32_, byte_code.size()));
        break;
      }
      default: {
        CopyByteCode(v.int32_, &reader, &byte_code);
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
  msgpack::Reader reader2(byte_code_generator.GetMetadataStream());
  while (reader2.Read(&v)) {
    switch (v.int32_) {
      case ByteCodeGenerator::kMetadataLabel: {
        msgpack::Value label;
        reader2.Read(&label);
        msgpack::Value name;
        reader2.Read(&name);
        auto it = offsets.find(label.int32_);
        assert(it != offsets.end() &&
               "cannot load program: cannot find label metadata");
        msgpack::WriteInteger(ByteCodeGenerator::kMetadataLabel, &metadata);
        msgpack::WriteInteger(it->second, &metadata);
        msgpack::WriteValue(name, &metadata);
        break;
      }
      case ByteCodeGenerator::kMetadataToken: {
        msgpack::Value token;
        reader2.Read(&token);
        msgpack::Value name;
        reader2.Read(&name);
        // msgpack::WriteInteger(ByteCodeGenerator::kMetadataToken, &metadata);
        // msgpack::WriteValue(token, &metadata);
        // msgpack::WriteValue(name, &metadata);
        break;
      }
    }
  }

  byte_code_ = byte_code;
  metadata_ = metadata;
}

void Program::Decompile(std::string* s) const {
  assert(s);
  std::stringstream ss;

  const char* mnemonic[ByteCode::kMax];
  memset(mnemonic, 0, sizeof(mnemonic));
  mnemonic[ByteCode::kAccept] = "accept";
  mnemonic[ByteCode::kBranchOnEqual] = "beq";
  mnemonic[ByteCode::kBranch] = "jmp";
  mnemonic[ByteCode::kCall] = "call";
  mnemonic[ByteCode::kReturn] = "ret";
  mnemonic[ByteCode::kError] = "err";
  mnemonic[ByteCode::kLabel] = "lbl";

  std::unordered_map<int, Slice> label_metadata;

  msgpack::Value v2;
  msgpack::Reader reader2(metadata_);
  while (reader2.Read(&v2)) {
    switch (v2.int32_) {
      case ByteCodeGenerator::kMetadataLabel: {
        msgpack::Value label;
        reader2.Read(&label);
        msgpack::Value name;
        reader2.Read(&name);
        label_metadata.insert(std::make_pair(label.int32_, name.s_));
        break;
      }
      case ByteCodeGenerator::kMetadataToken: {
        msgpack::Value token;
        reader2.Read(&token);
        msgpack::Value name;
        reader2.Read(&name);
        break;
      }
    }
  }

  msgpack::Value v;
  msgpack::Reader reader(byte_code_);
  for (const char* p0 = reader.GetPosition();;) {
    size_t offset = reader.GetPosition() - p0;
    if (!reader.Read(&v)) {
      break;
    }
    switch (v.int32_) {
      default: {
        ss << std::setw(4) << offset << " ";

        ss << std::setw(8);
        if (v.int32_ < ByteCode::kMax) {
          const char* mn = mnemonic[v.int32_];
          if (mn) {
            ss << mn << " ";
          } else {
            ss << v.int32_ << " ";
          }
        } else {
          ss << v.int32_ << " ";
        }

        for (int i = 0, arg_count = v.int32_ & 3; i < arg_count; i++) {
          if (i > 0) {
            ss << ", ";
          }
          if (!reader.Read(&v)) {
            fprintf(stderr, "cannot decompile: \n%s", ss.str().c_str());
            assert(false && "cannot decompile: unexpected end of stream");
            break;
          }
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

        auto it = label_metadata.find((int)offset);
        if (it != label_metadata.end()) {
          ss << "  ; " << it->second.ToString();
        }

        ss << std::endl;
      }
    }
  }

  s->swap(ss.str());
}

void Program::Save(std::string* s) const {
  // I did not expect PARC to turn out CRAP just becuase msgpack is using
  // big-endian. lol.
  s->append(5, '\0');
  msgpack::WriteUInt32(*((const uint32_t*)"CRAP"), &s->operator[](0));
  msgpack::WriteByteArray(metadata_, s);
  msgpack::WriteByteArray(byte_code_, s);
}
}
