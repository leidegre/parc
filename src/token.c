
#include "api.h"
#include "msgpack.h"

#include <assert.h>

int parc_token_store(const parc_token *t, struct msgpack_writer *w) {
  msgpack_write_map(5, w);

  msgpack_write_uint(1, w);  // 1: leading trivia
  msgpack_write_str(t->leading_trivia_.data_, t->leading_trivia_.size_, w);

  msgpack_write_uint(2, w);  // 2: type
  msgpack_write_int(t->type_, w);

  msgpack_write_uint(3, w);  // 3: string
  msgpack_write_str(t->s_.data_, t->s_.size_, w);

  msgpack_write_uint(4, w);  // 4: line number
  msgpack_write_int(t->line_num_, w);

  msgpack_write_uint(5, w);  // 5: char position
  msgpack_write_int(t->char_pos_, w);

  return 1;
}

int parc_token_load(struct msgpack_reader *r, parc_token *t) {
  assert(r && "reader cannot be null");
  memset(t, 0, sizeof(*t));
  msgpack_value v;
  if (!msgpack_read_value(r, &v)) {
    return 0;
  }
  assert(msgpack_value_is_class(&v, MSGPACK_CLASS_MAP));
  uint32_t count = msgpack_value_to_uint32(&v);
  for (size_t i = 0; i < count; i++) {
    if (!msgpack_read_value(r, &v)) {
      return 0;
    }
    assert(msgpack_value_is_class(&v, MSGPACK_CLASS_INTEGER));
    switch (msgpack_value_to_uint32(&v)) {
      case 1: {
        if (!msgpack_read_value(r, &v)) {
          return 0;
        }
        assert(msgpack_value_is_class(&v, MSGPACK_CLASS_STR));
        t->leading_trivia_ =
            parc_slice_create(v.s_, msgpack_value_to_uint32(&v));
        break;
      }
      case 2: {
        if (!msgpack_read_value(r, &v)) {
          return 0;
        }
        assert(msgpack_value_is_class(&v, MSGPACK_CLASS_INTEGER));
        t->type_ = msgpack_value_to_int32(&v);
        break;
      }
      case 3: {
        if (!msgpack_read_value(r, &v)) {
          return 0;
        }
        assert(msgpack_value_is_class(&v, MSGPACK_CLASS_STR));
        t->s_ = parc_slice_create(v.s_, msgpack_value_to_uint32(&v));
        break;
      }
      case 4: {
        if (!msgpack_read_value(r, &v)) {
          return 0;
        }
        assert(msgpack_value_is_class(&v, MSGPACK_CLASS_INTEGER));
        t->line_num_ = msgpack_value_to_int32(&v);
        break;
      }
      case 5: {
        if (!msgpack_read_value(r, &v)) {
          return 0;
        }
        assert(msgpack_value_is_class(&v, MSGPACK_CLASS_INTEGER));
        t->char_pos_ = msgpack_value_to_int32(&v);
        break;
      }
      default: {
        assert(0 && "not supported");
        break;
      }
    }
  }
  return 1;
}
