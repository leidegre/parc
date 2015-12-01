
#include "api.h"

#include <assert.h>

#include "util.h"

static int parc_syntax_tree_debug2(parc_syntax_tree *tree, int indent,
                                   string_builder *sb) {
  switch (tree->type_) {
    case PARC_SYNTAX_TREE_TYPE_NODE: {
      parc_syntax_node *node = (parc_syntax_node *)tree;
      if (!string_builder_append(sb, '(')) {
        return 0;
      }
      if (!string_builder_append_str(sb, node->label_)) {
        return 0;
      }
      parc_syntax_tree *next = node->first_child_;
      while (next) {
        if (!string_builder_append(sb, ' ')) {
          return 0;
        }
        if (next->type_ == PARC_SYNTAX_TREE_TYPE_NODE) {
          if (!string_builder_append(sb, '\n')) {
            return 0;
          }
          if (!string_builder_append_c(sb, ' ', indent + 1)) {
            return 0;
          }
        }
        if (!parc_syntax_tree_debug2(next, indent + 1, sb)) {
          return 0;
        }
        next = next->sibling_;
      }
      if (!string_builder_append(sb, ')')) {
        return 0;
      }
      break;
    }
    case PARC_SYNTAX_TREE_TYPE_TOKEN: {
      parc_syntax_token *node = (parc_syntax_token *)tree;
      if (!string_builder_append_strlen(sb, node->token_.s_.data_,
                                        node->token_.s_.size_)) {
        return 0;
      }
      break;
    }
  }
  return 1;
}

int parc_syntax_tree_debug(parc_syntax_tree *tree, char *buffer,
                           size_t buffer_size) {
  string_builder sb;
  string_builder_init(buffer, buffer_size, &sb);
  return parc_syntax_tree_debug2(tree, 0, &sb);
}
