
#include "test.h"

static int _c99_va_args_func1(int x) {
  return x;
}
static int _c99_va_args_func2(int x, int y) {
  return y;
}
static int _c99_va_args_func3(int x, int y, int z) {
  return z;
}
#define _c99_va_args_func(a, b, c, fname, ...) fname
#define c99_va_args_func(...)                                            \
  _c99_va_args_func(__VA_ARGS__, _c99_va_args_func3, _c99_va_args_func2, \
                    _c99_va_args_func1, __dummy__)(__VA_ARGS__)

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  // This is a test to ensure that the way we do function overloading depending
  // on the number of arguments passed works with whatever compiler is used.
  // Note that this method always requires at least 1 argument.
  TEST_NEW("c99_va_args_func_test") {
    ASSERT_EQUAL_INT(1, c99_va_args_func(1));
    ASSERT_EQUAL_INT(3, c99_va_args_func(2, 3));
    ASSERT_EQUAL_INT(6, c99_va_args_func(4, 5, 6));
  }
}
