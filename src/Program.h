#pragma once

#include "Slice.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace parc {
class ByteCodeGenerator;

class Program {
 public:
  void LoadFrom(const ByteCodeGenerator& byte_code_generator);

  void LoadFrom(const Slice& data);

  // REQUIRES: LoadFrom
  void Optimize();

  // REQUIRES: LoadFrom, Optimize
  int32_t GetAddress(const Slice& label);

  // REQUIRES: LoadFrom
  const std::string& GetByteCode() const { return byte_code_; }

  // REQUIRES: LoadFrom
  const std::string& GetMetadata() const { return metadata_; }

  // REQUIRES: LoadFrom
  void Disassemble(std::string* s) const;

  // REQUIRES: LoadFrom
  void Save(std::string* s) const;

 private:
  std::string byte_code_;
  std::string metadata_;
  // todo: remove, replace with linear metadata stream search
  std::unordered_map<std::string, size_t> labels_;
};
}
