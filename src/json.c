
#include "json.h"

#include <string.h>
#include <assert.h>

// json lexer

static int json_lexer_load(json_lexer *lexer) {
  const char ch = *lexer->front_;
  if (ch) {
    lexer->ch_ = ch;
    return 1;
  }
  lexer->ch_ = '\0';
  return 0;
}

void json_lexer_initialize(const char *json, json_lexer *lexer) {
  memset(lexer, 0, sizeof(*lexer));
  lexer->front_ = json;
  lexer->back_ = json;
  json_lexer_load(lexer);
}

static void json_lexer_accept_any(json_lexer *lexer) {
  lexer->front_++;  // move next
  json_lexer_load(lexer);
}

static int json_lexer_accept(json_lexer *lexer, const char ch) {
  if (lexer->ch_ == ch) {
    json_lexer_accept_any(lexer);
    return 1;
  }
  return 0;
}

static int is_white_space(const char ch) {
  return (ch == '\t') | (ch == '\n') | (ch == '\r') | (ch == ' ');
}

static int json_lexer_accept_white_space(json_lexer *lexer) {
  if (is_white_space(lexer->ch_)) {
    json_lexer_accept_any(lexer);
    return 1;
  }
  return 0;
}

static int json_lexer_accept_between(json_lexer *lexer, const char lower,
                                     const char upper) {
  if ((lower <= lexer->ch_) & (lexer->ch_ <= upper)) {
    json_lexer_accept_any(lexer);
    return 1;
  }
  return 0;
}

static int json_lexer_accept_string(json_lexer *lexer, const char *s) {
  json_lexer k = *lexer;
  while (*s && json_lexer_accept(lexer, *s)) {
    *s++;
  }
  if (*s == '\0') {
    return 1;
  }
  *lexer = k;  // backtrack
  return 0;
}

static void json_lexer_yield(json_lexer *lexer, json_token_type type,
                             json_token *token) {
  assert(lexer->back_ < lexer->front_);
  token->type_ = type;
  token->s_ = lexer->back_;
  token->length_ = lexer->front_ - lexer->back_;
  lexer->back_ = lexer->front_;
  lexer->error_ = JSON_ERROR_OK;
}

static int json_lexer_next_string(json_lexer *lexer, json_token *token) {
  if (!json_lexer_accept(lexer, '"')) {
    lexer->error_ = JSON_ERROR_STRING_TOKEN;
    return 0;
  }

  for (;;) {
    if (json_lexer_accept(lexer, '\\')) {
      // escape sequence
      if (json_lexer_accept(lexer, '"')) {
        continue;
      }
      if (json_lexer_accept(lexer, '\\')) {
        continue;
      }
      if (json_lexer_accept(lexer, '/')) {
        continue;
      }
      if (json_lexer_accept(lexer, 'b')) {
        continue;
      }
      if (json_lexer_accept(lexer, 'f')) {
        continue;
      }
      if (json_lexer_accept(lexer, 'n')) {
        continue;
      }
      if (json_lexer_accept(lexer, 'r')) {
        continue;
      }
      if (json_lexer_accept(lexer, 't')) {
        continue;
      }
      // unrecognized escape sequence
      lexer->error_ = JSON_ERROR_STRING_TOKEN;
      return 0;
    }
    if (json_lexer_accept(lexer, '"')) {
      // end of string
      break;
    }
    if (!lexer->ch_) {  // end of file
      lexer->error_ = JSON_ERROR_STRING_TOKEN;
      return 0;
    }
    json_lexer_accept_any(lexer);
  }

  json_lexer_yield(lexer, JSON_TOKEN_STRING, token);
  return 1;
}

static int json_lexer_next_number(json_lexer *lexer, json_token *token) {
  if (json_lexer_accept(lexer, '-')) {
    json_lexer_load(lexer);
  }
  if (json_lexer_accept(lexer, '0')) {
    json_lexer_load(lexer);
  } else {
    if (!json_lexer_accept_between(lexer, '1', '9') ||
        !json_lexer_load(lexer)) {
      lexer->error_ = JSON_ERROR_NUMBER_TOKEN;
      return 0;
    }
    while (json_lexer_accept_between(lexer, '0', '9')) {
      json_lexer_load(lexer);
    }
  }
  if (json_lexer_accept(lexer, '.')) {
    json_lexer_load(lexer);
    if (!json_lexer_accept_between(lexer, '0', '9') ||
        !json_lexer_load(lexer)) {
      lexer->error_ = JSON_ERROR_NUMBER_TOKEN;
      return 0;
    }
    while (json_lexer_accept_between(lexer, '0', '9')) {
      json_lexer_load(lexer);
    }
  }
  // todo: exponents
  json_lexer_yield(lexer, JSON_TOKEN_NUMBER, token);
  return 1;
}

int json_lexer_next(json_lexer *lexer, json_token *token) {
  // white space:
  if (json_lexer_accept_white_space(lexer)) {
    while (json_lexer_accept_white_space(lexer)) {
      // nom nom nom...
    }
    json_lexer_yield(lexer, JSON_TOKEN_WHITE_SPACE, token);
    // however, we don't return here since
    // we don't really care about white space
  }
  switch (lexer->ch_) {
    case '\0': {
      lexer->error_ = JSON_ERROR_END_OF_FILE;
      return 0;
    }
    case '"': {  // string
      return json_lexer_next_string(lexer, token);
    }
    case '-':  // number
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      return json_lexer_next_number(lexer, token);
    }
    case '{': {  // object
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_OBJECT_BEGIN, token);
      return 1;
    }
    case ':': {
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_KV_SEPARATOR, token);
      return 1;
    }
    case '}': {
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_OBJECT_END, token);
      return 1;
    }
    case '[': {  // array
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_ARRAY_BEGIN, token);
      return 1;
    }
    case ',': {
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_LIST_SEPARATOR, token);
      return 1;
    }
    case ']': {
      json_lexer_accept_any(lexer);
      json_lexer_yield(lexer, JSON_TOKEN_ARRAY_END, token);
      return 1;
    }
    case 't': {  // true
      if (json_lexer_accept_string(lexer, "true")) {
        json_lexer_yield(lexer, JSON_TOKEN_TRUE, token);
        return 1;
      }
      break;
    }
    case 'f': {  // false
      if (json_lexer_accept_string(lexer, "false")) {
        json_lexer_yield(lexer, JSON_TOKEN_FALSE, token);
        return 1;
      }
      break;
    }
    case 'n': {  // null
      if (json_lexer_accept_string(lexer, "null")) {
        json_lexer_yield(lexer, JSON_TOKEN_NULL, token);
        return 1;
      }
      break;
    }
  }
  lexer->error_ = JSON_ERROR_TOKEN;
  return 0;
}

// json parser

void json_parser_initialize(const char *json, json_parser *parser) {
  memset(parser, 0, sizeof(*parser));
  json_lexer_initialize(json, &parser->lexer_);
  json_parser_read(parser);
}

int json_parser_read(json_parser *parser) {
  if (json_lexer_next(&parser->lexer_, &parser->token_)) {
    parser->is_valid_ = 1;
    return 1;
  }
  parser->is_valid_ = 0;
  return 0;
}

int json_parser_accept(json_parser *parser, json_token_type type) {
  if ((parser->is_valid_ != 0) & (parser->token_.type_ == type)) {
    json_parser_read(parser);
    return 1;
  }
  return 0;
}

typedef struct json_string_reader {
  const char *front_;
  const char *back_;
} json_string_reader;

typedef struct json_string_builder {
  char *front_;
  char *back_;
} json_string_builder;

void json_string_reader_initialize(const char *buffer, uint32_t buffer_size,
                                   json_string_reader *reader) {
  reader->front_ = buffer;
  reader->back_ = buffer + buffer_size;
}

char json_string_reader_peek(json_string_reader *reader) {
  if (reader->front_ < reader->back_) {
    return *reader->front_;
  }
  return '\0';
}

int json_string_reader_read(json_string_reader *reader) {
  if (reader->front_ < reader->back_) {
    reader->front_++;
    return 1;
  }
  return 0;
}

void json_string_builder_initialize(char *buffer, uint32_t buffer_size,
                                    json_string_builder *sb) {
  sb->front_ = buffer;
  sb->back_ = buffer + buffer_size;
}

int json_string_builder_append(const char ch, json_string_builder *sb) {
  if (sb->front_ < sb->back_) {
    *sb->front_++ = ch;
    return 1;
  }
  return 0;
}

int json_unescape_string_literal(const char *s, uint32_t s_size,
                                 char *unescaped, uint32_t unescaped_size) {
  json_string_reader reader;
  json_string_reader_initialize(s, s_size, &reader);
  json_string_builder sb;
  json_string_builder_initialize(unescaped, unescaped_size, &sb);

  if (!(json_string_reader_peek(&reader) == '"')) {
    return 0;
  }
  json_string_reader_read(&reader);

  char ch;
  while ((ch = json_string_reader_peek(&reader))) {
    if ((ch != '"') & (ch != '\\')) {
      json_string_reader_read(&reader);
      if (!json_string_builder_append(ch, &sb)) {
        return 0;
      }
    } else {
      if (ch == '\\') {  // escape sequence
        json_string_reader_read(&reader);
        switch (json_string_reader_peek(&reader)) {
          case '"': {
            if (!json_string_builder_append('"', &sb)) {
              return 0;
            }
            break;
          }
          case '\\': {
            if (!json_string_builder_append('\\', &sb)) {
              return 0;
            }
            break;
          }
          case '/': {
            if (!json_string_builder_append('/', &sb)) {
              return 0;
            }
            break;
          }
          case 'b': {
            if (!json_string_builder_append('\b', &sb)) {
              return 0;
            }
            break;
          }
          case 'f': {
            if (!json_string_builder_append('\f', &sb)) {
              return 0;
            }
            break;
          }
          case 'n': {
            if (!json_string_builder_append('\n', &sb)) {
              return 0;
            }
            break;
          }
          case 'r': {
            if (!json_string_builder_append('\r', &sb)) {
              return 0;
            }
            break;
          }
          case 't': {
            if (!json_string_builder_append('\t', &sb)) {
              return 0;
            }
            break;
          }
          default: {
            // unrecognized escape sequence
            return 0;
          }
        }
        json_string_reader_read(&reader);
      } else {
        break;
      }
    }
  }

  if (!(json_string_reader_peek(&reader) == '"')) {
    return 0;
  }
  json_string_reader_read(&reader);

  int length = unescaped - sb.front_;  // compute string length without '\0'

  // ensure null-terminated
  if (!json_string_builder_append('\0', &sb)) {
    return 0;
  }

  return length;
}
