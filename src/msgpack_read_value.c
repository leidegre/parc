
#include "msgpack.h"

#include <string.h>
#include <assert.h>

int msgpack_read_value(msgpack_reader* reader, msgpack_value* value) {
  memset(value, 0, sizeof(*value));  // clear
  if (!msgpack_reader_ensure(reader, 1)) {
    return 0;
  }
  uint8_t b;
  msgpack_read_raw_uint8(reader, &b);
  switch (b) {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x5B:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x5F:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
    case 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x6F:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F: {
      value->type_ = MSGPACK_TYPE_FIXINT_POS;
      value->num_.uint8_ = b;
      break;
    }
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F: {
      value->type_ = MSGPACK_TYPE_FIXMAP;
      value->num_.uint8_ = b & 0xf;
      break;
    }
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F: {
      value->type_ = MSGPACK_TYPE_FIXARRAY;
      value->num_.uint8_ = b & 0xf;
      break;
    }
    case 0xA0:
    case 0xA1:
    case 0xA2:
    case 0xA3:
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
    case 0xAC:
    case 0xAD:
    case 0xAE:
    case 0xAF:
    case 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
    case 0xB4:
    case 0xB5:
    case 0xB6:
    case 0xB7:
    case 0xB8:
    case 0xB9:
    case 0xBA:
    case 0xBB:
    case 0xBC:
    case 0xBD:
    case 0xBE:
    case 0xBF: {
      value->type_ = MSGPACK_TYPE_FIXSTR;
      value->num_.uint8_ = b & 0x1f;
      if (!msgpack_reader_ensure(reader, value->num_.uint8_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint8_, &value->s_);
      break;
    }
    case 0xC0: {
      value->type_ = MSGPACK_TYPE_NIL;
      break;
    }
    case 0xC1: {
      assert(0 && "msgpack: (never used)");
      break;
    }
    case 0xC2: {
      value->type_ = MSGPACK_TYPE_FALSE;
      value->num_.int32_ = 0;
      break;
    }
    case 0xC3: {
      value->type_ = MSGPACK_TYPE_TRUE;
      value->num_.int32_ = 1;
      break;
    }
    case 0xC4: {
      value->type_ = MSGPACK_TYPE_BIN8;
      if (!msgpack_reader_ensure(reader, 1)) {
        return 0;
      }
      msgpack_read_raw_uint8(reader, &value->num_.uint8_);
      if (!msgpack_reader_ensure(reader, value->num_.uint8_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint8_, &value->s_);
      break;
    }
    case 0xC5: {
      value->type_ = MSGPACK_TYPE_BIN16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      if (!msgpack_reader_ensure(reader, value->num_.uint16_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint16_, &value->s_);
      break;
    }
    case 0xC6: {
      value->type_ = MSGPACK_TYPE_BIN32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      if (!msgpack_reader_ensure(reader, value->num_.uint32_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint32_, &value->s_);
      break;
    }
    case 0xC7: {
      value->type_ = MSGPACK_TYPE_EXT8;
      assert(0 && "msgpack: (ext 8)");
      break;
    }
    case 0xC8: {
      value->type_ = MSGPACK_TYPE_EXT16;
      assert(0 && "msgpack: (ext 16)");
      break;
    }
    case 0xC9: {
      value->type_ = MSGPACK_TYPE_EXT32;
      assert(0 && "msgpack: (ext 32)");
      break;
    }
    case 0xCA: {
      value->type_ = MSGPACK_TYPE_FLOAT32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_float32(reader, &value->num_.float32_);
      break;
    }
    case 0xCB: {
      value->type_ = MSGPACK_TYPE_FLOAT64;
      if (!msgpack_reader_ensure(reader, 8)) {
        return 0;
      }
      msgpack_read_raw_float64(reader, &value->num_.float64_);
      break;
    }
    case 0xCC: {
      value->type_ = MSGPACK_TYPE_UINT8;
      if (!msgpack_reader_ensure(reader, 1)) {
        return 0;
      }
      msgpack_read_raw_uint8(reader, &value->num_.uint8_);
      break;
    }
    case 0xCD: {
      value->type_ = MSGPACK_TYPE_UINT16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      break;
    }
    case 0xCE: {
      value->type_ = MSGPACK_TYPE_UINT32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      break;
    }
    case 0xCF: {
      value->type_ = MSGPACK_TYPE_UINT64;
      if (!msgpack_reader_ensure(reader, 8)) {
        return 0;
      }
      msgpack_read_raw_uint64(reader, &value->num_.uint64_);
      break;
    }
    case 0xD0: {
      value->type_ = MSGPACK_TYPE_INT8;
      if (!msgpack_reader_ensure(reader, 1)) {
        return 0;
      }
      msgpack_read_raw_uint8(reader, &value->num_.uint8_);
      break;
    }
    case 0xD1: {
      value->type_ = MSGPACK_TYPE_INT16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      break;
    }
    case 0xD2: {
      value->type_ = MSGPACK_TYPE_INT32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      break;
    }
    case 0xD3: {
      value->type_ = MSGPACK_TYPE_INT64;
      if (!msgpack_reader_ensure(reader, 8)) {
        return 0;
      }
      msgpack_read_raw_uint64(reader, &value->num_.uint64_);
      break;
    }
    case 0xD4: {
      value->type_ = MSGPACK_TYPE_FIXEXT1;
      assert(0 && "msgpack: (fixext 1)");
      break;
    }
    case 0xD5: {
      value->type_ = MSGPACK_TYPE_FIXEXT2;
      assert(0 && "msgpack: (fixext 2)");
      break;
    }
    case 0xD6: {
      value->type_ = MSGPACK_TYPE_FIXEXT4;
      assert(0 && "msgpack: (fixext 4)");
      break;
    }
    case 0xD7: {
      value->type_ = MSGPACK_TYPE_FIXEXT8;
      assert(0 && "msgpack: (fixext 8)");
      break;
    }
    case 0xD8: {
      value->type_ = MSGPACK_TYPE_FIXEXT16;
      assert(0 && "msgpack: (fixext 16)");
      break;
    }
    case 0xD9: {
      value->type_ = MSGPACK_TYPE_STR8;
      if (!msgpack_reader_ensure(reader, 1)) {
        return 0;
      }
      msgpack_read_raw_uint8(reader, &value->num_.uint8_);
      if (!msgpack_reader_ensure(reader, value->num_.uint8_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint8_, &value->s_);
      break;
    }
    case 0xDA: {
      value->type_ = MSGPACK_TYPE_STR16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      if (!msgpack_reader_ensure(reader, value->num_.uint16_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint16_, &value->s_);
      break;
    }
    case 0xDB: {
      value->type_ = MSGPACK_TYPE_STR32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      if (!msgpack_reader_ensure(reader, value->num_.uint32_)) {
        return 0;
      }
      msgpack_read_raw_str(reader, value->num_.uint32_, &value->s_);
      break;
    }
    case 0xDC: {
      value->type_ = MSGPACK_TYPE_ARRAY16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      break;
    }
    case 0xDD: {
      value->type_ = MSGPACK_TYPE_ARRAY32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      break;
    }
    case 0xDE: {
      value->type_ = MSGPACK_TYPE_MAP16;
      if (!msgpack_reader_ensure(reader, 2)) {
        return 0;
      }
      msgpack_read_raw_uint16(reader, &value->num_.uint16_);
      break;
    }
    case 0xDF: {
      value->type_ = MSGPACK_TYPE_MAP32;
      if (!msgpack_reader_ensure(reader, 4)) {
        return 0;
      }
      msgpack_read_raw_uint32(reader, &value->num_.uint32_);
      break;
    }
    case 0xE0:
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xE5:
    case 0xE6:
    case 0xE7:
    case 0xE8:
    case 0xE9:
    case 0xEA:
    case 0xEB:
    case 0xEC:
    case 0xED:
    case 0xEE:
    case 0xEF:
    case 0xF0:
    case 0xF1:
    case 0xF2:
    case 0xF3:
    case 0xF4:
    case 0xF5:
    case 0xF6:
    case 0xF7:
    case 0xF8:
    case 0xF9:
    case 0xFA:
    case 0xFB:
    case 0xFC:
    case 0xFD:
    case 0xFE:
    case 0xFF: {
      value->type_ = MSGPACK_TYPE_FIXINT_NEG;
      value->num_.int8_ = (int8_t)b;
      break;
    }
  }
  return 1;
}
