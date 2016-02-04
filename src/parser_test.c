
#include "api.h"
#include "msgpack.h"

#include "test.h"

#include <assert.h>

static const char* syntax_tree_test(parc_syntax_tree* syntax, size_t* size);

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("parc_parser_empty_namespace_test") {
    const char* source = "\"parc\" { }";
    parc_parser parser;
    parc_parser_init(source, strlen(source), &parser);
    parc_parser_main(&parser);
    puts(syntax_tree_test(parc_parser_syntax(&parser), NULL));
  }

  return 0;
}

typedef struct {
  char* begin_;
  char* end_;
} pc_buffer;

static void pc_buffer_init(char* data, const size_t size, pc_buffer* buf) {
  buf->begin_ = data;
  buf->end_ = data + size;
}

static int pc_buffer_append(pc_buffer* buf, const char* data,
                            const size_t size) {
  if ((buf->begin_ + size) <= buf->end_) {
    memcpy(buf->begin_, data, size);
    buf->begin_ += size;
    return 1;
  }
  return 0;
}

static int syntax_tree_test2(msgpack_reader* reader, pc_buffer* buf) {
  msgpack_value v;
  msgpack_read_value(reader, &v);
  assert(msgpack_value_is_class(&v, MSGPACK_CLASS_MAP));
  msgpack_read_value(reader, &v);
  msgpack_read_value(reader, &v);
  uint32_t type = msgpack_value_to_uint32(&v);
  switch (type) {
    case PARC_SYNTAX_TREE_TYPE_NODE: {
      pc_buffer_append(buf, "(", 1);

      msgpack_read_value(reader, &v);
      msgpack_read_value(reader, &v);
      assert(msgpack_value_is_class(&v, MSGPACK_CLASS_STR));
      pc_buffer_append(buf, v.s_, msgpack_value_to_uint32(&v));

      msgpack_read_value(reader, &v);
      msgpack_read_value(reader, &v);
      assert(msgpack_value_is_class(&v, MSGPACK_CLASS_ARRAY));
      uint32_t count = msgpack_value_to_uint32(&v);
      for (uint32_t i = 0; i < count; i++) {
        pc_buffer_append(buf, " ", 1);
        syntax_tree_test2(reader, buf);
      }

      pc_buffer_append(buf, ")", 1);
      break;
    }
    case PARC_SYNTAX_TREE_TYPE_TOKEN: {
      msgpack_read_value(reader, &v);
      assert(msgpack_value_is_class(&v, MSGPACK_CLASS_INTEGER));
      parc_token token;
      parc_token_load(reader, &token);
      pc_buffer_append(buf, token.s_.data_, token.s_.size_);
      break;
    }
    default: {
      assert(0 && "not supported");
      return 0;
    }
  }
  return 1;
}

static const char* syntax_tree_test(parc_syntax_tree* syntax, size_t* size) {
  char temp[1024 * 1024];
  static char scratch[1024 * 1024];

  msgpack_writer w;
  msgpack_writer_init(temp, sizeof(temp), &w);

  parc_syntax_tree_dump(syntax, &w);

  msgpack_reader r;
  msgpack_reader_init(temp, sizeof(temp), &r);

  pc_buffer buf;
  pc_buffer_init(scratch, sizeof(scratch), &buf);
  syntax_tree_test2(&r, &buf);
  pc_buffer_append(&buf, "\0", 1);
  if (size) {
    *size = buf.begin_ - scratch;
  }
  return scratch;
}
