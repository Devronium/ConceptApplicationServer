#ifndef __STATICLIST_H
#define __STATICLIST_H
#include "ConceptPools.h"
#include "TinyString.h"

#define CACHED_LIST

#ifdef CACHED_LIST
 #include "HashTable.h"
 #include "Codes.h"
#endif

#define STATICLIST_BLOCKSIZE    0x10

class StaticList {
public:
    char     **data;
    int      allocated;
    int      count;
    int      pooled_marker;

    int EnsureSpace();

#ifdef CACHED_LIST
    struct HashTable CachedElements;
#endif
public:
#ifdef CACHED_LIST
    static char UseMap;
#endif
    POOLED(StaticList)

    StaticList();
    void Add(const char *str, int len = -1);
    void PoolMap(int len);
    void Add(TinyString& str);
    int ContainsString(const char *str);

    char *operator[](int i);
    char *Item(int i);
    void Clear();
    void ReInit(int i, bool pooled);
    int Count();

    ~StaticList(void);
};
#endif //__STATICLIST_H
