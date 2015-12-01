
#include "util.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void string_builder_init(char* buffer, size_t buffer_size, string_builder* sb) {
  sb->front_ = buffer;
  sb->back_ = buffer + buffer_size;
}

int string_builder_ensure(string_builder* sb, size_t size) {
  return (sb->front_ + size) <= sb->back_;
}

int string_builder_append(string_builder* sb, const char ch) {
  if (string_builder_ensure(sb, 2)) {
    *sb->front_++ = ch;
    *sb->front_ = '\0';
    return 1;
  }
  return 0;
}

int string_builder_append_c(string_builder* sb, const char ch, int count) {
  for (int i = 0; i < count; i++) {
    if (!string_builder_append(sb, ch)) {
      return 0;
    }
  }
  return 1;
}

int string_builder_append_strlen(string_builder* sb, const char* s,
                              size_t length) {
  if (string_builder_ensure(sb, length + 1)) {
    memcpy(sb->front_, s, length);
    sb->front_ += length;
    *sb->front_ = '\0';
    return 1;
  }
  return 0;
}

int string_builder_append_str(string_builder* sb, const char* s) {
  return string_builder_append_strlen(sb, s, strlen(s));
}

int string_builder_append_f(string_builder* sb, const char* fmt, ...) {
  size_t size = sb->back_ - sb->front_;
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(sb->front_, size, fmt, args);
  va_end(args);
  if ((0 <= result) & ((size_t)result < size)) {
    sb->front_ += (size_t)result;
    return 1;
  }
  return 0;
}
