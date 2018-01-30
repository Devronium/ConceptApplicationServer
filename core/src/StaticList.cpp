#include "StaticList.h"
#include <string.h>
#include <time.h>
#include "SHManager.h"

#define STATIC_GET(data)    char **data = allocated ? (char **)((intptr_t) this + DataOffset) : 0;
#define STATIC_SET(data)    DataOffset  = (intptr_t)data - (intptr_t) this;

#ifdef CACHED_LIST
char StaticList::UseMap = 1;
#endif

StaticList::StaticList() {
    count         = 0;
    allocated     = 0;
    DataOffset    = 0;
    pooled_marker = 0;
}

int StaticList::EnsureSpace() {
    if (count >= allocated) {
        int new_size = allocated + STATICLIST_BLOCKSIZE;
        STATIC_GET(data)
        data = (char **)realloc(data, new_size * sizeof(char *));
        if (!data) {
            return 0;
        }
        allocated += STATICLIST_BLOCKSIZE;
        STATIC_SET(data)
        return 1;
    }
    return 1;
}

void StaticList::ReInit(int i, bool pooled) {
    this->Clear();
    char **data = (char **)malloc(i * sizeof(char *));
    if (data) {

        allocated = i;
        count     = 0;
        STATIC_SET(data)
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
        STATIC_GET(data)
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
        STATIC_GET(data)
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
            CachedElements.add(str, count);
#endif
    }
}

void StaticList::Add(AnsiString& str) {
    this->Add(str.c_str(), str.Length());
}

void StaticList::Add(TinyString& str) {
    this->Add(str.c_str(), str.Length());
}

char *StaticList::operator[](int i) {
    return this->Item(i);
}

int StaticList::ContainsString(const char *str, int len) {
    if ((!str) || (!len))
        return 0;
#ifdef CACHED_LIST
    if (UseMap) {
        int       pos = CachedElements[str];
        if (pos) {
            char *item = this->Item(pos - 1);
            if ((item) && (!strcmp(item, str)))
                return pos;
        } else
            return 0;
    }
#endif
    STATIC_GET(data)

    for (int i = 0; i < count; i++) {
        char *elem = data [i];
        if ((*elem == *str) && (!strcmp(elem, str))) {
            return i + 1;
        }
    }
    return 0;
}

char *StaticList::Item(int i) {
    if ((i >= 0) && (i < count)) {
        STATIC_GET(data)
        return data [i];
    }
    return 0;
}

void StaticList::Clear() {
    if (count) {
#ifdef CACHED_LIST
        if (UseMap)
            CachedElements.clear();
#endif
        STATIC_GET(data)
        for (int i = pooled_marker; i < count; i++) {
            // if data[i] is null, free does nothing
            free(data [i]);
        }
        free(data);
        count         = 0;
        allocated     = 0;
        DataOffset    = 0;
        pooled_marker = 0;
    }
}

int StaticList::Count() {
    return this->count;
}

StaticList::~StaticList(void) {
    this->Clear();
}
