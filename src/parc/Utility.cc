
#include "Utility.h"

namespace parc {
char GetNibble(unsigned char v) { return ("0123456789abcdef")[v & 0xf]; }

void GetHexDump(const Slice& data, std::string* hex) {
  assert(hex);
  const char* begin = data.GetData();
  const char* end = data.GetData() + data.GetSize();
  for (const char* it = begin; it < end; it += 16) {
    size_t offset = hex->size();
    hex->append(54, ' ');
    hex->append(1, '\n');
    char* buf = &hex->operator[](offset);

    size_t address = it - begin;
    buf[0] = GetNibble(address >> 12);
    buf[1] = GetNibble(address >> 8);
    buf[2] = GetNibble(address >> 4);
    buf[3] = GetNibble(address);

    size_t count = it + 16 < end ? 16 : end - it;
    for (size_t i = 0; i < count; i++) {
      unsigned char v = (unsigned char)it[i];

      size_t j = 4 + 1 + 2 * i;
      buf[j + 0] = GetNibble(v >> 4);
      buf[j + 1] = GetNibble(v);

      size_t k = (54 - 16) + i;
      if ((0x20 <= v) & (v <= 0x7f)) {
        buf[k] = it[i];
      } else {
        buf[k] = '.';  // mask
      }
    }
  }
}
}
