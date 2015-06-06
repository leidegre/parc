#pragma once

#include "Slice.h"

#include <string>
#include <unordered_map>

namespace parc {
class ByteCodeGenerator;

class Program {
 public:
  void LoadFrom(const ByteCodeGenerator& byte_code_generator);

  void LoadFrom(const Slice& data);

  // REQUIRES: LoadFrom
  void Initialize();

  // REQUIRES: Initialize
  size_t GetAddress(const std::string& label) const {
    auto it = labels_.find(label);
    if (it != labels_.end()) {
      return it->second;
    }
    return (size_t)-1;
  }

  // REQUIRES: Initialize
  const std::unordered_map<std::string, size_t>& GetAddressMap() const {
    return labels_;
  }

  // REQUIRES: LoadFrom
  const std::string& GetByteCode() const { return byte_code_; }

  // REQUIRES: LoadFrom
  const std::string& GetMetadata() const { return metadata_; }

  // REQUIRES: LoadFrom
  void Decompile(std::string* s) const;

  // REQUIRES: LoadFrom
  void Save(std::string* s) const;

 private:
  std::string byte_code_;
  std::string metadata_;
  std::unordered_map<std::string, size_t> labels_;
};
}
