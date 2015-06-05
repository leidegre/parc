
#include "VES.h"

#include "MsgPack.h"

namespace parc {
// a program is metadata and byte code (the metadata contains the symbolic
// names for tokens and production rules)
bool StackMachine::Exec(const Slice& byte_code, TokenInputStream* inp) {
  msgpack::Reader reader(byte_code);
  return false;
}
}
