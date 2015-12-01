
#include "mem.h"

static void * PARC_CALLCONV parc_stack_allocator_alloc(parc_allocator *allocator,
                                        size_t size) {
  parc_stack_allocator *stack_allocator = (parc_stack_allocator *)allocator;
  char *p = stack_allocator->stack_front_ + size;
  if (p <= stack_allocator->stack_back_) {
    stack_allocator->stack_front_ = p;
    return p;
  }
  return NULL;
}

void parc_stack_allocator_create(char *buffer, size_t buffer_size,
                                 parc_stack_allocator *stack_allocator) {
  memset(stack_allocator, 0, sizeof(parc_stack_allocator));
  stack_allocator->base_.alloc_ = &parc_stack_allocator_alloc;
  stack_allocator->stack_begin_ = buffer;
  stack_allocator->stack_end_ = buffer + buffer_size;
  stack_allocator->stack_front_ = buffer;
  stack_allocator->stack_back_ = buffer + buffer_size;
}