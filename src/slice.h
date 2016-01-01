#pragma once

#include <stddef.h>

// A read-only slice of memory. The life-time of this memory is managed
// elsewhere. Care must be taken as to not access memory that has been freed if
// slice is kept around.
typedef struct parc_slice {
  const char *data_;
  size_t size_;
} parc_slice;

// API

parc_slice parc_slice_create(const char *data, const size_t size);

parc_slice parc_slice_create_from_string(const char *s);

int parc_slice_compare(const parc_slice a, const parc_slice b);
