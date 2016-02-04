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

parc_slice _parc_slice_create1(const char *str);
parc_slice _parc_slice_create2(const char *data, const size_t size);
#define _parc_slice_create(_1, _2, fname, ...) fname
#define parc_slice_create(...)                                              \
  _parc_slice_create(__VA_ARGS__, _parc_slice_create2, _parc_slice_create1, \
                     __no_args__)(__VA_ARGS__)

int parc_slice_compare(const parc_slice a, const parc_slice b);
