#include "MemoryTable.h"

khash_t (intptrhashtable) *MemoryTable_hash(struct MemoryTable *self) {
    if (!self->htable)
        self->htable = kh_init(intptrhashtable);
    return self->htable;
}

void MemoryTable_init(struct MemoryTable *self) {
    self->htable = NULL;
}

intptr_t MemoryTable_size(struct MemoryTable *self) {
    if (self->htable)
        return kh_size(self->htable);
    return 0;
}

intptr_t MemoryTable_find(struct MemoryTable *self, const void *ptr) {
    khash_t (intptrhashtable) *handle = self->htable;
    if (!handle)
        return 0;

    khiter_t k = kh_get(intptrhashtable, handle, (intptr_t)ptr);
    if (k != kh_end(handle))
        return kh_val(handle, k);
    return 0;
}

intptr_t MemoryTable_add(struct MemoryTable *self, const void *ptr, intptr_t data) {
    if (!ptr)
        return -1;

    khash_t (intptrhashtable) *handle = MemoryTable_hash(self);
    if (!handle)
        return -1;
    khiter_t k = kh_get(intptrhashtable, handle, (intptr_t)ptr);
    if ((k != kh_end(handle)) && (kh_exist(handle, k))) {
        kh_value(handle, k) = data;
        return 0;
    }
    int ret;
    k = kh_put(intptrhashtable, handle, (intptr_t)ptr, &ret);
    kh_value(handle, k) = data;

    return ret;
}

intptr_t MemoryTable_erase(struct MemoryTable *self, const void *ptr) {
    if (!self->htable)
        return 0;

    khiter_t k = kh_get(intptrhashtable, self->htable, (intptr_t)ptr);
    intptr_t data = 0;
    if (k != kh_end(self->htable)) {
        data = kh_val(self->htable, k);
        kh_del(intptrhashtable, self->htable, k);
    }
    return data;
}

MemoryTableIterator MemoryTable_begin(struct MemoryTable *self) {
    if (!self->htable)
        return 0;
    return kh_begin(self->htable);
}

MemoryTableIterator MemoryTable_end(struct MemoryTable *self) {
    if (!self->htable)
        return 0;
    return kh_end(self->htable);
}

void *MemoryTable_key(struct MemoryTable *self, MemoryTableIterator k) {
    if (!self->htable)
        return NULL;

    if (kh_exist(self->htable, k))
        return (void *)kh_key(self->htable, k);

    return NULL;
}

intptr_t MemoryTable_val(struct MemoryTable *self, MemoryTableIterator k) {
    if (!self->htable)
        return 0;

    if (kh_exist(self->htable, k))
        return (intptr_t)kh_val(self->htable, k);

    return 0;
}

void MemoryTable_clear(struct MemoryTable *self) {
    if (!self->htable)
        return;

    kh_destroy(intptrhashtable, self->htable);
    self->htable = NULL;
}

void MemoryTable_deinit(struct MemoryTable *self) {
    MemoryTable_clear(self);
}
