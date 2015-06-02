#pragma once

#include <cstdint>
#include <string>

#include "Slice.h"

namespace parc {
namespace msgpack {
struct Value {
  enum {
    // this map represents our internal type system (not the msgpack spec, see
    // Reader::Read)
    kNull = 0,
    kBooleanFamily = (1 << 3),
    kBooleanFalse = kBooleanFamily | 0,
    kBooleanTrue = kBooleanFamily | 1,
    kIntFamily = (2 << 3),
    kUInt8 = kIntFamily | 0,
    kInt8 = kIntFamily | 1,
    kUInt16 = kIntFamily | 2,
    kInt16 = kIntFamily | 3,
    kUInt32 = kIntFamily | 4,
    kInt32 = kIntFamily | 5,
    kUInt64 = kIntFamily | 6,
    kInt64 = kIntFamily | 7,
    kStrFamily = (3 << 3),
    kStr = kStrFamily | 0,
  };
  uint8_t type_;
  union {
    bool bool_;
    int32_t int32_;
    uint32_t uint32_;
    int64_t int64_;
    uint64_t uint64_;
    size_t size_;
  };
  Slice s_;
};

void WriteNull(std::string* buf);
void WriteBoolean(const bool value, std::string* buf);
void WriteInteger(const uint32_t value, std::string* buf);
void WriteInteger(const uint64_t value, std::string* buf);
void WriteInteger(const int32_t value, std::string* buf);
void WriteInteger(const int64_t value, std::string* buf);
void WriteString(const Slice& value, std::string* buf);

class Reader {
 public:
  Reader(const Slice& data)
      : pos_(data.GetData()), end_(data.GetData() + data.GetSize()) {}

  bool Read(Value* value);

  uint8_t ReadUInt8();
  int8_t ReadInt8();
  uint16_t ReadUInt16();
  int16_t ReadInt16();
  uint32_t ReadUInt32();
  int32_t ReadInt32();
  uint64_t ReadUInt64();
  int64_t ReadInt64();

  Slice ReadLengthDelimited(size_t length);

  const char* GetPosition() const { return pos_; }

 private:
  const char* pos_;
  const char* end_;
};
}
}
