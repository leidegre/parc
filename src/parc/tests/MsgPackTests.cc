
#include "TestHarness.h"

#include "..\MsgPack.h"

BEGIN_TEST_CASE("MsgPack_Test") {
  using namespace parc;
  std::string buf;
  msgpack::WriteInteger(-129, &buf);
}
END_TEST_CASE
