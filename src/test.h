#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define _TEST_CONCAT2(a, b) a##b
#define _TEST_CONCAT(a, b) _TEST_CONCAT2(a, b)

#define _ASSERT_EQUAL(type, expected, actual)                          \
  do {                                                                 \
    test_assert_metadata m = {__FILE__, __LINE__, #expected, #actual}; \
    if (!_TEST_CONCAT(test_assert_eq_, type)(expected, actual, &m)) {  \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define ASSERT_FALSE(expr) _ASSERT_EQUAL(int, !!(expr), 0)

#define ASSERT_TRUE(expr) _ASSERT_EQUAL(int, !!(expr), 1)

#define ASSERT_EQUAL_UINT8(expected, actual) \
  _ASSERT_EQUAL(uint8_t, expected, actual)

#define ASSERT_EQUAL_UINT16(expected, actual) \
  _ASSERT_EQUAL(uint16_t, expected, actual)

#define ASSERT_EQUAL_UINT32(expected, actual) \
  _ASSERT_EQUAL(uint32_t, expected, actual)

#define ASSERT_EQUAL_UINT64(expected, actual) \
  _ASSERT_EQUAL(uint64_t, expected, actual)

#define ASSERT_EQUAL_INT(expected, actual) _ASSERT_EQUAL(int, expected, actual)

#define ASSERT_EQUAL_STRLEN(expected, actual, length)                        \
  do {                                                                       \
    if (!(strlen(expected) == length &&                                      \
          strncmp(expected, actual, length) == 0)) {                         \
      test_report_failure();                                                 \
      char temp[64];                                                         \
      const char* fmt;                                                       \
      fmt = length < 50 ? "%.*s" : "%.*s...";                                \
      sprintf(temp, fmt, length < 50 ? length : 50, actual);                 \
      fmt = "%s(%i): %s == %s; <%s> != <%s>\n";                              \
      fprintf(stderr, fmt, __FILE__, __LINE__, #expected, #actual, expected, \
              temp);                                                         \
      return 1;                                                              \
    }                                                                        \
  } while (0)

#define TEST_NEW(test_name)                                           \
  static test_case _TEST_CONCAT(test_, __LINE__) = {test_name, 0, 0}; \
  test_register(&_TEST_CONCAT(test_, __LINE__));                      \
  if (test_eval(&_TEST_CONCAT(test_, __LINE__)))

typedef enum test_status { TEST_STATUS_OK, TEST_STATUS_FAIL } test_status;

typedef struct test_case test_case;
struct test_case {
  const char* name_;
  test_case* next_;
  test_status status_;
};

void test_initialize(int argc, char* argv[]);

void test_register(test_case* test);

int test_eval(test_case* test);

void test_report_failure();

typedef struct test_assert_metadata {
  const char* file_;
  int line_;
  const char* expected_;
  const char* actual_;
} test_assert_metadata;

int test_assert_eq_uint8_t(uint8_t expected, uint8_t actual,
                           const test_assert_metadata* m);
int test_assert_eq_uint16_t(uint16_t expected, uint16_t actual,
                            const test_assert_metadata* m);
int test_assert_eq_uint32_t(uint32_t expected, uint32_t actual,
                            const test_assert_metadata* m);
int test_assert_eq_uint64_t(uint64_t expected, uint64_t actual,
                            const test_assert_metadata* m);

int test_assert_eq_int(int expected, int actual, const test_assert_metadata* m);
