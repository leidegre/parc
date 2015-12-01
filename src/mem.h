#pragma once

#include "api.h"

typedef struct parc_allocator parc_allocator;

typedef struct parc_allocator {
  void *(PARC_CALLCONV *alloc_)(parc_allocator *allocator, size_t size);
} parc_allocator;

typedef struct parc_stack_allocator {
  parc_allocator base_;
  char *stack_begin_;
  char *stack_end_;
  char *stack_front_;
  char *stack_back_;
} parc_stack_allocator;

void parc_stack_allocator_create(char *buffer, size_t buffer_size,
                                 parc_stack_allocator *stack_allocator);