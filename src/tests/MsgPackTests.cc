
#include "TestHarness.h"

#include "..\MsgPack.h"

BEGIN_TEST_CASE("MsgPack_Test") {
  using namespace parc;
  std::string buf;
  for (int i = -256; i <= 256; i++) {
    msgpack::WriteInteger(i, &buf);
  }
  msgpack::Reader reader(buf);
  for (int i = -256; i <= 256; i++) {
    msgpack::Value v;
    reader.Read(&v);
    ASSERT_EQ(i, v.int32_);
  }
}
END_TEST_CASE
