
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "log.h"

int file_open(const char* fn, file* f) {
  FILE* fp = fopen(fn, "rb");
  if (!fp) {
    log_errorf("Cannot open file '%s'.", fn);
    return 0;
  }
  if (fseek(fp, 0, SEEK_END) == -1) {
    log_errorf("Cannot seek to end of file '%s'.", fn);
    return 0;
  }
  long size;
  if ((size = ftell(fp)) == -1) {
    log_errorf("Cannot get file '%s' pointer location (file size).", fn);
    return 0;
  }
  if (fseek(fp, 0, SEEK_SET) == -1) {
    log_errorf("Cannot seek to begining of file '%s'.", fn);
    return 0;
  }
  char* data = malloc((size_t)size);
  if (!data) {
    log_errorf("Not enough memory (%ull bytes requested).", size);
    return 0;
  }
  if (fread(data, (size_t)size, 1, fp) != 1) {
    log_errorf("Cannot read file '%s' contents.", fn);
    return 0;
  }
  if (EOF == fclose(fp)) {
    log_errorf("Cannot close file '%s'.", fn);
    return 0;
  }

  f->data_ = data;
  f->size_ = (size_t)size;
  return 1;
}

int file_destroy(file* f) {
  free(f->data_);
  memset(f, 0, sizeof(*f));
  return 1;
}

int file_write_delete(const char* fn) {
  if (!(remove(fn) == 0)) {
    return 0;
  }
  return 1;
}

int file_write_data(const char* fn, const char* data, size_t size) {
  FILE* fp = fopen(fn, "wb");
  if (!fp) {
    log_errorf("Cannot create file '%s'.", fn);
    return 0;
  }
  if (0 == fwrite(data, size, 1, fp)) {
    log_errorf("Cannot write file '%s'.", fn);
    return 0;
  }
  if (EOF == fclose(fp)) {
    log_errorf("Cannot close file '%s'.", fn);
    return 0;
  }
  return 1;
}
