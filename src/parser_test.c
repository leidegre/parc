
#include "api.h"

#include "test.h"

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("parc_parser_test") {
    parc_parser parser;
	parc_parser_init("\"parc\" { \"white-space\" \"ignore\" ; token_a = ^ \"A\" .. \"Z\" + ; token_b = \"A\" \"B\" \"C\" ; }", &parser);
    parc_parser_main(&parser);

	char temp[4096];
	parc_syntax_tree_debug(parser.stack_, temp, sizeof(temp));
	puts(temp);
  }

  return 0;
}