
#include "api.h"
#include "msgpack.h"

#include <assert.h>

static size_t get_child_node_count(parc_syntax_tree *tree);

int parc_syntax_tree_dump(parc_syntax_tree *tree, msgpack_writer *writer) {
  switch (tree->type_) {
    case PARC_SYNTAX_TREE_TYPE_NODE: {
      parc_syntax_node *node = (parc_syntax_node *)tree;

      msgpack_write_map(3, writer);

      msgpack_write_uint(1, writer);  // 1: type
      msgpack_write_uint(PARC_SYNTAX_TREE_TYPE_NODE, writer);

      msgpack_write_uint(2, writer);  // 2: label
      msgpack_write_str(node->label_, strlen(node->label_), writer);

      msgpack_write_uint(3, writer);  // 3: child_nodes
      size_t count = get_child_node_count(tree);
      msgpack_write_array(count, writer);

      parc_syntax_tree *next = node->first_child_;
      while (next) {
        if (!parc_syntax_tree_dump(next, writer)) {
          return 0;
        }
        next = next->sibling_;
      }
      break;
    }
    case PARC_SYNTAX_TREE_TYPE_TOKEN: {
      parc_syntax_token *node = (parc_syntax_token *)tree;

      msgpack_write_map(2, writer);

      msgpack_write_uint(1, writer);  // 1: type
      msgpack_write_uint(PARC_SYNTAX_TREE_TYPE_TOKEN, writer);

      msgpack_write_uint(2, writer);  // 2: token
      parc_token_store(&node->token_, writer);
      break;
    }
    default: {
      assert(0 && "not supported");
      return 0;
    }
  }
  return 1;
}

static size_t get_child_node_count(parc_syntax_tree *tree) {
  size_t count = 0;
  parc_syntax_node *node = (parc_syntax_node *)tree;
  parc_syntax_tree *next = node->first_child_;
  while (next) {
    count++;
    next = next->sibling_;
  }
  return count;
}
