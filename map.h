#ifndef CRIPT_MAP_H
#define CRIPT_MAP_H

#include <stdlib.h>

#define KEY_TYPE    const char*
#define HASH_TYPE   size_t
#define VALUE_TYPE  void*

#define N_BUCKETS 6
#define START_SIZE 64

typedef struct {
    HASH_TYPE hash;
    VALUE_TYPE value;
} bucket[N_BUCKETS];

typedef struct {
    bucket* buckets;
    size_t size;
} hash_map;

HASH_TYPE hm_hash(KEY_TYPE str) {
    HASH_TYPE hval = 0xcbf29ce484222325UL;

    while (*str) {
        hval ^= (size_t)*str;
        hval *= 0x100000001b3UL;
        ++str;
    }

    return hval;
}

void hm_init(hash_map* map) {
    map->buckets = calloc(START_SIZE, sizeof(bucket));
    map->size = START_SIZE;
}

void hm_rehash(hash_map* map, size_t size);

void hm_add_hash(hash_map* map, HASH_TYPE hash, VALUE_TYPE value) {
    size_t n;

    do {
        n = hash % map->size;
        #pragma GCC unroll 6 /* N_BUCKETS */
        for (int i = 0; i < N_BUCKETS; ++i) {
            if (map->buckets[n][i].hash == 0) {
                map->buckets[n][i].hash = hash;
                map->buckets[n][i].value = value;
                return;
            }
        }
        hm_rehash(map, map->size*2);
    } while (1);
}

void hm_add(hash_map* map, KEY_TYPE key, VALUE_TYPE value) {
    hm_add_hash(map, hm_hash(key), value);
}

VALUE_TYPE* hm_get(hash_map* map, KEY_TYPE key) {
    HASH_TYPE hash = hm_hash(key);
    size_t n = hash % map->size;

    #pragma GCC unroll 6 /* N_BUCKETS */
    for (int i = 0; i < N_BUCKETS; ++i) {
        if (map->buckets[n][i].hash == hash) {
            return &map->buckets[n][i].value;
        }
    }

    return NULL;
}

VALUE_TYPE hm_set(hash_map* map, KEY_TYPE key, VALUE_TYPE value) {
    VALUE_TYPE* v = hm_get(map, key);
    if (!v) return NULL;
    return *v = value;
}

void hm_rehash(hash_map* map, size_t size) {
    hash_map t;
    t.buckets = calloc(size, sizeof(bucket));
    t.size = size;

    for (int i = 0; i < map->size; ++i) {
        for (int j = 0; j < N_BUCKETS; ++j) {
            hm_add_hash(&t, map->buckets[i][j].hash, map->buckets[i][j].value);
        }
    }

    free(map->buckets);
    map->buckets = t.buckets;
}

#undef N_BUCKETS
#undef START_SIZE

#undef KEY_TYPE
#undef HASH_TYPE
#undef VALUE_TYPE

#endif /* CRIPT_MAP_H */
