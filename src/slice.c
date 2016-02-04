
#include "slice.h"

#include <string.h>

parc_slice _parc_slice_create1(const char *str) {
  return _parc_slice_create2(str, strlen(str));
}

parc_slice _parc_slice_create2(const char *data, const size_t size) {
  parc_slice s;
  s.data_ = data;
  s.size_ = size;
  return s;
}

int parc_slice_compare(const parc_slice a, const parc_slice b) {
  size_t min_size = a.size_ < b.size_ ? a.size_ : b.size_;
  int c = memcmp(a.data_, b.data_, min_size);
  if (c == 0) {
    if (a.size_ < b.size_) {
      c = -1;
    }
    if (a.size_ > b.size_) {
      c = +1;
    }
  }
  return c;
}
