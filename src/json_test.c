
#include "test.h"
#include "json.h"

void dump_hex(const char *s, size_t size) {
  static const char *kDigits = "0123456789ABCDEF";
  const unsigned char *front = (unsigned char *)s;
  const unsigned char *back = (unsigned char *)s + size;
  while (front < back) {
    char temp0[16 + 7 + 1];  // 00 11 22 33 44 55 66 77
    char temp1[16 + 7 + 1];  // 88 99 AA BB CC DD EE FF
    char temp2[16 + 1 + 1];  // ........ ........
    memset(temp0, ' ', sizeof(temp0));
    memset(temp1, ' ', sizeof(temp1));
    memset(temp2, ' ', sizeof(temp2));
    int count = 16 < (ptrdiff_t)(back - front) ? 16 : (int)(back - front);
    for (int i = 0; i < count; i++) {
      unsigned char b = *front++;
      char *temp = i < 8 ? temp0 : temp1;
      int x = i & 7;
      temp[(x << 1) + 0 + x] = kDigits[b >> 4];
      temp[(x << 1) + 1 + x] = kDigits[b & 15];
      temp2[i + i / 8] = (' ' <= b) & (b <= '~') ? (char)b : '.';
    }
    temp0[sizeof(temp0) - 1] = '\0';
    temp1[sizeof(temp1) - 1] = '\0';
    temp2[sizeof(temp2) - 1] = '\0';
    printf("%s- %s %s\n", temp0, temp1, temp2);
  }
}

int main(int argc, char *argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("json_lexer_string_test") {
    json_lexer lexer;
    json_lexer_initialize("\"\" \"abc\" \"q\\\"uot\"", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_STRING, token.type_);
    ASSERT_EQUAL_STRLEN("\"\"", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_STRING, token.type_);
    ASSERT_EQUAL_STRLEN("\"abc\"", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_STRING, token.type_);
    ASSERT_EQUAL_STRLEN("\"q\\\"uot\"", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_number_test") {
    json_lexer lexer;
    json_lexer_initialize("0 1.0 25 3.667", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NUMBER, token.type_);
    ASSERT_EQUAL_STRLEN("0", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NUMBER, token.type_);
    ASSERT_EQUAL_STRLEN("1.0", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NUMBER, token.type_);
    ASSERT_EQUAL_STRLEN("25", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NUMBER, token.type_);
    ASSERT_EQUAL_STRLEN("3.667", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_true_test") {
    json_lexer lexer;
    json_lexer_initialize("true", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_TRUE, token.type_);
    ASSERT_EQUAL_STRLEN("true", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_false_test") {
    json_lexer lexer;
    json_lexer_initialize("false", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_FALSE, token.type_);
    ASSERT_EQUAL_STRLEN("false", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_null_test") {
    json_lexer lexer;
    json_lexer_initialize("null", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NULL, token.type_);
    ASSERT_EQUAL_STRLEN("null", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_array_test") {
    json_lexer lexer;
    json_lexer_initialize("[true, false, null]", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_ARRAY_BEGIN, token.type_);
    ASSERT_EQUAL_STRLEN("[", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_TRUE, token.type_);
    ASSERT_EQUAL_STRLEN("true", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_LIST_SEPARATOR, token.type_);
    ASSERT_EQUAL_STRLEN(",", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_FALSE, token.type_);
    ASSERT_EQUAL_STRLEN("false", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_LIST_SEPARATOR, token.type_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_NULL, token.type_);
    ASSERT_EQUAL_STRLEN("null", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_ARRAY_END, token.type_);
    ASSERT_EQUAL_STRLEN("]", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_lexer_object_test") {
    json_lexer lexer;
    json_lexer_initialize("{\"key\": \"value\"}", &lexer);
    json_token token;
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_OBJECT_BEGIN, token.type_);
    ASSERT_EQUAL_STRLEN("{", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_STRING, token.type_);
    ASSERT_EQUAL_STRLEN("\"key\"", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_KV_SEPARATOR, token.type_);
    ASSERT_EQUAL_STRLEN(":", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_STRING, token.type_);
    ASSERT_EQUAL_STRLEN("\"value\"", token.s_, token.length_);
    ASSERT_TRUE(json_lexer_next(&lexer, &token));
    ASSERT_EQUAL_INT(JSON_TOKEN_OBJECT_END, token.type_);
    ASSERT_EQUAL_STRLEN("}", token.s_, token.length_);
    ASSERT_FALSE(json_lexer_next(&lexer, &token));
  }

  TEST_NEW("json_unescape_empty_string_literal_test") {
    char temp[256];
    const char *s = "\"\"";
    json_unescape_string_literal(s, strlen(s), temp, sizeof(temp));
    ASSERT_EQUAL_STRLEN("", temp, strlen(temp));
  }

  TEST_NEW("json_unescape_abc_string_literal_test") {
    char temp[256];
    const char *s = "\"abc\"";
    json_unescape_string_literal(s, strlen(s), temp, sizeof(temp));
    ASSERT_EQUAL_STRLEN("abc", temp, strlen(temp));
  }

  TEST_NEW("json_unescape_quot_string_literal_test") {
    char temp[256];
    const char *s = "\"q\\\"uot\"";
    json_unescape_string_literal(s, strlen(s), temp, sizeof(temp));
    ASSERT_EQUAL_STRLEN("q\"uot", temp, strlen(temp));
  }

  return 0;
}