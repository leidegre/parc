
#include "TestHarness.h"

TestManager* TestManager::GetInstance() {
  static TestManager test_manager;
  return &test_manager;
}

void TestManager::Register(TestCase* test_case) {
  test_case->next_ = this->test_case_;
  this->test_case_ = test_case;
}

void TestManager::Run() {
  TestCase* test_case = test_case_;
  while (test_case) {
    putchar('.');  // progress indicator
    TestContext test_context;
    if (!test_case->Run(&test_context)) {
      puts("");  // new line
      printf("Test '%s' failed: %s", test_case->name_, test_context.message_);
      puts("");  // new line
    }
    test_case = test_case->next_;
  }
  puts("");
}
