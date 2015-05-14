
#include "tests/TestHarness.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (!TestManager::GetInstance()->Run()) {
    return 1;
  }
  return 0;
}
