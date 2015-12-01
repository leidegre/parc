#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct string_builder {
  char* front_;
  char* back_;
} string_builder;

void string_builder_init(char* buffer, size_t buffer_size, string_builder* sb);

int string_builder_ensure(string_builder* sb, size_t size);

int string_builder_append(string_builder* sb, const char ch);

int string_builder_append_c(string_builder* sb, const char ch, int count);

int string_builder_append_strlen(string_builder* sb, const char* s, size_t len);

int string_builder_append_str(string_builder* sb, const char* s);

int string_builder_append_f(string_builder* sb, const char* fmt, ...);
