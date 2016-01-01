
#include "test.h"
#include "json.h"

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

  TEST_NEW("json_writer_number_literal_test") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_number(123, &w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("123", str, len);
  }

  TEST_NEW("json_writer_number_literal_test2") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_number(1.0 / 2, &w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("0.5", str, len);
  }

  TEST_NEW("json_writer_empty_string_literal_test") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_string("", strlen(""), &w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("\"\"", str, len);
  }

  TEST_NEW("json_writer_string_literal_test") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_string("abc", strlen("abc"), &w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("\"abc\"", str, len);
  }

  TEST_NEW("json_writer_empty_object_literal_test") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_object_begin(&w);
    json_write_object_end(&w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("{\n\n}", str, len);
  }

  TEST_NEW("json_writer_object_test") {
    char temp[256];
    json_writer w;
    json_writer_init(temp, sizeof(temp), &w);
    json_write_object_begin(&w);

    json_write_number(1, &w);
    json_write_kv_separator(&w);
    json_write_string("test", strlen("test"), &w);

    json_write_list_separator(&w);
    json_write_newline(&w);
    json_write_number(2, &w);
    json_write_kv_separator(&w);
    json_write_array_begin(&w);
    json_write_number(1, &w);
    json_write_list_separator(&w);
    json_write_newline(&w);
    json_write_number(2, &w);
    json_write_list_separator(&w);
    json_write_newline(&w);
    json_write_number(3, &w);
    json_write_array_end(&w);

    json_write_object_end(&w);
    size_t len;
    const char *str = json_writer_str(&w, &len);
    ASSERT_EQUAL_STRLEN("{\n 1: \"test\",\n 2: [\n  1,\n  2,\n  3\n ]\n}", str,
                        len);
  }

  return 0;
}
