#ifndef __ARRAY_H
#define __ARRAY_H

#define OPTIMIZE_FAST_ARRAYS
// #define STDMAP_KEYS

#include "AnsiString.h"
#include "ConceptTypes.h"
#include "AnsiList.h"
#include "GarbageCollector.h"
#include "ConceptPools.h"
#ifdef STDMAP_KEYS
 #include <map>
#endif

#define     KEY_NUMBER          0

#define     ARRAY_INCREMENT     0x40
// mandatory multiple of ARRAY_INCREMENT
#define     BIG_ARRAY_TRESHOLD  ARRAY_INCREMENT * 0x80
// mandatory multiple of ARRAY_INCREMENT
#define     BIG_ARRAY_INCREMENT 0x400

#define     KEY_INCREMENT       0x20
#define     REALLOC_TRESHOLD    0x20

#define     MAX_DIRTY_DELTA     0x80
#define     DIRTY_TRESHOLD      0x1000

#define ArrayElement            VariableDATA *
#define ARRAY_COUNT_TYPE        int

#ifdef STDMAP_KEYS
struct TinyCmp {
    inline bool operator()(const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
};

typedef std::map<const char *, ARRAY_COUNT_TYPE, TinyCmp>   KeyMap;
typedef std::pair<const char *, ARRAY_COUNT_TYPE>           KeyMapPair;
#else
typedef struct _KEY {
    char             *KEY;
    ARRAY_COUNT_TYPE index;
    signed char      dirty;

    POOLED(_KEY)
} ArrayKey;
#endif

typedef struct _NODE {
    ArrayElement   *ELEMENTS;
    _NODE          *NEXT;
    unsigned short COUNT;
    POOLED(_NODE)
} NODE;

#ifdef OPTIMIZE_FAST_ARRAYS
 #define STATIC_ARRAY_THRESHOLD_MINSIZE    2048
 #define STATIC_ARRAY_THRESHOLD            8192
 #define FAST_CHECK(ARR, index)    (((ARR)->cached_data) && (index < (int)STATIC_ARRAY_THRESHOLD))  ? (ARR)->cached_data[index] : (VariableDATA *)0
#endif

class Array {
    friend int MarkRecursive(void *PIF, Array *arr, signed char reach_id_flag);

    friend int GetMemoryStatistics(void *PIF, void *RESULT);

private:
#ifdef OPTIMIZE_FAST_ARRAYS
    ARRAY_COUNT_TYPE LastNodeIndex;
    NODE *LastNode;
#endif

#ifdef STDMAP_KEYS
    KeyMap * Keys;
#else
    ArrayKey * Keys;
    ARRAY_COUNT_TYPE LastKey;
    ARRAY_COUNT_TYPE KeysCount;
#endif

    NODE             *FIRST;
    NODE             *LAST;
    ARRAY_COUNT_TYPE NODE_COUNT;
    ARRAY_COUNT_TYPE COUNT;

    void *PIF;
    ARRAY_COUNT_TYPE AddKey(const char *KEY, ARRAY_COUNT_TYPE index);
#ifndef STDMAP_KEYS
    ARRAY_COUNT_TYPE FindKeyPlace(const char *KEY, ARRAY_COUNT_TYPE *index);
    ARRAY_COUNT_TYPE FindOrAddKey(const char *KEY);
    ARRAY_COUNT_TYPE FindPlace(const char *KEY, ARRAY_COUNT_TYPE *in_dirty_zone = NULL);
    void CleanIndex(bool forced = false);
#endif

public:
#ifdef OPTIMIZE_FAST_ARRAYS
    VariableDATA * *cached_data;
#endif
    POOLED(Array)

    ARRAY_COUNT_TYPE FindKey(const char *KEY);
    ARRAY_COUNT_TYPE FindIndex(ARRAY_COUNT_TYPE index);

    int LINKS;
    ARRAY_COUNT_TYPE Count();

    void __GO_GARBAGE(void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects = -1);

    void UnlinkObjects();

    VariableDATA *NewArray();

    char *GetKey(ARRAY_COUNT_TYPE);
    ARRAY_COUNT_TYPE     GetIndex(const char *Key);

    VariableDATA *Get(ARRAY_COUNT_TYPE);
    VariableDATA *Get(VariableDATA *KEY);

    // for JIT
    VariableDATA *GetWithCreate(ARRAY_COUNT_TYPE i, NUMBER default_value = 0);
    VariableDATA *Add(VariableDATA *VAR_TO_ADD);
    VariableDATA *AddCopy(VariableDATA *VAR_TO_ADD);

    VariableDATA *ModuleGet(const char *key);

    VariableDATA *ModuleGet(ARRAY_COUNT_TYPE);

    Array *SortArrayElementsByKey();
    void GetKeys(char **container, int size);

    AnsiString ToString(int level = 0, Array *parent = 0, int parents = 0);
    Array *Concat(Array *array);

    void EnsureSize(ARRAY_COUNT_TYPE size, VariableDATA *default_value);

    Array(void *PIF);
    ~Array();

    signed char flags;
    signed char reachable;
    inline void operator delete(void *obj) {
        FreeArray(obj);
    }
};

typedef struct {
    Array         POOL[ARRAY_POOL_BLOCK_SIZE];
    void          *NEXT;
    void          *PREV;
    void          *PIF;
    unsigned char POOL_VARS;
    unsigned char FIRST_VAR;
} ArrayPool;
#endif // __ARRAY_H

