
#include "msgpack.h"

#include <assert.h>

int msgpack_write_fixint(int8_t v, msgpack_writer* writer) {
  assert((msgpack_fixint_min <= v) & (v <= msgpack_fixint_max));
  if (!msgpack_writer_ensure(writer, 1)) {
    return 0;
  }
  msgpack_write_raw_uint8((uint8_t)v, writer);
  return 1;
}

int msgpack_write_uint8(uint8_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 2)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_UINT8, writer);
  msgpack_write_raw_uint8(v, writer);
  return 1;
}

int msgpack_write_uint16(uint16_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 3)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_UINT16, writer);
  msgpack_write_raw_uint16(v, writer);
  return 1;
}

int msgpack_write_uint32(uint32_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 5)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_UINT32, writer);
  msgpack_write_raw_uint32(v, writer);
  return 1;
}

int msgpack_write_uint64(uint64_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 9)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_UINT64, writer);
  msgpack_write_raw_uint64(v, writer);
  return 1;
}

int msgpack_write_uint(uint64_t v, msgpack_writer* writer) {
  if (v <= msgpack_fixint_max) {
    return msgpack_write_fixint((int8_t)v, writer);
  }
  if (v <= UINT16_MAX) {
    return msgpack_write_uint16((uint16_t)v, writer);
  }
  if (v <= UINT32_MAX) {
    return msgpack_write_uint32((uint32_t)v, writer);
  }
  return msgpack_write_uint64(v, writer);
}

int msgpack_write_int8(int8_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 2)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_INT8, writer);
  msgpack_write_raw_uint8((uint8_t)v, writer);
  return 1;
}

int msgpack_write_int16(int16_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 3)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_INT16, writer);
  msgpack_write_raw_uint16((uint16_t)v, writer);
  return 1;
}

int msgpack_write_int32(int32_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 5)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_INT32, writer);
  msgpack_write_raw_uint32((uint32_t)v, writer);
  return 1;
}

int msgpack_write_int64(int64_t v, msgpack_writer* writer) {
  if (!msgpack_writer_ensure(writer, 9)) {
    return 0;
  }
  msgpack_write_raw_uint8(MSGPACK_TYPE_INT64, writer);
  msgpack_write_raw_uint64((uint64_t)v, writer);
  return 1;
}

int msgpack_write_int(int64_t v, msgpack_writer* writer) {
  if ((msgpack_fixint_min <= v) & (v <= msgpack_fixint_max)) {
    return msgpack_write_fixint((int8_t)v, writer);
  }
  if ((INT8_MIN <= v) & (v <= INT8_MAX)) {
    return msgpack_write_int8((int8_t)v, writer);
  }
  if ((INT16_MIN <= v) & (v <= INT16_MAX)) {
    return msgpack_write_int16((int16_t)v, writer);
  }
  if ((INT32_MIN <= v) & (v <= INT32_MAX)) {
    return msgpack_write_int32((int32_t)v, writer);
  }
  return msgpack_write_int64(v, writer);
}