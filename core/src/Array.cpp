#include "Array.h"
#include "ConceptInterpreter.h"
#include <string.h>

//POOLED_IMPLEMENTATION(_KEY)
//POOLED_IMPLEMENTATION(_NODE)
POOLED_IMPLEMENTATION(Array)

//--- macros ------------------------------------------------------------
#define CREATE_NODE(NEW_NODE)                               \
    NEW_NODE           = (NODE *)FAST_MALLOC(sizeof(NODE)); \
    NEW_NODE->ELEMENTS = NULL;                              \
    NEW_NODE->COUNT    = 0;                                 \
    NEW_NODE->NEXT     = 0;

#define DYNAMIC_INCREMENT(i)    (i < BIG_ARRAY_TRESHOLD ) ? ARRAY_INCREMENT : BIG_ARRAY_INCREMENT
#define DYNAMIC_TARGET(i)       (i < BIG_ARRAY_TRESHOLD ) ? (i / ARRAY_INCREMENT) : (BIG_ARRAY_TRESHOLD / ARRAY_INCREMENT) + (i - BIG_ARRAY_TRESHOLD) / BIG_ARRAY_INCREMENT
#define DYNAMIC_DISTRIBUTION(i) (i < BIG_ARRAY_TRESHOLD ) ? (i % ARRAY_INCREMENT) : (i - BIG_ARRAY_TRESHOLD) % BIG_ARRAY_INCREMENT

#define ENSURE_ELEMENTS(TARGET_NODE, INDEX)                                                                                     \
    if (TARGET_NODE->COUNT <= INDEX) {                                                                                          \
        int prec_size = TARGET_NODE->COUNT;                                                                                     \
        if (self->COUNT >= REALLOC_TRESHOLD)                                                                                    \
            TARGET_NODE->COUNT = DYNAMIC_INCREMENT(self->COUNT);                                                                \
        else                                                                                                                    \
            TARGET_NODE->COUNT = INDEX + 1;                                                                                     \
        TARGET_NODE->ELEMENTS = (ArrayElement *)FAST_REALLOC(TARGET_NODE->ELEMENTS, TARGET_NODE->COUNT * sizeof(ArrayElement)); \
        for (int i = prec_size; i < TARGET_NODE->COUNT; i++)                                                                    \
            TARGET_NODE->ELEMENTS[i] = NULL;                                                                                    \
    }

#define ADD_LINKED_VARIABLE(var)                                  \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = DYNAMIC_DISTRIBUTION(self->COUNT);\
    ARRAY_COUNT_TYPE TARGET_NODE = DYNAMIC_TARGET(self->COUNT);   \
    NODE             *REF_NODE;                                   \
    if (TARGET_NODE >= self->NODE_COUNT) {                        \
        NODE *NEW_NODE;                                           \
        CREATE_NODE(NEW_NODE);                                    \
        if (self->LAST)                                           \
            self->LAST->NEXT = NEW_NODE;                          \
        else                                                      \
            self->FIRST = NEW_NODE;                               \
        REF_NODE = NEW_NODE;                                      \
        self->LAST     = NEW_NODE;                                \
        if (!self->FIRST)                                         \
            self->FIRST = NEW_NODE;                               \
        self->NODE_COUNT++;                                       \
    } else {                                                      \
        if ((TARGET_NODE == self->NODE_COUNT - 1) && (self->LAST)) { \
            REF_NODE = self->LAST;                                \
        } else {                                                  \
            NODE *CURRENT_NODE = self->FIRST;                     \
            for (ARRAY_COUNT_TYPE i = 0; i < TARGET_NODE; i++) {  \
                CURRENT_NODE = CURRENT_NODE->NEXT; }              \
            REF_NODE = CURRENT_NODE;                              \
        }                                                         \
    }                                                             \
    ENSURE_ELEMENTS(REF_NODE, DISTRIBUTED_COUNT);                 \
    ArrayElement *ELEMENTS = REF_NODE->ELEMENTS;                  \
    ELEMENTS [DISTRIBUTED_COUNT] = var;                           \
    ELEMENTS [DISTRIBUTED_COUNT]->LINKS++;                        \
    ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;         \
    self->COUNT++;                                                \
    return ELEMENTS [DISTRIBUTED_COUNT];

#define ADD_COPY_VARIABLE(var, pif)                                         \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = DYNAMIC_DISTRIBUTION(self->COUNT); \
    ARRAY_COUNT_TYPE TARGET_NODE = DYNAMIC_TARGET(self->COUNT);             \
    NODE             *REF_NODE;                                             \
    if (TARGET_NODE >= self->NODE_COUNT) {                                  \
        NODE *NEW_NODE;                                                     \
        CREATE_NODE(NEW_NODE);                                              \
        if (self->LAST)                                                     \
            self->LAST->NEXT = NEW_NODE;                                    \
        else                                                                \
            self->FIRST = NEW_NODE;                                         \
        REF_NODE = NEW_NODE;                                                \
        self->LAST     = NEW_NODE;                                          \
        if (!self->FIRST)                                                   \
            self->FIRST = NEW_NODE;                                         \
        self->NODE_COUNT++;                                                 \
    } else {                                                                \
        if ((TARGET_NODE == self->NODE_COUNT - 1) && (self->LAST)) {        \
            REF_NODE = self->LAST;                                          \
        } else {                                                            \
            NODE *CURRENT_NODE = self->FIRST;                               \
            for (ARRAY_COUNT_TYPE i = 0; i < TARGET_NODE; i++) {            \
                CURRENT_NODE = CURRENT_NODE->NEXT; }                        \
            REF_NODE = CURRENT_NODE;                                        \
        }                                                                   \
    }                                                                       \
    ENSURE_ELEMENTS(REF_NODE, DISTRIBUTED_COUNT);                           \
    ArrayElement *ELEMENTS = REF_NODE->ELEMENTS;                            \
    ELEMENTS [DISTRIBUTED_COUNT]        = (VariableDATA *)VAR_ALLOC(pif);   \
    ELEMENTS [DISTRIBUTED_COUNT]->TYPE  = var->TYPE;                        \
    ELEMENTS [DISTRIBUTED_COUNT]->LINKS = 1;                                \
    if (var->TYPE == VARIABLE_NUMBER) {                                     \
        ELEMENTS [DISTRIBUTED_COUNT]->NUMBER_DATA = var->NUMBER_DATA;       \
    } else                                                                  \
    if (var->TYPE == VARIABLE_STRING) {                                     \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA     = 0;                   \
        CONCEPT_STRING(ELEMENTS [DISTRIBUTED_COUNT], var);                  \
    } else                                                                  \
    if (var->TYPE == VARIABLE_CLASS) {                                      \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = var->CLASS_DATA;         \
        ((struct CompiledClass *)var->CLASS_DATA)->LINKS++;                 \
    } else                                                                  \
    if (var->TYPE == VARIABLE_ARRAY) {                                      \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = var->CLASS_DATA;         \
        ((struct Array *)var->CLASS_DATA)->LINKS++;                         \
    } else                                                                  \
    if (var->TYPE == VARIABLE_DELEGATE) {                                   \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = copy_Delegate(var->CLASS_DATA); \
    }                                                                       \
    ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;                   \
    self->COUNT++;                                                          \
    ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], self->COUNT);         \
                                                                            \
    return ELEMENTS [DISTRIBUTED_COUNT];

#define ADD_VARIABLE(val, pif)                                            \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = DYNAMIC_DISTRIBUTION(self->COUNT); \
    ARRAY_COUNT_TYPE TARGET_NODE = DYNAMIC_TARGET(self->COUNT);           \
    NODE             *REF_NODE;                                           \
    if (TARGET_NODE >= self->NODE_COUNT) {                                \
        NODE *NEW_NODE;                                                   \
        CREATE_NODE(NEW_NODE);                                            \
        if (self->LAST)                                                   \
            self->LAST->NEXT = NEW_NODE;                                  \
        else                                                              \
            self->FIRST = NEW_NODE;                                       \
        REF_NODE = NEW_NODE;                                              \
        self->LAST     = NEW_NODE;                                        \
        self->NODE_COUNT++;                                               \
    } else {                                                              \
        if ((TARGET_NODE == self->NODE_COUNT - 1) && (self->LAST)) {      \
            REF_NODE = self->LAST;                                        \
        } else {                                                          \
            NODE *CURRENT_NODE = self->FIRST;                             \
            for (ARRAY_COUNT_TYPE i = 0; i < TARGET_NODE; i++) {          \
                CURRENT_NODE = CURRENT_NODE->NEXT; }                      \
            REF_NODE = CURRENT_NODE;                                      \
        }                                                                 \
    }                                                                     \
    ENSURE_ELEMENTS(REF_NODE, DISTRIBUTED_COUNT);                         \
    ArrayElement *ELEMENTS = REF_NODE->ELEMENTS;                          \
    ELEMENTS [DISTRIBUTED_COUNT]        = (VariableDATA *)VAR_ALLOC(pif); \
    ELEMENTS [DISTRIBUTED_COUNT]->TYPE  = VARIABLE_NUMBER;                \
    ELEMENTS [DISTRIBUTED_COUNT]->LINKS = 1;                              \
    ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;                 \
    ELEMENTS [DISTRIBUTED_COUNT]->NUMBER_DATA        = val;               \
                                                                          \
    ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], self->COUNT);       \
    self->COUNT++;                                                        \
    return ELEMENTS [DISTRIBUTED_COUNT];
\

#define ADD_MULTIPLE_VARIABLE2                                    \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = DYNAMIC_DISTRIBUTION(self->COUNT); \
    ARRAY_COUNT_TYPE TARGET_NODE = DYNAMIC_TARGET(self->COUNT);   \
    if (TARGET_NODE >= self->NODE_COUNT) {                        \
        NODE *NEW_NODE;                                           \
        CREATE_NODE(NEW_NODE);                                    \
        if (self->LAST)                                           \
            self->LAST->NEXT = NEW_NODE;                          \
        else                                                      \
            self->FIRST = NEW_NODE;                               \
        self->LAST = NEW_NODE;                                    \
        self->NODE_COUNT++;                                       \
    }                                                             \
    self->COUNT++;                                                \

#define CREATE_VARIABLE(DATA, pif)                    \
    {                                                 \
        DATA        = (VariableDATA *)VAR_ALLOC(pif); \
        DATA->TYPE  = VARIABLE_NUMBER;                \
        DATA->LINKS = 1;                              \
        DATA->IS_PROPERTY_RESULT = 0;                 \
        DATA->NUMBER_DATA        = 0;                 \
    }
//----------------------------------------------------------------------------------
#ifdef OPTIMIZE_FAST_ARRAYS
 #define CACHE_ARRAY_BLOCK                                                                               \
    if ((self->COUNT > STATIC_ARRAY_THRESHOLD_MINSIZE) && (i < STATIC_ARRAY_THRESHOLD)) {                \
        if (self->cached_data) {                                                                         \
            VariableDATA *res = self->cached_data[i];                                                    \
            if (res)                                                                                     \
                return res;                                                                              \
        } else {                                                                                         \
            self->cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *)); \
            memset(self->cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));               \
        }                                                                                                \
    }

 #define ARRAY_CACHED_RETURN(VAR, i_i)                         \
    if ((self->cached_data) && (i_i < STATIC_ARRAY_THRESHOLD)) \
        self->cached_data[i_i] = VAR;

 #define CACHE_CREATE_CHECK                                                                          \
    if ((!self->cached_data) && (self->COUNT > STATIC_ARRAY_THRESHOLD_MINSIZE)) {                    \
        self->cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *)); \
        memset(self->cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));               \
    }
#else
 #define CACHE_ARRAY_BLOCK
 #define ARRAY_CACHED_RETURN(VAR, i_i)
 #define CACHE_CREATE_CHECK
#endif

struct Array *new_Array(void *PIF, bool skip_top) {
    struct Array *self = (struct Array *)AllocArray(PIF, skip_top);
#ifdef OPTIMIZE_FAST_ARRAYS
    self->LastNodeIndex = -1;
    self->LastNode      = 0;
    self->cached_data   = 0;
#endif

    self->COUNT         = 0;
    self->FIRST         = 0;
    self->Keys          = 0;
    self->KeysCount     = 0;
    self->LastKey       = 0;
    self->LINKS         = 1;
    self->LAST          = self->FIRST;
    self->NODE_COUNT    = 0;
    self->PIF           = PIF;

    return self;
}

ARRAY_COUNT_TYPE Array_FindIndex(struct Array *self, ARRAY_COUNT_TYPE index) {
    if (index < self->COUNT) {
        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(index);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(index);
        NODE             *CURRENT    = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if ((d_count >= CURRENT->COUNT) || (!ELEMENTS [d_count])) {
            return -1;
        }
        return index;
    }
    return -1;
}

ARRAY_COUNT_TYPE Array_FindKey(struct Array *self, const char *KEY) {
    if (!self->KeysCount) {
        return -1;
    }

    if ((self->KeysCount > 10) && (self->LastKey != -1)) {
        if (self->LastKey < self->KeysCount) {
            if (!strcmp(KEY, self->Keys [self->LastKey].KEY)) {
                return self->Keys [self->LastKey].index;
            }
        } else
        if (self->LastKey < self->KeysCount - 1) {
            if (!strcmp(KEY, self->Keys [++self->LastKey].KEY)) {
                return self->Keys [self->LastKey].index;
            }
        }
    }
    ARRAY_COUNT_TYPE LocalKeysCount = self->KeysCount;
    for (int i = LocalKeysCount - 1; i >= 0; i--) {
        if (self->Keys [i].dirty) {
            LocalKeysCount--;
            // the strcmp part should be moved AFTER binary lookup
            // it would make more sense this way
            if (!strcmp(KEY, self->Keys [i].KEY))
                return self->Keys [i].index;
        } else
            break;
    }

    self->LastKey = -1;

    INTEGER start = 0;
    INTEGER end = LocalKeysCount - 1;
    INTEGER middle = end / 2;

    int order = 0;
    int last_middle = 0;
    while (start <= end) {
        order  = strcmp(KEY, self->Keys [middle].KEY);
        last_middle = middle;
        if (order > 0)
            start = middle + 1;
        else
        if (!order) {
            return self->Keys[middle].index;
        } else
            end = middle - 1;
        middle = (start + end) / 2;
    }
    return -1;
}

ARRAY_COUNT_TYPE Array_FindPlace(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE *in_dirty_zone) {
    if (!self->KeysCount)
        return 0;

    ARRAY_COUNT_TYPE LocalKeysCount = self->KeysCount;

    // ugly !
    ArrayKey *TargetKeys = self->Keys;

    if (in_dirty_zone) {
        LocalKeysCount = 0;
        for (int i = self->KeysCount - 1; i >= 0; i--) {
            if (self->Keys [i].dirty) {
                LocalKeysCount++;
                TargetKeys = &self->Keys[i];
            } else
                break;
        }
        if (!LocalKeysCount) {
            *in_dirty_zone = 0;
            return self->KeysCount;
        }
        *in_dirty_zone = self->KeysCount - LocalKeysCount;
    } else {
        for (int i = LocalKeysCount - 1; i >= 0; i--) {
            if (self->Keys [i].dirty)
                LocalKeysCount--;
            else
                break;
        }
    }

    if (LocalKeysCount > 10) {
        if (strcmp(KEY, TargetKeys [LocalKeysCount - 1].KEY) > 0) {
            return LocalKeysCount;
        }
    }

    INTEGER start = 0;
    INTEGER end = LocalKeysCount - 1;
    INTEGER middle = end / 2;

    int order = 0;
    int last_middle = 0;
    while (start <= end) {
        order  = strcmp(KEY, TargetKeys [middle].KEY);
        last_middle = middle;
        if (order > 0)
            start = middle + 1;
        else
        if (!order) {
            return middle;
        } else
            end = middle - 1;
        middle = (start + end) / 2;
    }
    if (order > 0)
        last_middle++;
    return last_middle;
}

void Array_CleanIndex(struct Array *self, bool forced) {
    ArrayKey         DirtyKeys[MAX_DIRTY_DELTA];
    ARRAY_COUNT_TYPE indexes[MAX_DIRTY_DELTA];
    ARRAY_COUNT_TYPE count         = 0;
    bool             time_to_clean = false;

    for (int i = self->KeysCount - 1; i >= 0; i--) {
        if (self->Keys [i].dirty) {
            count++;
            if (count >= MAX_DIRTY_DELTA) {
                time_to_clean = true;
                break;
            }
        } else
            break;
    }
    if ((count) && ((forced) || (time_to_clean))) {
        ARRAY_COUNT_TYPE delta = self->KeysCount - count;
        memcpy(DirtyKeys, &self->Keys[delta], sizeof(ArrayKey) * count);

        for (int i = 0; i < count; i++) {
            indexes[i]         = Array_FindPlace(self, DirtyKeys[i].KEY);
            DirtyKeys[i].dirty = 0;
        }

        ARRAY_COUNT_TYPE array_end  = self->KeysCount;
        ARRAY_COUNT_TYPE tail_end   = self->KeysCount - count;
        ARRAY_COUNT_TYPE next_space = -1;
        for (int i = count - 1; i >= 0; i--) {
            // copy tail
            ARRAY_COUNT_TYPE place = indexes[i];

            ARRAY_COUNT_TYPE delta = tail_end - place;

            ARRAY_COUNT_TYPE offset = array_end - delta;
            if (delta) {
                memmove(&self->Keys [offset], &self->Keys [place], sizeof(ArrayKey) * delta);
                next_space = offset - 1;
            }
            if (next_space < 0)
                next_space = offset - 1;

            if (self->Keys[next_space].KEY == DirtyKeys[i].KEY)
                self->Keys[next_space].dirty = 0;
            else
                memcpy(&self->Keys[next_space], &DirtyKeys[i], sizeof(ArrayKey));

            tail_end  = place;
            array_end = next_space;

            next_space--;
            if (next_space < 0)
                next_space = 0;
        }
    }
}

ARRAY_COUNT_TYPE Array_AddKey(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE index) {
    if (!(self->KeysCount % KEY_INCREMENT))
        self->Keys = (ArrayKey *)FAST_REALLOC(self->Keys, sizeof(ArrayKey) * (self->KeysCount / KEY_INCREMENT + 1) * KEY_INCREMENT);
    ARRAY_COUNT_TYPE place = Array_FindPlace(self, KEY);

    int len = KEY ? strlen(KEY) : 0;
    ARRAY_COUNT_TYPE delta = self->KeysCount - place;
    char             dirty = 0;

    if ((self->KeysCount > DIRTY_TRESHOLD) && (delta)) {
        ARRAY_COUNT_TYPE delta_place = 0;
        place  = Array_FindPlace(self, KEY, &delta_place);
        place += delta_place;
        delta  = self->KeysCount - place;
        dirty  = 1;
    }

    if (place < self->KeysCount)
        memmove((void *)&self->Keys [place + 1], (void *)&self->Keys [place], sizeof(ArrayKey) * delta);

    self->Keys [place].KEY   = (char *)FAST_MALLOC(len + 1);
    self->Keys [place].index = index;
    self->Keys [place].dirty = dirty;
    memcpy(self->Keys [place].KEY, KEY, len);
    self->Keys [place].KEY [len] = 0;
    self->KeysCount++;
    Array_CleanIndex(self);

    return self->KeysCount;
}

VariableDATA *Array_Add(struct Array *self, VariableDATA *VAR_TO_ADD) {
    ADD_LINKED_VARIABLE(VAR_TO_ADD);
    return 0;
}

VariableDATA *Array_AddCopy(struct Array *self, VariableDATA *VAR_TO_ADD) {
    ADD_COPY_VARIABLE(VAR_TO_ADD, self->PIF);
    return 0;
}

struct Array *Array_Concat(struct Array *self, struct Array *array) {
    ARRAY_COUNT_TYPE count = array->COUNT;

    for (ARRAY_COUNT_TYPE i = 0; i < count; i++) {
        Array_Add(self, Array_Get(array, i));
    }
    return self;
}

VariableDATA *Array_Get(struct Array *self, ARRAY_COUNT_TYPE i) {
    if (i < 0) {
        return 0;
    }
    if (i < self->COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(i);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(i);

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (self->LastNodeIndex == target_node) {
            CURRENT = self->LastNode;
        } else
        if ((self->LastNodeIndex != -1) && (self->LastNodeIndex < target_node)) {
            CURRENT = self->LastNode;
            for (ARRAY_COUNT_TYPE k = self->LastNodeIndex; k < target_node; k++)
                CURRENT = CURRENT->NEXT;
            self->LastNode      = CURRENT;
            self->LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++)
            CURRENT = CURRENT->NEXT;

#ifdef OPTIMIZE_FAST_ARRAYS
        self->LastNode      = CURRENT;
        self->LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }
    return 0;
}

VariableDATA *Array_Get(struct Array *self, VariableDATA *KEY) {
    ARRAY_COUNT_TYPE i = -1;

    if (KEY->TYPE == VARIABLE_NUMBER) {
        i = (ARRAY_COUNT_TYPE)KEY->NUMBER_DATA;
        if (i < 0) {
            return 0;
        }
    } else
    if (KEY->TYPE == VARIABLE_STRING) {
        const char *k_str = CONCEPT_C_STRING(KEY);
        i = Array_FindKey(self, k_str);

        if (i == -1) {
            Array_AddKey(self, k_str, self->COUNT);
            ADD_VARIABLE(0, self->PIF);
        }
    }

    if (i < self->COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(i);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(i);

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (self->LastNodeIndex == target_node) {
            CURRENT = self->LastNode;
        } else
        if ((self->LastNodeIndex != -1) && (self->LastNodeIndex < target_node)) {
            CURRENT = self->LastNode;
            for (ARRAY_COUNT_TYPE k = self->LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            self->LastNode      = CURRENT;
            self->LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }

#ifdef OPTIMIZE_FAST_ARRAYS
        self->LastNode      = CURRENT;
        self->LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], self->PIF);
        }
        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (self->COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, self->PIF);
    return 0;
}

VariableDATA *Array_GetWithCreate(struct Array *self, ARRAY_COUNT_TYPE i, NUMBER default_value) {
    if (i < self->COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(i);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(i);

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (self->LastNodeIndex == target_node) {
            CURRENT = self->LastNode;
        } else
        if ((self->LastNodeIndex != -1) && (self->LastNodeIndex < target_node)) {
            CURRENT = self->LastNode;
            for (ARRAY_COUNT_TYPE k = self->LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            self->LastNode      = CURRENT;
            self->LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }

#ifdef OPTIMIZE_FAST_ARRAYS
        self->LastNode      = CURRENT;
        self->LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], self->PIF);
        }

        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }

    ARRAY_COUNT_TYPE target = i;
    while (self->COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(default_value, self->PIF);
    return 0;
}

VariableDATA *Array_ModuleGet(struct Array *self, ARRAY_COUNT_TYPE i) {
    if (i <self-> COUNT) {
        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(i);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(i);

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (self->LastNodeIndex == target_node) {
            CURRENT = self->LastNode;
        } else
        if ((self->LastNodeIndex != -1) && (self->LastNodeIndex < target_node)) {
            CURRENT = self->LastNode;
            for (ARRAY_COUNT_TYPE k = self->LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            self->LastNode      = CURRENT;
            self->LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }
#ifdef OPTIMIZE_FAST_ARRAYS
        self->LastNode      = CURRENT;
        self->LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], self->PIF);
        }
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (self->COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, self->PIF);
    return 0;
}

ARRAY_COUNT_TYPE Array_FindKeyPlace(struct Array *self, const char *KEY, ARRAY_COUNT_TYPE *index) {
    *index = 0;
    if (!self->KeysCount) {
        return -1;
    }

    ARRAY_COUNT_TYPE LocalKeysCount = self->KeysCount;
    for (int i = LocalKeysCount - 1; i >= 0; i--) {
        if (self->Keys [i].dirty) {
            LocalKeysCount--;
            if (!strcmp(KEY, self->Keys [i].KEY))
                return self->Keys [i].index;
        } else
            break;
    }

    self->LastKey = -1;

    INTEGER start = 0;
    INTEGER end = LocalKeysCount - 1;
    INTEGER middle = end / 2;

    int order = 0;
    int last_middle = 0;
    while (start <= end) {
        order  = strcmp(KEY, self->Keys [middle].KEY);
        last_middle = middle;
        if (order > 0)
            start = middle + 1;
        else
        if (!order) {
            return self->Keys[middle].index;
        } else
            end = middle - 1;
        middle = (start + end) / 2;
    }
    if (order > 0)
        last_middle++;
    *index = last_middle;
    return -1;
}

ARRAY_COUNT_TYPE Array_FindOrAddKey(struct Array *self, const char *KEY) {
    ARRAY_COUNT_TYPE index;
    ARRAY_COUNT_TYPE place = Array_FindKeyPlace(self, KEY, &index);
    if (place != -1)
        return place;
    place = index;

    if (!(self->KeysCount % KEY_INCREMENT))
        self->Keys = (ArrayKey *)FAST_REALLOC(self->Keys, sizeof(ArrayKey) * (self->KeysCount / KEY_INCREMENT + 1) * KEY_INCREMENT);

    int len = KEY ? strlen(KEY) : 0;
    ARRAY_COUNT_TYPE delta = self->KeysCount - place;
    char             dirty = 0;

    if ((self->KeysCount > DIRTY_TRESHOLD) && (delta)) {
        ARRAY_COUNT_TYPE delta_place = 0;
        place  = Array_FindPlace(self, KEY, &delta_place);
        place += delta_place;
        delta  = self->KeysCount - place;
        dirty  = 1;
    }

    if (place < self->KeysCount)
        memmove((void *)&self->Keys [place + 1], (void *)&self->Keys [place], sizeof(ArrayKey) * delta);

    self->Keys [place].KEY   = (char *)FAST_MALLOC(len + 1);
    self->Keys [place].index = self->COUNT;
    self->Keys [place].dirty = dirty;
    memcpy(self->Keys [place].KEY, KEY, len);
    self->Keys [place].KEY [len] = 0;
    self->KeysCount++;
    Array_CleanIndex(self);
    return -1;
    //return KeysCount;
}

VariableDATA *Array_ModuleGet(struct Array *self, const char *key) {
    ARRAY_COUNT_TYPE i = -1;

    i = Array_FindOrAddKey(self, key);
    if (i == -1) {
        ADD_VARIABLE(0, self->PIF);
    }

    if (i < self->COUNT) {
        ARRAY_COUNT_TYPE target_node = DYNAMIC_TARGET(i);
        ARRAY_COUNT_TYPE d_count     = DYNAMIC_DISTRIBUTION(i);

        NODE *CURRENT = self->FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], self->PIF);
        }
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (self->COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, self->PIF);
    return 0;
}

void Array_EnsureSize(struct Array *self, ARRAY_COUNT_TYPE size, VariableDATA *default_value) {
#ifdef OPTIMIZE_FAST_ARRAYS
    if ((size > STATIC_ARRAY_THRESHOLD_MINSIZE) && (!self->cached_data)) {
        self->cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));
        memset(self->cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));
    }
#endif
    ARRAY_COUNT_TYPE PREC_TARGET_NODE = -1;
    NODE *REF_NODE = 0;
    while (self->COUNT < size) {
        ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = DYNAMIC_DISTRIBUTION(self->COUNT);
        ARRAY_COUNT_TYPE TARGET_NODE = DYNAMIC_TARGET(self->COUNT);
        if (PREC_TARGET_NODE != TARGET_NODE) {
            if (TARGET_NODE >= self->NODE_COUNT) {
                NODE *NEW_NODE;
                CREATE_NODE(NEW_NODE);
                if (self->LAST)
                    self->LAST->NEXT = NEW_NODE;
                else
                    self->FIRST = NEW_NODE;
                REF_NODE = NEW_NODE;
                self->LAST     = NEW_NODE;
                if (!self->FIRST)
                    self->FIRST = NEW_NODE;
                self->NODE_COUNT++;
            } else {
                NODE *CURRENT_NODE = self->FIRST;
                for (ARRAY_COUNT_TYPE i = 0; i < TARGET_NODE; i++)
                    CURRENT_NODE = CURRENT_NODE->NEXT;
                REF_NODE = CURRENT_NODE;
            }
            PREC_TARGET_NODE = TARGET_NODE;
        }
        ENSURE_ELEMENTS(REF_NODE, DISTRIBUTED_COUNT);
        ArrayElement *ELEMENTS = REF_NODE->ELEMENTS;
        ELEMENTS [DISTRIBUTED_COUNT]        = (VariableDATA *)VAR_ALLOC(self->PIF);
        ELEMENTS [DISTRIBUTED_COUNT]->TYPE  = default_value->TYPE;
        ELEMENTS [DISTRIBUTED_COUNT]->LINKS = 1;
        if (default_value->TYPE == VARIABLE_STRING) {
            ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA     = 0;
            CONCEPT_STRING(ELEMENTS [DISTRIBUTED_COUNT], default_value);
        } else {
            if (default_value->CLASS_DATA) {
                ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = default_value->CLASS_DATA;
                if (default_value->TYPE == VARIABLE_CLASS)
                    ((struct CompiledClass *)default_value->CLASS_DATA)->LINKS++;
                else
                if (default_value->TYPE == VARIABLE_ARRAY)
                    ((struct Array *)default_value->CLASS_DATA)->LINKS++;
                else
                if (default_value->TYPE == VARIABLE_DELEGATE) {
                    ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = copy_Delegate(default_value->CLASS_DATA);
                }
            } else
            if (default_value->TYPE == VARIABLE_NUMBER)
                ELEMENTS [DISTRIBUTED_COUNT]->NUMBER_DATA = default_value->NUMBER_DATA;
        }
        ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;
        ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], self->COUNT)
        self->COUNT++;
    }
}

void Array_GO_GARBAGE(struct Array *self, void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects, char main_call) {
    if (self->LINKS < 0)
        return;

    bool single_link = ((main_call) && (self->LINKS == 1));
    self->LINKS = -1;
    NODE *CURRENT = self->FIRST;

    for (ARRAY_COUNT_TYPE i = 0; i < self->NODE_COUNT; i++) {
        for (ARRAY_COUNT_TYPE j = 0; j < CURRENT->COUNT; j++) {
            VariableDATA *Var = CURRENT->ELEMENTS [j];
            if (Var) {
                // faster, but problematic with destructor call
                if ((single_link) && (Var->LINKS == 1) && ((Var->TYPE == VARIABLE_NUMBER) || (Var->TYPE == VARIABLE_STRING)) && (!__gc_vars->IsReferenced(Var))) {
                     FREE_VARIABLE(Var);
                     CURRENT->ELEMENTS [j] = NULL;
                     continue;
                }
                if (check_objects == -1) {
                    Var->LINKS++;
                    __gc_vars->Reference(Var);
                } else {
                    if (Var->LINKS <= 1)
                        __gc_vars->Reference(Var);
                    else
                        Var->LINKS--;
                }
                if (Var->CLASS_DATA) {
                    if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                        void *CLASS_DATA = Var->CLASS_DATA;
                        if (Var->TYPE == VARIABLE_DELEGATE) {
                            CLASS_DATA = delegate_Class(CLASS_DATA);
                            free_Delegate(Var->CLASS_DATA);
                            Var->TYPE = VARIABLE_NUMBER;
                            Var->NUMBER_DATA = 0;
                        }
                        if ((check_objects == -1) || ((((struct CompiledClass *)CLASS_DATA)->reachable & check_objects) != check_objects)) {
                            __gc_obj->Reference(CLASS_DATA);
                            CompiledClass__GO_GARBAGE((struct CompiledClass *)CLASS_DATA, PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
                        } else {
                            RESET_VARIABLE(Var);
                        }
                    } else
                    if (Var->TYPE == VARIABLE_ARRAY) {
                        if ((check_objects == -1) || ((((struct Array *)Var->CLASS_DATA)->reachable & check_objects) != check_objects)) {
                            __gc_array->Reference(Var->CLASS_DATA);
                            Array_GO_GARBAGE((struct Array *)Var->CLASS_DATA, PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
                        } else {
                            RESET_VARIABLE(Var);
                        }
                    }
                }
            }
        }
        CURRENT = CURRENT->NEXT;
    }
}

VariableDATA *Array_NewArray(struct Array *self) {
    VariableDATA *DATA = (VariableDATA *)VAR_ALLOC(self->PIF);
    struct Array *newARRAY   = new_Array(self->PIF);

    DATA->TYPE               = VARIABLE_ARRAY;
    DATA->LINKS              = 0;
    DATA->CLASS_DATA         = newARRAY;
    DATA->IS_PROPERTY_RESULT = 0;

    Array_Concat(newARRAY, self);
    return DATA;
}

ARRAY_COUNT_TYPE Array_Count(struct Array *self) {
    return self->COUNT;
}

const char *Array_GetKey(struct Array *self, ARRAY_COUNT_TYPE index) {
    for (ARRAY_COUNT_TYPE i = 0; i < self->KeysCount; i++) {
        if (self->Keys [i].index == index) {
            return self->Keys [i].KEY;
        }
    }
    return 0;
}

ARRAY_COUNT_TYPE Array_GetIndex(struct Array *self, const char *Key) {
    return Array_FindKey(self, Key);
}

#define DO_LEVEL(RESULT, LEVEL)    for (int lev_index = 0; lev_index < LEVEL; lev_index++) { plainstring_add(RESULT, "  "); }

struct plainstring *Array_ToString(struct Array *self, int level, Array *parent, int parents) {
    struct plainstring *result = plainstring_new();

    DO_LEVEL(result, level)

    plainstring_add(result, "Array {\n");

    for (ARRAY_COUNT_TYPE i = 0; i < self->COUNT; i++) {
        VariableDATA *VD = Array_Get(self, i);
        if (VD) {
            DO_LEVEL(result, level)

            const char *key = Array_GetKey(self, i);
            plainstring_add(result, "  [");
            plainstring_add_double(result, i);
            if (key) {
                plainstring_add(result, ",\"");
                plainstring_add(result, key);
                plainstring_add(result, "\"] => ");
            } else {
                plainstring_add(result, "] => ");
            }
            switch (VD->TYPE) {
                case VARIABLE_NUMBER:
                    plainstring_add_double(result, VD->NUMBER_DATA);
                    break;

                case VARIABLE_STRING:
                    plainstring_add_plainstring(result, (struct plainstring *)VD->CLASS_DATA);
                    break;

                case VARIABLE_ARRAY:
                    {
                        Array *arr = (struct Array *)VD->CLASS_DATA;
                        if (arr == self) {
                            plainstring_add(result, "This array");
                        } else
                        if (arr == parent) {
                            plainstring_add(result, "Parent array");
                        } else
                        if (level > 10) {
                            plainstring_add(result, "Array");
                        } else {
                            plainstring_add(result, "\n");
                            struct plainstring *temp = Array_ToString(arr, level + 2, self);
                            plainstring_add_plainstring(result, temp);
                            plainstring_delete(temp);
                        }
                    }
                    break;

                case VARIABLE_CLASS:
                    plainstring_add(result, ((struct CompiledClass *)VD->CLASS_DATA)->_Class->NAME.c_str());
                    break;

                case VARIABLE_DELEGATE:
                    plainstring_add(result, ((struct CompiledClass *)delegate_Class(VD->CLASS_DATA))->_Class->NAME.c_str());
                    plainstring_add(result, "::");
                    plainstring_add(result, ((struct CompiledClass *)delegate_Class(VD->CLASS_DATA))->_Class->pMEMBERS [(INTEGER)delegate_Member(VD->CLASS_DATA) - 1]->NAME);
                    break;
            }
            plainstring_add(result, "\n");
        }
    }
    DO_LEVEL(result, level)

    plainstring_add(result, "}");
    return result;
}

void Array_UnlinkObjects(struct Array *self) {
    NODE *CURRENT = self->FIRST;

    for (ARRAY_COUNT_TYPE i = 0; i < self->NODE_COUNT; i++) {
        for (ARRAY_COUNT_TYPE j = 0; j < CURRENT->COUNT; j++) {
            VariableDATA *Var = CURRENT->ELEMENTS [j];
            if (Var) {
                // don't delete any object ! ... it's the garbage collector's job
                if (Var->CLASS_DATA) {
                    Var->TYPE       = VARIABLE_NUMBER;
                    Var->CLASS_DATA = 0;

                }
                FREE_VARIABLE(Var);
            }
        }
        NODE *NEXT = CURRENT->NEXT;
        if (CURRENT->ELEMENTS)
            FAST_FREE(CURRENT->ELEMENTS);
        FAST_FREE(CURRENT);
        CURRENT = NEXT;
    }
    self->COUNT      = 0;
    self->FIRST      = 0;
    self->NODE_COUNT = 0;
}

void delete_Array(struct Array *self) {
    bool delete_var = true;

    if (self->LINKS < 0)
        delete_var = false;

    self->LINKS = -1;
#ifdef OPTIMIZE_FAST_ARRAYS
    if (self->cached_data) {
        FAST_FREE(self->cached_data);
        self->cached_data = 0;
    }
#endif
    NODE *CURRENT = self->FIRST;
    for (ARRAY_COUNT_TYPE i = 0; i < self->NODE_COUNT; i++) {
        if (delete_var) {
            for (ARRAY_COUNT_TYPE j = 0; j < CURRENT->COUNT; j++) {
                VariableDATA *Var = CURRENT->ELEMENTS [j];
                if (Var) {
                    FREE_VARIABLE(Var);
                }
            }
        }
        NODE *NEXT = CURRENT->NEXT;
        if (CURRENT->ELEMENTS)
            FAST_FREE(CURRENT->ELEMENTS);
        FAST_FREE(CURRENT);
        CURRENT = NEXT;
    }
    if (self->Keys) {
        for (ARRAY_COUNT_TYPE i = 0; i < self->KeysCount; i++) {
            if (self->Keys [i].KEY)
                FAST_FREE(self->Keys [i].KEY);
        }
        FAST_FREE(self->Keys);
    }
    FreeArray(self);
}

struct Array *Array_SortArrayElementsByKey(struct Array *self) {
    Array *newARRAY = new_Array(self->PIF);
    Array_CleanIndex(self, true);
    for (ARRAY_COUNT_TYPE i = 0; i < self->KeysCount; i++) {
        Array_Add(newARRAY, Array_Get(self, self->Keys [i].index));
        Array_AddKey(newARRAY, self->Keys [i].KEY, i);
    }
    return newARRAY;
}

void Array_GetKeys(struct Array *self, char **container, int size) {
    memset(container, 0, size * sizeof(char *));
    Array_CleanIndex(self, true);
    for (ARRAY_COUNT_TYPE i = 0; i < self->KeysCount; i++) {
        if ((self->Keys [i].index >= 0) && (self->Keys [i].index < size))
            container[self->Keys [i].index] = self->Keys [i].KEY;
    }
}
