
#include "api.h"
#include "cli.h"
#include "file.h"
#include "msgpack.h"
#include "json.h"

#include <stdio.h>

static char scratch[8 * 1024 * 1024];

static void to_json(msgpack_writer*);

int main(int argc, char* argv[]) {
  parc_cli cli;
  if (!parc_cli_parse(&cli, argc, argv)) {
    fprintf(stderr, "Cannot parse command-line. Use '-h' for help.\n");
    return 2;
  }
  if (!cli.command_) {
    fprintf(stderr, "No command specified. Use '-h' for help.\n");
    return 1;
  }

  switch (cli.command_) {
    case PARC_CLI_COMMAND_LEX: {
      parc_cli_arg* arg0 = parc_cli_get(&cli, PARC_CLI_COMMAND_LEX);
      const char* input_fn = arg0->value_;
      file f;
      if (!file_open(input_fn, &f)) {
        return 1;
      }

      msgpack_writer w;
      msgpack_writer_init(scratch, sizeof(scratch), &w);

      msgpack_write_map(3, &w);

      msgpack_write_uint(1, &w);  // 1: type
      msgpack_write_str("token_stream", &w);

      msgpack_write_uint(2, &w);  // 2: version
      msgpack_write_str("1.0.0", &w);

      msgpack_write_uint(3, &w);  // 3: data
      msgpack_writer w2 = w;      // fix length later
      msgpack_write_array32(0, &w);

      parc_lexer lexer;
      parc_lexer_init(f.data_, f.size_, &lexer);

      size_t count = 0;

      parc_token token;
      while (parc_lexer_next(&lexer, &token)) {
        msgpack_write_array(5, &w);
        msgpack_write_str(token.leading_trivia_.data_,
                          token.leading_trivia_.size_, &w);
        msgpack_write_int(token.type_, &w);
        msgpack_write_str(token.s_.data_, token.s_.size_, &w);
        msgpack_write_int(token.line_num_, &w);
        msgpack_write_int(token.char_pos_, &w);
        count++;
      }

      msgpack_write_array32(count, &w2);

      char temp[4096];

      msgpack_reader mr;
      msgpack_reader_init(scratch, w.front_ - scratch, &mr);

      json_writer jw;
      json_writer_init(temp, sizeof(temp), &jw);

      msgpack_dump_json(&mr, &jw);

      puts(json_writer_str(&jw, NULL));

      break;
    }
    case PARC_CLI_COMMAND_PARSE: {
      const char* fn = parc_cli_get(&cli, PARC_CLI_COMMAND_PARSE)->value_;
      file f;
      if (!file_open(fn, &f)) {
        return 1;
      }

      parc_parser parser;
      parc_parser_init(f.data_, f.size_, &parser);
      if (!parc_parser_main(&parser)) {
        // syntax error
        return 1;
      }

      parc_syntax_tree* syntax = parc_parser_syntax(&parser);
      msgpack_writer writer;
      msgpack_writer_init(scratch, sizeof(scratch), &writer);
      parc_syntax_tree_dump(syntax, &writer);
      to_json(&writer);
      break;
    }
  }

  return 0;
}

static void to_json(msgpack_writer* writer) {
  char temp[1024 * 1024];
  json_writer json;
  json_writer_init(temp, sizeof(temp), &json);
  msgpack_reader reader;
  size_t size;
  const char* data = msgpack_writer_data(writer, &size);
  msgpack_reader_init(data, size, &reader);
  msgpack_dump_json(&reader, &json);
  puts(json_writer_str(&json, NULL));
}
