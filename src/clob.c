
#include "api.h"

typedef struct parc_clob { parc_slice s_; } parc_clob;

parc_clob *parc_clob_create_from_string(const char *s) {
  const size_t size = strlen(s);
  parc_clob *clob = malloc(sizeof(parc_clob) + size);
  char *dst = (char *)(clob + 1);
  memcpy(dst, s, size);
  clob->s_ = parc_slice_create(dst, size);
  return clob;
}

parc_slice parc_clob_get(parc_clob *clob) { return clob->s_; }

void parc_clob_destroy(parc_clob *clob) { free(clob); }