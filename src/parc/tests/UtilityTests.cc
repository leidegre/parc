
#include "TestHarness.h"

#include "../Utility.h"

BEGIN_TEST_CASE("Utility_HexDumpTest") {
  using namespace parc;
  const char* data = "0123456789ABCDEFabcdefghijklmnop";
  for (size_t i = 0; i <= 32; i++) {
    std::string hex;
    GetHexDump(Slice(data, i), &hex);
    TEST_OUTPUT(hex);
  }
}
END_TEST_CASE
