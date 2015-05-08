#pragma once

#include <string>
#include <cassert>

#include "Slice.h"
#include "Coding.h"

namespace parc {

class TextView {
 public:
  TextView(const Slice& s)
      : begin_(s.GetData()), end_(s.GetData() + s.GetSize()) {}

  bool const IsEmpty() const { return !(begin_ < end_); }

  bool Lookahead(const Slice& s) const {
    if ((begin_ + s.GetSize()) <= end_) {
      return memcmp(begin_, s.GetData(), s.GetSize()) == 0;
    }
    return false;
  }

  char Peek(int k = 0) const {
    if ((begin_ + k) < end_) {
      return begin_[k];
    }
    return '\0';
  }

  void Take(int k = 1) {
    if ((begin_ + k) <= end_) {
      begin_ = begin_ + k;
    }
  }

 private:
  const char* begin_;
  const char* end_;
};
}
