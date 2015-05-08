
#include "tests/TestHarness.h"

#include <iostream>

int main(int argc, char* argv[]) {
  TestManager::GetInstance()->Run();
  return 0;
}