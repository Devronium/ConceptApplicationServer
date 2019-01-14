#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include "khash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

KHASH_MAP_INIT_STR(strhashtable, intptr_t)

typedef khint_t HashTableIterator;

struct HashTable {
    khash_t (strhashtable) *htable;
};

void HashTable_init(struct HashTable *self);
intptr_t HashTable_size(struct HashTable *self);
intptr_t HashTable_find(struct HashTable *self, const char *str);
intptr_t HashTable_add(struct HashTable *self, const char *str, intptr_t index, int len);
intptr_t HashTable_erase(struct HashTable *self, const char *str);
HashTableIterator HashTable_begin(struct HashTable *self);
HashTableIterator HashTable_end(struct HashTable *self);
const char *HashTable_key(struct HashTable *self, HashTableIterator k);
intptr_t HashTable_val(struct HashTable *self, HashTableIterator k);
void HashTable_clear(struct HashTable *self);
void HashTable_deinit(struct HashTable *self);

#ifdef __cplusplus
}
#endif

#endif // __HASHTABLE_H
