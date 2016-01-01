#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum LogFlags {
  LOG_ERROR = (1 << 0),
} LogFlags;

void log_setflag(uint32_t mask, int value);

uint32_t log_getflag(uint32_t mask);

void log_format(uint32_t mask, const char* file, int line, const char* label,
                const char* fmt, ...);

// These log "functions" are actually macros

#define log_error(msg) \
  log_format(LOG_ERROR, __FILE__, __LINE__, "error", "%s", msg)

#define log_errorf(fmt, ...) \
  log_format(LOG_ERROR, __FILE__, __LINE__, "error", fmt, __VA_ARGS__)
