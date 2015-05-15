
#include "tests/TestHarness.h"

#include <iostream>

int main(int argc, char* argv[]) {
  int flags = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      flags |= TestContext::kVerbose;
    }
  }
  if (!TestManager::GetInstance()->Run(flags)) {
    return 1;
  }
  return 0;
}
