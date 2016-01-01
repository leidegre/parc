#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct file {
  char* data_;
  size_t size_;
} file;

int file_open(const char* fn, file* f);

int file_destroy(file* f);

int file_write_delete(const char* fn);

int file_write_data(const char* fn, const char* data, size_t size);
