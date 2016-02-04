#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PARC_CALLCONV
#ifdef _WIN32
#define PARC_CALLCONV __stdcall
#else
#define PARC_CALLCONV
#endif
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct parc_token parc_token;
typedef struct parc_lexer parc_lexer;
typedef struct parc_parser parc_parser;
typedef struct parc_syntax_tree parc_syntax_tree;
typedef struct parc_syntax_node parc_syntax_node;
typedef struct parc_syntax_token parc_syntax_token;

// forward declarations for intermidate representation
struct msgpack_writer;
struct msgpack_reader;

typedef enum {
  // No error, operation completed successfully
  PARC_ERROR_OK = 0,
  // Not an error, but the operation did not do anything, end of file was
  // reached
  PARC_ERROR_END_OF_FILE = 1,
  PARC_ERROR_INVALID_UTF8_BYTE_SEQUENCE = 2,
  // cannot tokenize input
  PARC_ERROR_TOKEN = 3,
  // the parser cannot apply a specific token or production but it was expected.
  // the input does not conform to the grammar.
  PARC_ERROR_EXPECT = 4,
} parc_error;

#include "slice.h"

typedef enum {
  PARC_TOKEN_END_OF_ARRAY = -2,
  PARC_TOKEN_END_OF_STREAM = -1,
  PARC_TOKEN_NONE = 0,
  PARC_TOKEN_WHITE_SPACE,
  PARC_TOKEN_STRING_LITERAL,
  PARC_TOKEN_LEFT_PARENTHESIS,
  PARC_TOKEN_RIGHT_PARENTHESIS,
  PARC_TOKEN_ASTERISK,
  PARC_TOKEN_PLUS_SIGN,
  PARC_TOKEN_RANGE,
  PARC_TOKEN_SEMICOLON,
  PARC_TOKEN_EQUALS_SIGN,
  PARC_TOKEN_QUESTION_MARK,
  PARC_TOKEN_CIRCUMFLEX_ACCENT,
  PARC_TOKEN_LEFT_BRACE,
  PARC_TOKEN_PIPE,
  PARC_TOKEN_RIGHT_BRACE,
  PARC_TOKEN_LOCAL,
  PARC_TOKEN_GLOBAL,
} parc_token_type;

struct parc_token {
  parc_slice leading_trivia_;
  parc_token_type type_;
  parc_slice s_;
  int line_num_;
  int char_pos_;
};

// token API

int parc_token_store(const parc_token *token, struct msgpack_writer *writer);

int parc_token_load(struct msgpack_reader *reader, parc_token *token);

// parc_lexer API

struct parc_lexer {
  const char *head_;
  const char *tail_;
  const char *end_;
  int ch_;
  int ch_byte_count_;
  parc_token leading_trivia_;
  parc_error error_;
};

void parc_lexer_init(const char *data, const size_t size, parc_lexer *lexer);

int parc_lexer_next(parc_lexer *lexer, parc_token *token);

// syntax tree API

typedef enum {
  PARC_SYNTAX_TREE_TYPE_NONE,
  PARC_SYNTAX_TREE_TYPE_NODE,
  PARC_SYNTAX_TREE_TYPE_TOKEN,
} parc_syntax_tree_type;

// The syntax tree is an intrusive linked list. To add depth, Each syntax node
// has a first child member that points to the first item in the child node list
// for that syntax node. A syntax token is a leaf node and does not have
// additional depth information.

struct parc_syntax_tree {
  parc_syntax_tree_type type_;
  parc_syntax_tree *sibling_;
};

struct parc_syntax_node {
  parc_syntax_tree base_;
  const char *label_;
  parc_syntax_tree *first_child_;
};

struct parc_syntax_token {
  parc_syntax_tree base_;
  parc_token token_;
};

int parc_syntax_tree_dump(parc_syntax_tree *tree,
                          struct msgpack_writer *writer);

// parser API

struct parc_parser {
  parc_lexer lexer_;
  parc_token token_;
  parc_syntax_tree *stack_;
  parc_error error_;
};

void parc_parser_init(const char *data, const size_t size, parc_parser *parser);

parc_syntax_tree *parc_parser_syntax(parc_parser *parser);

int parc_parser_main(parc_parser *parser);

// compiler API

#include "hash_map.h"

typedef enum parc_nfa_flags {
  PARC_NFA_FLAGS_NONE = 0 << 0,
  PARC_NFA_FLAG_BACKLINK = 1 << 0,
} parc_nfa_flags;

// the control flow graph is a nondeterministic finite automaton (NFA)
typedef struct parc_nfa_graph parc_nfa_graph;
typedef struct parc_nfa_node parc_nfa_node;
typedef struct parc_nfa_edge parc_nfa_edge;
typedef struct parc_nfa_guard parc_nfa_guard;

// the guard set is the only thing that is different between lexer and parser.
struct parc_nfa_guard {
  union {
    struct {
      int is_complement_;
      int lower_;
      int upper_;
    } lexer_;
  } u_;
  parc_nfa_guard *next_;
};

struct parc_nfa_edge {
  parc_nfa_node *u_;
  parc_nfa_node *v_;
  parc_nfa_guard *guard_set_;
  parc_nfa_flags flags_;
};

struct parc_nfa_node {
  int node_id_;
  parc_nfa_node *next_;
};

struct parc_nfa_graph {
  int node_id_;
  parc_nfa_node root_;
  hash_map adjacency_list_;
};

typedef struct {
  parc_syntax_tree *syntax_;
  parc_nfa_graph *nfa_;
  parc_nfa_node *nfa_stack_;
  // the a running count of negations we've passed through,
  // think of it as a flip/flop
  // when this number is odd we emit the complement of range operations
  int neg_;
} parc_compiler;

void parc_compiler_init(parc_syntax_tree *syntax, parc_compiler *compiler);

int parc_compiler_compile(parc_compiler *compiler);

// write the NFA to a msgpack stream
int parc_nfa_dump(parc_nfa_graph *nfa, char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif
