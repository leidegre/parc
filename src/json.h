#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum {
  JSON_ERROR_END_OF_FILE = -1,
  JSON_ERROR_OK = 0,
  JSON_ERROR_TOKEN,
  JSON_ERROR_STRING_TOKEN,
  JSON_ERROR_NUMBER_TOKEN,
} json_error;

typedef enum {
  JSON_TOKEN_END_OF_FILE = -1,
  JSON_TOKEN_NONE = 0,
  JSON_TOKEN_WHITE_SPACE,
  JSON_TOKEN_STRING,
  JSON_TOKEN_NUMBER,
  JSON_TOKEN_TRUE,
  JSON_TOKEN_FALSE,
  JSON_TOKEN_NULL,
  JSON_TOKEN_OBJECT_BEGIN,
  JSON_TOKEN_KV_SEPARATOR,
  JSON_TOKEN_OBJECT_END,
  JSON_TOKEN_ARRAY_BEGIN,
  JSON_TOKEN_LIST_SEPARATOR,
  JSON_TOKEN_ARRAY_END,
} json_token_type;

typedef struct {
  json_token_type type_;
  const char* s_;
  uint32_t length_;
} json_token;

typedef struct {
  const char* front_;
  const char* back_;
  char ch_;
  json_error error_;
} json_lexer;

typedef struct {
  json_lexer lexer_;
  json_token token_;
  int is_valid_;
} json_parser;

typedef struct json_writer json_writer;
struct json_writer {
  const char* s_;
  char* front_;
  char* back_;
  int indent_;
  int is_newline_;
};

// JSON lexer API

void json_lexer_initialize(const char* json, json_lexer* lexer);

int json_lexer_next(json_lexer* lexer, json_token* token);

// JSON parser API

void json_parser_initialize(const char* json, json_parser* lexer);

int json_parser_read(json_parser* parser);

int json_parser_accept(json_parser* parser, json_token_type type);

// JSON writer API

void json_writer_init(char* buffer, size_t buffer_size, json_writer* writer);

const char* json_writer_str(json_writer* writer, size_t* length);

int json_write_newline(json_writer* writer);
int json_write_string(const char* str, size_t len, json_writer* writer);
int json_write_number(const double num, json_writer* writer);
int json_write_true(json_writer* writer);
int json_write_false(json_writer* writer);
int json_write_null(json_writer* writer);
int json_write_object_begin(json_writer* writer);
int json_write_kv_separator(json_writer* writer);
int json_write_object_end(json_writer* writer);
int json_write_array_begin(json_writer* writer);
int json_write_list_separator(json_writer* writer);
int json_write_array_end(json_writer* writer);

// JSON utility API

int json_unescape_string_literal(const char* s, uint32_t s_size,
                                 char* unescaped, uint32_t unescaped_size);
