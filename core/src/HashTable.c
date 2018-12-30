#include "HashTable.h"

khash_t (strhashtable) *HashTable_hash(struct HashTable *self) {
    if (!self->htable)
        self->htable = kh_init(strhashtable);
    return self->htable;
}

void HashTable_init(struct HashTable *self) {
    self->htable = NULL;
}

intptr_t HashTable_size(struct HashTable *self) {
    if (self->htable)
        return kh_size(self->htable);
    return 0;
}

intptr_t HashTable_find(struct HashTable *self, const char *str) {
    khash_t (strhashtable) *handle = self->htable;
    if (!handle)
        return 0;

    khiter_t k = kh_get(strhashtable, handle, str);
    if (k != kh_end(handle))
        return kh_val(handle, k);
    return 0;
}

intptr_t HashTable_add(struct HashTable *self, const char *str, intptr_t index, int len) {
    if ((!str) || (!str[0]))
        return -1;

    khash_t (strhashtable) *handle = HashTable_hash(self);
    if (!handle)
        return -1;

    if (len <= 0)
        len = strlen(str);

    khiter_t k = kh_get(strhashtable, handle, str);
    if ((k != kh_end(handle)) && (kh_exist(handle, k))) {
        kh_value(handle, k) = index;
        return 0;
    }
    int ret;
    char *str2 = (char *)malloc(len + 1);
    if (!str2)
        return -1;
    memcpy(str2, str, len);
    str2[len] = 0;

    k = kh_put(strhashtable, handle, str2, &ret);
    kh_value(handle, k) = index;

    return ret;
}

intptr_t HashTable_erase(struct HashTable *self, const char *str) {
    if (!self->htable)
        return 0;

    khiter_t k = kh_get(strhashtable, self->htable, str);
    intptr_t index = 0;
    if (k != kh_end(self->htable)) {
        char *key = NULL;
        if (kh_exist(self->htable, k))
            key = (char*)kh_key(self->htable, k);
        index = kh_val(self->htable, k);
        kh_del(strhashtable, self->htable, k);
        if (key)
            free(key);
    }
    return index;
}

HashTableIterator HashTable_begin(struct HashTable *self) {
    if (!self->htable)
        return 0;
    return kh_begin(self->htable);
}

HashTableIterator HashTable_end(struct HashTable *self) {
    if (!self->htable)
        return 0;
    return kh_end(self->htable);
}

const char *HashTable_key(struct HashTable *self, HashTableIterator k) {
    if (!self->htable)
        return NULL;

    if (kh_exist(self->htable, k))
        return (const char *)kh_key(self->htable, k);

    return NULL;
}

intptr_t HashTable_val(struct HashTable *self, HashTableIterator k) {
    if (!self->htable)
        return 0;

    if (kh_exist(self->htable, k))
        return (intptr_t)kh_val(self->htable, k);

    return 0;
}

void HashTable_clear(struct HashTable *self) {
    if (!self->htable)
        return;

    for (khint_t k = kh_begin(self->htable); k != kh_end(self->htable); ++k) {
        if (kh_exist(self->htable, k))
            free((char*)kh_key(self->htable, k));
    }

    kh_destroy(strhashtable, self->htable);
    self->htable = NULL;
}

void HashTable_deinit(struct HashTable *self) {
    HashTable_clear(self);
}
