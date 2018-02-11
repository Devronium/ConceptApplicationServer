#ifndef __ARRAY_H
#define __ARRAY_H

#define OPTIMIZE_FAST_ARRAYS

#include "ConceptTypes.h"
#include "GarbageCollector.h"
#include "ConceptPools.h"

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


typedef struct _KEY {
    char             *KEY;
    ARRAY_COUNT_TYPE index;
    signed char      dirty;

    POOLED(_KEY)
} ArrayKey;

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

struct Array {
#ifdef OPTIMIZE_FAST_ARRAYS
    ARRAY_COUNT_TYPE LastNodeIndex;
    NODE *LastNode;
#endif

    ArrayKey * Keys;
    ARRAY_COUNT_TYPE LastKey;
    ARRAY_COUNT_TYPE KeysCount;

    NODE             *FIRST;
    NODE             *LAST;
    ARRAY_COUNT_TYPE NODE_COUNT;
    ARRAY_COUNT_TYPE COUNT;

    void *PIF;

#ifdef OPTIMIZE_FAST_ARRAYS
    VariableDATA * *cached_data;
#endif
    int LINKS;

    signed char flags;
    signed char reachable;
};

ARRAY_COUNT_TYPE Array_AddKey(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE index);
ARRAY_COUNT_TYPE Array_FindKeyPlace(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE *index);
ARRAY_COUNT_TYPE Array_FindOrAddKey(struct Array *self, const char *KEY);
ARRAY_COUNT_TYPE Array_FindPlace(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE *in_dirty_zone = NULL);
void Array_CleanIndex(struct Array *self, bool forced = false);
ARRAY_COUNT_TYPE Array_FindKey(struct Array *self, const char *KEY);
ARRAY_COUNT_TYPE Array_FindIndex(struct Array *self, ARRAY_COUNT_TYPE index);
ARRAY_COUNT_TYPE Array_Count(struct Array *self);
void Array_GO_GARBAGE(struct Array *self, void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects = -1, char main_call = 0);
void Array_UnlinkObjects(struct Array *self);
VariableDATA *Array_NewArray(struct Array *self);
const char *Array_GetKey(struct Array *self, ARRAY_COUNT_TYPE);
ARRAY_COUNT_TYPE Array_GetIndex(struct Array *self, const char *Key);
VariableDATA *Array_Get(struct Array *self, ARRAY_COUNT_TYPE);
VariableDATA *Array_Get(struct Array *self, VariableDATA *KEY);
VariableDATA *Array_GetWithCreate(struct Array *self, ARRAY_COUNT_TYPE i, NUMBER default_value = 0);
VariableDATA *Array_Add(struct Array *self, VariableDATA *VAR_TO_ADD);
VariableDATA *Array_AddCopy(struct Array *self, VariableDATA *VAR_TO_ADD);
VariableDATA *Array_ModuleGet(struct Array *self, const char *key);
VariableDATA *Array_ModuleGet(struct Array *self, ARRAY_COUNT_TYPE);
struct Array *Array_SortArrayElementsByKey(struct Array *self);
void Array_GetKeys(struct Array *self, char **container, int size);
struct plainstring *Array_ToString(struct Array *self, int level = 0, Array *parent = 0, int parents = 0);
struct Array *Array_Concat(struct Array *self, struct Array *array);
void Array_EnsureSize(struct Array *self, ARRAY_COUNT_TYPE size, VariableDATA *default_value);
struct Array *new_Array(void *PIF, bool skip_top = false);
void delete_Array(struct Array *self);

typedef struct {
    Array         POOL[ARRAY_POOL_BLOCK_SIZE];
    void          *NEXT;
    void          *PREV;
    void          *PIF;
    unsigned char POOL_VARS;
    unsigned char FIRST_VAR;
} ArrayPool;
#endif // __ARRAY_H

