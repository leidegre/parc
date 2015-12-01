
#include "api.h"

#include <assert.h>

static int utf8_get_mb_count(unsigned int v) {
  // 1 byte: 0xxx xxxx
  if (v <= 127) {
    return 1;
  }
  // 2 byte: 110x xxxx
  if (v <= 223) {
    return 2;
  }
  // 3 byte: 1110 xxxx
  if (v <= 239) {
    return 3;
  }
  // 4 byte: 1111 0xxx
  if (v <= 247) {
    return 4;
  }
  // 5 byte: 1111 10xx
  if (v <= 251) {
    return 5;
  }
  // 6 byte: 1111 110x
  if (v <= 253) {
    return 6;
  }
  return -1;
}

static int parc_lexer_load(parc_lexer *lexer) {
  // Decode a single Unicode code point from an UTF-8 byte sequence.
  if (!(lexer->front_ < lexer->end_)) {
    lexer->ch_ = 0;
    lexer->ch_byte_count_ = 0;
    lexer->error_ = PARC_ERROR_END_OF_FILE;
    return 0;
  }
  byte *b = (byte *)lexer->front_;
  int n = utf8_get_mb_count(b[0]);
  if (!(lexer->front_ + n <= lexer->end_)) {
    lexer->ch_ = 0;
    lexer->ch_byte_count_ = 0;
    lexer->error_ = PARC_ERROR_INVALID_UTF8_BYTE_SEQUENCE;
    return 0;
  }
  switch (n) {
    case 1: {
      lexer->ch_ = (int)b[0];
      lexer->ch_byte_count_ = n;
      lexer->error_ = PARC_ERROR_OK;
      return 1;
    }
    case 2: {
      lexer->ch_ = ((int)(b[0] & 0x1f) << 6) | (int)(b[1] & 0x3f);
      lexer->ch_byte_count_ = n;
      lexer->error_ = PARC_ERROR_OK;
      return 1;
    }
    case 3: {
      lexer->ch_ = ((int)(b[0] & 0x0f) << 12) | ((int)(b[1] & 0x3f) << 6) |
                   (int)(b[2] & 0x3f);
      lexer->ch_byte_count_ = n;
      lexer->error_ = PARC_ERROR_OK;
      return 1;
    }
    case 4: {
      lexer->ch_ = ((int)(b[0] & 0x07) << 18) | ((int)(b[1] & 0x3f) << 12) |
                   ((int)(b[2] & 0x3f) << 6) | (int)(b[3] & 0x3f);
      lexer->ch_byte_count_ = n;
      lexer->error_ = PARC_ERROR_OK;
      return 1;
    }
  }
  lexer->ch_ = 0;
  lexer->ch_byte_count_ = 0;
  lexer->error_ = PARC_ERROR_INVALID_UTF8_BYTE_SEQUENCE;
  return 0;
}

// REQUIRES: parc_lexer_load()
static void parc_lexer_read(parc_lexer *lexer) {
  assert(lexer->ch_byte_count_ > 0);
  lexer->front_ += lexer->ch_byte_count_;
  lexer->ch_byte_count_ = 0;
}

static bool is_white_space(const int ch) {
  return (ch == '\t') | (ch == '\n') | (ch == '\r') | (ch == ' ');
}

static bool is_between(const int ch, const int lower, const int upper) {
  return (lower <= ch) & (ch <= upper);
}

// Accept any character except the null character, '\0'
static int parc_lexer_accept_any(parc_lexer *lexer) {
  if (lexer->ch_ != '\0') {
    parc_lexer_read(lexer);
    parc_lexer_load(lexer);
    return 1;
  }
  return 0;
}

static int parc_lexer_accept(parc_lexer *lexer, const int ch) {
  if (lexer->ch_ == ch) {
    parc_lexer_read(lexer);
    parc_lexer_load(lexer);
    return 1;
  }
  return 0;
}

static int parc_lexer_accept_white_space(parc_lexer *lexer) {
  if (is_white_space(lexer->ch_)) {
    parc_lexer_read(lexer);
    parc_lexer_load(lexer);
    return 1;
  }
  return 0;
}

static int parc_lexer_accept_between(parc_lexer *lexer, const int lower,
                                     const int upper) {
  if (is_between(lexer->ch_, lower, upper)) {
    parc_lexer_read(lexer);
    parc_lexer_load(lexer);
    return 1;
  }
  return 0;
}

static void parc_lexer_yield(parc_lexer *lexer, parc_token_type token_type,
                             parc_token *token) {
  token->leading_trivia_ = lexer->leading_trivia_.s_;
  token->type_ = token_type;
  token->s_ = parc_slice_create(lexer->back_, lexer->front_ - lexer->back_);
  token->line_num_ = 1;
  token->char_pos_ = 1;

  lexer->back_ = lexer->front_;
}

void parc_lexer_initialize(const char *source, parc_lexer *lexer) {
  memset(lexer, 0, sizeof(*lexer));
  lexer->front_ = source;
  lexer->back_ = source;
  lexer->end_ = source + strlen(source);
  parc_lexer_load(lexer);
}

static int parc_lexer_next_string(parc_lexer *lexer, parc_token *token) {
  if (!parc_lexer_accept(lexer, '"')) {
    return 0;
  }
  for (;;) {
    if (parc_lexer_accept(lexer, '\\')) {
      // escape sequence
      switch (lexer->ch_) {
        case '0':
        case 'n':  // we don't need \r if we normalize new-line, \r\n -> \n
        case 't':
        case '"':
        case '\\': {
          parc_lexer_accept_any(lexer);
          break;
        }
        case 'x': {
          parc_lexer_accept_any(lexer);
          if (!(parc_lexer_accept_between(lexer, '0', '9') ||
                parc_lexer_accept_between(lexer, 'A', 'F') ||
                parc_lexer_accept_between(lexer, 'a', 'f'))) {
            return 0;
          }
          if (!(parc_lexer_accept_between(lexer, '0', '9') ||
                parc_lexer_accept_between(lexer, 'A', 'F') ||
                parc_lexer_accept_between(lexer, 'a', 'f'))) {
            return 0;
          }
          break;
        }
        default: {
          // unrecognized escape sequence
          return 0;
        }
      }
    }
    if (parc_lexer_accept(lexer, '"')) {
      break;
    }
    // note the following line will accept new-line
    if (!parc_lexer_accept_any(lexer)) {
      return 0;
    }
  }
  return 1;
}

static int parc_lexer_next_local(parc_lexer *lexer, parc_token *token) {
  if (!parc_lexer_accept_between(lexer, 'a', 'z')) {
    return 0;
  }
  while (parc_lexer_accept_between(lexer, '0', '9') ||
         parc_lexer_accept(lexer, '_') ||
         parc_lexer_accept_between(lexer, 'a', 'z')) {
    // nom nom nom...
  }
  return 1;
}

static int parc_lexer_next_global(parc_lexer *lexer, parc_token *token) {
  if (!parc_lexer_accept_between(lexer, 'A', 'Z')) {
    return 0;
  }
  while (parc_lexer_accept_between(lexer, '0', '9') ||
         parc_lexer_accept_between(lexer, 'A', 'Z') ||
         parc_lexer_accept_between(lexer, 'a', 'z')) {
    // nom nom nom...
  }
  return 1;
}

int parc_lexer_next(parc_lexer *lexer, parc_token *token) {
  // white space:
  if (parc_lexer_accept_white_space(lexer)) {
    while (parc_lexer_accept_white_space(lexer)) {
      // nom nom nom...
    }
    parc_lexer_yield(lexer, PARC_TOKEN_WHITE_SPACE, &lexer->leading_trivia_);
    // we don't return here and instead put white space in an intermediate token
  }
  switch (lexer->ch_) {
    case '\0': {
      lexer->error_ = PARC_ERROR_END_OF_FILE;
      return 0;
    }
    case '"': {
      if (parc_lexer_next_string(lexer, token)) {
        parc_lexer_yield(lexer, PARC_TOKEN_STRING_LITERAL, token);
        return 1;
      }
      break;
    }
    case '(': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_LEFT_PARENTHESIS, token);
      return 1;
    }
    case ')': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_RIGHT_PARENTHESIS, token);
      return 1;
    }
    case '*': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_ASTERISK, token);
      return 1;
    }
    case '+': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_PLUS_SIGN, token);
      return 1;
    }
    case '.': {
      parc_lexer_accept_any(lexer);
      if (!parc_lexer_accept(lexer, '.')) {
        lexer->error_ = PARC_ERROR_TOKEN;
        return 0;
      }
      parc_lexer_yield(lexer, PARC_TOKEN_RANGE, token);
      return 1;
    }
    case ';': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_SEMICOLON, token);
      return 1;
    }
    case '=': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_EQUALS_SIGN, token);
      return 1;
    }
    case '?': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_QUESTION_MARK, token);
      return 1;
    }
    case '^': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_CIRCUMFLEX_ACCENT, token);
      return 1;
    }
    case '{': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_LEFT_BRACE, token);
      return 1;
    }
    case '|': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_PIPE, token);
      return 1;
    }
    case '}': {
      parc_lexer_accept_any(lexer);
      parc_lexer_yield(lexer, PARC_TOKEN_RIGHT_BRACE, token);
      return 1;
    }
    default: {
      if (is_between(lexer->ch_, 'a', 'z')) {
        if (parc_lexer_next_local(lexer, token)) {
          parc_lexer_yield(lexer, PARC_TOKEN_LOCAL, token);
          return 1;
        }
      }
      if (is_between(lexer->ch_, 'A', 'Z')) {
        if (parc_lexer_next_global(lexer, token)) {
          parc_lexer_yield(lexer, PARC_TOKEN_GLOBAL, token);
          return 1;
        }
      }
      break;
    }
  }
  lexer->error_ = PARC_ERROR_TOKEN;
  return 0;
}
