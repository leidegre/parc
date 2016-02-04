
#include "api.h"
#include "msgpack.h"
#include "util.h"

#include "test.h"

static char scratch[1024];

static void test_roundtrip_token(const parc_token* t, parc_token* u) {
  msgpack_writer w;
  msgpack_writer_init(scratch, sizeof(scratch), &w);

  parc_token_store(t, &w);

  msgpack_reader r;
  msgpack_reader_init(scratch, sizeof(scratch), &r);

  parc_token_load(&r, u);
}

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("parc_token_serialization_test") {
    parc_token t, u;
    t.leading_trivia_ = parc_slice_create(" ");
    t.type_ = PARC_TOKEN_LOCAL;
    t.s_ = parc_slice_create("token");
    t.line_num_ = 1;
    t.char_pos_ = 3;

    test_roundtrip_token(&t, &u);

    ASSERT_EQUAL_STRLEN(" ", u.leading_trivia_.data_, u.leading_trivia_.size_);
    ASSERT_EQUAL_INT(PARC_TOKEN_LOCAL, u.type_);
    ASSERT_EQUAL_STRLEN("token", u.s_.data_, u.s_.size_);
    ASSERT_EQUAL_INT(1, u.line_num_);
    ASSERT_EQUAL_INT(3, u.char_pos_);
  }

  return 0;
}
