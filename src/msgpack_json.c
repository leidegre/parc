
#include "msgpack.h"
#include "json.h"

#include <stdlib.h> // strtod

static int msgpack_parse_json_value(json_parser* parser,
                                    msgpack_writer* writer);

static int msgpack_parse_json_string(json_parser* parser,
                                     msgpack_writer* writer) {
  json_token token = parser->token_;
  if (json_parser_accept(parser, JSON_TOKEN_STRING)) {
    // todo: string escaping
    msgpack_write_str(token.s_, token.length_, writer);
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_number(json_parser* parser,
                                     msgpack_writer* writer) {
  json_token token = parser->token_;
  if (json_parser_accept(parser, JSON_TOKEN_NUMBER)) {
    double v = strtod(token.s_, NULL);
    msgpack_write_float64(v, writer);
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_object(json_parser* parser,
                                     msgpack_writer* writer) {
  if (json_parser_accept(parser, JSON_TOKEN_OBJECT_BEGIN)) {
    msgpack_writer k = *writer;
    msgpack_write_map32(0, writer);  // reserve
    size_t count = 0;
    while ((count == 0) ||
           json_parser_accept(parser, JSON_TOKEN_LIST_SEPARATOR)) {
      if (!msgpack_parse_json_string(parser, writer)) {
        if (count == 0) {
          // empty object
          break;
        }
        return 0;
      }
      if (!json_parser_accept(parser, JSON_TOKEN_KV_SEPARATOR)) {
        return 0;
      }
      if (!msgpack_parse_json_value(parser, writer)) {
        return 0;
      }
      count++;
    }
    if (!json_parser_accept(parser, JSON_TOKEN_OBJECT_END)) {
      return 0;
    }
    msgpack_write_map32(count, &k);  // fix count
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_array(json_parser* parser,
                                    msgpack_writer* writer) {
  if (json_parser_accept(parser, JSON_TOKEN_ARRAY_BEGIN)) {
    msgpack_writer k = *writer;
    msgpack_write_array32(0, writer);  // reserve
    size_t count = 0;
    while ((count == 0) ||
           json_parser_accept(parser, JSON_TOKEN_LIST_SEPARATOR)) {
      if (!msgpack_parse_json_value(parser, writer)) {
        if (count == 0) {
          // empty array
          break;
        }
        return 0;
      }
      count++;
    }
    if (!json_parser_accept(parser, JSON_TOKEN_ARRAY_END)) {
      return 0;
    }
    msgpack_write_array32(count, &k);  // fix count
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_boolean(json_parser* parser,
                                      msgpack_writer* writer) {
  if (json_parser_accept(parser, JSON_TOKEN_TRUE)) {
    msgpack_write_bool(1, writer);
    return 1;
  }
  if (json_parser_accept(parser, JSON_TOKEN_FALSE)) {
    msgpack_write_bool(0, writer);
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_null(json_parser* parser,
                                   msgpack_writer* writer) {
  if (json_parser_accept(parser, JSON_TOKEN_NULL)) {
    msgpack_write_nil(writer);
    return 1;
  }
  return 0;
}

static int msgpack_parse_json_value(json_parser* parser,
                                    msgpack_writer* writer) {
  if (msgpack_parse_json_string(parser, writer)) {
    return 1;
  }
  if (msgpack_parse_json_number(parser, writer)) {
    return 1;
  }
  if (msgpack_parse_json_object(parser, writer)) {
    return 1;
  }
  if (msgpack_parse_json_array(parser, writer)) {
    return 1;
  }
  if (msgpack_parse_json_boolean(parser, writer)) {
    return 1;
  }
  if (msgpack_parse_json_null(parser, writer)) {
    return 1;
  }
  return 0;
}

int msgpack_parse_json(const char* json, msgpack_writer* writer) {
  json_parser parser;
  json_parser_initialize(json, &parser);
  return msgpack_parse_json_value(&parser, writer);
}