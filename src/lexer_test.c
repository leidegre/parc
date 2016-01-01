
#include "api.h"
#include "test.h"

static void parc_lexer_init2(const char* str, parc_lexer* lexer) {
  parc_lexer_init(str, strlen(str), lexer);
}

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("parc_lexer_test") {
    parc_lexer lexer;
    parc_lexer_init2("\"parc\" { \"white-space\" \"ignore\" ; }", &lexer);

    parc_token token;

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_STRING_LITERAL, token.type_);
    ASSERT_EQUAL_STRLEN("\"parc\"", token.s_.data_, token.s_.size_);

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_LEFT_BRACE, token.type_);
    ASSERT_EQUAL_STRLEN("{", token.s_.data_, token.s_.size_);

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_STRING_LITERAL, token.type_);
    ASSERT_EQUAL_STRLEN("\"white-space\"", token.s_.data_, token.s_.size_);

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_STRING_LITERAL, token.type_);
    ASSERT_EQUAL_STRLEN("\"ignore\"", token.s_.data_, token.s_.size_);

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_SEMICOLON, token.type_);
    ASSERT_EQUAL_STRLEN(";", token.s_.data_, token.s_.size_);

    ASSERT_TRUE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_TOKEN_RIGHT_BRACE, token.type_);
    ASSERT_EQUAL_STRLEN("}", token.s_.data_, token.s_.size_);

    // end of file
    ASSERT_FALSE(parc_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(PARC_ERROR_END_OF_FILE, lexer.error_);
  }

  return 0;
}
