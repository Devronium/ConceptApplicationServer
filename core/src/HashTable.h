#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include "khash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

KHASH_MAP_INIT_STR(strhashtable, intptr_t);

typedef khint_t HashTableIterator;

class HashTable {
protected:
    khash_t (strhashtable) *htable;

    khash_t (strhashtable) *hash() {
        if (!htable)
            htable = kh_init(strhashtable);
        return htable;
    }
public:
    HashTable() {
        htable = NULL;
    }

    intptr_t size() {
        if (htable)
            return kh_size(htable);
        return 0;
    }

    intptr_t find(const char *str) {
        khash_t (strhashtable) *handle = htable;
        if (!handle)
            return 0;

        khiter_t k = kh_get(strhashtable, handle, str);
        if (k != kh_end(handle))
            return kh_val(handle, k);
        return 0;
    }

    intptr_t operator[](const char *str) {
        return find(str);
    }

    intptr_t add(const char *str, intptr_t index, int len = -1) {
        if ((!str) || (!str[0]))
            return -1;

        khash_t (strhashtable) *handle = hash();
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

    intptr_t erase(const char *str) {
        if (!htable)
            return 0;

        khiter_t k = kh_get(strhashtable, htable, str);
        intptr_t index = 0;
        if (k != kh_end(htable)) {
            char *key = NULL;
            if (kh_exist(htable, k))
                key = (char*)kh_key(htable, k);
            index = kh_val(htable, k);
            kh_del(strhashtable, htable, k);
            if (key)
                free(key);
        }
        return index;
    }

    HashTableIterator begin() {
        if (!htable)
            return 0;
        return kh_begin(htable);
    }

    HashTableIterator end() {
        if (!htable)
            return 0;
        return kh_end(htable);
    }

    const char *key(HashTableIterator k) {
        if (!htable)
            return NULL;

        if (kh_exist(htable, k))
            return (const char *)kh_key(htable, k);

        return NULL;
    }

    intptr_t val(HashTableIterator k) {
        if (!htable)
            return 0;

        if (kh_exist(htable, k))
            return (intptr_t)kh_val(htable, k);

        return 0;
    }

    void clear() {
        if (!htable)
            return;

        for (khint_t k = kh_begin(htable); k != kh_end(htable); ++k) {
            if (kh_exist(htable, k))
                free((char*)kh_key(htable, k));
        }

        kh_destroy(strhashtable, htable);
        htable = NULL;
    }

    ~HashTable() {
        clear();
    }
};

#endif // __HASHTABLE_H
