
#include "api.h"

#include <assert.h>

// we don't care about this memory leaking, for now. the idea is to just
// allocate a large chunk up front and use that. then throw it all away once
// everything is done.

static parc_syntax_token *parc_syntax_token_create(parc_token token) {
  parc_syntax_token *syntax_token = malloc(sizeof(parc_syntax_token));
  memset(syntax_token, 0, sizeof(*syntax_token));
  syntax_token->base_.type_ = PARC_SYNTAX_TREE_TYPE_TOKEN;
  syntax_token->token_ = token;
  return syntax_token;
}

static parc_syntax_node *parc_syntax_node_create(
    const char *label, parc_syntax_tree *first_child) {
  parc_syntax_node *syntax_node = malloc(sizeof(parc_syntax_node));
  memset(syntax_node, 0, sizeof(*syntax_node));
  syntax_node->base_.type_ = PARC_SYNTAX_TREE_TYPE_NODE;
  syntax_node->label_ = label;
  syntax_node->first_child_ = first_child;
  return syntax_node;
}

static void parc_parser_push(parc_parser *parser,
                             parc_syntax_tree *syntax_tree) {
  // insert token at the front of this list
  syntax_tree->sibling_ = parser->stack_;
  parser->stack_ = syntax_tree;
}

static int parc_parser_accept(parc_parser *parser, parc_token_type type) {
  // accept is only functional if no error has been set.
  // this way an error will short circuit subsequent calls to accept and
  // eventually terminate the parser.
  if ((parser->error_ == PARC_ERROR_OK) & (parser->token_.type_ == type)) {
    parc_syntax_token *syntax_token = parc_syntax_token_create(parser->token_);
    parc_parser_push(parser, &syntax_token->base_);
    parc_lexer_next(&parser->lexer_, &parser->token_);
    return 1;
  }
  return 0;
}

static void parc_parser_expect(parc_parser *parser, parc_token_type type) {
  if (!parc_parser_accept(parser, type)) {
    parser->error_ = PARC_ERROR_EXPECT;
  }
}

static void parc_parser_reduce(parc_parser *parser, const char *label,
                               int count) {
  assert(count > 0 && "count must be greater than zero");
  // this will rebuild the top of the stack while reversing the order
  parc_syntax_tree *node = parser->stack_;
  parc_syntax_tree *prev = NULL;
  for (int i = 0; i < count; i++) {
    assert(node && "node cannot be null");
    parc_syntax_tree *next = node->sibling_;  // next may be null
    node->sibling_ = prev;
    prev = node;
    node = next;  // now node may be null (check for that)
  }
  // node is our new head
  parser->stack_ = node;
  // prev is now our child node list
  parc_syntax_node *syntax_node = parc_syntax_node_create(label, prev);
  parc_parser_push(parser, &syntax_node->base_);
}

void parc_parser_init(const char *source, parc_parser *parser) {
  memset(parser, 0, sizeof(*parser));
  parc_lexer_initialize(source, &parser->lexer_);
  parc_lexer_next(&parser->lexer_, &parser->token_);
};

static int parc_parse_grammar_unit(parc_parser *parser);
static int parc_parse_namespace(parc_parser *parser);
static int parc_parse_rule(parc_parser *parser);
static int parc_parse_option(parc_parser *parser);
static int parc_parse_token_rule(parc_parser *parser);
static int parc_parse_token_primary(parc_parser *parser);
static int parc_parse_token_expr(parc_parser *parser);
static int parc_parse_token_rep(parc_parser *parser);
static int parc_parse_production_rule(parc_parser *parser);

int parc_parser_main(parc_parser *parser) {
  if (parc_parse_grammar_unit(parser)) {
    return 1;
  }
  return 0;
}

static int parc_parse_grammar_unit(parc_parser *parser) {
  int c = 0;
  if (parc_parse_namespace(parser)) {
    c++;
    while (parc_parse_namespace(parser)) {
      c++;
    }
    parc_parser_reduce(parser, "Grammar", c);
    return 1;
  }
  return 0;
}

static int parc_parse_namespace(parc_parser *parser) {
  int c = 0;
  if (parc_parser_accept(parser, PARC_TOKEN_STRING_LITERAL)) {
    c++;
    parc_parser_expect(parser, PARC_TOKEN_LEFT_BRACE);
    c++;
    while (parc_parse_rule(parser)) {
      c++;
    }
    parc_parser_expect(parser, PARC_TOKEN_RIGHT_BRACE);
    c++;
    parc_parser_reduce(parser, "Namespace", c);
    return 1;
  }
  return 0;
}

static int parc_parse_rule(parc_parser *parser) {
  if (parc_parse_option(parser)) {
    return 1;
  }
  if (parc_parse_token_rule(parser)) {
    return 1;
  }
  if (parc_parse_production_rule(parser)) {
    return 1;
  }
  return 0;
}

static int parc_parse_option(parc_parser *parser) {
  int c = 0;
  if (parc_parser_accept(parser, PARC_TOKEN_STRING_LITERAL)) {
    c++;
    parc_parser_expect(parser, PARC_TOKEN_STRING_LITERAL);
    c++;
    parc_parser_expect(parser, PARC_TOKEN_SEMICOLON);
    c++;
    parc_parser_reduce(parser, "Option", c);
    return 1;
  }
  return 0;
}

static int parc_parse_token_rule(parc_parser *parser) {
  int c = 0;
  if (parc_parser_accept(parser, PARC_TOKEN_LOCAL)) {
    c++;
    parc_parser_expect(parser, PARC_TOKEN_EQUALS_SIGN);
    c++;

    // TokenExpr+
    if (!parc_parse_token_rep(parser)) {
      parser->error_ = PARC_ERROR_EXPECT;
      return 0;
    }
    c++;
    while (parc_parse_token_rep(parser)) {
      c++;
    }

    parc_parser_expect(parser, PARC_TOKEN_SEMICOLON);
    c++;
    parc_parser_reduce(parser, "Token", c);
    return 1;
  }
  return 0;
}

static int parc_parse_token_primary(parc_parser *parser) {
  int c = 0;
  if (parc_parser_accept(parser, PARC_TOKEN_STRING_LITERAL)) {
    c++;
    parc_parser_reduce(parser, "Literal", c);
    return 1;
  }
  // unary prefix operator
  if (parc_parser_accept(parser, PARC_TOKEN_CIRCUMFLEX_ACCENT)) {
    c++;
    if (!parc_parse_token_expr(parser)) {
      parser->error_ = PARC_ERROR_EXPECT;
      return 0;
    }
    c++;
    parc_parser_reduce(parser, "Negation", c);
    return 1;
  }
  if (parc_parser_accept(parser, PARC_TOKEN_LEFT_PARENTHESIS)) {
    c++;

    if (!parc_parse_token_expr(parser)) {
      parser->error_ = PARC_ERROR_EXPECT;
      return 0;
    }
    c++;

    parc_parser_expect(parser, PARC_TOKEN_RIGHT_PARENTHESIS);
    c++;

    parc_parser_reduce(parser, "Eval", c);
    return 1;
  }
  return 0;
}

static int parc_parse_token_expr(parc_parser *parser) {
  int c = 0;
  if (parc_parse_token_primary(parser)) {
    c++;
    // binary operator
    if (parc_parser_accept(parser, PARC_TOKEN_PIPE)) {
      c++;
      if (!parc_parse_token_expr(parser)) {
        parser->error_ = PARC_ERROR_EXPECT;
        return 0;
      }
      c++;
      parc_parser_reduce(parser, "Choice", c);
      return 1;
    }
    if (parc_parser_accept(parser, PARC_TOKEN_RANGE)) {
      c++;
      if (!parc_parse_token_expr(parser)) {
        parser->error_ = PARC_ERROR_EXPECT;
        return 0;
      }
      c++;
      parc_parser_reduce(parser, "Range", c);
      return 1;
    }
    return 1;
  }
  return 0;
}

static int parc_parse_token_rep(parc_parser *parser) {
  int c = 0;
  if (parc_parse_token_expr(parser)) {
    c++;
    // unary suffix operator
    if (parc_parser_accept(parser, PARC_TOKEN_ASTERISK) ||
        parc_parser_accept(parser, PARC_TOKEN_PLUS_SIGN) ||
        parc_parser_accept(parser, PARC_TOKEN_QUESTION_MARK)) {
      c++;
      parc_parser_reduce(parser, "Quantifier", c);
      return 1;
    }
    return 1;
  }
  return 0;
}

static int parc_parse_production_rule(parc_parser *parser) {
  return 0;
}