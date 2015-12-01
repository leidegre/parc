
#include "api.h"

#include <assert.h>

// note: we do a lot of allocations this does smooth things over a bit
#define new_obj(type, name)                  \
  type *name = (type *)malloc(sizeof(type)); \
  assert(name && "out of memory");           \
  memset(name, 0, sizeof(type))

static void parc_nfa_edge_add_guard(parc_nfa_edge *edge,
                                    parc_nfa_guard *guard) {
  assert(guard && edge);
  // insert first
  parc_nfa_guard *fst = edge->guard_set_;  // note: guard_set_ may be null
  edge->guard_set_ = guard;
  guard->next_ = fst;
}

static void parc_nfa_node_add_edge(parc_nfa_node *node, parc_nfa_edge *edge,
                                   parc_compiler *compiler) {
  assert(edge && node);
  edge->u_ = node;  // from u to v

  // parc_nfa_edge *fst = node->edge_list_;  // note: edge_list_ may be null
  // node->edge_list_ = edge;
  // edge->next_ = fst;

  hash_map_insert_ptr(&compiler->nfa_->adjacency_list_, node, edge);
}

static void parc_compiler_nfa_stack_push(parc_compiler *compiler,
                                         parc_nfa_node *state) {
  parc_nfa_node *first = compiler->nfa_stack_;  // may be null
  state->next_ = first;
  compiler->nfa_stack_ = state;
}

static parc_nfa_node *parc_compiler_nfa_stack_top(parc_compiler *compiler) {
  assert(compiler->nfa_stack_ && "stack is empty");
  return compiler->nfa_stack_;
}

static void parc_compiler_nfa_stack_pop(parc_compiler *compiler) {
  parc_nfa_node *top = parc_compiler_nfa_stack_top(compiler);
  compiler->nfa_stack_ = top->next_;
  top->next_ = NULL;
}

//
static void parc_compiler_accept_between(int lower, int upper,
                                         parc_compiler *compiler) {
  parc_nfa_node *curr = parc_compiler_nfa_stack_top(compiler);
  if (hash_map_find_ptr(&compiler->nfa_->adjacency_list_, curr)) {
    // look for an existing transition
  } else {
    // create a new transition
    new_obj(parc_nfa_node, next);
    next->node_id_ = compiler->nfa_->node_id_++;

    new_obj(parc_nfa_guard, guard);
	guard->u_.lexer_.is_complement_ = 0;
    guard->u_.lexer_.lower_ = lower;
    guard->u_.lexer_.upper_ = upper;

    new_obj(parc_nfa_edge, transition);
    transition->u_ = curr;
    transition->v_ = next;

    parc_nfa_edge_add_guard(transition, guard);

    parc_nfa_node_add_edge(curr, transition, compiler);

    // replace top
    parc_compiler_nfa_stack_pop(compiler);
    parc_compiler_nfa_stack_push(compiler, next);
  }
}

void parc_compiler_init(parc_syntax_tree *syntax, parc_compiler *compiler) {
  assert(syntax && "syntax tree cannot be NULL");
  memset(compiler, 0, sizeof(*compiler));
  compiler->syntax_ = syntax;
}

static void parc_compiler_visit_grammar(parc_compiler *compiler,
                                        parc_syntax_node *syntax_node);
static void parc_compiler_visit_namespace(parc_compiler *compiler,
                                          parc_syntax_node *syntax_node);
static void parc_compiler_visit_option(parc_compiler *compiler,
                                       parc_syntax_node *syntax_node);
static void parc_compiler_visit_eval(parc_compiler *compiler,
                                     parc_syntax_node *syntax_node);
static void parc_compiler_visit_token(parc_compiler *compiler,
                                      parc_syntax_node *syntax_node);
static void parc_compiler_visit_token_choice(parc_compiler *compiler,
                                             parc_syntax_node *syntax_node);
static void parc_compiler_visit_token_quantifier(parc_compiler *compiler,
                                                 parc_syntax_node *syntax_node);
static void parc_compiler_visit_token_negation(parc_compiler *compiler,
                                               parc_syntax_node *syntax_node);
static void parc_compiler_visit_token_range(parc_compiler *compiler,
                                            parc_syntax_node *syntax_node);
static void parc_compiler_visit_token_literal(parc_compiler *compiler,
                                              parc_syntax_node *syntax_node);

static void parc_compiler_visit(parc_compiler *compiler,
                                parc_syntax_tree *syntax_tree) {
  // let's use a linear scan for now
  // (we can use a particle to generate a perfect hash function for this)
  assert(syntax_tree->type_ == PARC_SYNTAX_TREE_TYPE_NODE);
  parc_syntax_node *syntax_node = (parc_syntax_node *)syntax_tree;
  if (strcmp(syntax_node->label_, "Grammar") == 0) {
    parc_compiler_visit_grammar(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Namespace") == 0) {
    parc_compiler_visit_namespace(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Option") == 0) {
    parc_compiler_visit_option(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Eval") == 0) {
    parc_compiler_visit_eval(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Token") == 0) {
    parc_compiler_visit_token(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Choice") == 0) {
    parc_compiler_visit_token_choice(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Quantifier") == 0) {
    parc_compiler_visit_token_quantifier(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Negation") == 0) {
    parc_compiler_visit_token_negation(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Range") == 0) {
    parc_compiler_visit_token_range(compiler, syntax_node);
    return;
  }
  if (strcmp(syntax_node->label_, "Literal") == 0) {
    parc_compiler_visit_token_literal(compiler, syntax_node);
    return;
  }
  assert(0 && "this syntax node is not supported here");
}

int parc_compiler_compile(parc_compiler *compiler) {
  compiler->nfa_ = (parc_nfa_graph *)malloc(sizeof(parc_nfa_graph));

  hash_map_create(&compiler->nfa_->adjacency_list_);

  parc_compiler_visit(compiler, compiler->syntax_);
  return 0;
}

static void parc_compiler_visit_grammar(parc_compiler *compiler,
                                        parc_syntax_node *syntax_node) {
  parc_syntax_tree *fst = syntax_node->first_child_;
  parc_syntax_tree *curr = fst;
  while (curr) {
    parc_compiler_visit(compiler, curr);
    curr = curr->sibling_;
  }
}
static void parc_compiler_visit_namespace(parc_compiler *compiler,
                                          parc_syntax_node *syntax_node) {
  parc_syntax_tree *literal = syntax_node->first_child_;
  parc_syntax_tree *l_brace = literal->sibling_;
  parc_syntax_tree *curr = l_brace->sibling_;
  while (curr && curr->type_ == PARC_SYNTAX_TREE_TYPE_NODE) {
    parc_compiler_visit(compiler, curr);
    curr = curr->sibling_;
  }
}
static void parc_compiler_visit_option(parc_compiler *compiler,
                                       parc_syntax_node *syntax_node) {
  // don't care at the moment
}
static void parc_compiler_visit_eval(parc_compiler *compiler,
                                     parc_syntax_node *syntax_node) {
  parc_syntax_tree *l_paren = syntax_node->first_child_;
  parc_syntax_tree *expr = l_paren->sibling_;
  parc_compiler_visit(compiler, expr);
  // parc_syntax_tree *r_paren = expr->sibling_;
}

static void parc_compiler_visit_token(parc_compiler *compiler,
                                      parc_syntax_node *syntax_node) {
  parc_syntax_tree *identifier = syntax_node->first_child_;

  parc_compiler_nfa_stack_push(compiler, &compiler->nfa_->root_);

  parc_syntax_tree *eq_sign = identifier->sibling_;
  parc_syntax_tree *curr = eq_sign->sibling_;
  while (curr) {
    if (curr->type_ == PARC_SYNTAX_TREE_TYPE_NODE) {
      parc_compiler_visit(compiler, curr);
    }
    curr = curr->sibling_;
  }

  // the nfa stack should now contain a new token, we reach this through the
  // root node
  parc_compiler_nfa_stack_pop(compiler);
}

static void parc_compiler_visit_token_choice(parc_compiler *compiler,
                                             parc_syntax_node *syntax_node) {
  new_obj(parc_nfa_node, enter);
  enter->node_id_ = compiler->nfa_->node_id_++;

  new_obj(parc_nfa_node, leave);
  leave->node_id_ = compiler->nfa_->node_id_++;

  parc_nfa_node *top = parc_compiler_nfa_stack_top(compiler);
  parc_compiler_nfa_stack_pop(compiler);

  // connect top with enter
  new_obj(parc_nfa_edge, e);
  e->v_ = enter;
  parc_nfa_node_add_edge(top, e, compiler);

  parc_syntax_tree *expr = syntax_node->first_child_;
  do {
    if (expr->type_ == PARC_SYNTAX_TREE_TYPE_NODE) {
      parc_compiler_nfa_stack_push(compiler, enter);
      parc_compiler_visit(compiler, expr);
      parc_nfa_node *inner = parc_compiler_nfa_stack_top(compiler);
      parc_compiler_nfa_stack_pop(compiler);
      // connect inner with leave
      new_obj(parc_nfa_edge, e2);
      e2->v_ = leave;
      parc_nfa_node_add_edge(inner, e2, compiler);
    }
    expr = expr->sibling_;
  } while (expr);

  // return leave
  parc_compiler_nfa_stack_push(compiler, leave);
}

static void parc_compiler_visit_token_quantifier(
    parc_compiler *compiler, parc_syntax_node *syntax_node) {
  parc_syntax_tree *expr = syntax_node->first_child_;
  parc_syntax_token *quantifier = (parc_syntax_token *)expr->sibling_;
  parc_compiler_visit(compiler, expr);

  const int ch = quantifier->token_.s_.data_[0];  // todo: UTF-8 decode
  switch (ch) {
    case '*':
    case '+': {
      parc_nfa_node *curr = parc_compiler_nfa_stack_top(compiler);

      new_obj(parc_nfa_node, enter);
      enter->node_id_ = compiler->nfa_->node_id_++;

      new_obj(parc_nfa_node, leave);
      leave->node_id_ = compiler->nfa_->node_id_++;

      parc_compiler_nfa_stack_push(compiler, enter);

      parc_compiler_visit(compiler, expr);

      parc_nfa_node *inner = parc_compiler_nfa_stack_top(compiler);
      parc_compiler_nfa_stack_pop(compiler);
      parc_compiler_nfa_stack_push(compiler, leave);

      // let's connect stuff

      new_obj(parc_nfa_edge, curr_to_enter);
      curr_to_enter->u_ = curr;
      curr_to_enter->v_ = enter;
      parc_nfa_node_add_edge(curr, curr_to_enter, compiler);

      if (ch == '*') {
        // if kleene star, then the repetition is optional this we
        // accomplish by adding a shortcut that skips the inner state.

        new_obj(parc_nfa_edge, curr_to_leave);
        curr_to_leave->u_ = curr;
        curr_to_leave->v_ = leave;
        parc_nfa_node_add_edge(curr, curr_to_leave, compiler);
      }

      new_obj(parc_nfa_edge, inner_to_enter);
      inner_to_enter->u_ = inner;
      inner_to_enter->v_ = enter;
      inner_to_enter->flags_ |= PARC_NFA_FLAG_BACKLINK;
      parc_nfa_node_add_edge(inner, inner_to_enter, compiler);

      new_obj(parc_nfa_edge, inner_to_leave);
      inner_to_leave->u_ = inner;
      inner_to_leave->v_ = leave;
      parc_nfa_node_add_edge(inner, inner_to_leave, compiler);

      break;
    }
    case '?': {  // not sure if we should keep this...
      assert(0 && "not implemented");
      break;
    }
  }
}

static void parc_compiler_visit_token_negation(parc_compiler *compiler,
                                               parc_syntax_node *syntax_node) {
  // keep a running count of the number of negations
  compiler->neg_++;

  parc_compiler_visit(compiler, syntax_node->first_child_);

  assert(compiler->neg_ >= 0);
  compiler->neg_--;
}

static void parc_compiler_visit_token_range(parc_compiler *compiler,
                                            parc_syntax_node *syntax_node) {
  parc_syntax_tree *literal_a = syntax_node->first_child_;
  parc_syntax_tree *op = literal_a->sibling_;
  parc_syntax_tree *literal_b = op->sibling_;

  parc_syntax_token *a =
      (parc_syntax_token *)((parc_syntax_node *)literal_a)->first_child_;

  parc_syntax_token *b =
      (parc_syntax_token *)((parc_syntax_node *)literal_b)->first_child_;

  // todo: check that both a and b are single character literals
  // todo: check that a <= b

  const int lower = a->token_.s_.data_[1];  // todo: UTF-8 decode
  const int upper = b->token_.s_.data_[1];  // todo: UTF-8 decode
  parc_compiler_accept_between(lower, upper, compiler);
}

static void parc_compiler_visit_token_literal(parc_compiler *compiler,
                                              parc_syntax_node *syntax_node) {
  parc_syntax_token *token = (parc_syntax_token *)syntax_node->first_child_;
  // note: since the two code paths are identical the distinction isn't really
  // needed but we keept it for clarity sake
  if (token->token_.s_.size_ == 1) {
    // single character token
    const int ch = token->token_.s_.data_[0];  // todo: UTF-8 decode
    parc_compiler_accept_between(ch, ch, compiler);
  } else {
    // literal string, i.e. keyword
    for (size_t i = 0; i < token->token_.s_.size_; i++) {
      const int ch = token->token_.s_.data_[i];  // todo: UTF-8 decode
      parc_compiler_accept_between(ch, ch, compiler);
    }
  }
}