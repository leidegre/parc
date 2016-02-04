
#include "test.h"

// note: TEST_ASSERT_* macros return exit codes

static int test_assert_true_test() {
  int true_ = 1;
  ASSERT_TRUE(true_);
  ASSERT_FALSE(!true_);
  int x;
  ASSERT_TRUE(&x);
  return 0;
}

static int test_assert_false_test() {
  int false_ = 0;
  ASSERT_FALSE(false_);
  ASSERT_TRUE(!false_);
  int* x = NULL;
  ASSERT_FALSE(x);
  ASSERT_TRUE(!x);
  return 0;
}

static int test_assert_str_eq_test() {
  ASSERT_EQUAL_STRLEN("", "", 0);
  ASSERT_EQUAL_STRLEN("a", "a", 1);
  ASSERT_EQUAL_STRLEN("ab", "ab", 2);
  ASSERT_EQUAL_STRLEN("abc", "abc", 3);
  return 0;
}

// static int test_assert_str_ne_test() {
//   ASSERT_EQUAL_STRLEN("a", "", 0);
//   return 0;
// }
//
// static int test_assert_str_ne_test2() {
//   ASSERT_EQUAL_STRLEN("", "a", 1);
//   return 0;
// }
//
// static int test_assert_str_ne_test3() {
//   ASSERT_EQUAL_STRLEN("ab", "a", 1);
//   return 0;
// }
//
// static int test_assert_str_ne_test4() {
//   ASSERT_EQUAL_STRLEN("ab", "ac", 2);
//   return 0;
// }

#define TEST_PASS(test_func)     \
  TEST_NEW(#test_func) {         \
    int exit_code = test_func(); \
    if (exit_code)               \
      return exit_code;          \
  }

#define TEST_FAIL(test_func)     \
  TEST_NEW(#test_func) {         \
    int exit_code = test_func(); \
    if (!exit_code)              \
      return 1;                  \
  }

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_PASS(test_assert_true_test);
  TEST_PASS(test_assert_false_test);
  TEST_PASS(test_assert_str_eq_test);
  // TEST_FAIL(test_assert_str_ne_test);
  // TEST_FAIL(test_assert_str_ne_test2);
  // TEST_FAIL(test_assert_str_ne_test3);
  // TEST_FAIL(test_assert_str_ne_test4);

  return 0;
}
