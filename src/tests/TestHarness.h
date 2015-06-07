#pragma once

#include <string>
#include <vector>

class TestCase;

class TestManager {
 public:
  static TestManager* GetInstance();

  void Register(TestCase* test_case);

  bool Run(const std::vector<const char*>& test_list, int flags = 0);

 private:
  TestCase* test_case_;

  TestManager() : test_case_(nullptr) {}
  TestManager(const TestManager&) = delete;
  TestManager& operator=(const TestManager&) = delete;
};

struct TestContext {
  enum {
    kNone = 0,
    kVerbose = (1 << 0),
  };
  int flags_;
  std::string output_;
  char message_[2048];
};

class TestCase {
 public:
  const char* name_;
  TestCase* next_;

  TestCase(const char* name) : name_(name), next_(nullptr){};

  virtual bool Run(TestContext* test_context_) = 0;

 protected:
  static bool AssertEquals(const char* expected, const std::string& actual) {
    return AssertEquals(expected, actual.c_str());
  }

  static bool AssertEquals(const char* expected, const char* actual) {
    return (strcmp(expected, actual) == 0);
  }

  template <class T, class U>
  static bool AssertEquals(T expected, U actual) {
    return expected == static_cast<T>(actual);
  }

  static const size_t kFormatValueBufferSize = 1024;

  static void FormatValue(const std::string& s, char* temp) {
    FormatValue(s.c_str(), temp);
  }

  static void FormatValue(const char* s, char* temp) {
    strncpy(temp, s, kFormatValueBufferSize);
    temp[kFormatValueBufferSize - 1] = 0;
  }

  static void FormatValue(int v, char* temp) { sprintf(temp, "%i", v); }

  static void FormatValue(unsigned int v, char* temp) {
    sprintf(temp, "%u", v);
  }

  static const char* GetFileName(const char* path) {
    if (auto fn = strrchr(path, '/')) {
      return fn + 1;
    }
    if (auto fn = strrchr(path, '\\')) {
      return fn + 1;
    }
    return path;
  }
};

#define ASSERT_TRUE(expr)                                                    \
  if (!(expr)) {                                                             \
    sprintf(test_context_->message_, "Assertion '%s' failed in file %s:%i.", \
            #expr, __FILE__, __LINE__);                                      \
    return false;                                                            \
  }

#define ASSERT_EQ(expected, actual)                                            \
  if (!AssertEquals(expected, actual)) {                                       \
    \
char v1[kFormatValueBufferSize];                                               \
    \
char v2[kFormatValueBufferSize];                                               \
    \
FormatValue(expected, v1);                                                     \
    \
FormatValue(actual, v2);                                                       \
    \
sprintf(test_context_->message_, "'%s' does not equal '%s' in file %s:%i", v1, \
        v2, GetFileName(__FILE__), __LINE__);                                  \
    \
return false;                                                                  \
  \
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

#define TEST_OUTPUT(s)                               \
  if (test_context_->flags_ & TestContext::kVerbose) \
  test_context_->output_.append(s)
