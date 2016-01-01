
#include "msgpack.h"

#include <assert.h>
#include <string.h>

void msgpack_writer_initialize(char* buffer, size_t buffer_size,
                               msgpack_writer* writer) {
  writer->front_ = buffer;
  writer->back_ = buffer + buffer_size;
}

int msgpack_writer_ensure(msgpack_writer* writer, uint32_t required) {
  return (writer->front_ + required) <= writer->back_;
}

void msgpack_write_raw_uint8(uint8_t v, msgpack_writer* writer) {
  uint8_t* front = (uint8_t*)writer->front_;
  front[0] = v;
  writer->front_ += 1;
}

void msgpack_write_raw_uint16(uint16_t v, msgpack_writer* writer) {
  uint8_t* front = (uint8_t*)writer->front_;
  front[0] = (uint8_t)(v >> 8);
  front[1] = (uint8_t)v;
  writer->front_ += 2;
}

void msgpack_write_raw_uint32(uint32_t v, msgpack_writer* writer) {
  uint8_t* front = (uint8_t*)writer->front_;
  front[0] = (uint8_t)(v >> 24);
  front[1] = (uint8_t)(v >> 16);
  front[2] = (uint8_t)(v >> 8);
  front[3] = (uint8_t)v;
  writer->front_ += 4;
}

void msgpack_write_raw_uint64(uint64_t v, msgpack_writer* writer) {
  uint8_t* front = (uint8_t*)writer->front_;
  front[0] = (uint8_t)(v >> 56);
  front[1] = (uint8_t)(v >> 48);
  front[2] = (uint8_t)(v >> 40);
  front[3] = (uint8_t)(v >> 32);
  front[4] = (uint8_t)(v >> 24);
  front[5] = (uint8_t)(v >> 16);
  front[6] = (uint8_t)(v >> 8);
  front[7] = (uint8_t)v;
  writer->front_ += 4;
}

void msgpack_write_raw_str(const char* s, size_t size, msgpack_writer* writer) {
  char* front = writer->front_;
  memcpy(front, s, size);
  writer->front_ += size;
}

int msgpack_write_fixstr(const char* s, size_t size, msgpack_writer* writer) {
  assert(size <= 31);
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_FIXSTR | size;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_str(s, size, writer);
  return 1;
}

int msgpack_write_str8(const char* s, size_t size, msgpack_writer* writer) {
  assert(size <= UINT8_MAX);
  if (!msgpack_writer_ensure(writer, 2)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_STR8;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint8((uint8_t)size, writer);
  msgpack_write_raw_str(s, size, writer);
  return 1;
}

int msgpack_write_str16(const char* s, size_t size, msgpack_writer* writer) {
  assert(size <= UINT16_MAX);
  if (!msgpack_writer_ensure(writer, 3)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_STR16;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint16((uint16_t)size, writer);
  msgpack_write_raw_str(s, size, writer);
  return 1;
}

int msgpack_write_str32(const char* s, size_t size, msgpack_writer* writer) {
  assert(size <= UINT32_MAX);
  if (!msgpack_writer_ensure(writer, 4)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_STR32;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint32((uint32_t)size, writer);
  msgpack_write_raw_str(s, size, writer);
  return 1;
}

int _msgpack_write_str3(const char* s, size_t size, msgpack_writer* writer) {
  if (size <= 31) {
    return msgpack_write_fixstr(s, size, writer);
  }
  if (size <= UINT8_MAX) {
    return msgpack_write_str8(s, size, writer);
  }
  if (size <= UINT16_MAX) {
    return msgpack_write_str16(s, size, writer);
  }
  return msgpack_write_str32(s, size, writer);
}

int _msgpack_write_str2(const char* s, msgpack_writer* writer) {
  return _msgpack_write_str3(s, strlen(s), writer);
}

int msgpack_write_float32(float v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 5)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_FLOAT32;
  union {
    uint32_t u;
    float f;
  } x;
  x.f = v;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint32(x.u, writer);
  return 1;
}

int msgpack_write_float64(double v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 9)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_FLOAT64;
  union {
    uint64_t u;
    double f;
  } x;
  x.f = v;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint64(x.u, writer);
  return 1;
}

int msgpack_write_fixmap(size_t n, msgpack_writer* writer) {
  assert(n <= 15);
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_FIXMAP | n;
  msgpack_write_raw_uint8(b, writer);
  return 1;
}

int msgpack_write_map16(size_t n, msgpack_writer* writer) {
  assert(n <= UINT16_MAX);
  if (!msgpack_writer_ensure(writer, 3)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_MAP16;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint16((uint16_t)n, writer);
  return 1;
}

int msgpack_write_map32(size_t n, msgpack_writer* writer) {
  assert(n <= UINT32_MAX);
  if (!msgpack_writer_ensure(writer, 5)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_MAP32;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint32((uint32_t)n, writer);
  return 1;
}

int msgpack_write_map(size_t n, msgpack_writer* writer) {
  if (n <= 15) {
    return msgpack_write_fixmap(n, writer);
  }
  if (n <= UINT16_MAX) {
    return msgpack_write_map16(n, writer);
  }
  return msgpack_write_map32(n, writer);
}

int msgpack_write_fixarray(size_t n, msgpack_writer* writer) {
  assert(n <= 15);
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_FIXARRAY | n;
  msgpack_write_raw_uint8(b, writer);
  return 1;
}

int msgpack_write_array16(size_t n, msgpack_writer* writer) {
  assert(n <= UINT16_MAX);
  if (!msgpack_writer_ensure(writer, 3)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_ARRAY16;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint16((uint16_t)n, writer);
  return 1;
}

int msgpack_write_array32(size_t n, msgpack_writer* writer) {
  assert(n <= UINT32_MAX);
  if (!msgpack_writer_ensure(writer, 5)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_ARRAY32;
  msgpack_write_raw_uint8(b, writer);
  msgpack_write_raw_uint32((uint32_t)n, writer);
  return 1;
}

int msgpack_write_array(size_t n, msgpack_writer* writer) {
  if (n <= 15) {
    return msgpack_write_fixarray(n, writer);
  }
  if (n <= UINT16_MAX) {
    return msgpack_write_array16(n, writer);
  }
  return msgpack_write_array32(n, writer);
}

int msgpack_write_nil(msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  uint8_t b = MSGPACK_TYPE_NIL;
  msgpack_write_raw_uint8(b, writer);
  return 1;
}

int msgpack_write_bool(int v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  uint8_t b = (v != 0) ? MSGPACK_TYPE_TRUE : MSGPACK_TYPE_FALSE;
  msgpack_write_raw_uint8(b, writer);
  return 1;
}
