
#include "json.h"

#include <stdio.h>
#include <string.h>

static void json_writer_indent(json_writer* writer);

static int json_writer_append(json_writer* writer, const char ch) {
  json_writer_indent(writer);
  char* f = writer->front_;
  if (f < writer->back_) {
    *f++ = ch;
    writer->front_ = f;
    return 1;
  }
  return 0;
}

static int json_writer_append_n(json_writer* writer, const char ch, int n) {
  json_writer_indent(writer);
  char* f = writer->front_;
  if (f + n <= writer->back_) {
    for (int i = 0; i < n; i++) {
      *f++ = ch;
    }
    writer->front_ = f;
    return 1;
  }
  return 0;
}

static int json_writer_append_str(json_writer* writer, const char* str,
                                  size_t n) {
  json_writer_indent(writer);
  char* f = writer->front_;
  if (f + n <= writer->back_) {
    memcpy(f, str, n);
    writer->front_ = f + n;
    return 1;
  }
  return 0;
}

static void json_writer_indent(json_writer* writer) {
  // no error detection in this function
  if (writer->is_newline_) {
    writer->is_newline_ = 0;  // clear flag first otherwise inf. recursion!
    json_writer_append_n(writer, ' ', writer->indent_);
  }
}

static int json_writer_begin(json_writer* writer, const char bracket,
                             int indent) {
  if (!json_writer_append(writer, bracket)) {
    return 0;
  }
  if (!json_write_newline(writer)) {
    return 0;
  }
  writer->indent_ += indent;
  return 1;
}

static int json_writer_end(json_writer* writer, const char bracket,
                           int indent) {
  writer->indent_ -= indent;
  if (!json_write_newline(writer)) {
    return 0;
  }
  if (!json_writer_append(writer, bracket)) {
    return 0;
  }
  return 1;
}

////

void json_writer_init(char* buffer, size_t buffer_size, json_writer* writer) {
  memset(writer, 0, sizeof(*writer));
  writer->s_ = buffer;
  writer->front_ = buffer;
  writer->back_ = buffer + buffer_size;
}

const char* json_writer_str(json_writer* writer, size_t* length) {
  char* f = writer->front_;
  if (f < writer->back_) {
    *f = '\0';  // ensure null terminated
    if (length) {
      *length = f - writer->s_;
    }
    return writer->s_;
  }
  return NULL;
}

////

int json_write_newline(json_writer* writer) {
  if (!json_writer_append(writer, '\n')) {
    return 0;
  }
  writer->is_newline_ = 1;
  return 1;
}

int json_write_string(const char* str, size_t len, json_writer* writer) {
  if (!json_writer_append(writer, '"')) {
    return 0;
  }
  const char* end = str + len;
  while (str < end) {
    const char ch = *str++;
    switch (ch) {
      case '"': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, '"');
        break;
      }
      case '\\': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, '\\');
        break;
      }
      case '/': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, '/');
        break;
      }
      case '\b': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, 'b');
        break;
      }
      case '\f': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, 'f');
        break;
      }
      case '\r': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, 'r');
        break;
      }
      case '\n': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, 'n');
        break;
      }
      case '\t': {
        if (!json_writer_append(writer, '\\')) {
          return 0;
        }
        json_writer_append(writer, 't');
        break;
      }
      default: {
        if (!json_writer_append(writer, ch)) {
          return 0;
        }
        break;
      }
    }
  }
  if (!json_writer_append(writer, '"')) {
    return 0;
  }
  return 1;
}

int json_write_number(const double num, json_writer* writer) {
  char temp[64];
  sprintf(temp, "%g", num);
  return json_writer_append_str(writer, temp, strlen(temp));
}

int json_write_true(json_writer* writer) {
  return json_writer_append_str(writer, "true", strlen("null"));
}

int json_write_false(json_writer* writer) {
  return json_writer_append_str(writer, "false", strlen("null"));
}

int json_write_null(json_writer* writer) {
  return json_writer_append_str(writer, "null", strlen("null"));
}

int json_write_object_begin(json_writer* writer) {
  return json_writer_begin(writer, '{', 1);
}

int json_write_kv_separator(json_writer* writer) {
  return json_writer_append_str(writer, ": ", strlen(": "));
}

int json_write_object_end(json_writer* writer) {
  return json_writer_end(writer, '}', 1);
}

int json_write_array_begin(json_writer* writer) {
  return json_writer_begin(writer, '[', 1);
}

int json_write_list_separator(json_writer* writer) {
  return json_writer_append(writer, ',');
}

int json_write_array_end(json_writer* writer) {
  return json_writer_end(writer, ']', 1);
}
