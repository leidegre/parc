#pragma once

typedef enum {
  PARC_CLI_NONE,
  // global options
  PARC_CLI_GLOBAL_VERBOSE,
  PARC_CLI_GLOBAL_GRAMMAR_FILE,
  // commands
  PARC_CLI_COMMAND_LEX,
  PARC_CLI_COMMAND_PARSE,
  PARC_CLI_COMMAND_BIND,
  PARC_CLI_COMMAND_EMIT,
  PARC_CLI_COMMAND_RUN,
  // ...
  PARC_CLI_OPTION_MAX
} parc_cli_option;

typedef struct parc_cli_arg parc_cli_arg;
struct parc_cli_arg {
  const char* value_;
  parc_cli_arg* next_;
};

typedef struct {
  parc_cli_arg args_[PARC_CLI_OPTION_MAX];
  int command_;
} parc_cli;

int parc_cli_parse(parc_cli* cli, int argc, char* argv[]);

parc_cli_arg* parc_cli_get(parc_cli* cli, int option);
