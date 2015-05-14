
#include "SyntaxTree.h"

namespace parc {
void SyntaxTree::DebugString(std::string* s, size_t indent) {
  assert(s);
  if (tree_.size() > 1) {
    s->append(1, '(');
    s->append(label_);
    for (const auto& node : tree_) {
      s->append(1, '\n');
      s->append(indent + 1, ' ');
      node->DebugString(s, indent + 1);
    }
    s->append(1, '\n');
    s->append(indent, ' ');
    s->append(1, ')');
  } else {
    s->append(1, '(');
    s->append(label_);
    for (const auto& node : tree_) {
      s->append(1, ' ');
      node->DebugString(s, indent);
    }
    s->append(1, ')');
  }
}

void TokenNode::DebugString(std::string* s, size_t indent) {
  assert(s);
  const auto& value = token_.GetString();
  s->append(value.GetData(), value.GetSize());
}
}
