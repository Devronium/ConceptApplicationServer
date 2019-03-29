#include "StaticList.h"
#include <string.h>
#include <time.h>
#include "SHManager.h"

#ifdef CACHED_LIST
char StaticList::UseMap = 1;
#endif

StaticList::StaticList() {
    count         = 0;
    allocated     = 0;
    data          = NULL;
    pooled_marker = 0;
#ifdef CACHED_LIST
    HashTable_init(&this->CachedElements);
#endif
}

int StaticList::EnsureSpace() {
    if (count >= allocated) {
        int new_size = allocated + STATICLIST_BLOCKSIZE;
        char **old_data = data;
        data = (char **)realloc(data, new_size * sizeof(char *));
        if (!data) {
            free(old_data);
            return 0;
        }

        allocated += STATICLIST_BLOCKSIZE;
        return 1;
    }
    return 1;
}

void StaticList::ReInit(int i, bool pooled) {
    this->Clear();
    data = (char **)malloc(i * sizeof(char *));
    if (data) {

        allocated = i;
        count     = 0;
        if (pooled) {
            pooled_marker = i;
        }
    }
}

void StaticList::PoolMap(int len) {
    if (len <= 0) {
        return;
    }
    if (count < pooled_marker) {
        char *s      = (char *)SHAlloc(len);
        data [count] = s;
        count++;
    }
}

void StaticList::Add(const char *str, int len) {
    if ((!str) || (!len)) {
        return;
    }
    if (len < 0) {
        len = strlen(str);
    }
    if (!len) {
        return;
    }
    if (this->EnsureSpace()) {
        char *s = 0;
        if (count < pooled_marker) {
            s = (char *)SHAlloc(len + 1);
        } else {
            s = (char *)malloc(len + 1);
        }
        memcpy(s, str, len);
        s [len]      = 0;
        data [count] = s;
        count++;

#ifdef CACHED_LIST
        if (UseMap)
            HashTable_add(&CachedElements, str, count, -1);
#endif
    }
}

void StaticList::Add(TinyString& str) {
    this->Add(str.c_str(), str.Length());
}

char *StaticList::operator[](int i) {
    return this->Item(i);
}

int StaticList::ContainsString(const char *str) {
    if (!str)
        return 0;
#ifdef CACHED_LIST
    if (UseMap) {
        int pos = HashTable_find(&CachedElements, str);
        if (pos) {
            char *item = this->Item(pos - 1);
            if ((item) && (!strcmp(item, str)))
                return pos;
        } else
            return 0;
    }
#endif
    for (int i = 0; i < count; i++) {
        char *elem = data [i];
        if ((*elem == *str) && (!strcmp(elem, str))) {
            return i + 1;
        }
    }
    return 0;
}

char *StaticList::Item(int i) {
    if ((i >= 0) && (i < count))
        return data [i];
    return 0;
}

void StaticList::Clear() {
    if (count) {
#ifdef CACHED_LIST
        if (UseMap)
            HashTable_clear(&CachedElements);
#endif
        for (int i = pooled_marker; i < count; i++) {
            // if data[i] is null, free does nothing
            free(data [i]);
        }
        count         = 0;
        allocated     = 0;
        pooled_marker = 0;
    }
    if (data) {
        free(data);
        data = NULL;
    }
}

int StaticList::Count() {
    return this->count;
}

StaticList::~StaticList(void) {
    this->Clear();
#ifdef CACHED_LIST
    HashTable_deinit(&this->CachedElements);
#endif
}
