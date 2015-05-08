
#include "TestHarness.h"

#include "..\Coding.h"

using parc::Utf8Decoder;

BEGIN_TEST_CASE("Coding_Utf8DecoderEmptyStringTest") {
  Utf8Decoder decoder("");
  ASSERT_TRUE(!decoder.Load());  // Cannot load from empty string
}
END_TEST_CASE

BEGIN_TEST_CASE("Coding_Utf8DecoderSingleAsciiCharacterTest") {
  Utf8Decoder decoder("A");
  ASSERT_TRUE(decoder.Load());
  ASSERT_TRUE(decoder.Peek() == 'A');
  ASSERT_TRUE(!decoder.Read());
}
END_TEST_CASE

BEGIN_TEST_CASE("Coding_Utf8DecoderManyAsciiCharacterTest") {
  Utf8Decoder decoder("Abc");
  ASSERT_TRUE(decoder.Load());
  ASSERT_TRUE(decoder.Peek() == 'A');
  ASSERT_TRUE(decoder.Read() && decoder.Peek() == 'b');
  ASSERT_TRUE(decoder.Read() && decoder.Peek() == 'c');
  ASSERT_TRUE(!decoder.Read());
}
END_TEST_CASE

BEGIN_TEST_CASE("Coding_Utf8DecoderManyLoadTest") {
  // load does not advance the decoder and may be called many times
  Utf8Decoder decoder("ABC");
  ASSERT_TRUE(decoder.Load());
  ASSERT_TRUE(decoder.Peek() == 'A');
  ASSERT_TRUE(decoder.Load());
  ASSERT_TRUE(decoder.Peek() == 'A');
  ASSERT_TRUE(decoder.Load());
  ASSERT_TRUE(decoder.Peek() == 'A');
}
END_TEST_CASE

// todo: proper UTF-8 byte sequences åäöÅÄÖüÿìí
