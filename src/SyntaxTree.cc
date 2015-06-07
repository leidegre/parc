
#include "SyntaxTree.h"

namespace parc {
void SyntaxNode::DebugString(std::string* s, size_t indent) {
  assert(s);
  s->append(1, '(');
  s->append(label_.GetData(), label_.GetSize());
  if ((tree_.size() > 1) & (indent != (size_t)-1)) {
    for (const auto& node : tree_) {
      s->append(1, '\n');
      s->append(indent + 1, ' ');
      node->DebugString(s, indent + 1);
    }
    s->append(1, '\n');
    s->append(indent, ' ');
  } else {
    for (const auto& node : tree_) {
      s->append(1, ' ');
      node->DebugString(s, indent);
    }
  }
  s->append(1, ')');
}

void TokenNode::DebugString(std::string* s, size_t indent) {
  assert(s);
  const auto& value = token_.GetString();
  s->append(value.GetData(), value.GetSize());
}
}
