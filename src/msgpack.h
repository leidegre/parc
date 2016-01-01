#pragma once

// This is a lean and mean implementation of the
// MessagePack http://msgpack.org/index.html spec.

// This stand alone implementation depends on a minimal set of header files. The
// various source files that make up the implementation can be cherry picked if
// only a subset of the functionality is needed. There are no additional
// intertwined dependencies.

#include <stddef.h>
#include <stdint.h>

static const int8_t msgpack_fixint_min = -32;
static const int8_t msgpack_fixint_max = +127;

typedef enum msgpack_type {
  MSGPACK_TYPE_FIXINT_POS = 0x00,
  MSGPACK_TYPE_FIXMAP = 0x80,
  MSGPACK_TYPE_FIXARRAY = 0x90,
  MSGPACK_TYPE_FIXSTR = 0xA0,
  MSGPACK_TYPE_NIL = 0xC0,
  MSGPACK_TYPE_NEVER_USED = 0xC1,
  MSGPACK_TYPE_FALSE = 0xC2,
  MSGPACK_TYPE_TRUE = 0xC3,
  MSGPACK_TYPE_BIN8 = 0xC4,
  MSGPACK_TYPE_BIN16 = 0xC5,
  MSGPACK_TYPE_BIN32 = 0xC6,
  MSGPACK_TYPE_EXT8 = 0xC7,
  MSGPACK_TYPE_EXT16 = 0xC8,
  MSGPACK_TYPE_EXT32 = 0xC9,
  MSGPACK_TYPE_FLOAT32 = 0xCA,
  MSGPACK_TYPE_FLOAT64 = 0xCB,
  MSGPACK_TYPE_UINT8 = 0xCC,
  MSGPACK_TYPE_UINT16 = 0xCD,
  MSGPACK_TYPE_UINT32 = 0xCE,
  MSGPACK_TYPE_UINT64 = 0xCF,
  MSGPACK_TYPE_INT8 = 0xD0,
  MSGPACK_TYPE_INT16 = 0xD1,
  MSGPACK_TYPE_INT32 = 0xD2,
  MSGPACK_TYPE_INT64 = 0xD3,
  MSGPACK_TYPE_FIXEXT1 = 0xD4,
  MSGPACK_TYPE_FIXEXT2 = 0xD5,
  MSGPACK_TYPE_FIXEXT4 = 0xD6,
  MSGPACK_TYPE_FIXEXT8 = 0xD7,
  MSGPACK_TYPE_FIXEXT16 = 0xD8,
  MSGPACK_TYPE_STR8 = 0xD9,
  MSGPACK_TYPE_STR16 = 0xDA,
  MSGPACK_TYPE_STR32 = 0xDB,
  MSGPACK_TYPE_ARRAY16 = 0xDC,
  MSGPACK_TYPE_ARRAY32 = 0xDD,
  MSGPACK_TYPE_MAP16 = 0xDE,
  MSGPACK_TYPE_MAP32 = 0xDF,
  MSGPACK_TYPE_FIXINT_NEG = 0xE0
} msgpack_type;

typedef enum msgpack_class {
  MSGPACK_CLASS_NONE,
  MSGPACK_CLASS_INTEGER,
  MSGPACK_CLASS_NIL,
  MSGPACK_CLASS_BOOLEAN,
  MSGPACK_CLASS_FLOAT,
  MSGPACK_CLASS_STR,
  MSGPACK_CLASS_BIN,
  MSGPACK_CLASS_ARRAY,
  MSGPACK_CLASS_MAP,
  MSGPACK_CLASS_EXT
} msgpack_class;

typedef struct msgpack_writer {
  char* front_;
  char* back_;
} msgpack_writer;

typedef struct msgpack_value {
  uint8_t type_;
  union {
    float float32_;
    double float64_;
    uint8_t uint8_;
    uint16_t uint16_;
    uint32_t uint32_;
    uint64_t uint64_;
    int8_t int8_;
    int16_t int16_;
    int32_t int32_;
    int64_t int64_;
  } num_;
  const char* s_;
} msgpack_value;

typedef struct msgpack_reader {
  const char* front_;
  const char* back_;
} msgpack_reader;

// msgpack writer API

void msgpack_writer_initialize(char* buffer, size_t buffer_size,
                               msgpack_writer* writer);

// check that writer has enough free space
int msgpack_writer_ensure(msgpack_writer* writer, uint32_t required);

void msgpack_write_raw_uint8(uint8_t v, msgpack_writer* writer);
void msgpack_write_raw_uint16(uint16_t v, msgpack_writer* writer);
void msgpack_write_raw_uint32(uint32_t v, msgpack_writer* writer);
void msgpack_write_raw_uint64(uint64_t v, msgpack_writer* writer);
void msgpack_write_raw_str(const char* s, size_t size, msgpack_writer* writer);

int msgpack_write_nil(msgpack_writer* writer);

int msgpack_write_bool(int v, msgpack_writer* writer);

int msgpack_write_float32(float v, msgpack_writer* writer);
int msgpack_write_float64(double v, msgpack_writer* writer);

// REQUIRES: -32 <= v && v <= 127
int msgpack_write_fixint(int8_t v, msgpack_writer* writer);

int msgpack_write_uint8(uint8_t v, msgpack_writer* writer);
int msgpack_write_uint16(uint16_t v, msgpack_writer* writer);
int msgpack_write_uint32(uint32_t v, msgpack_writer* writer);
int msgpack_write_uint64(uint64_t v, msgpack_writer* writer);

// use the smallest possible msgpack representation of the unsigned integer
int msgpack_write_uint(uint64_t v, msgpack_writer* writer);

int msgpack_write_int8(int8_t v, msgpack_writer* writer);
int msgpack_write_int16(int16_t v, msgpack_writer* writer);
int msgpack_write_int32(int32_t v, msgpack_writer* writer);
int msgpack_write_int64(int64_t v, msgpack_writer* writer);

// use the smallest possible msgpack representation of the signed integer
int msgpack_write_int(int64_t v, msgpack_writer* writer);

// REQUIRES: size <= 31
int msgpack_write_fixstr(const char* s, size_t size, msgpack_writer* writer);
int msgpack_write_str8(const char* s, size_t size, msgpack_writer* writer);
int msgpack_write_str16(const char* s, size_t size, msgpack_writer* writer);
int msgpack_write_str32(const char* s, size_t size, msgpack_writer* writer);

// *magic* function overloading based on number of arguments
int _msgpack_write_str2(const char* s, msgpack_writer* writer);
int _msgpack_write_str3(const char* s, size_t size, msgpack_writer* writer);
#define _msgpack_write_str(_1, _2, _3, fname, ...) fname
#define msgpack_write_str(...)                                              \
  _msgpack_write_str(__VA_ARGS__, _msgpack_write_str3, _msgpack_write_str2, \
                     __dummy__)(__VA_ARGS__)

// REQUIRES: n <= 15
int msgpack_write_fixmap(size_t n, msgpack_writer* writer);
int msgpack_write_map16(size_t n, msgpack_writer* writer);
int msgpack_write_map32(size_t n, msgpack_writer* writer);

int msgpack_write_map(size_t n, msgpack_writer* writer);

// REQUIRES: n <= 15
int msgpack_write_fixarray(size_t n, msgpack_writer* writer);
int msgpack_write_array16(size_t n, msgpack_writer* writer);
int msgpack_write_array32(size_t n, msgpack_writer* writer);

int msgpack_write_array(size_t n, msgpack_writer* writer);

// msgpack reader API

void msgpack_reader_initialize(const char* buffer, size_t buffer_size,
                               msgpack_reader* reader);

int msgpack_reader_ensure(msgpack_reader* reader, uint32_t required);

void msgpack_read_raw_float32(msgpack_reader* reader, float* v);
void msgpack_read_raw_float64(msgpack_reader* reader, double* v);
void msgpack_read_raw_uint8(msgpack_reader* reader, uint8_t* v);
void msgpack_read_raw_uint16(msgpack_reader* reader, uint16_t* v);
void msgpack_read_raw_uint32(msgpack_reader* reader, uint32_t* v);
void msgpack_read_raw_uint64(msgpack_reader* reader, uint64_t* v);
void msgpack_read_raw_str(msgpack_reader* reader, size_t size, const char** s);

int msgpack_read_value(msgpack_reader* reader, msgpack_value* value);

msgpack_class msgpack_value_get_class(msgpack_value* value);

int msgpack_value_is_class(msgpack_value* value, msgpack_class class_);

float msgpack_value_to_float32(msgpack_value* value);
double msgpack_value_to_float64(msgpack_value* value);
int32_t msgpack_value_to_int32(msgpack_value* value);
uint32_t msgpack_value_to_uint32(msgpack_value* value);

// transpile json to msgpack format (or vice versa)
int msgpack_parse_json(const char* json, msgpack_writer* writer);

struct json_writer;

int msgpack_dump_json(msgpack_reader* reader, struct json_writer* writer);
