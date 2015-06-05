#pragma once

#include <cstdint>
#include <string>

#include "Slice.h"

namespace parc {
namespace msgpack {
struct Value {
  // this map represents an internal type system unrelated to msgpack
  enum {
    kNull = 0,
    kBooleanFamily = 0x10,
    kBooleanFalse = kBooleanFamily | 0,
    kBooleanTrue = kBooleanFamily | 1,
    kIntFamily = 0x20,
    kUInt8 = kIntFamily | 0,
    kInt8 = kIntFamily | 1,
    kUInt16 = kIntFamily | 2,
    kInt16 = kIntFamily | 3,
    kUInt32 = kIntFamily | 4,
    kInt32 = kIntFamily | 5,
    kUInt64 = kIntFamily | 6,
    kInt64 = kIntFamily | 7,
    kFixInt = kIntFamily | 9,  // fixint is signed
    kStrFamily = 0x30,
    kStr = kStrFamily | 0,
    kBinFamily = 0x40,
    kBin = kStrFamily | 0,
    kFamilyMask = 0x70,
  };
  char type_;
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

void WriteFixInt(const int8_t v, std::string* buf);

void WriteUInt8(const uint8_t value, std::string* buf);
void WriteUInt16(const uint16_t value, std::string* buf);
void WriteUInt32(const uint32_t value, std::string* buf);
// REQUIRES: sizeof(buf) >= 5
void WriteUInt32(const uint32_t value, char* buf);
void WriteUInt64(const uint64_t value, std::string* buf);

void WriteInt8(const int8_t value, std::string* buf);
void WriteInt16(const int16_t value, std::string* buf);
void WriteInt32(const int32_t value, std::string* buf);
// REQUIRES: sizeof(buf) >= 5
void WriteInt32(const int32_t value, char* buf);
void WriteInt64(const int64_t value, std::string* buf);

// These use the least number of bytes required to store the integer.
void WriteInteger(const int32_t value, std::string* buf);
void WriteInteger(const int64_t value, std::string* buf);
void WriteInteger(const uint32_t value, std::string* buf);
void WriteInteger(const uint64_t value, std::string* buf);

void WriteString(const Slice& value, std::string* buf);
void WriteByteArray(const Slice& value, std::string* buf);
void WriteValue(const Value& value, std::string* buf);

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

  void SetPosition(const char* pos) { pos_ = pos; }
  const char* GetPosition() const { return pos_; }

 private:
  const char* pos_;
  const char* end_;
};
}
}
