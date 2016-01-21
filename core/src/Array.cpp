#include "Array.h"
#include "ConceptInterpreter.h"
#include <string.h>

//POOLED_IMPLEMENTATION(_KEY)
//POOLED_IMPLEMENTATION(_NODE)
POOLED_IMPLEMENTATION(Array)

//--- In line functions ------------------------------------------------------------
#define CREATE_NODE(NEW_NODE)                               \
    NEW_NODE           = (NODE *)FAST_MALLOC(sizeof(NODE)); \
    NEW_NODE->ELEMENTS = NULL;                              \
    NEW_NODE->COUNT    = 0;                                 \
    NEW_NODE->NEXT     = 0;

#define ENSURE_ELEMENTS(TARGET_NODE, INDEX)                                                                                     \
    if (TARGET_NODE->COUNT <= INDEX) {                                                                                          \
        int prec_size = TARGET_NODE->COUNT;                                                                                     \
        if (COUNT >= REALLOC_TRESHOLD)                                                                                          \
            TARGET_NODE->COUNT = ARRAY_INCREMENT;                                                                               \
        else                                                                                                                    \
            TARGET_NODE->COUNT = INDEX + 1;                                                                                     \
        TARGET_NODE->ELEMENTS = (ArrayElement *)FAST_REALLOC(TARGET_NODE->ELEMENTS, TARGET_NODE->COUNT * sizeof(ArrayElement)); \
        for (int i = prec_size; i < TARGET_NODE->COUNT; i++)                                                                    \
            TARGET_NODE->ELEMENTS[i] = NULL;                                                                                    \
    }

#define ADD_LINKED_VARIABLE(var)                                  \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = COUNT % ARRAY_INCREMENT; \
    ARRAY_COUNT_TYPE TARGET_NODE = COUNT / ARRAY_INCREMENT;       \
    NODE             *REF_NODE;                                   \
    if (TARGET_NODE >= NODE_COUNT) {                              \
        NODE *NEW_NODE;                                           \
        CREATE_NODE(NEW_NODE);                                    \
        if (LAST)                                                 \
            LAST->NEXT = NEW_NODE;                                \
        else                                                      \
            FIRST = NEW_NODE;                                     \
        REF_NODE = NEW_NODE;                                      \
        LAST     = NEW_NODE;                                      \
        if (!FIRST)                                               \
            FIRST = NEW_NODE;                                     \
        NODE_COUNT++;                                             \
    } else {                                                      \
        if ((TARGET_NODE == NODE_COUNT - 1) && (LAST)) {          \
            REF_NODE = LAST;                                      \
        } else {                                                  \
            NODE *CURRENT_NODE = FIRST;                           \
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
    COUNT++;                                                      \
    return ELEMENTS [DISTRIBUTED_COUNT];

#define ADD_COPY_VARIABLE(var, pif)                                         \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = COUNT % ARRAY_INCREMENT;           \
    ARRAY_COUNT_TYPE TARGET_NODE = COUNT / ARRAY_INCREMENT;                 \
    NODE             *REF_NODE;                                             \
    if (TARGET_NODE >= NODE_COUNT) {                                        \
        NODE *NEW_NODE;                                                     \
        CREATE_NODE(NEW_NODE);                                              \
        if (LAST)                                                           \
            LAST->NEXT = NEW_NODE;                                          \
        else                                                                \
            FIRST = NEW_NODE;                                               \
        REF_NODE = NEW_NODE;                                                \
        LAST     = NEW_NODE;                                                \
        if (!FIRST)                                                         \
            FIRST = NEW_NODE;                                               \
        NODE_COUNT++;                                                       \
    } else {                                                                \
        if ((TARGET_NODE == NODE_COUNT - 1) && (LAST)) {                    \
            REF_NODE = LAST;                                                \
        } else {                                                            \
            NODE *CURRENT_NODE = FIRST;                                     \
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
        CONCEPT_STRING(ELEMENTS [DISTRIBUTED_COUNT]) = CONCEPT_STRING(var); \
    } else                                                                  \
    if (var->TYPE == VARIABLE_CLASS) {                                      \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = var->CLASS_DATA;         \
        ((CompiledClass *)var->CLASS_DATA)->LINKS++;                        \
    } else                                                                  \
    if (var->TYPE == VARIABLE_ARRAY) {                                      \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = var->CLASS_DATA;         \
        ((Array *)var->CLASS_DATA)->LINKS++;                                \
    } else                                                                  \
    if (var->TYPE == VARIABLE_DELEGATE) {                                   \
        ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA    = var->CLASS_DATA;      \
        ELEMENTS [DISTRIBUTED_COUNT]->DELEGATE_DATA = var->DELEGATE_DATA;   \
        ((CompiledClass *)var->CLASS_DATA)->LINKS++;                        \
    }                                                                       \
    ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;                   \
    COUNT++;                                                                \
    ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], COUNT);               \
                                                                            \
    return ELEMENTS [DISTRIBUTED_COUNT];

#define ADD_VARIABLE(val, pif)                                            \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = COUNT % ARRAY_INCREMENT;         \
    ARRAY_COUNT_TYPE TARGET_NODE = COUNT / ARRAY_INCREMENT;               \
    NODE             *REF_NODE;                                           \
    if (TARGET_NODE >= NODE_COUNT) {                                      \
        NODE *NEW_NODE;                                                   \
        CREATE_NODE(NEW_NODE);                                            \
        if (LAST)                                                         \
            LAST->NEXT = NEW_NODE;                                        \
        else                                                              \
            FIRST = NEW_NODE;                                             \
        REF_NODE = NEW_NODE;                                              \
        LAST     = NEW_NODE;                                              \
        NODE_COUNT++;                                                     \
    } else {                                                              \
        if ((TARGET_NODE == NODE_COUNT - 1) && (LAST)) {                  \
            REF_NODE = LAST;                                              \
        } else {                                                          \
            NODE *CURRENT_NODE = FIRST;                                   \
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
    ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], COUNT);             \
    COUNT++;                                                              \
    return ELEMENTS [DISTRIBUTED_COUNT];
\

#define ADD_MULTIPLE_VARIABLE2                                    \
    ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = COUNT % ARRAY_INCREMENT; \
    ARRAY_COUNT_TYPE TARGET_NODE = COUNT / ARRAY_INCREMENT;       \
    if (TARGET_NODE >= NODE_COUNT) {                              \
        NODE *NEW_NODE;                                           \
        CREATE_NODE(NEW_NODE);                                    \
        if (LAST)                                                 \
            LAST->NEXT = NEW_NODE;                                \
        else                                                      \
            FIRST = NEW_NODE;                                     \
        LAST = NEW_NODE;                                          \
        NODE_COUNT++;                                             \
    }                                                             \
    COUNT++;                                                      \

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
    if ((COUNT > STATIC_ARRAY_THRESHOLD_MINSIZE) && (i < STATIC_ARRAY_THRESHOLD)) {                      \
        if (cached_data) {                                                                               \
            VariableDATA *res = cached_data[i];                                                          \
            if (res)                                                                                     \
                return res;                                                                              \
        } else {                                                                                         \
            cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *)); \
            memset(cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));                     \
        }                                                                                                \
    }

 #define ARRAY_CACHED_RETURN(VAR, i_i)                   \
    if ((cached_data) && (i_i < STATIC_ARRAY_THRESHOLD)) \
        cached_data[i_i] = VAR;

 #define CACHE_CREATE_CHECK                                                                          \
    if ((!cached_data) && (COUNT > STATIC_ARRAY_THRESHOLD_MINSIZE)) {                                \
        cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *)); \
        memset(cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));                     \
    }
#else
 #define CACHE_ARRAY_BLOCK
 #define ARRAY_CACHED_RETURN(VAR, i_i)
 #define CACHE_CREATE_CHECK
#endif

Array::Array(void *PIF) {
#ifdef OPTIMIZE_FAST_ARRAYS
    LastNodeIndex = -1;
    LastNode      = 0;
    cached_data   = 0;
#endif

    COUNT = 0;
    FIRST = 0;

#ifdef STDMAP_KEYS
    Keys = 0;
#else
    Keys      = 0;
    KeysCount = 0;
    LastKey   = 0;
#endif
    LINKS      = 1;
    LAST       = FIRST;
    NODE_COUNT = 0;
    this->PIF  = PIF;
}

ARRAY_COUNT_TYPE Array::FindIndex(ARRAY_COUNT_TYPE index) {
    if (index < COUNT) {
        ARRAY_COUNT_TYPE target_node = index / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = index % ARRAY_INCREMENT;
        NODE             *CURRENT    = FIRST;
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

#ifdef STDMAP_KEYS
ARRAY_COUNT_TYPE Array::FindKey(char *KEY) {
    if (!Keys)
        return -1;

    KeyMap::iterator iter = Keys->find(KEY);
    if (iter != Keys->end())
        return iter->second;
    return -1;
}

#else

ARRAY_COUNT_TYPE Array::FindKey(char *KEY) {
    if (!KeysCount) {
        return -1;
    }

    char *str = KEY;
    if ((KeysCount > 10) && (LastKey != -1)) {
        if (LastKey < KeysCount) {
            if (!strcmp(str, Keys [LastKey].KEY)) {
                return Keys [LastKey].index;
            }
        } else
        if (LastKey < KeysCount - 1) {
            if (!strcmp(str, Keys [++LastKey].KEY)) {
                return Keys [LastKey].index;
            }
        }
    }
    ARRAY_COUNT_TYPE LocalKeysCount = this->KeysCount;
    for (int i = LocalKeysCount - 1; i >= 0; i--) {
        if (Keys [i].dirty) {
            LocalKeysCount--;
            // the strcmp part should be moved AFTER binary lookup
            // it would make more sense this way
            if (!strcmp(str, Keys [i].KEY))
                return Keys [i].index;
        } else
            break;
    }

    LastKey = -1;

    ARRAY_COUNT_TYPE START = LocalKeysCount / 2;
    ARRAY_COUNT_TYPE DELTA = LocalKeysCount - START;
    while (1) {
        ARRAY_COUNT_TYPE order;
        if ((START < 0) || (START >= LocalKeysCount)) {
            return -1;
        }
        order = strcmp(str, Keys [START].KEY);
        if (DELTA) {
            if (!order) {
                LastKey = START;
                return Keys [START].index;
            }

            ARRAY_COUNT_TYPE PREC_START = START;
            if (order > 0) {
                START += DELTA;
                if (START >= LocalKeysCount) {
                    START = LocalKeysCount - 1;
                    if (PREC_START == START)
                        return -1;
                }
            } else {
                START -= DELTA;
                if (START < 0) {
                    START = 0;
                    if (PREC_START == START)
                        return -1;
                }
            }
            DELTA = DELTA == 1 ? 0 : DELTA / 2 + DELTA % 2;
        } else {
            if (!order) {
                LastKey = START;
                return Keys [START].index;
            }
            return -1;
        }
    }
    return -1;
}
#endif

#ifndef STDMAP_KEYS
ARRAY_COUNT_TYPE Array::FindPlace(char *KEY, ARRAY_COUNT_TYPE *in_dirty_zone) {
    if (!KeysCount)
        return 0;

    ARRAY_COUNT_TYPE LocalKeysCount = this->KeysCount;

    // ugly !
    ArrayKey *TargetKeys = this->Keys;

    if (in_dirty_zone) {
        LocalKeysCount = 0;
        for (int i = KeysCount - 1; i >= 0; i--) {
            if (Keys [i].dirty) {
                LocalKeysCount++;
                TargetKeys = &Keys[i];
            } else
                break;
        }
        if (!LocalKeysCount) {
            *in_dirty_zone = 0;
            return KeysCount;
        }
        *in_dirty_zone = KeysCount - LocalKeysCount;
    } else {
        for (int i = LocalKeysCount - 1; i >= 0; i--) {
            if (Keys [i].dirty)
                LocalKeysCount--;
            else
                break;
        }
    }

    char *str = KEY;
    if (LocalKeysCount > 10) {
        if (strcmp(str, TargetKeys [LocalKeysCount - 1].KEY) > 0) {
            return LocalKeysCount;
        }
    }

    ARRAY_COUNT_TYPE START = LocalKeysCount / 2;
    ARRAY_COUNT_TYPE DELTA = LocalKeysCount - START;
    while (1) {
        ARRAY_COUNT_TYPE order;
        if (START < 0) {
            return 0;
        }
        if (START >= LocalKeysCount) {
            return LocalKeysCount;
        }
        if (!DELTA) {
            order  = strcmp(str, TargetKeys [START].KEY);
            START += (order  >= 0);
            if (START < 0) {
                START = 0;
            } else
            if (START >= LocalKeysCount) {
                START = LocalKeysCount;
            }
            return START;
        } else {
            order = strcmp(str, TargetKeys [START].KEY);
            ARRAY_COUNT_TYPE PREC_START = START;
            if (order > 0) {
                START += DELTA;
                if (START >= LocalKeysCount) {
                    START = LocalKeysCount - 1;
                    if (PREC_START == START) {
                        START++;
                        return START;
                    }
                }
            } else {
                START -= DELTA;
                if (START < 0) {
                    START = 0;
                    if (PREC_START == START) {
                        START += (order >= 0);
                        return START;
                    }
                }
            }

            DELTA = DELTA == 1 ? 0 : DELTA / 2 + DELTA % 2;
        }
    }
    return START;
}

void Array::CleanIndex(bool forced) {
    ArrayKey         DirtyKeys[MAX_DIRTY_DELTA];
    ARRAY_COUNT_TYPE indexes[MAX_DIRTY_DELTA];
    ARRAY_COUNT_TYPE count         = 0;
    bool             time_to_clean = false;

    for (int i = KeysCount - 1; i >= 0; i--) {
        if (Keys [i].dirty) {
            count++;
            if (count >= MAX_DIRTY_DELTA) {
                time_to_clean = true;
                break;
            }
        } else
            break;
    }
    if ((count) && ((forced) || (time_to_clean))) {
        ARRAY_COUNT_TYPE delta = KeysCount - count;
        memcpy(DirtyKeys, &Keys[delta], sizeof(ArrayKey) * count);

        for (int i = 0; i < count; i++) {
            indexes[i]         = FindPlace(DirtyKeys[i].KEY);
            DirtyKeys[i].dirty = 0;
        }

        ARRAY_COUNT_TYPE array_end  = KeysCount;
        ARRAY_COUNT_TYPE tail_end   = KeysCount - count;
        ARRAY_COUNT_TYPE next_space = -1;
        for (int i = count - 1; i >= 0; i--) {
            // copy tail
            ARRAY_COUNT_TYPE place = indexes[i];

            ARRAY_COUNT_TYPE delta = tail_end - place;

            ARRAY_COUNT_TYPE offset = array_end - delta;
            if (delta) {
                memmove(&Keys [offset], &Keys [place], sizeof(ArrayKey) * delta);
                next_space = offset - 1;
            }
            if (next_space < 0)
                next_space = offset - 1;

            if (Keys[next_space].KEY == DirtyKeys[i].KEY)
                Keys[next_space].dirty = 0;
            else
                memcpy(&Keys[next_space], &DirtyKeys[i], sizeof(ArrayKey));

            tail_end  = place;
            array_end = next_space;

            next_space--;
            if (next_space < 0)
                next_space = 0;
        }
    }
}
#endif

ARRAY_COUNT_TYPE Array::AddKey(AnsiString *KEY, ARRAY_COUNT_TYPE index) {
#ifdef STDMAP_KEYS
    if (!Keys)
        Keys = new KeyMap();

    int  len  = KEY->Length();
    char *buf = (char *)FAST_MALLOC(len + 1);
    memcpy(buf, KEY->c_str(), len);
    buf[len] = 0;
    Keys->insert(KeyMapPair(buf, index));
    return Keys->size();
#else
    if (!(KeysCount % KEY_INCREMENT))
        Keys = (ArrayKey *)FAST_REALLOC(Keys, sizeof(ArrayKey) * (KeysCount / KEY_INCREMENT + 1) * KEY_INCREMENT);
    ARRAY_COUNT_TYPE place = FindPlace(KEY->c_str());

    int len = KEY->Length();
    ARRAY_COUNT_TYPE delta = KeysCount - place;
    char             dirty = 0;

    if ((KeysCount > DIRTY_TRESHOLD) && (delta)) {
        ARRAY_COUNT_TYPE delta_place = 0;
        place  = FindPlace(KEY->c_str(), &delta_place);
        place += delta_place;
        delta  = KeysCount - place;
        dirty  = 1;
    }

    if (place < KeysCount)
        memmove((void *)&Keys [place + 1], (void *)&Keys [place], sizeof(ArrayKey) * delta);

    Keys [place].KEY   = (char *)FAST_MALLOC(len + 1);
    Keys [place].index = index;
    Keys [place].dirty = dirty;
    memcpy(Keys [place].KEY, KEY->c_str(), len);
    Keys [place].KEY [len] = 0;
    KeysCount++;
    CleanIndex();

    return KeysCount;
#endif
}

VariableDATA *Array::Add(VariableDATA *VAR_TO_ADD) {
    ADD_LINKED_VARIABLE(VAR_TO_ADD);
    return 0;
}

VariableDATA *Array::AddCopy(VariableDATA *VAR_TO_ADD) {
    ADD_COPY_VARIABLE(VAR_TO_ADD, PIF);
    return 0;
}

Array *Array::Concat(Array *array) {
    ARRAY_COUNT_TYPE count = array->COUNT;

    for (ARRAY_COUNT_TYPE i = 0; i < count; i++) {
        Add(array->Get(i));
    }
    return this;
}

VariableDATA *Array::Get(ARRAY_COUNT_TYPE i) {
    if (i < 0) {
        return 0;
    }
    if (i < COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = i / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = i % ARRAY_INCREMENT;

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (LastNodeIndex == target_node) {
            CURRENT = LastNode;
        } else
        if ((LastNodeIndex != -1) && (LastNodeIndex < target_node)) {
            CURRENT = LastNode;
            for (ARRAY_COUNT_TYPE k = LastNodeIndex; k < target_node; k++)
                CURRENT = CURRENT->NEXT;
            LastNode      = CURRENT;
            LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++)
            CURRENT = CURRENT->NEXT;

#ifdef OPTIMIZE_FAST_ARRAYS
        LastNode      = CURRENT;
        LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }
    return 0;
}

VariableDATA *Array::Get(VariableDATA *KEY) {
    ARRAY_COUNT_TYPE i = -1;

    if (KEY->TYPE == VARIABLE_NUMBER) {
        i = (ARRAY_COUNT_TYPE)KEY->NUMBER_DATA;
        if (i < 0) {
            return 0;
        }
    } else
    if (KEY->TYPE == VARIABLE_STRING) {
        i = FindKey(CONCEPT_STRING(KEY).c_str());

        if (i == -1) {
            AddKey(&CONCEPT_STRING(KEY), COUNT);
            ADD_VARIABLE(0, PIF);
        }
    }

    if (i < COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = i / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = i % ARRAY_INCREMENT;

        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (LastNodeIndex == target_node) {
            CURRENT = LastNode;
        } else
        if ((LastNodeIndex != -1) && (LastNodeIndex < target_node)) {
            CURRENT = LastNode;
            for (ARRAY_COUNT_TYPE k = LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            LastNode      = CURRENT;
            LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }

#ifdef OPTIMIZE_FAST_ARRAYS
        LastNode      = CURRENT;
        LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], PIF);
        }
        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, PIF);
    return 0;
}

VariableDATA *Array::GetWithCreate(ARRAY_COUNT_TYPE i, NUMBER default_value) {
    if (i < COUNT) {
        CACHE_ARRAY_BLOCK;

        ARRAY_COUNT_TYPE target_node = i / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = i % ARRAY_INCREMENT;
        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (LastNodeIndex == target_node) {
            CURRENT = LastNode;
        } else
        if ((LastNodeIndex != -1) && (LastNodeIndex < target_node)) {
            CURRENT = LastNode;
            for (ARRAY_COUNT_TYPE k = LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            LastNode      = CURRENT;
            LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }

#ifdef OPTIMIZE_FAST_ARRAYS
        LastNode      = CURRENT;
        LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], PIF);
        }

        ARRAY_CACHED_RETURN(ELEMENTS [d_count], i);
        return ELEMENTS [d_count];
    }

    ARRAY_COUNT_TYPE target = i;
    while (COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(default_value, PIF);
    return 0;
}

VariableDATA *Array::ModuleGet(ARRAY_COUNT_TYPE i) {
    if (i < COUNT) {
        ARRAY_COUNT_TYPE target_node = i / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = i % ARRAY_INCREMENT;
        NODE *CURRENT;
#ifdef OPTIMIZE_FAST_ARRAYS
        if (LastNodeIndex == target_node) {
            CURRENT = LastNode;
        } else
        if ((LastNodeIndex != -1) && (LastNodeIndex < target_node)) {
            CURRENT = LastNode;
            for (ARRAY_COUNT_TYPE k = LastNodeIndex; k < target_node; k++) {
                CURRENT = CURRENT->NEXT;
            }
            LastNode      = CURRENT;
            LastNodeIndex = target_node;
        } else {
#endif
        CURRENT = FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }
#ifdef OPTIMIZE_FAST_ARRAYS
        LastNode      = CURRENT;
        LastNodeIndex = target_node;
    }
#endif
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], PIF);
        }
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, PIF);
    return 0;
}

VariableDATA *Array::ModuleGet(char *key) {
    ARRAY_COUNT_TYPE i = -1;

    i = FindKey(key);

    if (i == -1) {
        AnsiString tmp(key);
        AddKey(&tmp, COUNT);
        ADD_VARIABLE(0, PIF);
    }

    if (i < COUNT) {
        ARRAY_COUNT_TYPE target_node = i / ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE d_count     = i % ARRAY_INCREMENT;
        NODE *CURRENT = FIRST;
        for (ARRAY_COUNT_TYPE k = 0; k < target_node; k++) {
            CURRENT = CURRENT->NEXT;
        }
        ENSURE_ELEMENTS(CURRENT, d_count);
        ArrayElement *ELEMENTS = CURRENT->ELEMENTS;
        if (!ELEMENTS [d_count]) {
            CREATE_VARIABLE(ELEMENTS [d_count], PIF);
        }
        return ELEMENTS [d_count];
    }
    ARRAY_COUNT_TYPE target = i;
    while (COUNT < target) {
        ADD_MULTIPLE_VARIABLE2;
    }
    ADD_VARIABLE(0, PIF);
    return 0;
}

void Array::EnsureSize(ARRAY_COUNT_TYPE size, VariableDATA *default_value) {
#ifdef OPTIMIZE_FAST_ARRAYS
    if ((size > STATIC_ARRAY_THRESHOLD_MINSIZE) && (!cached_data)) {
        cached_data = (VariableDATA **)FAST_MALLOC(STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));
        memset(cached_data, 0, STATIC_ARRAY_THRESHOLD * sizeof(VariableDATA *));
    }
#endif
    ARRAY_COUNT_TYPE PREC_TARGET_NODE = -1;
    NODE *REF_NODE = 0;
    while (COUNT < size) {
        ARRAY_COUNT_TYPE DISTRIBUTED_COUNT = COUNT % ARRAY_INCREMENT;
        ARRAY_COUNT_TYPE TARGET_NODE       = COUNT / ARRAY_INCREMENT;
        if (PREC_TARGET_NODE != TARGET_NODE) {
            if (TARGET_NODE >= NODE_COUNT) {
                NODE *NEW_NODE;
                CREATE_NODE(NEW_NODE);
                if (LAST)
                    LAST->NEXT = NEW_NODE;
                else
                    FIRST = NEW_NODE;
                REF_NODE = NEW_NODE;
                LAST     = NEW_NODE;
                if (!FIRST)
                    FIRST = NEW_NODE;
                NODE_COUNT++;
            } else {
                NODE *CURRENT_NODE = FIRST;
                for (ARRAY_COUNT_TYPE i = 0; i < TARGET_NODE; i++)
                    CURRENT_NODE = CURRENT_NODE->NEXT;
                REF_NODE = CURRENT_NODE;
            }
            PREC_TARGET_NODE = TARGET_NODE;
        }
        ENSURE_ELEMENTS(REF_NODE, DISTRIBUTED_COUNT);
        ArrayElement *ELEMENTS = REF_NODE->ELEMENTS;
        ELEMENTS [DISTRIBUTED_COUNT]        = (VariableDATA *)VAR_ALLOC(PIF);
        ELEMENTS [DISTRIBUTED_COUNT]->TYPE  = default_value->TYPE;
        ELEMENTS [DISTRIBUTED_COUNT]->LINKS = 1;
        if (default_value->TYPE == VARIABLE_STRING) {
            ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA     = 0;
            CONCEPT_STRING(ELEMENTS [DISTRIBUTED_COUNT]) = CONCEPT_STRING(default_value);
        } else {
            if (default_value->CLASS_DATA) {
                ELEMENTS [DISTRIBUTED_COUNT]->CLASS_DATA = default_value->CLASS_DATA;
                if (default_value->TYPE == VARIABLE_CLASS)
                    ((CompiledClass *)default_value->CLASS_DATA)->LINKS++;
                else
                if (default_value->TYPE == VARIABLE_ARRAY)
                    ((Array *)default_value->CLASS_DATA)->LINKS++;
                else
                if (default_value->TYPE == VARIABLE_DELEGATE) {
                    ((CompiledClass *)default_value->CLASS_DATA)->LINKS++;
                    ELEMENTS [DISTRIBUTED_COUNT]->DELEGATE_DATA = default_value->DELEGATE_DATA;
                }
            } else
            if (default_value->TYPE == VARIABLE_NUMBER)
                ELEMENTS [DISTRIBUTED_COUNT]->NUMBER_DATA = default_value->NUMBER_DATA;
        }
        ELEMENTS [DISTRIBUTED_COUNT]->IS_PROPERTY_RESULT = 0;
        ARRAY_CACHED_RETURN(ELEMENTS [DISTRIBUTED_COUNT], COUNT)
        COUNT++;
    }
}

void Array::__GO_GARBAGE(void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects) {
    if (this->LINKS < 0)
        return;

    this->LINKS = -1;
    NODE *CURRENT = FIRST;

    for (ARRAY_COUNT_TYPE i = 0; i < NODE_COUNT; i++) {
        for (ARRAY_COUNT_TYPE j = 0; j < CURRENT->COUNT; j++) {
            VariableDATA *Var = CURRENT->ELEMENTS [j];
            if (Var) {
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
                        if ((check_objects == -1) || ((((CompiledClass *)Var->CLASS_DATA)->reachable & check_objects) != check_objects)) {
                            __gc_obj->Reference(Var->CLASS_DATA);
                            ((CompiledClass *)Var->CLASS_DATA)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
                        } else {
                            RESET_VARIABLE(Var);
                        }
                    } else
                    if (Var->TYPE == VARIABLE_ARRAY) {
                        if ((check_objects == -1) || ((((Array *)Var->CLASS_DATA)->reachable & check_objects) != check_objects)) {
                            __gc_array->Reference(Var->CLASS_DATA);
                            ((Array *)Var->CLASS_DATA)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
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

VariableDATA *Array::NewArray() {
    VariableDATA *DATA = (VariableDATA *)VAR_ALLOC(PIF);
    Array *newARRAY    = new(AllocArray(PIF))Array(PIF);

    DATA->TYPE               = VARIABLE_ARRAY;
    DATA->LINKS              = 0;
    DATA->CLASS_DATA         = newARRAY;
    DATA->IS_PROPERTY_RESULT = 0;

    newARRAY->Concat(this);
    return DATA;
}

ARRAY_COUNT_TYPE Array::Count() {
    return COUNT;
}

char *Array::GetKey(ARRAY_COUNT_TYPE index) {
#ifdef STDMAP_KEYS
    if (!Keys)
        return 0;

    KeyMap::iterator end = Keys->end();
    for (KeyMap::iterator iter = Keys->begin(); iter != end; ++iter) {
        if (iter->second == index)
            return (char *)iter->first;
    }
    return 0;
#else
    for (ARRAY_COUNT_TYPE i = 0; i < KeysCount; i++) {
        if (Keys [i].index == index) {
            return Keys [i].KEY;
        }
    }
    return 0;
#endif
}

ARRAY_COUNT_TYPE Array::GetIndex(char *Key) {
#ifdef STDMAP_KEYS
    if (!Keys)
        return -1;

    KeyMap::iterator iter = Keys->find(Key);
    if (iter != Keys->end())
        return iter->second;
    return -1;
#else
    return this->FindKey(Key);
#endif
}

#define DO_LEVEL(RESULT, LEVEL)    for (int lev_index = 0; lev_index < LEVEL; lev_index++) { RESULT += "  "; }

AnsiString Array::ToString(int level, Array *parent, int parents) {
    AnsiString result;

    DO_LEVEL(result, level)

    result += "Array {\n";

    for (ARRAY_COUNT_TYPE i = 0; i < COUNT; i++) {
        VariableDATA *VD = this->Get(i);
        if (VD) {
            DO_LEVEL(result, level)

            AnsiString key = AnsiString(GetKey(i));
            result        += "  ";
            result        += "[";
            result        += AnsiString(i);
            if (key != NULL_STRING) {
                result += ",\"";
                result += key;
                result += "\"] => ";
            } else {
                result += "] => ";
            }
            switch (VD->TYPE) {
                case VARIABLE_NUMBER:
                    result += AnsiString(VD->NUMBER_DATA);
                    break;

                case VARIABLE_STRING:
                    result += CONCEPT_STRING(VD);
                    break;

                case VARIABLE_ARRAY:
                    {
                        Array *arr = (Array *)VD->CLASS_DATA;
                        if (arr == this) {
                            result += "This array";
                        } else
                        if (arr == parent) {
                            result += "Parent array";
                        } else
                        if (level > 10) {
                            result += "Array";
                        } else {
                            result += "\n";
                            result += arr->ToString(level + 2, this);
                        }
                    }
                    break;

                case VARIABLE_CLASS:
                    result += ((CompiledClass *)VD->CLASS_DATA)->_Class->NAME.c_str();
                    break;

                case VARIABLE_DELEGATE:
                    result += ((CompiledClass *)VD->CLASS_DATA)->_Class->NAME.c_str();
                    result += "::";
                    result += ((CompiledClass *)VD->CLASS_DATA)->_Class->pMEMBERS [(INTEGER)VD->DELEGATE_DATA - 1]->NAME;
                    break;
            }
            result += "\n";
        }
    }
    DO_LEVEL(result, level)

    result += "}";
    return result;
}

void Array::UnlinkObjects() {
    NODE *CURRENT = FIRST;

    for (ARRAY_COUNT_TYPE i = 0; i < NODE_COUNT; i++) {
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
    COUNT      = 0;
    FIRST      = 0;
    NODE_COUNT = 0;
}

Array::~Array() {
    bool delete_var = true;

    if (this->LINKS < 0)
        delete_var = false;

    this->LINKS = -1;
#ifdef OPTIMIZE_FAST_ARRAYS
    if (cached_data) {
        FAST_FREE(cached_data);
        cached_data = 0;
    }
#endif
    NODE *CURRENT = FIRST;
    for (ARRAY_COUNT_TYPE i = 0; i < NODE_COUNT; i++) {
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
#ifdef STDMAP_KEYS
    if (Keys) {
        KeyMap::iterator end = Keys->end();
        for (KeyMap::iterator iter = Keys->begin(); iter != end; ++iter) {
            FAST_FREE((char *)iter->first);
        }
        delete Keys;
    }
#else
    if (Keys) {
        for (ARRAY_COUNT_TYPE i = 0; i < KeysCount; i++) {
            if (Keys [i].KEY)
                FAST_FREE(Keys [i].KEY);
        }
        FAST_FREE(Keys);
    }
#endif
}

Array *Array::SortArrayElementsByKey() {
    Array *newARRAY = new(AllocArray(PIF))Array(PIF);

#ifdef STDMAP_KEYS
    if (Keys) {
        KeyMap::iterator end = Keys->end();
        AnsiString key;
        ARRAY_COUNT_TYPE i = 0;
        for (KeyMap::iterator iter = Keys->begin(); iter != end; ++iter) {
            newARRAY->Add(Get(iter->second));
            key = (char *)iter->first;
            newARRAY->AddKey(&key, i++);
        }
    }
#else
    CleanIndex(true);
    for (ARRAY_COUNT_TYPE i = 0; i < KeysCount; i++) {
        AnsiString key = Keys [i].KEY;
        newARRAY->Add(Get(Keys [i].index));
        newARRAY->AddKey(&key, i);
    }
#endif

    return newARRAY;
}

void Array::GetKeys(char **container, int size) {
    memset(container, 0, size * sizeof(char *));
#ifdef STDMAP_KEYS
    if (Keys) {
        KeyMap::iterator end = Keys->end();
        AnsiString key;
        ARRAY_COUNT_TYPE i = 0;
        for (KeyMap::iterator iter = Keys->begin(); iter != end; ++iter) {
            key = (char *)iter->first;
            if ((iter->second >= 0) && (iter->second < size))
                container[iter->second] = key;
        }
    }
#else
    CleanIndex(true);
    for (ARRAY_COUNT_TYPE i = 0; i < KeysCount; i++) {
        if ((Keys [i].index >= 0) && (Keys [i].index < size))
            container[Keys [i].index] = Keys [i].KEY;
    }
#endif
}

