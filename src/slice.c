
#include "slice.h"

#include <string.h>

parc_slice parc_slice_create(const char *data, const size_t size) {
  parc_slice s;
  s.data_ = data;
  s.size_ = size;
  return s;
}

parc_slice parc_slice_create_from_string(const char *str) {
  return parc_slice_create(str, strlen(str));
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
