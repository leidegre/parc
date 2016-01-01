
#include "msgpack.h"

#include <string.h>
#include <assert.h>

void msgpack_reader_initialize(const char* buffer, size_t buffer_size,
                               msgpack_reader* reader) {
  memset(reader, 0, sizeof(*reader));
  reader->front_ = buffer;
  reader->back_ = buffer + buffer_size;
}

int msgpack_reader_ensure(msgpack_reader* reader, uint32_t required) {
  return (reader->front_ + required) <= reader->back_;
}

void msgpack_read_raw_float32(msgpack_reader* reader, float* v) {
  union {
    float f;
    uint32_t u;
  } x;
  msgpack_read_raw_uint32(reader, &x.u);
  *v = x.f;
}

void msgpack_read_raw_float64(msgpack_reader* reader, double* v) {
  union {
    double f;
    uint64_t u;
  } x;
  msgpack_read_raw_uint64(reader, &x.u);
  *v = x.f;
}

void msgpack_read_raw_uint8(msgpack_reader* reader, uint8_t* v) {
  const uint8_t* x = (const uint8_t*)reader->front_;
  uint8_t y = *x++;
  *v = y;
  reader->front_ = (const char*)x;
}

void msgpack_read_raw_uint16(msgpack_reader* reader, uint16_t* v) {
  const uint8_t* x = (const uint8_t*)reader->front_;
  uint16_t y = 0;
  for (size_t i = 0; i < 2; i++) {
    y <<= 8;
    y |= *x++;
  }
  *v = y;
  reader->front_ = (const char*)x;
}

void msgpack_read_raw_uint32(msgpack_reader* reader, uint32_t* v) {
  const uint8_t* x = (const uint8_t*)reader->front_;
  uint32_t y = 0;
  for (size_t i = 0; i < 4; i++) {
    y <<= 8;
    y |= *x++;
  }
  *v = y;
  reader->front_ = (const char*)x;
}

void msgpack_read_raw_uint64(msgpack_reader* reader, uint64_t* v) {
  const uint8_t* x = (const uint8_t*)reader->front_;
  uint64_t y = 0;
  for (size_t i = 0; i < 8; i++) {
    y <<= 8;
    y |= *x++;
  }
  *v = y;
  reader->front_ = (const char*)x;
}

void msgpack_read_raw_str(msgpack_reader* reader, size_t size, const char** s) {
  *s = reader->front_;
  reader->front_ += size;
}

msgpack_class msgpack_value_get_class(msgpack_value* value) {
  switch (value->type_) {
    case MSGPACK_TYPE_FIXINT_POS:
    case MSGPACK_TYPE_UINT8:
    case MSGPACK_TYPE_UINT16:
    case MSGPACK_TYPE_UINT32:
    case MSGPACK_TYPE_UINT64:
    case MSGPACK_TYPE_INT8:
    case MSGPACK_TYPE_INT16:
    case MSGPACK_TYPE_INT32:
    case MSGPACK_TYPE_INT64:
    case MSGPACK_TYPE_FIXINT_NEG: {
      return MSGPACK_CLASS_INTEGER;
    }
    case MSGPACK_TYPE_NIL: {
      return MSGPACK_CLASS_NIL;
    }
    case MSGPACK_TYPE_FALSE:
    case MSGPACK_TYPE_TRUE: {
      return MSGPACK_CLASS_BOOLEAN;
    }
    case MSGPACK_TYPE_FLOAT32:
    case MSGPACK_TYPE_FLOAT64: {
      return MSGPACK_CLASS_FLOAT;
    }
    case MSGPACK_TYPE_FIXSTR:
    case MSGPACK_TYPE_STR8:
    case MSGPACK_TYPE_STR16:
    case MSGPACK_TYPE_STR32: {
      return MSGPACK_CLASS_STR;
    }
    case MSGPACK_TYPE_BIN8:
    case MSGPACK_TYPE_BIN16:
    case MSGPACK_TYPE_BIN32: {
      return MSGPACK_CLASS_BIN;
    }
    case MSGPACK_TYPE_FIXARRAY:
    case MSGPACK_TYPE_ARRAY16:
    case MSGPACK_TYPE_ARRAY32: {
      return MSGPACK_CLASS_ARRAY;
    }
    case MSGPACK_TYPE_FIXMAP:
    case MSGPACK_TYPE_MAP16:
    case MSGPACK_TYPE_MAP32: {
      return MSGPACK_CLASS_MAP;
    }
    case MSGPACK_TYPE_FIXEXT1:
    case MSGPACK_TYPE_FIXEXT2:
    case MSGPACK_TYPE_FIXEXT4:
    case MSGPACK_TYPE_FIXEXT8:
    case MSGPACK_TYPE_FIXEXT16:
    case MSGPACK_TYPE_EXT8:
    case MSGPACK_TYPE_EXT16:
    case MSGPACK_TYPE_EXT32: {
      return MSGPACK_CLASS_EXT;
    }
  }
  return MSGPACK_CLASS_NONE;
}

int msgpack_value_is_class(msgpack_value* value, msgpack_class class_) {
  return msgpack_value_get_class(value) == class_;
}

int32_t msgpack_value_to_int32(msgpack_value* value) {
  switch (value->type_) {
    case MSGPACK_TYPE_FIXINT_POS:
    case MSGPACK_TYPE_FIXMAP:
    case MSGPACK_TYPE_FIXARRAY:
    case MSGPACK_TYPE_FIXSTR:
    case MSGPACK_TYPE_BIN8:
    case MSGPACK_TYPE_STR8: {
      return value->num_.uint8_;
    }
    case MSGPACK_TYPE_FLOAT32: {
      return (int32_t)value->num_.float32_;
    }
    case MSGPACK_TYPE_FLOAT64: {
      return (int32_t)value->num_.float64_;
    }
    case MSGPACK_TYPE_UINT8: {
      return value->num_.uint8_;
    }
    case MSGPACK_TYPE_UINT16:
    case MSGPACK_TYPE_BIN16:
    case MSGPACK_TYPE_STR16:
    case MSGPACK_TYPE_ARRAY16:
    case MSGPACK_TYPE_MAP16: {
      return value->num_.uint16_;
    }
    case MSGPACK_TYPE_UINT32:
    case MSGPACK_TYPE_BIN32:
    case MSGPACK_TYPE_STR32:
    case MSGPACK_TYPE_ARRAY32:
    case MSGPACK_TYPE_MAP32: {
      return (int32_t)value->num_.uint32_;
    }
    case MSGPACK_TYPE_UINT64: {
      return (int32_t)value->num_.uint64_;
    }
    case MSGPACK_TYPE_INT8: {
      return value->num_.int8_;
    }
    case MSGPACK_TYPE_INT16: {
      return value->num_.int16_;
    }
    case MSGPACK_TYPE_INT32: {
      return value->num_.int32_;
    }
    case MSGPACK_TYPE_INT64: {
      return (int32_t)value->num_.int64_;
    }
    case MSGPACK_TYPE_FIXINT_NEG: {
      return value->num_.int8_;
    }
  }
  assert(0 && "cannot cast value to int32");
  return INT32_MIN;
}

uint32_t msgpack_value_to_uint32(msgpack_value* value) {
  switch (value->type_) {
    case MSGPACK_TYPE_FIXINT_POS:
    case MSGPACK_TYPE_FIXMAP:
    case MSGPACK_TYPE_FIXARRAY:
    case MSGPACK_TYPE_FIXSTR:
    case MSGPACK_TYPE_BIN8:
    case MSGPACK_TYPE_STR8: {
      return value->num_.uint8_;
    }
    case MSGPACK_TYPE_FLOAT32: {
      return (uint32_t)value->num_.float32_;
    }
    case MSGPACK_TYPE_FLOAT64: {
      return (uint32_t)value->num_.float64_;
    }
    case MSGPACK_TYPE_UINT8: {
      return value->num_.uint8_;
    }
    case MSGPACK_TYPE_UINT16:
    case MSGPACK_TYPE_BIN16:
    case MSGPACK_TYPE_STR16:
    case MSGPACK_TYPE_ARRAY16:
    case MSGPACK_TYPE_MAP16: {
      return value->num_.uint16_;
    }
    case MSGPACK_TYPE_UINT32:
    case MSGPACK_TYPE_BIN32:
    case MSGPACK_TYPE_STR32:
    case MSGPACK_TYPE_ARRAY32:
    case MSGPACK_TYPE_MAP32: {
      return value->num_.uint32_;
    }
    case MSGPACK_TYPE_UINT64: {
      return (uint32_t)value->num_.uint64_;
    }
    case MSGPACK_TYPE_INT8: {
      return (uint32_t)value->num_.int8_;
    }
    case MSGPACK_TYPE_INT16: {
      return (uint32_t)value->num_.int16_;
    }
    case MSGPACK_TYPE_INT32: {
      return (uint32_t)value->num_.int32_;
    }
    case MSGPACK_TYPE_INT64: {
      return (uint32_t)value->num_.int64_;
    }
    case MSGPACK_TYPE_FIXINT_NEG: {
      return (uint32_t)value->num_.int8_;
    }
  }
  assert(0 && "cannot cast value to uint32");
  return UINT32_MAX;
}
