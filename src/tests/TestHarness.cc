
#include "TestHarness.h"

TestManager* TestManager::GetInstance() {
  static TestManager test_manager;
  return &test_manager;
}

void TestManager::Register(TestCase* test_case) {
  test_case->next_ = this->test_case_;
  this->test_case_ = test_case;
}

namespace {
bool RunTestInList(const TestCase* test_case,
                   const std::vector<const char*>& test_list) {
  if (test_list.empty()) {
    return true;
  }
  for (const char* test : test_list) {
    if (strcmp(test_case->name_, test) == 0) {
      return true;
    }
  }
  return false;
}
}

bool TestManager::Run(const std::vector<const char*>& test_list, int flags) {
  int tests_run = 0, tests_failed = 0;
  TestCase* test_case = test_case_;
  while (test_case) {
    if (RunTestInList(test_case, test_list)) {
      tests_run++;
      putchar('.');  // progress indicator
      TestContext test_context;
      test_context.flags_ = flags;
      if (!test_case->Run(&test_context)) {
        tests_failed++;
        puts("");  // new line
        printf("Test '%s' failed: %s", test_case->name_, test_context.message_);
        puts("");  // new line
      }
      if (!test_context.output_.empty()) {
        puts("");  // new line
        puts(test_context.output_.c_str());
      }
    }
    test_case = test_case->next_;
  }
  puts("");
  printf("%i %s run, %i %s failed\n", tests_run,
         tests_run != 1 ? "tests" : "test", tests_failed,
         tests_failed != 1 ? "tests" : "test");
  return tests_failed == 0;
}
