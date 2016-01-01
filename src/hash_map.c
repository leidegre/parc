
#include "hash_map.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hash.h"

static const uint32_t HASH_MAP_KEY_IDENTITY = 0x80000000;

struct hash_map_bucket {
  const void* k_;
  void* v_;
  hash_map_bucket* next_;
  uint32_t hash_;
  uint16_t size_;
};

static int hash_map_is_equal_key(hash_map_bucket* a, hash_map_bucket* b) {
  if ((a->hash_ == b->hash_) & (a->size_ == b->size_)) {
    if (a->hash_ & HASH_MAP_KEY_IDENTITY) {
      return a->k_ == b->k_;
    }
    return memcmp(a->k_, b->k_, a->size_) == 0;
  }
  return 0;
}

static void hash_map_key_ptr(const void* k, hash_map_bucket* bucket) {
  uintptr_t x = (uintptr_t)k;
  bucket->k_ = k;
  bucket->hash_ = XXH32(&x, sizeof(x), 0) | HASH_MAP_KEY_IDENTITY;
  bucket->size_ = (uint16_t)sizeof(x);
}

static void hash_map_key_slice(const char* k, size_t l,
                               hash_map_bucket* bucket) {
  assert(l <= 0xFFFF && "key is too large for container");
  bucket->k_ = k;
  bucket->hash_ = XXH32(k, l, 0) & ~HASH_MAP_KEY_IDENTITY;
  bucket->size_ = (uint16_t)l;
}

static hash_map_bucket* hash_map_find(hash_map* map, hash_map_bucket* entry) {
  hash_map_bucket** bucket =
      map->bucket_list_ + (entry->hash_ & (map->capacity_ - 1));
  if (bucket) {
    hash_map_bucket* curr = *bucket;
    while (curr) {
      if (hash_map_is_equal_key(curr, entry)) {
        return curr;
      }
      curr = curr->next_;
    }
  }
  return NULL;
}

static int hash_map_insert(hash_map* map, hash_map_bucket* entry) {
  hash_map_bucket** bucket =
      map->bucket_list_ + (entry->hash_ & (map->capacity_ - 1));
  if (*bucket) {
    // assumption is that traversal in-order doesn't matter, we may revise this.
    entry->next_ = *bucket;
    *bucket = entry;
  } else {  // unallocated
    *bucket = entry;
  }
  return 1;
}

//
void hash_map_create(hash_map* map) {
  memset(map, 0, sizeof(*map));
  map->capacity_ = 64;  // must be power of 2
  size_t bucket_list_size = map->capacity_ * sizeof(hash_map_bucket*);
  map->bucket_list_ = (hash_map_bucket**)malloc(bucket_list_size);
  memset(map->bucket_list_, 0, bucket_list_size);
}

void hash_map_destroy(hash_map* map) {
  for (uint32_t i = 0; i < map->capacity_; i++) {
    hash_map_bucket* bucket = map->bucket_list_[i];
    while (bucket) {
      hash_map_bucket* temp = bucket->next_;
      free(bucket);
      bucket = temp;
    }
  }
  free(map->bucket_list_);
  memset(map, 0, sizeof(*map));
}

//
hash_map_bucket* hash_map_next(hash_map_bucket* node) {
  hash_map_bucket* next = node->next_;
  while (next) {
    if (hash_map_is_equal_key(node, next)) {
      return next;
    }
    next = next->next_;
  }
  return next;
}

//
void* hash_map_value_get(hash_map_bucket* node) {
  return node->v_;
}

size_t hash_map_value_get_size(hash_map_bucket* node) {
  return node->size_;
}

void hash_map_value_set(hash_map_bucket* node, void* v) {
  node->v_ = v;
}

//
int hash_map_insert_ptr(hash_map* map, const void* k, void* v) {
  hash_map_bucket* entry = (hash_map_bucket*)malloc(sizeof(hash_map_bucket));
  hash_map_key_ptr(k, entry);
  entry->v_ = v;
  entry->next_ = NULL;
  return hash_map_insert(map, entry);
}

int hash_map_insert_slice(hash_map* map, const char* k, size_t l, void* v) {
  hash_map_bucket* entry = (hash_map_bucket*)malloc(sizeof(hash_map_bucket));
  hash_map_key_slice(k, l, entry);
  entry->v_ = v;
  entry->next_ = NULL;
  return hash_map_insert(map, entry);
}

int hash_map_insert_str(hash_map* map, const char* k, void* v) {
  return hash_map_insert_slice(map, k, strlen(k), v);
}

//
hash_map_bucket* hash_map_find_ptr(hash_map* map, const void* k) {
  hash_map_bucket entry;
  hash_map_key_ptr(k, &entry);
  return hash_map_find(map, &entry);
}

hash_map_bucket* hash_map_find_slice(hash_map* map, const char* k, size_t l) {
  hash_map_bucket entry;
  hash_map_key_slice(k, l, &entry);
  return hash_map_find(map, &entry);
}

hash_map_bucket* hash_map_find_str(hash_map* map, const char* k) {
  return hash_map_find_slice(map, k, strlen(k));
}

// iterator
int hash_map_iter_create(hash_map* map, hash_map_iter* it) {
  memset(it, 0, sizeof(*it));
  it->bucket_list_ = map->bucket_list_;
  it->capacity_ = map->capacity_;
  return 1;
}

int hash_map_iter_destroy(hash_map_iter* it) {
  // nop
  return 1;
}

int hash_map_iter_next(hash_map_iter* it) {
_Next:
  switch (it->state_) {
    case 0: {
      if (it->index_ < it->capacity_) {
        it->current_ = it->bucket_list_[it->index_++];
        it->state_ = 1;
        goto _Next;
      }
      break;
    }
    case 1: {
      if (it->current_) {
        it->state_ = 2;  // this function will resume here
        return 1;        // valid iterator
      } else {
        it->state_ = 0;
        goto _Next;
      }
    }
    case 2: {
      it->current_ = it->current_->next_;
      it->state_ = 1;
      goto _Next;
    }
  }
  // end of sequence
  return 0;
}
