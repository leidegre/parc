
#include <cassert>

#include "MsgPack.h"

#if _MSC_VER
#include <cstdlib>
#endif

namespace {
void WriteLengthDelimited(uint8_t type_code, size_t size, std::string* buf) {
  if (size <= UINT8_MAX) {
    buf->append(1, (char)type_code);
    buf->append(1, (char)(size & 0xff));
    return;
  }
  if (size <= UINT16_MAX) {
    buf->append(1, (char)(type_code + 1));
    buf->append(1, (char)((size >> 8) & 0xff));
    buf->append(1, (char)(size & 0xff));
    return;
  }
  if (size <= UINT32_MAX) {
    buf->append(1, (char)(type_code + 2));
    buf->append(1, (char)((size >> 24) & 0xff));
    buf->append(1, (char)((size >> 16) & 0xff));
    buf->append(1, (char)((size >> 8) & 0xff));
    buf->append(1, (char)(size & 0xff));
    return;
  }
  assert(false);
}

// if the host platform is big-endian replace these with an identity function
uint16_t ByteSwap(uint16_t v) {
#if _MSC_VER
  return _byteswap_ushort(v);
#else
#pragma error("fallback needed for byte swap")
#endif
}

uint32_t ByteSwap(uint32_t v) {
#if _MSC_VER
  return _byteswap_ulong(v);
#else
#pragma error("fallback needed for byte swap")
#endif
}

uint64_t ByteSwap(uint64_t v) {
#if _MSC_VER
  return _byteswap_uint64(v);
#else
#pragma error("fallback needed for byte swap")
#endif
}
}

namespace parc {
namespace msgpack {
void WriteNull(std::string* buf) { buf->append(1, (char)0xc0); }

void WriteBoolean(bool value, std::string* buf) {
  buf->append(1, (char)(0xc2 + (value ? 1 : 0)));
}

void WriteInteger(const uint32_t value, std::string* buf) {
  if (value <= 0x7f) {  // 1 byte (7-bit positive integer)
    buf->append(1, (char)value);
    return;
  }
  if (value <= UINT8_MAX) {  // 2 bytes (8-bit unsigned integer)
    buf->append(1, (char)0xcc);
    buf->append(1, (char)value);
    return;
  }
  if (value <= UINT16_MAX) {  // 3 bytes (16-bit big-endian unsigned integer)
    buf->append(1, (char)0xcd);
    buf->append(1, (char)((value >> 8) & 0xff));
    buf->append(1, (char)(value & 0xff));
    return;
  }
  // 5 bytes (32-bit big-endian unsigned integer)
  buf->append(1, (char)0xce);
  buf->append(1, (char)((value >> 24) & 0xff));
  buf->append(1, (char)((value >> 16) & 0xff));
  buf->append(1, (char)((value >> 8) & 0xff));
  buf->append(1, (char)(value & 0xff));
}

void WriteInteger(const int32_t value, std::string* buf) {
  if ((0 <= value) & (value <= 0x7f)) {  // 1 byte (7-bit positive integer)
    buf->append(1, (char)value);
    return;
  }
  if ((-32 <= value) & (value <= -1)) {  // 1 byte (5-bit negative integer)
    buf->append(1, (char)value);
    return;
  }
  // 2 bytes (8-bit signed integer)
  if ((INT8_MIN <= value) & (value <= INT8_MAX)) {
    buf->append(1, (char)0xd0);
    buf->append(1, (char)value);
    return;
  }
  // 3 bytes (16-bit big-endian signed integer)
  if ((INT16_MIN <= value) & (value <= INT16_MAX)) {
    buf->append(1, (char)0xd1);
    buf->append(1, (char)((value >> 8) & 0xff));
    buf->append(1, (char)(value & 0xff));
    return;
  }
  // 5 bytes (32-bit big-endian signed integer)
  buf->append(1, (char)0xd2);
  buf->append(1, (char)((value >> 24) & 0xff));
  buf->append(1, (char)((value >> 16) & 0xff));
  buf->append(1, (char)((value >> 8) & 0xff));
  buf->append(1, (char)(value & 0xff));
}

void WriteInteger(const uint64_t value, std::string* buf) {
  if (value <= UINT32_MAX) {
    WriteInteger((uint32_t)value, buf);
    return;
  }
  // 9 bytes (64-bit big-endian unsigned integer)
  buf->append(1, (char)0xcf);
  buf->append(1, (char)((value >> 56) & 0xff));
  buf->append(1, (char)((value >> 48) & 0xff));
  buf->append(1, (char)((value >> 40) & 0xff));
  buf->append(1, (char)((value >> 32) & 0xff));
  buf->append(1, (char)((value >> 24) & 0xff));
  buf->append(1, (char)((value >> 16) & 0xff));
  buf->append(1, (char)((value >> 8) & 0xff));
  buf->append(1, (char)(value & 0xff));
}

void WriteInteger(const int64_t value, std::string* buf) {
  if ((INT32_MIN <= value) & (value <= INT32_MAX)) {  // 1 byte
    WriteInteger((int32_t)value, buf);
    return;
  }
  // 9 bytes (64-bit big-endian signed integer)
  buf->append(1, (char)0xd3);
  buf->append(1, (char)((value >> 56) & 0xff));
  buf->append(1, (char)((value >> 48) & 0xff));
  buf->append(1, (char)((value >> 40) & 0xff));
  buf->append(1, (char)((value >> 32) & 0xff));
  buf->append(1, (char)((value >> 24) & 0xff));
  buf->append(1, (char)((value >> 16) & 0xff));
  buf->append(1, (char)((value >> 8) & 0xff));
  buf->append(1, (char)(value & 0xff));
}

void WriteString(const Slice& value, std::string* buf) {
  assert(value.GetSize() <= UINT32_MAX);
  if (value.GetSize() <= 31) {
    buf->append(1, (char)(0xa0 | value.GetSize()));
  } else {
    WriteLengthDelimited(0xd9, value.GetSize(), buf);
  }
  buf->append(value.GetData(), value.GetSize());
}

bool Reader::Read(Value* value) {
  // see https://github.com/msgpack/msgpack/blob/master/spec.md#formats-overview
  if (pos_ < end_) {
    uint8_t type_code = *pos_++;  // todo: don't move ptr if error in stream
    if (type_code <= 0x7f) {      // 0x00 - 0x7f
      value->type_ = Value::kInt8;
      value->int64_ = type_code;
    } else if (type_code <= 0x8f) {  // 0x80 - 0x8f
      assert(false);
    } else if (type_code <= 0x9f) {  // 0x90 - 0x9f
      assert(false);
    } else if (type_code <= 0xbf) {  // 0xa0 - 0xbf
      value->type_ = Value::kStr;
      size_t size = type_code & 0x1f;
      value->s_ = ReadLengthDelimited(size);
    } else if (type_code <= 0xdf) {  // 0xc0 - 0xdf
      switch (type_code) {
        case 0xc0: {
          value->type_ = Value::kNull;
          value->uint64_ = 0;
          break;
        }
        case 0xc2: {  // false
          value->type_ = Value::kBooleanFalse;
          value->uint64_ = 0;
          break;
        }
        case 0xc3: {  // true
          value->type_ = Value::kBooleanTrue;
          value->uint64_ = 1;
          break;
        }
        case 0xcc: {
          value->type_ = Value::kUInt8;
          value->uint64_ = ReadUInt8();
          break;
        }
        case 0xcd: {
          value->type_ = Value::kUInt16;
          value->uint64_ = ReadUInt16();
          break;
        }
        case 0xce: {
          value->type_ = Value::kUInt32;
          value->uint64_ = ReadUInt32();
          break;
        }
        case 0xcf: {
          value->type_ = Value::kUInt64;
          value->uint64_ = ReadUInt64();
          break;
        }
        case 0xd0: {
          value->type_ = Value::kInt8;
          value->int64_ = ReadInt8();
          break;
        }
        case 0xd1: {
          value->type_ = Value::kInt16;
          value->int64_ = ReadInt16();
          break;
        }
        case 0xd2: {
          value->type_ = Value::kInt32;
          value->int64_ = ReadInt32();
          break;
        }
        case 0xd3: {
          value->type_ = Value::kInt64;
          value->int64_ = ReadInt64();
          break;
        }
        case 0xd9: {
          value->type_ = Value::kStr;
          size_t size = ReadUInt8();
          value->s_ = ReadLengthDelimited(size);
          break;
        }
        case 0xda: {
          value->type_ = Value::kStr;
          size_t size = ReadUInt16();
          value->s_ = ReadLengthDelimited(size);
          break;
        }
        case 0xdb: {
          value->type_ = Value::kStr;
          size_t size = ReadUInt32();
          value->s_ = ReadLengthDelimited(size);
          break;
        }
        default: { assert(false); }
      }
    } else {  // 0xe0 - 0xff
      value->type_ = Value::kInt8;
      value->int64_ = (int8_t)type_code;
    }
    return true;
  }
  return false;
}

uint8_t Reader::ReadUInt8() {
  uint8_t v = *(const uint8_t*)pos_;
  pos_ += 1;
  return v;
}
int8_t Reader::ReadInt8() {
  uint8_t v = *(const uint8_t*)pos_;
  pos_ += 1;
  return (int8_t)v;
}
uint16_t Reader::ReadUInt16() {
  uint16_t v = *(const uint16_t*)pos_;
  pos_ += 2;
  v = ByteSwap(v);
  return v;
}
int16_t Reader::ReadInt16() {
  uint16_t v = *(const uint16_t*)pos_;
  pos_ += 2;
  v = ByteSwap(v);
  return (int16_t)v;
}
uint32_t Reader::ReadUInt32() {
  uint32_t v = *(const uint32_t*)pos_;
  pos_ += 4;
  v = ByteSwap(v);
  return v;
}
int32_t Reader::ReadInt32() {
  uint32_t v = *(const uint32_t*)pos_;
  pos_ += 4;
  v = ByteSwap(v);
  return (int32_t)v;
}
uint64_t Reader::ReadUInt64() {
  uint64_t v = *(const uint64_t*)pos_;
  pos_ += 8;
  v = ByteSwap(v);
  return v;
}
int64_t Reader::ReadInt64() {
  uint64_t v = *(const uint64_t*)pos_;
  pos_ += 8;
  v = ByteSwap(v);
  return (int64_t)v;
}

Slice Reader::ReadLengthDelimited(size_t length) {
  Slice v((const char*)pos_, length);
  pos_ += length;
  return v;
}
}
}
