#pragma once

#include <string>
#include <vector>

class TestCase;

class TestManager {
 public:
  static TestManager* GetInstance();

  void Register(TestCase* test_case);

  bool Run();

 private:
  TestCase* test_case_;

  TestManager() : test_case_(nullptr) {}
  TestManager(const TestManager&) = delete;
  TestManager& operator=(const TestManager&) = delete;
};

struct TestContext {
  std::string output_;
  char message_[2048];
};

class TestCase {
 public:
  const char* name_;
  TestCase* next_;

  TestCase(const char* name) : name_(name), next_(nullptr){};

  virtual bool Run(TestContext* test_context_) = 0;
};

#define ASSERT_TRUE(expr)                                                    \
  if (!(expr)) {                                                             \
    sprintf(test_context_->message_, "Assertion '%s' failed in file %s:%i.", \
            #expr, __FILE__, __LINE__);                                      \
    return false;                                                            \
  }

#define _TEST_CONCAT(_Value1, _Value2) _Value1##_Value2
#define TEST_CONCAT(_Value1, _Value2) _TEST_CONCAT(_Value1, _Value2)

#define BEGIN_TEST_CASE(name)                                           \
  \
namespace {                                                             \
    static class TEST_CONCAT(TestCase, __LINE__) : protected TestCase { \
     public:                                                            \
      TEST_CONCAT(TestCase, __LINE__)() : TestCase(name) {              \
        TestManager::GetInstance()->Register(this);                     \
      }                                                                 \
                                                                        \
      virtual bool Run(TestContext* test_context_) override {
#define END_TEST_CASE                \
  return true;                       \
  }                                  \
  }                                  \
  TEST_CONCAT(test_case_, __LINE__); \
  \
}

#define TEST_OUTPUT(s) test_context_->output_.append(s)
