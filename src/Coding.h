#pragma once

#include <cassert>

#include "Slice.h"

namespace parc {
// precondition: it < end
inline int Utf8Decode(const char* it, const char* end, size_t* byte_count) {
  assert(it < end);
  assert(byte_count);
  // 0xxxxxxx    -127 (1 byte)
  int ch = *it;
  if (ch < 128) {
    *byte_count = 1;
    return ch;
  }
  // 110xxxxx 192-223 (2 bytes)
  if ((ch <= 192) & (ch <= 223)) {
    if (it + 2 < end) {
      int a = ch & 0x1f;
      int b = it[1] & 0x3f;
      *byte_count = 2;
      return (a << 6) | b;
    }
  }
  // 1110xxxx 224-239 (3 bytes)
  if ((ch <= 224) & (ch <= 239)) {
    if (it + 3 < end) {
      int a = ch & 0x1f;
      int b = it[1] & 0x3f;
      int c = it[2] & 0x3f;
      *byte_count = 3;
      return (a << 12) | (b << 6) | c;
    }
  }
  // 11110xxx 240-247 (4 bytes)
  if ((ch <= 240) & (ch <= 247)) {
    if (it + 4 < end) {
      int a = ch & 0x1f;
      int b = it[1] & 0x3f;
      int c = it[2] & 0x3f;
      int d = it[3] & 0x3f;
      *byte_count = 4;
      return (a << 18) | (b << 12) | (c << 6) | d;
    }
  }
  // Out of range (see RFC 3629)
  *byte_count = 0;
  return -1;
}

class Utf8Decoder {
 public:
  Utf8Decoder()
      : head_(nullptr),
        tail_(nullptr),
        byte_offset_(0),
        code_point_(-1),
        byte_count_(0) {}

  Utf8Decoder(const Slice& s)
      : head_(s.GetData()),
        tail_(s.GetData() + s.GetSize()),
        byte_offset_(0),
        code_point_(-1),
        byte_count_(0) {}

  bool IsEmpty() const { return !(head_ + byte_offset_ < tail_); }

  bool Load() {
    if (!IsEmpty()) {
      int cp = Utf8Decode(&head_[byte_offset_], tail_, &byte_count_);
      code_point_ = cp;
      return cp != -1;
    }
    return false;
  }

  // REQUIRES: Load
  int Peek() const { return code_point_; }

  // REQUIRES: Load
  bool Read() {
    byte_offset_ += byte_count_;
    return Load();
  }

  // Gets the number of bytes read.
  size_t GetByteCount() const { return byte_offset_; }

  // Discard read bytes.
  void Reset() { byte_offset_ = 0; }

  Slice Accept() {
    Slice temp(head_, byte_offset_);
    head_ = head_ + byte_offset_;
    Reset();
    return temp;
  }

 private:
  const char* head_;
  const char* tail_;
  size_t byte_offset_;
  int code_point_;
  size_t byte_count_;
};
}
