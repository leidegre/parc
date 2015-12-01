

#include "hash_map.h"

#include "test.h"

int main(int argc, char* argv[]) {
  test_initialize(argc, argv);

  TEST_NEW("hash_map_insert_ptr_test") {
    hash_map map;
    hash_map_create(&map);
    ASSERT_TRUE(hash_map_insert_ptr(&map, 1, 3));
    hash_map_bucket* it = hash_map_find_ptr(&map, 1);
    ASSERT_TRUE(it != NULL);
    ASSERT_EQUAL_INT(3, (int)hash_map_value_get(it));
    ASSERT_FALSE((it = hash_map_next(it)) != NULL);
    hash_map_destroy(&map);
  }

  TEST_NEW("hash_map_insert_str_test") {
    hash_map map;
    hash_map_create(&map);
    ASSERT_TRUE(hash_map_insert_str(&map, "a", 1));
    ASSERT_TRUE(hash_map_insert_str(&map, "a", 2));
    ASSERT_TRUE(hash_map_insert_str(&map, "a", 3));
    hash_map_bucket* it = hash_map_find_str(&map, "a");
    ASSERT_TRUE(it != NULL);
    ASSERT_EQUAL_INT(3, (int)hash_map_value_get(it));
    ASSERT_TRUE((it = hash_map_next(it)) != NULL);
    ASSERT_EQUAL_INT(2, (int)hash_map_value_get(it));
    ASSERT_TRUE((it = hash_map_next(it)) != NULL);
    ASSERT_EQUAL_INT(1, (int)hash_map_value_get(it));
    ASSERT_FALSE((it = hash_map_next(it)) != NULL);
    hash_map_destroy(&map);
  }

  TEST_NEW("hash_map_iter_test") {
    hash_map map;
    hash_map_create(&map);
    for (int i = 0; i < 64; i++) {
      ASSERT_TRUE(hash_map_insert_ptr(&map, i, i));
    }
    hash_map_iter it;
    hash_map_iter_create(&map, &it);
    uint64_t x = 0;
    while (hash_map_iter_next(&it)) {
      int y = (int)hash_map_value_get(it.current_);
      ASSERT_TRUE((x & (1ULL << y)) == 0);
      x |= 1ULL << y;
    }
    ASSERT_TRUE(x == 0xFFFFFFFFFFFFFFFF);
    hash_map_destroy(&map);
  }

  TEST_NEW("hash_map_iter_test2") {
    hash_map map;
    hash_map_create(&map);
    for (int i = 64 - 1; i >= 0; i--) {
      ASSERT_TRUE(hash_map_insert_ptr(&map, i, i));
    }
    hash_map_iter it;
    hash_map_iter_create(&map, &it);
    uint64_t x = 0;
    while (hash_map_iter_next(&it)) {
      int y = (int)hash_map_value_get(it.current_);
      ASSERT_TRUE((x & (1ULL << y)) == 0);
      x |= 1ULL << y;
    }
    ASSERT_TRUE(x == 0xFFFFFFFFFFFFFFFF);
    hash_map_destroy(&map);
  }

  return 0;
}