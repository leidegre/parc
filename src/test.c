
#include "test.h"

#include <stdlib.h>
#include <assert.h>

typedef struct test_context {
  int is_dry_run_;
  test_case* curr_;
  test_case* last_;
} test_context;

static test_context g_test_context;

static void test_at_exit(void) {
  puts("");  // new line

  int passed = 0, failed = 0;

  test_case* test = g_test_context.last_;
  while (test) {
    switch (test->status_) {
      case TEST_STATUS_OK: {
        passed++;  // assuming that we ran them
        break;
      }
      case TEST_STATUS_FAIL: {
        failed++;
        break;
      }
    }
    test = test->next_;
  }

  printf("%i %s passed, %i %s failed\n", passed, passed == 1 ? "test" : "tests",
         failed, failed == 1 ? "test" : "tests");
}

void test_initialize(int argc, char* argv[]) {
  atexit(&test_at_exit);
}

void test_register(test_case* test) {
  test->next_ = g_test_context.last_;
  g_test_context.last_ = test;
}

int test_eval(test_case* test) {
  g_test_context.curr_ = test;
  putchar('.');  // progress indicator
  fflush(stdout);
  return 1;
}

void test_report_failure() {
  assert(g_test_context.curr_);
  fprintf(stderr, "%s failed!\n", g_test_context.curr_->name_);
  g_test_context.curr_->status_ = TEST_STATUS_FAIL;  // test failed
  g_test_context.curr_ = NULL;
}

int test_assert_eq_str(const test_str* expected, const test_str* actual,
                       const test_assert_metadata* m) {
  if (!(expected->size_ == actual->size_ &&
        memcmp(expected->data_, actual->data_, expected->size_) == 0)) {
    test_report_failure();
    char temp[64];
    sprintf(temp, actual->size_ < 50 ? "<%.*s>" : "<%.*s...",
            actual->size_ < 50 ? (int)actual->size_ : 50, actual->data_);
    fprintf(stderr, "%s(%i): %s == %s; <%.*s> != %s\n", m->file_, m->line_,
            m->expected_, m->actual_, (int)expected->size_, expected->data_,
            temp);
    return 0;
  }
  return 1;
}

#define GEN_ASSERT_EQ(type, fmt)                                           \
  int _TEST_CONCAT(test_assert_eq_, type)(type expected, type actual,      \
                                          const test_assert_metadata* m) { \
    if (!(expected == actual)) {                                           \
      test_report_failure();                                               \
      const char* s = "%s(%i): %s == %s; " fmt " != " fmt "\n";            \
      fprintf(stderr, s, m->file_, m->line_, m->expected_, m->actual_,     \
              expected, actual);                                           \
      return 0;                                                            \
    }                                                                      \
    return 1;                                                              \
  }

GEN_ASSERT_EQ(int, "%i")
GEN_ASSERT_EQ(uint8_t, "%u")
GEN_ASSERT_EQ(uint16_t, "%u")
GEN_ASSERT_EQ(uint32_t, "%u")
GEN_ASSERT_EQ(uint64_t, "%ull")
