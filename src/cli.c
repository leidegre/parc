
#include "cli.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct option {
  int scope_;
  int key_;
  const char* name_;
  const char* desc_;
  int arg_count_;
  const char* arg_name_;
  int flags_;
} option;

static option S_OPTION_ARRAY[] = {
    {-1, PARC_CLI_GLOBAL_GRAMMAR_FILE, "-g",
     "Specify grammar file. If no grammar file has been specified particle "
     "defaults to the particle grammar.",
     1, "grammar-file", 0},
    {0, PARC_CLI_COMMAND_LEX, "lex", NULL, 1, "text-file", 0},
    {0, PARC_CLI_COMMAND_PARSE, "parse", NULL, 1, "token-stream", 0},
    {0, PARC_CLI_COMMAND_BIND, "bind", NULL, 1, "syntax-tree", 0},
    {0, PARC_CLI_COMMAND_EMIT, "emit", NULL, 1, "translation-unit", 0},
    {0, PARC_CLI_COMMAND_RUN, "run", NULL, 1, "program", 0},
    {0}};

typedef struct reader {
  int argc_;
  char** argv_;
  int index_;
  parc_cli* results_;
} reader;

static int reader_is_valid(reader* cv) {
  return cv->index_ < cv->argc_;
}

static const char* reader_current(reader* cv) {
  return cv->argv_[cv->index_];
}

static option* reader_find(reader* cv, const int scope) {
  const char* name = reader_current(cv);
  // find first option in scope
  option* opt = &S_OPTION_ARRAY[0];
  while (opt->key_) {
    if (scope == opt->scope_) {
      if (strcmp(name, opt->name_) == 0) {
        return opt;
      }
    }
    opt++;
  }
  return NULL;
}

static void reader_read(reader* cv, option* opt) {
  cv->results_->args_[opt->key_].value_ = cv->argv_[cv->index_++];
}

static int reader_read_args_check(reader* cv, option* opt) {
  int r = cv->argc_ - cv->index_;
  if (!(opt->arg_count_ <= r)) {
    fprintf(stderr, "Option '%s' missing %i required argument(s).\n",
            opt->name_, opt->arg_count_ - r);
    return 0;
  }
  return 1;
}

static int reader_read_args(reader* cv, option* opt) {
  if (!reader_read_args_check(cv, opt)) {
    return 0;
  }
  parc_cli_arg* prev = &cv->results_->args_[opt->key_];
  for (int j = 0; j < opt->arg_count_; j++) {
    parc_cli_arg* arg = malloc(sizeof(parc_cli_arg));
    arg->value_ = cv->argv_[cv->index_ + j];
    prev->next_ = arg;
    arg->next_ = NULL;
    prev = arg;
  }
  cv->index_ += opt->arg_count_;
  return 1;
}

static void print_help();

int parc_cli_parse(parc_cli* cli, int argc, char* argv[]) {
  memset(cli, 0, sizeof(*cli));

  reader cv;
  cv.argc_ = argc;
  cv.argv_ = argv;
  cv.index_ = 1;
  cv.results_ = cli;

  // Built-in help
  if (reader_is_valid(&cv)) {
    if (strcmp("-h", reader_current(&cv)) == 0) {
      print_help();
      return 0;
    }
  }

  // Global options
  while (reader_is_valid(&cv)) {
    option* opt = reader_find(&cv, -1);
    if (opt) {
      reader_read(&cv, opt);
      if (!reader_read_args(&cv, opt)) {
        return 0;
      }
    } else {
      break;
    }
  }

  // Command
  if (reader_is_valid(&cv)) {
    option* cmd = reader_find(&cv, 0);
    if (cmd) {
      reader_read(&cv, cmd);
      // Options
      while (reader_is_valid(&cv)) {
        option* opt = reader_find(&cv, cmd->key_);
        if (opt) {
          reader_read(&cv, opt);
          if (!reader_read_args(&cv, opt)) {
            return 0;
          }
        } else {
          break;
        }
      }
      // Arguments
      if (!reader_read_args(&cv, cmd)) {
        return 0;
      }
      cv.results_->command_ = cmd->key_;
    }
  }

  if (reader_is_valid(&cv)) {
    fprintf(stderr, "Unknown option '%s'. Use '-h' for help.\n",
            reader_current(&cv));
    return 0;
  }

  return 1;
}

parc_cli_arg* parc_cli_get(parc_cli* cli, int option) {
  if (cli->args_[option].value_) {
    return cli->args_[option].next_;
  }
  return NULL;
}

static void print_help_option(const char* indent, option* opt) {
  printf("%s%s", indent, opt->name_);
  for (int i = 0; i < opt->arg_count_; i++) {
    printf(" <%s>", opt->arg_name_);
  }
  printf("\n");
}

static void print_help() {
  printf("Command-line summary for particle:\n\n");

  printf("  particle [options ...] [<command> [options ...] [args ...]]\n\n");

  printf("Global options:\n");
  for (option* opt = &S_OPTION_ARRAY[0]; opt->key_; opt++) {
    if (opt->scope_ == -1) {
      print_help_option("  ", opt);
    }
  }
  printf("\n");

  printf("Commands:\n");
  for (option* cmd = &S_OPTION_ARRAY[0]; cmd->key_; cmd++) {
    if (cmd->scope_ == 0) {
      print_help_option("  ", cmd);
      for (option* opt = &S_OPTION_ARRAY[0]; opt->key_; opt++) {
        if (opt->scope_ == cmd->key_) {
          print_help_option("    ", opt);
        }
      }
    }
  }
  printf("\n");
}
