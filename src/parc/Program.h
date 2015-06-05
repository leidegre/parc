#pragma once

#include <string>

namespace parc {
class ByteCodeGenerator;

class Program {
 public:
  void LoadFrom(const ByteCodeGenerator& byte_code_generator);

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
};
}
