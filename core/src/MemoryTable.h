#ifndef __HASHTABLEINT_H
#define __HASHTABLEINT_H

#include "khash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef khint_t MemoryTableIterator;

#if __SIZEOF_POINTER__ == 8
    KHASH_MAP_INIT_INT64(intptrhashtable, intptr_t);
#else
    KHASH_MAP_INIT_INT(intptrhashtable, intptr_t);
#endif

struct MemoryTable {
    khash_t (intptrhashtable) *htable;
};

void MemoryTable_init(struct MemoryTable *self);
intptr_t MemoryTable_size(struct MemoryTable *self);
intptr_t MemoryTable_find(struct MemoryTable *self, const void *ptr);
intptr_t MemoryTable_add(struct MemoryTable *self, const void *str, intptr_t data);
intptr_t MemoryTable_erase(struct MemoryTable *self, const void *ptr);
MemoryTableIterator MemoryTable_begin(struct MemoryTable *self);
MemoryTableIterator MemoryTable_end(struct MemoryTable *self);
void *MemoryTable_key(struct MemoryTable *self, MemoryTableIterator k);
intptr_t MemoryTable_val(struct MemoryTable *self, MemoryTableIterator k);
void MemoryTable_clear(struct MemoryTable *self);
void MemoryTable_deinit(struct MemoryTable *self);

#ifdef __cplusplus
}
#endif

#endif // __HASHTABLEINT_H
