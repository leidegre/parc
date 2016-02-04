
#include "api.h"

#include "test.h"

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("lexer_compiler_test") {
    // example source
    const char* s =
        "\"parc\" { "
        "token = \"a\" .. \"z\" ( ( \"0\" .. \"9\" ) | \"_\" | ( \"a\" .. "
        "\"z\" ) ) * ;"
        "}";

    // first we do semantic analysis to create a syntax tree
    parc_parser parser;
    parc_parser_init(s, strlen(s), &parser);
    parc_parser_main(&parser);

    return 0;

    // we then check that the syntax tree is OK,
    // and that it does not have ambiguities.
    // if it does not we create a nfa for the grammar.
    parc_compiler compiler;
    parc_compiler_init(parser.stack_, &compiler);
    parc_compiler_compile(&compiler);

    parc_nfa_graph* g = compiler.nfa_;

    // dump
    {
      hash_map_iter it;
      hash_map_iter_create(&g->adjacency_list_, &it);
      while (hash_map_iter_next(&it)) {
        parc_nfa_edge* edge = (parc_nfa_edge*)hash_map_value_get(it.current_);
        printf("%p %3i -> %3i (%x)\n", (void*)edge, edge->u_->node_id_,
               edge->v_->node_id_, edge->flags_);
        parc_nfa_guard* guard = edge->guard_set_;
        while (guard) {
          const char* fmt =
              guard->u_.lexer_.is_complement_ ? "  ]%i,%i[\n" : "  [%i,%i]\n";
          printf(fmt, guard->u_.lexer_.lower_, guard->u_.lexer_.upper_);
          guard = guard->next_;
        }
      }
    }
  }

  return 0;
}
