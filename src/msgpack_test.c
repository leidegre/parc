
#include "msgpack.h"

#include "test.h"

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("msgpack_write_uint8_test") {
    char temp[9];
    msgpack_writer writer;
    msgpack_writer_initialize(temp, sizeof(temp), &writer);
    msgpack_write_uint8(1, &writer);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT8, temp[0]);
    ASSERT_EQUAL_UINT8(1, temp[1]);
    ASSERT_TRUE(msgpack_writer_ensure(&writer, 7));
  }

  TEST_NEW("msgpack_write_uint16_test") {
    char temp[9];
    msgpack_writer writer;
    msgpack_writer_initialize(temp, sizeof(temp), &writer);
    msgpack_write_uint16(258, &writer);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT16, temp[0]);
    ASSERT_EQUAL_UINT8(1, temp[1]);  // msb
    ASSERT_EQUAL_UINT8(2, temp[2]);
    ASSERT_TRUE(msgpack_writer_ensure(&writer, 6));
  }

  TEST_NEW("msgpack_write_uint32_test") {
    char temp[9];
    msgpack_writer writer;
    msgpack_writer_initialize(temp, sizeof(temp), &writer);
    msgpack_write_uint32(16909060, &writer);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT32, temp[0]);
    ASSERT_EQUAL_UINT8(1, temp[1]);  // msb
    ASSERT_EQUAL_UINT8(2, temp[2]);
    ASSERT_EQUAL_UINT8(3, temp[3]);
    ASSERT_EQUAL_UINT8(4, temp[4]);
    ASSERT_TRUE(msgpack_writer_ensure(&writer, 4));
  }

  TEST_NEW("msgpack_write_uint64_test") {
    char temp[9];
    msgpack_writer writer;
    msgpack_writer_initialize(temp, sizeof(temp), &writer);
    msgpack_write_uint64(72623859790382856, &writer);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT64, temp[0]);
    ASSERT_EQUAL_UINT8(1, temp[1]);  // msb
    ASSERT_EQUAL_UINT8(2, temp[2]);
    ASSERT_EQUAL_UINT8(3, temp[3]);
    ASSERT_EQUAL_UINT8(4, temp[4]);
    ASSERT_EQUAL_UINT8(5, temp[5]);
    ASSERT_EQUAL_UINT8(6, temp[6]);
    ASSERT_EQUAL_UINT8(7, temp[7]);
    ASSERT_EQUAL_UINT8(8, temp[8]);
    ASSERT_TRUE(msgpack_writer_ensure(&writer, 0));
  }

  TEST_NEW("msgpack_read_uint8_test") {
    char temp[9] = {MSGPACK_TYPE_UINT8, 1, 0, 0, 0, 0, 0, 0, 0};
    msgpack_reader reader;
    msgpack_reader_initialize(temp, sizeof(temp), &reader);
    msgpack_value v;
    msgpack_read_value(&reader, &v);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT8, v.type_);
    ASSERT_EQUAL_UINT8(1, v.num_.uint8_);
    ASSERT_TRUE(msgpack_reader_ensure(&reader, 7));
  }

  TEST_NEW("msgpack_read_uint16_test") {
    char temp[9] = {MSGPACK_TYPE_UINT16, 1, 2, 0, 0, 0, 0, 0, 0};
    msgpack_reader reader;
    msgpack_reader_initialize(temp, sizeof(temp), &reader);
    msgpack_value v;
    msgpack_read_value(&reader, &v);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT16, v.type_);
    ASSERT_EQUAL_UINT16(258, v.num_.uint16_);
    ASSERT_TRUE(msgpack_reader_ensure(&reader, 6));
  }

  TEST_NEW("msgpack_read_uint32_test") {
    char temp[9] = {MSGPACK_TYPE_UINT32, 1, 2, 3, 4, 0, 0, 0, 0};
    msgpack_reader reader;
    msgpack_reader_initialize(temp, sizeof(temp), &reader);
    msgpack_value v;
    msgpack_read_value(&reader, &v);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT32, v.type_);
    ASSERT_EQUAL_UINT32(16909060, v.num_.uint32_);
    ASSERT_TRUE(msgpack_reader_ensure(&reader, 4));
  }

  TEST_NEW("msgpack_read_uint64_test") {
    char temp[9] = {MSGPACK_TYPE_UINT64, 1, 2, 3, 4, 5, 6, 7, 8};
    msgpack_reader reader;
    msgpack_reader_initialize(temp, sizeof(temp), &reader);
    msgpack_value v;
    msgpack_read_value(&reader, &v);
    ASSERT_EQUAL_UINT8(MSGPACK_TYPE_UINT64, v.type_);
    ASSERT_EQUAL_UINT64(72623859790382856, v.num_.uint64_);
    ASSERT_TRUE(msgpack_reader_ensure(&reader, 0));
  }

  TEST_NEW("msgpack_fixint_range_test") {
    char temp[256];
    msgpack_writer writer;
    msgpack_writer_initialize(temp, sizeof(temp), &writer);
    for (int i = msgpack_fixint_min; i <= msgpack_fixint_max; i++) {
      msgpack_write_fixint(i, &writer);
    }
    msgpack_reader reader;
    msgpack_reader_initialize(temp, sizeof(temp), &reader);
    for (int i = msgpack_fixint_min; i <= msgpack_fixint_max; i++) {
      msgpack_value v;
      msgpack_read_value(&reader, &v);
      if (i < 0) {
        ASSERT_EQUAL_UINT8(MSGPACK_TYPE_FIXINT_NEG, v.type_);
      } else {
        ASSERT_EQUAL_UINT8(MSGPACK_TYPE_FIXINT_POS, v.type_);
      }
      ASSERT_EQUAL_INT(i, v.num_.int8_);
    }
  }

  return 0;
}
