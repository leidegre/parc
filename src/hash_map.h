#pragma once

#include <stddef.h>
#include <stdint.h>

// this is a generalization of an associative array in which more than one value
// may be associated with and returned for a given key.

typedef struct hash_map_bucket hash_map_bucket;

typedef struct hash_map {
	hash_map_bucket** bucket_list_;
	uint32_t capacity_;
	uint32_t count_;
} hash_map;
//

void hash_map_create(hash_map* map);

void hash_map_destroy(hash_map* map);

// advance iterator.
// a valid iterator (non-NULL) or NULL if the enumeration is complete.
hash_map_bucket* hash_map_next(hash_map_bucket* it);

//
// gets the value associated with iterator.
void* hash_map_value_get(hash_map_bucket* it);

// gets the size of the value associated with iterator.
size_t hash_map_value_get_size(hash_map_bucket* it);

// sets the value associated with iterator.
void hash_map_value_set(hash_map_bucket* it, void* v);
//

// insert a value using the address as key.
int hash_map_insert_ptr(hash_map* map, const void* k, void* v);

// insert a value using a length-delimited string as key.
// WARNING: this function does not create a copy of the key. if the key goes out
// of scope while in use bad stuff will happen.
int hash_map_insert_slice(hash_map* map, const char* k, size_t l, void* v);

// insert a value using a null-terminated string as key.
// WARNING: this function does not create a copy of the key. if the key goes out
// of scope while in use bad stuff will happen.
int hash_map_insert_str(hash_map* map, const char* k, void* v);
//

// lookup the values associated with the address.
// the return value is either a valid iterator or NULL.
hash_map_bucket* hash_map_find_ptr(hash_map* map, const void* k);

// lookup the values associated with given length-delimited string key.
// the return value is either a valid iterator or NULL.
hash_map_bucket* hash_map_find_slice(hash_map* map, const char* k, size_t l);

// lookup the values associated with given null-terminated string key.
// the return value is either a valid iterator or NULL.
hash_map_bucket* hash_map_find_str(hash_map* map, const char* k);

// iterator
typedef struct hash_map_iter {
  hash_map_bucket** bucket_list_;
  hash_map_bucket* current_;
  uint32_t index_;
  uint32_t capacity_;
  uint32_t state_;
} hash_map_iter;

int hash_map_iter_create(hash_map* map, hash_map_iter* it);

int hash_map_iter_destroy(hash_map_iter* it);

int hash_map_iter_next(hash_map_iter* it);
