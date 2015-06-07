#pragma once

#include <string>
#include <cassert>

namespace parc {
class Slice {
 public:
  Slice() : data_(nullptr), size_(0) {}

  Slice(const char* data) : data_(data), size_(strlen(data)) {}

  Slice(const char* data, size_t size) : data_(data), size_(size) {}

  Slice(const std::string& s) : data_(s.c_str()), size_(s.size()) {}

  bool IsEmpty() const { return size_ == 0; }

  const char* GetData() const { return data_; }

  size_t GetSize() const { return size_; }

  Slice Offset(size_t offset) const {
    assert(offset <= size_);
    return Slice(data_ + offset, size_ - offset);
  }

  std::string ToString() const {
    return IsEmpty() ? std::string() : std::string(data_, size_);
  }

 private:
  const char* data_;
  size_t size_;
};
}
