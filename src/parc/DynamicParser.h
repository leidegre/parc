#pragma once

#include <cassert>
#include "Slice.h"

namespace parc {
class DynamicParserByteCodeGenerator;  // this does not belong here

// represents a recursive decent parser control flow graph.
class DynamicParserNode {
 public:
  enum {
    kNone,
    kAccept,
    kApply,
    kSelect,
    kError,
    kReturn,
  };

 protected:
  DynamicParserNode(int type) : type_(type), next_(nullptr), label_() {}

 public:
  DynamicParserNode* GetNext() const { return next_; }
  void SetNext(DynamicParserNode* next) { next_ = next; }

  bool HasLabel() const { return !label_.IsEmpty(); }
  Slice GetLabel() const { return label_; }
  void SetLabel(const Slice& label) { label_ = label; };

  virtual void Emit(DynamicParserByteCodeGenerator* byte_code_generator) = 0;

 private:
  int type_;
  DynamicParserNode* next_;
  Slice label_;
};

// todo: How to deal with Expr ( "&&" | "||" ) Expr ?
//       or anything where the choice is complex (like more than one condition)
class DynamicParserAcceptNode
    : public DynamicParserNode {  // todo: rename IfAccept?
 public:
  explicit DynamicParserAcceptNode(int token = 0,
                                   DynamicParserNode* target = nullptr)
      : DynamicParserNode(kAccept), token_(token), target_(target) {}

  int GetToken() const { return token_; }
  void SetToken(int token) { token_ = token; }

  DynamicParserNode* GetTarget() const { return target_; }
  void SetTarget(DynamicParserNode* target) { target_ = target; }

  virtual void Emit(
      DynamicParserByteCodeGenerator* byte_code_generator) override;

 private:
  int token_;
  DynamicParserNode* target_;
};

class DynamicParserApplyNode : public DynamicParserNode {
 public:
  explicit DynamicParserApplyNode(DynamicParserNode* prod = nullptr)
      : DynamicParserNode(kApply), prod_(prod) {}

  DynamicParserNode* GetProduction() const { return prod_; }
  void SetProduction(DynamicParserNode* prod) { prod_ = prod; }

  virtual void Emit(
      DynamicParserByteCodeGenerator* byte_code_generator) override;

 private:
  DynamicParserNode* prod_;
};

class DynamicParserSelectNode : public DynamicParserNode {
 public:
  // NOTE: A projection where the label is null implies a pop count of 0.
  //       (It simply yields the syntax tree node at the top of the stack.)
  explicit DynamicParserSelectNode(const char* type_name = nullptr,
                                   int pop_count = 0)
      : DynamicParserNode(kSelect),
        type_name_(type_name),
        pop_count_(pop_count) {}

  Slice GetTypeName() const { return type_name_; }
  void SetTypeName(const Slice& type_name) { type_name_ = type_name; }

  int GetPopCount() const { return pop_count_; }
  void SetPopCount(int pop_count) { pop_count_ = pop_count; }

  virtual void Emit(
      DynamicParserByteCodeGenerator* byte_code_generator) override;

 private:
  Slice type_name_;
  int pop_count_;
};

class DynamicParserErrorNode : public DynamicParserNode {
 public:
  explicit DynamicParserErrorNode() : DynamicParserNode(kError) {}

  virtual void Emit(
      DynamicParserByteCodeGenerator* byte_code_generator) override;

 private:
};

class DynamicParserReturnNode : public DynamicParserNode {
 public:
  explicit DynamicParserReturnNode() : DynamicParserNode(kReturn) {}

  virtual void Emit(
      DynamicParserByteCodeGenerator* byte_code_generator) override;

 private:
};
}
