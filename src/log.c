
#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static uint32_t log_flags = LOG_ERROR;

void log_setflag(uint32_t mask, int value) {
  log_flags = (log_flags & ~mask) | (((uint32_t)-value) & mask);
}

uint32_t log_getflag(uint32_t mask) {
  return log_flags;
}

static const char* get_filename(const char* file) {
  const char* end = file + strlen(file);
  while (end > file) {
    const char ch = *end;
    if ((ch == '/') | (ch == '\\')) {
      break;
    }
    end--;
  }
  return end;
}

void log_format(uint32_t mask, const char* file, int line, const char* label,
                const char* fmt, ...) {
  if (log_getflag(mask)) {
    char temp[2048];
    size_t n = snprintf(temp, sizeof(temp), "%s:%i: %8s: ", get_filename(file),
                        line, label);

    va_list args;
    va_start(args, fmt);
    vsnprintf(temp + n, sizeof(temp) - n, fmt, args);
    va_end(args);

    fprintf(stderr, "%s\n", temp);
  }
}
