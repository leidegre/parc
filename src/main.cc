
#include "tests/TestHarness.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::vector<const char*> list;
  int flags = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      flags |= TestContext::kVerbose;
    } else {
      list.push_back(argv[i]);
    }
  }
  if (!TestManager::GetInstance()->Run(list, flags)) {
    return 1;
  }
  return 0;
}
