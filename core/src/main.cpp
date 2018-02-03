//#define DEBUG
//#define TIMMING
#ifdef _WIN32
 #include <windows.h>
#endif
#include "ConceptPools.h"
#include "SHManager.h"
#include "AnsiString.h"
#include "PIFAlizator.h"
#include "library.h"
#include "Debugger.h"
#include "ConceptTypes.h"
#include "ConceptInterpreter.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
 #include <io.h>

 #include <sys/utime.h>
#else
 #include <unistd.h>
 #include <utime.h>
#endif
#include <stdio.h>
#include <string>
#include <stdlib.h>

#ifdef TIMMING
 #include <time.h>
#endif

POOLED_IMPLEMENTATION(tsVariableDESCRIPTOR);
POOLED_IMPLEMENTATION(tsVariableDATA);
POOLED_IMPLEMENTATION(tsUndefinedMember)
POOLED_IMPLEMENTATION(tsAnalizerElement)
POOLED_IMPLEMENTATION(tsSHORT_AnalizerElement)
POOLED_IMPLEMENTATION(tsPartialAnalizerElement)
POOLED_IMPLEMENTATION(tsOptimizedElement)
POOLED_IMPLEMENTATION(tsParamList)

THREADED_FLAG NOTIFY_PARENT RemoteNotifyParent = 0;
THREADED_FLAG void *cache_buffer = 0;

struct Container {
    void *R1;
    void *R2;
    void *R3;
    char is_cgi;
};

static int last_result        = 0;
static int cached_direct_pipe = -1;

void *GetPOOLContext(void *refVAR) {
    if ((!refVAR) || (((VariableDATA *)refVAR)->flags < 0))
        return 0;

    VARPool *CURRENT = (VARPool *)((((uintptr_t)refVAR) - sizeof(VariableDATA) * (((VariableDATA *)refVAR)->flags) - POOL_OFFSET(VARPool, POOL)));
    if (CURRENT)
        return CURRENT->PIF;

    return 0;
}

void VarClean(PIFAlizator *pif) {
    VARPool *CURRENT   = pif->POOL;
    VARPool *NEXT      = (VARPool *)CURRENT->NEXT;
    int     iterations = 0;

    while (NEXT) {
        // skip first block
        CURRENT = NEXT;
        NEXT    = (VARPool *)CURRENT->NEXT;
        if (CURRENT->POOL_VARS == POOL_BLOCK_SIZE) {
            VARPool *PREV = (VARPool *)CURRENT->PREV;

            PREV->NEXT = NEXT;
            if (NEXT)
                NEXT->PREV = PREV;
            pif->CACHEDPOOL = NULL;
            ((PIFAlizator *)CURRENT->PIF)->free_vars -= POOL_BLOCK_SIZE;

            if (((PIFAlizator *)CURRENT->PIF)->free_vars <= POOL_BLOCK_SIZE)
                NEXT = NULL;
            FAST_FREE(CURRENT);
            iterations = 0;
        } else {
            iterations++;
            if (iterations == 5)
                break;
        }
    }
}

void RemoveBlock(VARPool *CURRENT) {
    VARPool *PREV = (VARPool *)CURRENT->PREV;
    VARPool *NEXT = (VARPool *)CURRENT->NEXT;
    if (PREV)
        PREV->NEXT = NEXT;
    if (NEXT)
        NEXT->PREV = CURRENT->PREV;
    ((PIFAlizator *)CURRENT->PIF)->free_vars -= POOL_BLOCK_SIZE;
    if (((PIFAlizator *)CURRENT->PIF)->CACHEDPOOL == CURRENT)
        ((PIFAlizator *)CURRENT->PIF)->CACHEDPOOL = NULL;
    if (((PIFAlizator *)CURRENT->PIF)->POOL == CURRENT)
        ((PIFAlizator *)CURRENT->PIF)->POOL = NEXT;
    FAST_FREE(CURRENT);
}

void FreeMultipleVars(void **refVARs, int count) {
    VariableDATA **CONTEXT = (VariableDATA **)refVARs;
    VARPool      *CURRENT  = NULL;
    PIFAlizator  *pif      = NULL;

    for (int i = count - 1; i >= 0; i--) {
        VariableDATA *refVAR = CONTEXT[i];
        if ((refVAR) && (refVAR->LINKS <= 0)) {
            if (refVAR->flags >= 0) {
                CURRENT = (VARPool *)(((uintptr_t)refVAR) - sizeof(VariableDATA) * (((VariableDATA *)refVAR)->flags) - POOL_OFFSET(VARPool, POOL));
                if (((VariableDATA *)refVAR)->flags < CURRENT->FIRST_VAR)
                    CURRENT->FIRST_VAR = ((VariableDATA *)refVAR)->flags;
                pif = (PIFAlizator *)CURRENT->PIF;
                pif->free_vars++;
                ((VariableDATA *)refVAR)->flags = -1;
                CURRENT->POOL_VARS++;
                if (CURRENT->POOL_VARS == POOL_BLOCK_SIZE) {
                    RemoveBlock(CURRENT);
                    if (CURRENT == pif->CACHEDPOOL)
                        pif->CACHEDPOOL = NULL;
                    CURRENT = NULL;
                }
            } else
            if (refVAR->flags == -2) {
                // module allocated!
                FAST_FREE(refVAR);
            }
        }
    }
    /*if (pif) {
        if (pif->free_vars > BLOCK_SIZE * 10)
           VarClean(pif);
        else
           pif->CACHEDPOOL = CURRENT;
    }*/
}

void FreeVAR(void *refVAR) {
    if (!refVAR)
        return;

    if (((VariableDATA *)refVAR)->flags == -1)
        return;

    if (((VariableDATA *)refVAR)->flags == -2) {
        // not a pooled variable (module-created variable) for backwards compatibility
        FAST_FREE(refVAR);
        return;
    }

    VARPool *CURRENT = (VARPool *)(((uintptr_t)refVAR) - sizeof(VariableDATA) * (((VariableDATA *)refVAR)->flags) - POOL_OFFSET(VARPool, POOL));
    ((PIFAlizator *)CURRENT->PIF)->free_vars++;
    if (((VariableDATA *)refVAR)->flags < CURRENT->FIRST_VAR)
        CURRENT->FIRST_VAR = ((VariableDATA *)refVAR)->flags;
    ((VariableDATA *)refVAR)->flags = -1;
    CURRENT->POOL_VARS++;
    VARPool *PREV = (VARPool *)CURRENT->PREV;
    VARPool *NEXT = (VARPool *)CURRENT->NEXT;
    PIFAlizator *PIF = (PIFAlizator *)CURRENT->PIF;
    if (CURRENT->POOL_VARS == POOL_BLOCK_SIZE) /*&& (PREV))*/ {
        /*PREV->NEXT = NEXT;
        if (NEXT)
            NEXT->PREV = CURRENT->PREV;*/
        ((PIFAlizator *)CURRENT->PIF)->free_vars -= POOL_BLOCK_SIZE;
        if (((PIFAlizator *)CURRENT->PIF)->CACHEDPOOL == CURRENT)
            ((PIFAlizator *)CURRENT->PIF)->CACHEDPOOL = NULL;
        /*FAST_FREE(CURRENT);*/
        RemoveBlock(CURRENT);
    } else
    if ((PREV) && (NEXT)) {
        PIFAlizator *PIF = (PIFAlizator *)CURRENT->PIF;
        PREV->NEXT = NEXT;
        NEXT->PREV = PREV;

        CURRENT->PREV = NULL;
        CURRENT->NEXT = PIF->POOL;
        PIF->POOL->PREV = CURRENT;

        PIF->POOL = CURRENT;
    }
    //}
}

void AllocMultipleVars(void **context, void *PIF, int count, int offset) {
    // at least one variable (this_ref)
    if (!count)
        count = 1;

    VARPool *POOL = ((PIFAlizator *)PIF)->POOL;
    VARPool *NEXT_POOL;
    int     i;
    if (!POOL) {
        POOL            = (VARPool *)FAST_MALLOC(sizeof(VARPool));
        POOL->POOL_VARS = POOL_BLOCK_SIZE;
        POOL->FIRST_VAR = 0;
        POOL->NEXT      = NULL;
        POOL->PREV      = NULL;
        POOL->PIF       = PIF;
        for (i = 0; i < POOL_BLOCK_SIZE; i++)
            POOL->POOL[i].flags = -1;
        ((PIFAlizator *)PIF)->POOL       = POOL;
        ((PIFAlizator *)PIF)->free_vars += POOL_BLOCK_SIZE;
    }

    NEXT_POOL = POOL;
    if (((PIFAlizator *)PIF)->free_vars) {
        int iterations = 0;
        while ((NEXT_POOL) && (((PIFAlizator *)PIF)->free_vars)) {
            //while (NEXT_POOL->POOL_VARS) {
            for (i = NEXT_POOL->FIRST_VAR; (NEXT_POOL->POOL_VARS) && (i < POOL_BLOCK_SIZE); i++) {
                if (NEXT_POOL->POOL[i].flags == -1) {
                    NEXT_POOL->POOL_VARS--;
                    NEXT_POOL->POOL[i].flags = i;
                    ((PIFAlizator *)PIF)->free_vars--;
                    context[offset++] = &NEXT_POOL->POOL[i];
                    count--;
                    NEXT_POOL->FIRST_VAR = i + 1;
                    if (!count)
                        return;
                }
            }
            //}
            iterations++;
            if (((PIFAlizator *)PIF)->free_vars < POOL_BLOCK_SIZE)
                break;
            // memory is too fragmented ...
            if (iterations > 1000)
                break;
            NEXT_POOL = (VARPool *)NEXT_POOL->NEXT;
        }
    }

    while (count) {
        NEXT_POOL = (VARPool *)FAST_MALLOC(sizeof(VARPool));
        if (!NEXT_POOL) {
            ((PIFAlizator *)PIF)->out->ClientError(ERR1400 "\n");
            exit(-100);
        }
        if (count < POOL_BLOCK_SIZE) {
            NEXT_POOL->FIRST_VAR             = count;
            NEXT_POOL->POOL_VARS             = POOL_BLOCK_SIZE - count;
            ((PIFAlizator *)PIF)->free_vars += NEXT_POOL->POOL_VARS;
        } else {
            NEXT_POOL->FIRST_VAR = POOL_BLOCK_SIZE;
            NEXT_POOL->POOL_VARS = 0;
        }

        NEXT_POOL->PIF = PIF;
        for (i = 0; i < POOL_BLOCK_SIZE; i++) {
            if (count) {
                count--;
                NEXT_POOL->POOL[i].flags = i;
                context[offset++]        = &NEXT_POOL->POOL[i];
            } else
                NEXT_POOL->POOL[i].flags = -1;
        }

        NEXT_POOL->PREV            = POOL->PREV;
        NEXT_POOL->NEXT            = POOL;
        POOL->PREV                 = NEXT_POOL;
        ((PIFAlizator *)PIF)->POOL = NEXT_POOL;
        POOL = NEXT_POOL;
    }
}

void *AllocVAR(void *PIF) {
    if (!PIF) {
        // pool variable
        VariableDATA *notPOOLED = (VariableDATA *)FAST_MALLOC(sizeof(VariableDATA));
        if (notPOOLED)
            notPOOLED->flags = -2;
        return notPOOLED;
    }
    VARPool *POOL = ((PIFAlizator *)PIF)->POOL;
    VARPool *NEXT_POOL;
    int     i;
    if (!POOL) {
        POOL            = (VARPool *)FAST_MALLOC(sizeof(VARPool));
        POOL->FIRST_VAR = 0;
        POOL->POOL_VARS = POOL_BLOCK_SIZE;
        POOL->NEXT      = NULL;
        POOL->PREV      = NULL;
        POOL->PIF       = PIF;
        for (i = 0; i < POOL_BLOCK_SIZE; i++)
            POOL->POOL[i].flags = -1;
        ((PIFAlizator *)PIF)->POOL       = POOL;
        ((PIFAlizator *)PIF)->free_vars += POOL_BLOCK_SIZE;
    }
    NEXT_POOL = ((PIFAlizator *)PIF)->CACHEDPOOL ? ((PIFAlizator *)PIF)->CACHEDPOOL : POOL;
    if (((PIFAlizator *)PIF)->free_vars) {
        int iterations = 0;
        while ((NEXT_POOL) && (((PIFAlizator *)PIF)->free_vars)) {
            if (NEXT_POOL->POOL_VARS) {
                for (i = NEXT_POOL->FIRST_VAR; i < POOL_BLOCK_SIZE; i++) {
                    if (NEXT_POOL->POOL[i].flags == -1) {
                        NEXT_POOL->POOL_VARS--;
                        NEXT_POOL->POOL[i].flags = i;
                        if (NEXT_POOL->POOL_VARS)
                            NEXT_POOL->FIRST_VAR = i + 1;
                        else
                            NEXT_POOL->FIRST_VAR = POOL_BLOCK_SIZE;
                        ((PIFAlizator *)PIF)->free_vars--;
                        if (((PIFAlizator *)PIF)->free_vars)
                            ((PIFAlizator *)PIF)->CACHEDPOOL = NEXT_POOL;
                        return &NEXT_POOL->POOL[i];
                    }
                }
            }
            iterations++;
            if (((PIFAlizator *)PIF)->free_vars < POOL_BLOCK_SIZE)
                break;
            // memory is too fragmented ...
            if (iterations > 200)
                break;
            NEXT_POOL = (VARPool *)NEXT_POOL->NEXT;
        }
    }
    ((PIFAlizator *)PIF)->CACHEDPOOL = NULL;
    NEXT_POOL = (VARPool *)FAST_MALLOC(sizeof(VARPool));
    if (!NEXT_POOL) {
        ((PIFAlizator *)PIF)->out->ClientError(ERR1400 "\n");
        exit(-100);
    }
    NEXT_POOL->FIRST_VAR             = 1;
    NEXT_POOL->PIF                   = PIF;
    NEXT_POOL->POOL_VARS             = POOL_BLOCK_SIZE - 1;
    ((PIFAlizator *)PIF)->free_vars += NEXT_POOL->POOL_VARS;
    for (i = 1; i < POOL_BLOCK_SIZE; i++)
        NEXT_POOL->POOL[i].flags = -1;
    NEXT_POOL->PREV = POOL->PREV;
    NEXT_POOL->NEXT = POOL;
    POOL->PREV      = NEXT_POOL;
    ((PIFAlizator *)PIF)->CACHEDPOOL = NEXT_POOL;
    ((PIFAlizator *)PIF)->POOL       = NEXT_POOL;
    NEXT_POOL->POOL[0].flags         = 0;
    return &NEXT_POOL->POOL[0];
}

int ModuleCheckReachability(void *PIF) {
#ifndef SIMPLE_MULTI_THREADING
    if (CheckReachability(PIF)) {
        ((PIFAlizator *)PIF)->dirty_limit = 1000;
        VarClean((PIFAlizator *)PIF);
        return 1;
    }
    return 0;
#else
    return -1;
#endif
}

void *AllocClassObject(void *PIF) {
    ALLOC_LOCK
#ifndef SIMPLE_MULTI_THREADING
    if (PIF) {
        int delta = ((PIFAlizator *)PIF)->object_count - ((PIFAlizator *)PIF)->last_gc_run;
        if (delta >= ((PIFAlizator *)PIF)->dirty_limit) {
            if (CheckReachability(PIF)) {
                // bad programmer, bad !
                ((PIFAlizator *)PIF)->dirty_limit = 1000;
            } else {
                // good boy, good programmer
                if (((PIFAlizator *)PIF)->dirty_limit < DIRTY_LIMIT) {
                    ((PIFAlizator *)PIF)->dirty_limit *= 3;
                    if (((PIFAlizator *)PIF)->dirty_limit > DIRTY_LIMIT)
                        ((PIFAlizator *)PIF)->dirty_limit = DIRTY_LIMIT;
                } else
                    ((PIFAlizator *)PIF)->dirty_limit = ((PIFAlizator *)PIF)->object_count * 2;
            }
        }
    }
#endif
    ClassPool * POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    ClassPool *NEXT_POOL;
    int       i;
    ((PIFAlizator *)PIF)->object_count++;
    if (!POOL) {
        POOL            = (ClassPool *)malloc(sizeof(ClassPool));
        POOL->FIRST_VAR = 0;
        POOL->POOL_VARS = OBJECT_POOL_BLOCK_SIZE;
        POOL->NEXT      = NULL;
        POOL->PREV      = NULL;
        POOL->PIF       = PIF;
        for (i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++)
            POOL->POOL[i].flags = -1;
        ((PIFAlizator *)PIF)->CLASSPOOL           = POOL;
        ((PIFAlizator *)PIF)->free_class_objects += OBJECT_POOL_BLOCK_SIZE;
    }
    NEXT_POOL = ((PIFAlizator *)PIF)->CACHEDCLASSPOOL ? (ClassPool *)((PIFAlizator *)PIF)->CACHEDCLASSPOOL : POOL;
    if (((PIFAlizator *)PIF)->free_class_objects) {
        while ((NEXT_POOL) && (((PIFAlizator *)PIF)->free_class_objects)) {
            if (NEXT_POOL->POOL_VARS) {
                for (i = NEXT_POOL->FIRST_VAR; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                    if (NEXT_POOL->POOL[i].flags == -1) {
                        NEXT_POOL->POOL_VARS--;
                        NEXT_POOL->POOL[i].flags     = i;
                        NEXT_POOL->POOL[i].reachable = 0;
                        if (NEXT_POOL->POOL_VARS)
                            NEXT_POOL->FIRST_VAR = i + 1;
                        else
                            NEXT_POOL->FIRST_VAR = OBJECT_POOL_BLOCK_SIZE;
                        ((PIFAlizator *)PIF)->free_class_objects--;
                        if (((PIFAlizator *)PIF)->free_class_objects)
                            ((PIFAlizator *)PIF)->CACHEDCLASSPOOL = NEXT_POOL;
                        ALLOC_UNLOCK
                        return &NEXT_POOL->POOL[i];
                    }
                }
            }
            if (((PIFAlizator *)PIF)->free_class_objects < OBJECT_POOL_BLOCK_SIZE)
                break;
            NEXT_POOL = (ClassPool *)NEXT_POOL->NEXT;
        }
    }
    ((PIFAlizator *)PIF)->CACHEDCLASSPOOL = NULL;
    NEXT_POOL = (ClassPool *)malloc(sizeof(ClassPool));
    if (!NEXT_POOL) {
        ((PIFAlizator *)PIF)->out->ClientError(ERR1400 "\n");
        exit(-100);
    }
    NEXT_POOL->FIRST_VAR = 1;
    NEXT_POOL->PIF       = PIF;
    NEXT_POOL->POOL_VARS = OBJECT_POOL_BLOCK_SIZE - 1;
    ((PIFAlizator *)PIF)->free_class_objects += NEXT_POOL->POOL_VARS;
    for (i = 1; i < OBJECT_POOL_BLOCK_SIZE; i++)
        NEXT_POOL->POOL[i].flags = -1;
    NEXT_POOL->PREV = POOL->PREV;
    NEXT_POOL->NEXT = POOL;
    POOL->PREV      = NEXT_POOL;
    ((PIFAlizator *)PIF)->CACHEDCLASSPOOL = NEXT_POOL;
    ((PIFAlizator *)PIF)->CLASSPOOL       = NEXT_POOL;
    NEXT_POOL->POOL[0].flags     = 0;
    NEXT_POOL->POOL[0].reachable = 0;
    ALLOC_UNLOCK
    return &NEXT_POOL->POOL[0];
}

void FreeClassObject(void *refObject) {
    if (!refObject)
        return;

    if (((CompiledClass *)refObject)->flags == -1)
        return;

    ClassPool   *CURRENT = (ClassPool *)(((uintptr_t)refObject) - sizeof(CompiledClass) * (((CompiledClass *)refObject)->flags) - POOL_OFFSET(ClassPool, POOL));
    PIFAlizator *PIF     = (PIFAlizator *)CURRENT->PIF;
    ALLOC_LOCK
    memset(refObject, 0, sizeof(CompiledClass));
    ((PIFAlizator *)PIF)->object_count--;
    PIF->free_class_objects++;
    if (((CompiledClass *)refObject)->flags < CURRENT->FIRST_VAR)
        CURRENT->FIRST_VAR = ((CompiledClass *)refObject)->flags;
    ((CompiledClass *)refObject)->flags = -1;
    CURRENT->POOL_VARS++;
    ClassPool *PREV = (ClassPool *)CURRENT->PREV;
    ClassPool *NEXT = (ClassPool *)CURRENT->NEXT;
    if ((CURRENT->POOL_VARS == OBJECT_POOL_BLOCK_SIZE) && (PREV)) {
        PREV->NEXT = NEXT;
        if (NEXT)
            NEXT->PREV = CURRENT->PREV;
        PIF->free_class_objects -= OBJECT_POOL_BLOCK_SIZE;
        if (PIF->CACHEDCLASSPOOL == CURRENT)
            PIF->CACHEDCLASSPOOL = NULL;
        free(CURRENT);
    } else
    if ((CURRENT != PIF->CLASSPOOL) && (CURRENT->POOL_VARS > ((ClassPool *)PIF->CLASSPOOL)->POOL_VARS)) {
        PIF->CACHEDCLASSPOOL = CURRENT;
        if (NEXT)
            NEXT->PREV = CURRENT->PREV;
        PREV->NEXT = NEXT;

        CURRENT->NEXT = PIF->CLASSPOOL;
        CURRENT->PREV = NULL;
        ((ClassPool *)PIF->CLASSPOOL)->PREV = CURRENT;
        PIF->CLASSPOOL = CURRENT;
    }
    //}
    ALLOC_UNLOCK
}

void *AllocArray(void *PIF, bool skip_top) {
    if (!PIF) {
        Array *notPOOLED = (Array *)malloc(sizeof(Array));
        if (notPOOLED)
            notPOOLED->flags = -2;
        return notPOOLED;
    }

    ALLOC_LOCK
#ifndef SIMPLE_MULTI_THREADING
    int delta = ((PIFAlizator *)PIF)->object_count - ((PIFAlizator *)PIF)->last_gc_run;
    if ((delta >= ((PIFAlizator *)PIF)->dirty_limit) && (!skip_top)) {
        if (CheckReachability(PIF, skip_top)) {
            // bad programmer, bad !
            ((PIFAlizator *)PIF)->dirty_limit = 1000;
        } else {
            // good boy, good programmer
            if (((PIFAlizator *)PIF)->dirty_limit < DIRTY_LIMIT) {
                ((PIFAlizator *)PIF)->dirty_limit *= 2;
                if (((PIFAlizator *)PIF)->dirty_limit > DIRTY_LIMIT)
                    ((PIFAlizator *)PIF)->dirty_limit = DIRTY_LIMIT;
            }
        }
    }
#endif
    ((PIFAlizator *)PIF)->object_count++;
    ArrayPool *POOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    ArrayPool *NEXT_POOL;
    int       i;
    if (!POOL) {
        POOL            = (ArrayPool *)malloc(sizeof(ArrayPool));
        POOL->FIRST_VAR = 0;
        POOL->POOL_VARS = OBJECT_POOL_BLOCK_SIZE;
        POOL->NEXT      = NULL;
        POOL->PREV      = NULL;
        POOL->PIF       = PIF;
        for (i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++)
            POOL->POOL[i].flags = -1;
        ((PIFAlizator *)PIF)->ARRAYPOOL    = POOL;
        ((PIFAlizator *)PIF)->free_arrays += ARRAY_POOL_BLOCK_SIZE;
    }
    NEXT_POOL = ((PIFAlizator *)PIF)->CACHEDARRAYPOOL ? (ArrayPool *)((PIFAlizator *)PIF)->CACHEDARRAYPOOL : POOL;
    if (((PIFAlizator *)PIF)->free_arrays) {
        int iterations = 0;
        while ((NEXT_POOL) && (((PIFAlizator *)PIF)->free_arrays)) {
            if (NEXT_POOL->POOL_VARS) {
                for (i = NEXT_POOL->FIRST_VAR; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                    if (NEXT_POOL->POOL[i].flags == -1) {
                        NEXT_POOL->POOL_VARS--;
                        NEXT_POOL->POOL[i].flags     = i;
                        NEXT_POOL->POOL[i].reachable = 0;
                        if (NEXT_POOL->POOL_VARS)
                            NEXT_POOL->FIRST_VAR = i + 1;
                        else
                            NEXT_POOL->FIRST_VAR = ARRAY_POOL_BLOCK_SIZE;
                        ((PIFAlizator *)PIF)->free_arrays--;
                        if (((PIFAlizator *)PIF)->free_arrays)
                            ((PIFAlizator *)PIF)->CACHEDARRAYPOOL = NEXT_POOL;
                        ALLOC_UNLOCK
                        return &NEXT_POOL->POOL[i];
                    }
                }
            }
            iterations++;
            if (((PIFAlizator *)PIF)->free_arrays < ARRAY_POOL_BLOCK_SIZE)
                break;
            // memory is too fragmented ...
            NEXT_POOL = (ArrayPool *)NEXT_POOL->NEXT;
        }
    }
    ((PIFAlizator *)PIF)->CACHEDARRAYPOOL = NULL;
    NEXT_POOL = (ArrayPool *)malloc(sizeof(ArrayPool));
    if (!NEXT_POOL) {
        ((PIFAlizator *)PIF)->out->ClientError(ERR1400 "\n");
        exit(-100);
    }
    NEXT_POOL->FIRST_VAR = 1;
    NEXT_POOL->PIF       = PIF;
    NEXT_POOL->POOL_VARS = ARRAY_POOL_BLOCK_SIZE - 1;
    ((PIFAlizator *)PIF)->free_arrays += NEXT_POOL->POOL_VARS;
    for (i = 1; i < ARRAY_POOL_BLOCK_SIZE; i++)
        NEXT_POOL->POOL[i].flags = -1;
    NEXT_POOL->PREV = POOL->PREV;
    NEXT_POOL->NEXT = POOL;
    POOL->PREV      = NEXT_POOL;
    ((PIFAlizator *)PIF)->CACHEDARRAYPOOL = NEXT_POOL;
    ((PIFAlizator *)PIF)->ARRAYPOOL       = NEXT_POOL;
    NEXT_POOL->POOL[0].flags     = 0;
    NEXT_POOL->POOL[0].reachable = 0;
    ALLOC_UNLOCK
    return &NEXT_POOL->POOL[0];
}

void FreeArray(void *refObject) {
    if (!refObject)
        return;

    if (((Array *)refObject)->flags == -1)
        return;

    if (((Array *)refObject)->flags == -2) {
        // not a pooled array (module-created array) for backwards compatibility
        free(refObject);
        return;
    }

    ArrayPool   *CURRENT = (ArrayPool *)(((uintptr_t)refObject) - sizeof(Array) * (((Array *)refObject)->flags) - POOL_OFFSET(ArrayPool, POOL));
    PIFAlizator *PIF     = (PIFAlizator *)CURRENT->PIF;
    ALLOC_LOCK
        ((PIFAlizator *)PIF)->object_count--;
    PIF->free_arrays++;
    if (((Array *)refObject)->flags < CURRENT->FIRST_VAR)
        CURRENT->FIRST_VAR = ((Array *)refObject)->flags;
    ((Array *)refObject)->flags = -1;
    CURRENT->POOL_VARS++;
    ArrayPool *PREV = (ArrayPool *)CURRENT->PREV;
    ArrayPool *NEXT = (ArrayPool *)CURRENT->NEXT;
    if ((CURRENT->POOL_VARS == ARRAY_POOL_BLOCK_SIZE) && (PREV)) {
        PREV->NEXT = NEXT;
        if (NEXT)
            NEXT->PREV = CURRENT->PREV;
        PIF->free_arrays -= ARRAY_POOL_BLOCK_SIZE;
        if (PIF->CACHEDARRAYPOOL == CURRENT)
            PIF->CACHEDARRAYPOOL = NULL;
        free(CURRENT);
    } else
    if ((CURRENT != PIF->ARRAYPOOL) && (CURRENT->POOL_VARS > ((ArrayPool *)PIF->ARRAYPOOL)->POOL_VARS)) {
        PIF->CACHEDARRAYPOOL = CURRENT;
        if (NEXT)
            NEXT->PREV = CURRENT->PREV;
        PREV->NEXT = NEXT;

        CURRENT->NEXT = PIF->ARRAYPOOL;
        CURRENT->PREV = NULL;
        ((ArrayPool *)PIF->ARRAYPOOL)->PREV = CURRENT;
        PIF->ARRAYPOOL = CURRENT;
    }
    //}
    ALLOC_UNLOCK
}

int MarkRecursive(void *PIF, Array *arr, signed char reach_id_flag, signed char forced_flag) {
    arr->reachable = (arr->reachable & 0x1C) | reach_id_flag;
    int      res      = 1;
    NODE     *CURRENT = arr->FIRST;
    for (intptr_t i = 0; i < arr->NODE_COUNT; i++) {
        for (intptr_t j = 0; j < CURRENT->COUNT; j++) {
            VariableDATA *Var = CURRENT->ELEMENTS [j];
            if ((Var) && (Var->CLASS_DATA)) {
                if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                    CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                    if ((CC2->reachable & 0x03) != reach_id_flag)
                        res += MarkRecursive(PIF, CC2, reach_id_flag, forced_flag ? forced_flag : CC2->reachable & 0x1C);
                    CC2->reachable |= forced_flag;
                } else
                if (Var->TYPE == VARIABLE_ARRAY) {
                    Array *ARR2 = (Array *)Var->CLASS_DATA;
                    if ((ARR2->reachable & 0x03) != reach_id_flag)
                        res += MarkRecursive(PIF, ARR2, reach_id_flag, forced_flag ? forced_flag : ARR2->reachable & 0x1C);
                    ARR2->reachable |= forced_flag;
                }
            }
        }
        NODE *NEXT = CURRENT->NEXT;
        CURRENT = NEXT;
    }
    return res;
}

int MarkRecursive(void *PIF, CompiledClass *CC, signed char reach_id_flag, signed char forced_flag) {
    CC->reachable = (CC->reachable & 0x1C) | reach_id_flag;
    ClassCode *base = CC->_Class;
    int       res   = 1;
    if (!CC->_CONTEXT)
        return res;

#ifdef USE_RECURSIVE_MARKINGS
    for (int i = 0; i < base->DataMembersCount; i++) {
        VariableDATA *Var = CC->_CONTEXT[i];
        if ((Var) && (Var->CLASS_DATA)) {
            if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                if ((CC2->reachable & 0x03) != reach_id_flag)
                    res += MarkRecursive(PIF, CC2, reach_id_flag, forced_flag ? forced_flag : CC2->reachable & 0x1C);
                CC2->reachable |= forced_flag;
            } else
            if (Var->TYPE == VARIABLE_ARRAY) {
                Array *ARR2 = (Array *)Var->CLASS_DATA;
                if ((ARR2->reachable & 0x03) != reach_id_flag)
                    res += MarkRecursive(PIF, ARR2, reach_id_flag, forced_flag ? forced_flag : ARR2->reachable & 0x1C);
                ARR2->reachable |= forced_flag;
            }
        }
    }
#else
    int           inspectSize = INITIAL_INSPECT_SIZE;
    CompiledClass **toInspect = (CompiledClass **)realloc(NULL, sizeof(CompiledClass *) * inspectSize);
    toInspect[0] = CC;
    int inspectPos = 1;
    for (int j = 0; j < inspectPos; j++) {
        CC   = toInspect[j];
        base = CC->_Class;
        for (int i = 0; i < base->DataMembersCount; i++) {
            VariableDATA *Var = CC->_CONTEXT[i];
            if ((Var) && (Var->CLASS_DATA)) {
                if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                    CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                    if ((CC2->reachable & 0x03) != reach_id_flag) {
                        if (CC2->_CONTEXT) {
                            if (inspectPos < inspectSize)
                                toInspect[inspectPos++] = CC2;
                            else {
                                inspectSize            += INSPECT_INCREMENT;
                                toInspect               = (CompiledClass **)realloc(toInspect, sizeof(CompiledClass *) * inspectSize);
                                toInspect[inspectPos++] = CC2;
                            }
                        }
                        CC2->reachable = (CC2->reachable & 0x1C) | reach_id_flag;
                        if ((!forced_flag) && (CC2->reachable & 0x1C))
                            res += MarkRecursive(PIF, CC2, reach_id_flag, 0x1C);
                    }
                    CC2->reachable |= forced_flag;
                } else
                if (Var->TYPE == VARIABLE_ARRAY) {
                    Array *ARR2 = (Array *)Var->CLASS_DATA;
                    if ((ARR2->reachable & 0x03) != reach_id_flag)
                        res += MarkRecursive(PIF, ARR2, reach_id_flag, forced_flag ? forced_flag : ARR2->reachable & 0x1C);
                    ARR2->reachable |= forced_flag;
                }
            }
        }
    }
    free(toInspect);
#endif
    return res;
}

int ClearRecursive(void *PIF, CompiledClass *CC, int CLSID, signed char reach_id_flag, signed char forced_flag) {
    ClassCode *base = CC->_Class;
    CC->reachable = (CC->reachable & 0x1C) | reach_id_flag;
    int       res   = 1;
    if (!CC->_CONTEXT)
        return res;

    for (int i = 0; i < base->DataMembersCount; i++) {
        VariableDATA *Var = CC->_CONTEXT[i];
        if ((Var) && (Var->CLASS_DATA)) {
            if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                if ((CC2->reachable & 0x03) != reach_id_flag) {
                    if (CC2->_Class->GetCLSID() == CLSID) {
                        CLASS_CHECK(Var);
                        Var->TYPE = VARIABLE_NUMBER;
                        Var->NUMBER_DATA = 0;
                    } else {
                        ClearRecursive(PIF, CC2, CLSID, reach_id_flag, forced_flag);
                        CC2->reachable |= forced_flag;
                    }
                }
            } else
            if (Var->TYPE == VARIABLE_ARRAY) {
                Array *ARR2 = (Array *)Var->CLASS_DATA;
                if ((ARR2->reachable & 0x03) != reach_id_flag) {
                    // ClearRecursive(PIF, ARR2, CLSID, reach_id_flag, forced_flag);
                    ARR2->reachable |= forced_flag;
                }
            }
        }
    }
    return res;
}

int ClearRecursive(void *PIF, Array *arr, int CLSID, signed char reach_id_flag, signed char forced_flag) {
    arr->reachable = (arr->reachable & 0x1C) | reach_id_flag;
    int      res      = 1;
    NODE     *CURRENT = arr->FIRST;
    for (intptr_t i = 0; i < arr->NODE_COUNT; i++) {
        for (intptr_t j = 0; j < CURRENT->COUNT; j++) {
            VariableDATA *Var = CURRENT->ELEMENTS [j];
            if ((Var) && (Var->CLASS_DATA)) {
                if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                    CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                    if ((CC2->reachable & 0x03) != reach_id_flag)
                        res += ClearRecursive(PIF, CC2, CLSID, reach_id_flag, forced_flag ? forced_flag : CC2->reachable & 0x1C);
                    CC2->reachable |= forced_flag;
                    if (CC2->GetClass()->GetCLSID() == CLSID) {
                        CLASS_CHECK(Var);
                        Var->TYPE = VARIABLE_NUMBER;
                        Var->NUMBER_DATA = 0;
                    }
                } else
                if (Var->TYPE == VARIABLE_ARRAY) {
                    Array *ARR2 = (Array *)Var->CLASS_DATA;
                    if ((ARR2->reachable & 0x03) != reach_id_flag)
                        res += ClearRecursive(PIF, ARR2, CLSID, reach_id_flag, forced_flag ? forced_flag : ARR2->reachable & 0x1C);
                    ARR2->reachable |= forced_flag;
                }
            }
        }
        NODE *NEXT = CURRENT->NEXT;
        CURRENT = NEXT;
    }
    return res;
}

int ClearVariablesByCLSID(void *PIF, int CLSID) {
    if ((!PIF) || (!((PIFAlizator *)PIF)->RootInstance) || (((PIFAlizator *)PIF)->in_gc))
        return 0;
    int cleared = 1;
    ((PIFAlizator *)PIF)->in_gc = 1;
    GCRoot *root = ((PIFAlizator *)PIF)->GCROOT;
    while (root) {
        SCStack *STACK_TRACE = (SCStack *)((SCStack *)root->STACK_TRACE)->ROOT;
        if (STACK_TRACE)
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;

        while (STACK_TRACE) {
            if (STACK_TRACE->len == -1) {
                // concurrent write, abort
                //ALLOC_UNLOCK
                ((PIFAlizator *)PIF)->in_gc = 0;
                return 0;
            }
            STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }
        root = (GCRoot *)root->NEXT;
    }

    CompiledClass *CC_ROOT = (CompiledClass *)((PIFAlizator *)PIF)->RootInstance;
    if ((!CC_ROOT) || (CC_ROOT->GetClass()->GetCLSID() == CLSID)) {
        ((PIFAlizator *)PIF)->in_gc = 0;
        return 0;
    }
    signed char  reach_id_flag = CC_ROOT->reachable & 0x03;

    if (reach_id_flag == 1)
        reach_id_flag = 2;
    else
        reach_id_flag = 1;

    ClearRecursive(PIF, CC_ROOT, CLSID, reach_id_flag, 0);

    if (((PIFAlizator *)PIF)->var_globals)
        ClearRecursive(PIF, (Array *)((PIFAlizator *)PIF)->var_globals, CLSID, reach_id_flag, 0);

    root = ((PIFAlizator *)PIF)->GCROOT;
    while (root) {
        SCStack *STACK_TRACE = (SCStack *)((SCStack *)root->STACK_TRACE)->ROOT;
        if (STACK_TRACE)
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;

        while (STACK_TRACE) {
            VariableDATA **context = (VariableDATA **)STACK_TRACE->LOCAL_CONTEXT;
            if (context) {
                for (int i = 0; i < STACK_TRACE->len; i++) {
                    VariableDATA *Var = context[i];
                    if ((Var) && (Var->CLASS_DATA)) {
                        if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                            CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                            if ((CC2->reachable & 0x03) != reach_id_flag)
                                ClearRecursive(PIF, CC2, CLSID, reach_id_flag, CC2->reachable & 0x1C);
                            if (CC2->GetClass()->GetCLSID() == CLSID) {
                                CLASS_CHECK(Var);
                                Var->TYPE = VARIABLE_NUMBER;
                                Var->NUMBER_DATA = 0;
                            }
                        } else
                        if (Var->TYPE == VARIABLE_ARRAY) {
                            Array *ARR2 = (Array *)Var->CLASS_DATA;
                            if ((ARR2->reachable & 0x03) != reach_id_flag)
                                ClearRecursive(PIF, ARR2, CLSID, reach_id_flag, ARR2->reachable & 0x1C);
                        }
                    }
                }
            }
            STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }
        root = (GCRoot *)root->NEXT;
    }
    // =================================== //
    ArrayPool *ARRAYPOOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    while (ARRAYPOOL) {
        if (ARRAYPOOL->POOL_VARS < ARRAY_POOL_BLOCK_SIZE) {
            for (int i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                Array *ARR = &ARRAYPOOL->POOL[i];
                if ((ARR->flags >= 0) && (ARR->reachable >= 0x1C)) {
                    if ((ARR->reachable & 0x03) != reach_id_flag)
                        ClearRecursive(PIF, ARR, CLSID, reach_id_flag, ARR->reachable & 0x1C);
                }
            }
        }
        ARRAYPOOL = (ArrayPool *)ARRAYPOOL->NEXT;
    }
    ClassPool *POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    while (POOL) {
        if (POOL->POOL_VARS < OBJECT_POOL_BLOCK_SIZE) {
            for (int i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                CompiledClass *CC = &POOL->POOL[i];
                if ((CC->flags >= 0) && (CC->reachable >= 0x1C)) {
                    if (CC->GetClass()->GetCLSID() == CLSID)
                        cleared = 0;
                    if ((CC->reachable & 0x03) != reach_id_flag)
                        ClearRecursive(PIF, CC, CLSID, reach_id_flag, CC->reachable & 0x1C);
                }
            }
        }
        POOL = (ClassPool *)POOL->NEXT;
    }
    // =================================== //
    ((PIFAlizator *)PIF)->in_gc = 0;
    return cleared;
}

int CheckReachability(void *PIF, bool skip_top) {
    if ((!PIF) || (!((PIFAlizator *)PIF)->RootInstance) || (((PIFAlizator *)PIF)->in_gc))
        return 0;

    ((PIFAlizator *)PIF)->in_gc = 1;
    int res = 0;
    //ALLOC_LOCK
    // ============================================================================== //
    // check if garbage collector may run
    // do this before entering the loop (and modifying states)
    GCRoot *root = ((PIFAlizator *)PIF)->GCROOT;
    while (root) {
        SCStack *STACK_TRACE = (SCStack *)((SCStack *)root->STACK_TRACE)->ROOT;
        if (STACK_TRACE)
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;

        while (STACK_TRACE) {
            if (STACK_TRACE->len == -1) {
                // concurrent write, abort
                //ALLOC_UNLOCK
                ((PIFAlizator *)PIF)->in_gc = 0;
                return 0;
            }
            STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }
        root = (GCRoot *)root->NEXT;
    }
    // ============================================================================== //

    CompiledClass *CC_ROOT      = (CompiledClass *)((PIFAlizator *)PIF)->RootInstance;
    signed char   reach_id_flag = CC_ROOT->reachable & 0x03;

    if (reach_id_flag == 1)
        reach_id_flag = 2;
    else
        reach_id_flag = 1;

    if (CC_ROOT)
        MarkRecursive(PIF, CC_ROOT, reach_id_flag, 0);
    if (((PIFAlizator *)PIF)->var_globals)
        MarkRecursive(PIF, (Array *)((PIFAlizator *)PIF)->var_globals, reach_id_flag, 0);

    root = ((PIFAlizator *)PIF)->GCROOT;
    while (root) {
        SCStack *STACK_TRACE = (SCStack *)((SCStack *)root->STACK_TRACE)->ROOT;
        if (STACK_TRACE) {
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;
            if ((STACK_TRACE) && (skip_top))
                STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }

        while (STACK_TRACE) {
            VariableDATA **context = (VariableDATA **)STACK_TRACE->LOCAL_CONTEXT;
            if (context) {
                for (int i = 0; i < STACK_TRACE->len; i++) {
                    VariableDATA *Var = context[i];
                    if ((Var) && (Var->CLASS_DATA)) {
                        if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                            CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                            if ((CC2->reachable & 0x03) != reach_id_flag)
                                MarkRecursive(PIF, CC2, reach_id_flag, CC2->reachable & 0x1C);
                        } else
                        if (Var->TYPE == VARIABLE_ARRAY) {
                            Array *ARR2 = (Array *)Var->CLASS_DATA;
                            if ((ARR2->reachable & 0x03) != reach_id_flag)
                                MarkRecursive(PIF, ARR2, reach_id_flag, ARR2->reachable & 0x1C);
                        }
                    }
                }
            }
            STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }
        root = (GCRoot *)root->NEXT;
    }

    // Mark locked objects.
    // A C library may hold a lock on an object or array.
    // In this case, all child object must be locked to avoid 
    // garbage collected.
    // =================================== //
    ClassPool *POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    while (POOL) {
        if (POOL->POOL_VARS < OBJECT_POOL_BLOCK_SIZE) {
            for (int i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                CompiledClass *CC = &POOL->POOL[i];
                if ((CC->flags >= 0) && (CC->reachable >= 0x1C)) {
                    if ((CC->reachable & 0x03) != reach_id_flag)
                        MarkRecursive(PIF, CC, reach_id_flag, CC->reachable & 0x1C);
                }
            }
        }
        POOL = (ClassPool *)POOL->NEXT;
    }
    ArrayPool *ARRAYPOOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    while (ARRAYPOOL) {
        if (ARRAYPOOL->POOL_VARS < ARRAY_POOL_BLOCK_SIZE) {
            for (int i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                Array *ARR = &ARRAYPOOL->POOL[i];
                if ((ARR->flags >= 0) && (ARR->reachable >= 0x1C)) {
                    if ((ARR->reachable & 0x03) != reach_id_flag)
                        MarkRecursive(PIF, ARR, reach_id_flag, ARR->reachable & 0x1C);
                }
            }
        }
        ARRAYPOOL = (ArrayPool *)ARRAYPOOL->NEXT;
    }
    // =================================== //

    GarbageCollector __gc_obj;
    GarbageCollector __gc_array;
    GarbageCollector __gc_vars;

    POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    while (POOL) {
        if (POOL->POOL_VARS < OBJECT_POOL_BLOCK_SIZE) {
            for (int i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                CompiledClass *CC = &POOL->POOL[i];
                if ((CC->flags >= 0) && (CC->reachable < 0x1C)) {
                    if (CC->reachable != reach_id_flag) {
                        res++;
                        CC->reachable = reach_id_flag;
                        __gc_obj.Reference(CC);
                        CC->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars, reach_id_flag);
                    }
                }
            }
        }
        POOL = (ClassPool *)POOL->NEXT;
    }

    int arr = 0;
    ARRAYPOOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    while (ARRAYPOOL) {
        if (ARRAYPOOL->POOL_VARS < ARRAY_POOL_BLOCK_SIZE) {
            for (int i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                Array *ARR = &ARRAYPOOL->POOL[i];
                if ((ARR->flags >= 0) && (ARR->reachable < 0x1C)) {
                    if (ARR->reachable != reach_id_flag) {
                        res++;
                        __gc_array.Reference(ARR);
                        ARR->reachable = reach_id_flag;
                        ARR->__GO_GARBAGE(PIF, &__gc_obj, &__gc_array, &__gc_vars, reach_id_flag);
                    }
                }
            }
        }
        ARRAYPOOL = (ArrayPool *)ARRAYPOOL->NEXT;
    }
    __gc_obj.EndOfExecution_SayBye_Objects();
    __gc_array.EndOfExecution_SayBye_Arrays();
    __gc_vars.EndOfExecution_SayBye_Variables();
    //ALLOC_UNLOCK
    ((PIFAlizator *)PIF)->in_gc = 0;
    ((PIFAlizator *)PIF)->last_gc_run = ((PIFAlizator *)PIF)->object_count;
    return res;
}

void AddGCRoot(void *PIF, void *CONTEXT) {
    ALLOC_LOCK
    GCRoot *root = (GCRoot *)FAST_MALLOC(sizeof(GCRoot));

    root->STACK_TRACE            = CONTEXT;
    root->NEXT                   = ((PIFAlizator *)PIF)->GCROOT;
    ((PIFAlizator *)PIF)->GCROOT = root;
    ALLOC_UNLOCK
}

void RemoveGCRoot(void *PIF, void *CONTEXT) {
    ALLOC_LOCK
    GCRoot *root = ((PIFAlizator *)PIF)->GCROOT;
    GCRoot *prec = 0;

    while (root) {
        if (root->STACK_TRACE == CONTEXT) {
            if (prec)
                prec->NEXT = root->NEXT;
            else
                ((PIFAlizator *)PIF)->GCROOT = (GCRoot *)root->NEXT;
            FAST_FREE(root);
            break;
        }
        prec = root;
        root = (GCRoot *)root->NEXT;
    }
    ALLOC_UNLOCK
}

int GetMemoryStatistics(void *PIF, void *RESULT) {
    if (((PIFAlizator *)PIF)->in_gc)
        return 0;
    ((PIFAlizator *)PIF)->in_gc = 1;
    VariableDATA *objects_var = ((Array *)RESULT)->ModuleGet("objects");
    VariableDATA *array_var   = ((Array *)RESULT)->ModuleGet("arrays");
    VariableDATA *array_elements   = ((Array *)RESULT)->ModuleGet("arrays_elements");

    CLASS_CHECK(objects_var);
    CLASS_CHECK(array_var);
    CLASS_CHECK(array_elements);

    objects_var->TYPE       = VARIABLE_NUMBER;
    array_var->TYPE         = VARIABLE_NUMBER;
    objects_var->CLASS_DATA = new(AllocArray((PIFAlizator *)PIF))Array(PIF);
    // array_var->CLASS_DATA   = new(AllocArray((PIFAlizator *)PIF))Array(PIF);
    objects_var->TYPE       = VARIABLE_ARRAY;
    // array_var->TYPE         = VARIABLE_ARRAY;

    ((Array *)objects_var->CLASS_DATA)->reachable = 0x1C;
    // ((Array *)array_var->CLASS_DATA)->reachable = 0x1C;

#ifdef POOL_BLOCK_ALLOC
    VariableDATA *variable_var  = ((Array *)RESULT)->ModuleGet("variables");
    VariableDATA *string_var    = ((Array *)RESULT)->ModuleGet("strings");
    VariableDATA *number_var    = ((Array *)RESULT)->ModuleGet("numbers");
    VariableDATA *delegate_var  = ((Array *)RESULT)->ModuleGet("delegates");
    VariableDATA *lock_obj_var  = ((Array *)RESULT)->ModuleGet("objects_locked");
    VariableDATA *lock_arr_var  = ((Array *)RESULT)->ModuleGet("arrays_locked");
    VariableDATA *var_pool_var  = ((Array *)RESULT)->ModuleGet("variables_pool_memory");
    VariableDATA *obj_pool_var  = ((Array *)RESULT)->ModuleGet("objects_pool_memory");
    VariableDATA *arr_pool_var  = ((Array *)RESULT)->ModuleGet("arrays_pool_memory");
#endif
    VariableDATA *string_memory = ((Array *)RESULT)->ModuleGet("strings_memory");
    VariableDATA *memory = ((Array *)RESULT)->ModuleGet("memory");
    VariableDATA *unreachable_objects = ((Array *)RESULT)->ModuleGet("unreachable_objects");
    VariableDATA *unreachable_arrays  = ((Array *)RESULT)->ModuleGet("unreachable_arrays");
#ifdef POOL_BLOCK_ALLOC
    VARPool *NEXT_POOL = ((PIFAlizator *)PIF)->POOL;
    while (NEXT_POOL) {
        var_pool_var->NUMBER_DATA += sizeof(VARPool);
        if (NEXT_POOL->POOL_VARS) {
            for (int i = 0; i < POOL_BLOCK_SIZE; i++) {
                if (NEXT_POOL->POOL[i].flags != -1) {
                    variable_var->NUMBER_DATA += 1;
                    memory->NUMBER_DATA       += sizeof(VariableDATA);
                    switch (NEXT_POOL->POOL[i].TYPE) {
                        case VARIABLE_NUMBER:
                            number_var->NUMBER_DATA += 1;
                            break;

                        case VARIABLE_STRING:
                            string_var->NUMBER_DATA += 1;
                            if (NEXT_POOL->POOL[i].CLASS_DATA) {
                                intptr_t len = ((AnsiString *)NEXT_POOL->POOL[i].CLASS_DATA)->MemoryLength();
                                memory->NUMBER_DATA        += sizeof(AnsiString);
                                string_memory->NUMBER_DATA += len;
                            }
                            break;

                        case VARIABLE_DELEGATE:
                            delegate_var->NUMBER_DATA += 1;
                            break;
                    }
                }
            }
        }
        NEXT_POOL = (VARPool *)NEXT_POOL->NEXT;
    }
#endif
    ALLOC_LOCK
    CompiledClass *CC_ROOT      = (CompiledClass *)((PIFAlizator *)PIF)->RootInstance;
    signed char   reach_id_flag = CC_ROOT->reachable & 0x03;

    if (reach_id_flag == 1)
        reach_id_flag = 2;
    else
        reach_id_flag = 1;
    if (CC_ROOT)
        MarkRecursive(PIF, CC_ROOT, reach_id_flag, 0);
    if (((PIFAlizator *)PIF)->var_globals)
        MarkRecursive(PIF, (Array *)((PIFAlizator *)PIF)->var_globals, reach_id_flag, 0);

    GCRoot *root = ((PIFAlizator *)PIF)->GCROOT;
    while (root) {
        SCStack *STACK_TRACE = (SCStack *)((SCStack *)root->STACK_TRACE)->ROOT;
        if (STACK_TRACE)
            STACK_TRACE = (SCStack *)STACK_TRACE->TOP;

        while (STACK_TRACE) {
            if (STACK_TRACE->len == -1) {
                // concurrent write, abort
                //ALLOC_UNLOCK
                ((PIFAlizator *)PIF)->in_gc = 0;
                return 0;
            }
            VariableDATA **context = (VariableDATA **)STACK_TRACE->LOCAL_CONTEXT;
            if (context) {
                for (int i = 0; i < STACK_TRACE->len; i++) {
                    VariableDATA *Var = context[i];
                    if ((Var) && (Var->CLASS_DATA)) {
                        if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                            CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                            if ((CC2->reachable & 0x03) != reach_id_flag)
                                MarkRecursive(PIF, CC2, reach_id_flag, CC2->reachable & 0x1C);
                        } else
                        if (Var->TYPE == VARIABLE_ARRAY) {
                            Array *ARR2 = (Array *)Var->CLASS_DATA;
                            if ((ARR2->reachable & 0x03) != reach_id_flag)
                                MarkRecursive(PIF, ARR2, reach_id_flag, ARR2->reachable & 0x1C);
                        }
                    }
                }
            }
            STACK_TRACE = (SCStack *)STACK_TRACE->PREV;
        }
        root = (GCRoot *)root->NEXT;
    }

    // Mark locked objects.
    // A C library may hold a lock on an object or array.
    // In this case, all child object must be locked to avoid 
    // garbage collected.
    // =================================== //
    ClassPool *POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    while (POOL) {
        if (POOL->POOL_VARS < OBJECT_POOL_BLOCK_SIZE) {
            for (int i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                CompiledClass *CC = &POOL->POOL[i];
                if ((CC->flags >= 0) && (CC->reachable >= 0x1C)) {
                    if ((CC->reachable & 0x03) != reach_id_flag) {
#ifdef POOL_BLOCK_ALLOC
                        lock_obj_var->NUMBER_DATA++;
#endif
                        MarkRecursive(PIF, CC, reach_id_flag, CC->reachable & 0x1C);
                    }
                }
            }
        }
        POOL = (ClassPool *)POOL->NEXT;
    }
    ArrayPool *ARRAYPOOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    while (ARRAYPOOL) {
        if (ARRAYPOOL->POOL_VARS < ARRAY_POOL_BLOCK_SIZE) {
            for (int i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                Array *ARR = &ARRAYPOOL->POOL[i];
                if ((ARR->flags >= 0) && (ARR->reachable >= 0x1C)) {
                    if ((ARR->reachable & 0x03) != reach_id_flag) {
#ifdef POOL_BLOCK_ALLOC
                        lock_arr_var->NUMBER_DATA++;
#endif
                        MarkRecursive(PIF, ARR, reach_id_flag, ARR->reachable & 0x1C);
                    }
                }
            }
        }
        ARRAYPOOL = (ArrayPool *)ARRAYPOOL->NEXT;
    }
    // =================================== //
    POOL = (ClassPool *)((PIFAlizator *)PIF)->CLASSPOOL;
    while (POOL) {
#ifdef POOL_BLOCK_ALLOC
        obj_pool_var->NUMBER_DATA += sizeof(ClassPool);
#endif
        if (POOL->POOL_VARS < OBJECT_POOL_BLOCK_SIZE) {
            for (int i = 0; i < OBJECT_POOL_BLOCK_SIZE; i++) {
                CompiledClass *CC = &POOL->POOL[i];
                if (CC->flags >= 0) {
                    VariableDATA *ref = ((Array *)objects_var->CLASS_DATA)->ModuleGet(CC->GetClass()->NAME.c_str());
                    ref->NUMBER_DATA    += 1;
                    memory->NUMBER_DATA += sizeof(CompiledClass);
                    memory->NUMBER_DATA += CC->GetClass()->DataMembersCount * sizeof(VariableDATA *);
                    if ((CC->flags >= 0) && (CC->reachable < 0x1C) && (CC->reachable != reach_id_flag))
                        unreachable_objects->NUMBER_DATA += 1;
                }
            }
        }
        POOL = (ClassPool *)POOL->NEXT;
    }
    ARRAYPOOL = (ArrayPool *)((PIFAlizator *)PIF)->ARRAYPOOL;
    intptr_t  index      = 0;
    intptr_t  all_arrays = 0;
    while (ARRAYPOOL) {
#ifdef POOL_BLOCK_ALLOC
        arr_pool_var->NUMBER_DATA += sizeof(ArrayPool);
#endif
        if (ARRAYPOOL->POOL_VARS < ARRAY_POOL_BLOCK_SIZE) {
            for (int i = 0; i < ARRAY_POOL_BLOCK_SIZE; i++) {
                Array *ARR = &ARRAYPOOL->POOL[i];
                if (ARR->flags >= 0) {
                    // VariableDATA *ref = ((Array *)array_var->CLASS_DATA)->ModuleGet(index++);
                    // ref->NUMBER_DATA = ARR->Count();
                    array_var->NUMBER_DATA ++;
                    all_arrays += ARR->Count();
#ifdef STDMAP_KEYS
                    memory->NUMBER_DATA += sizeof(Array) + ARR->Count() * sizeof(VariableDATA *) + ARR->NODE_COUNT * sizeof(NODE);
#else
                    memory->NUMBER_DATA += sizeof(Array) + ARR->Count() * sizeof(VariableDATA *) + ARR->NODE_COUNT * sizeof(NODE) + ARR->KeysCount/KEY_INCREMENT * KEY_INCREMENT * sizeof(ArrayKey);
                    if (ARR->KeysCount % KEY_INCREMENT)
                        memory->NUMBER_DATA += KEY_INCREMENT * sizeof(ArrayKey);
                    ArrayKey *KEYS = ARR->Keys;
                    if (KEYS) {
                        for (int j = 0; j < ARR->KeysCount; j++) {
                            if (KEYS[j].KEY)
                                string_memory->NUMBER_DATA += strlen(KEYS[j].KEY) + 1;
                        }
                    }
#endif
                    if ((ARR->flags >= 0) && (ARR->reachable < 0x1C) && (ARR->reachable != reach_id_flag))
                        unreachable_arrays->NUMBER_DATA += 1;
                }
            }
        }
        ARRAYPOOL = (ArrayPool *)ARRAYPOOL->NEXT;
    }
    array_elements->NUMBER_DATA = all_arrays;
    ((PIFAlizator *)PIF)->in_gc = 0;
    ALLOC_UNLOCK
    return 0;
}

CachedFILE *cached_fopen(const char *filename, const char *mode) {
    CachedFILE *fin = 0;
    FILE       *in  = fopen(filename, mode);

    if (in) {
        fin = (CachedFILE *)malloc(sizeof(CachedFILE));
        if (fin) {
            fin->buf_fill = 0;
            fin->buf_pos  = 0;
            fin->in       = in;
            return fin;
        } else {
            fclose(in);
        }
    }
    return 0;
}

int cached_fclose(CachedFILE *in) {
    int res = 0;

    if (in) {
        res = fclose(in->in);
        free(in);
    }
    return res;
}

int cached_fseek(CachedFILE *in, intptr_t offset, int origin) {
    switch (origin) {
        case SEEK_CUR:
            {
                int delta = in->buf_fill - in->buf_pos;
                if (offset < delta) {
                    in->buf_pos += offset;
                } else {
                    offset -= delta;
                    if (offset) {
                        fseek(in->in, offset, SEEK_CUR);
                    }
                    in->buf_fill = 0;
                    in->buf_pos  = 0;
                }
            }
            return 0;
            break;

        case SEEK_SET:
        case SEEK_END:
        default:
            in->buf_fill = 0;
            in->buf_pos  = 0;
            return fseek(in->in, offset, origin);
            break;
    }
    return -1;
}

int cached_fread(void *buf, size_t size, size_t len, CachedFILE *in) {
    int total_size = size * len;
    int total      = 0;

    if ((!in->buf_fill) || (in->buf_pos >= in->buf_fill)) {
        in->buf_fill = fread(in->sbuf, 1, sizeof(in->sbuf), in->in);
        in->buf_pos  = 0;
    }
    if (in->buf_fill > 0) {
        int remaining = in->buf_fill - in->buf_pos;
        if (total_size <= remaining) {
            memcpy(buf, in->sbuf + in->buf_pos, total_size);
            in->buf_pos += total_size;
            total        = total_size;
        } else {
            if (remaining) {
                memcpy(buf, in->sbuf + in->buf_pos, remaining);
                total      += remaining;
                total_size -= remaining;
            }
            do {
                in->buf_fill = fread(in->sbuf, 1, sizeof(in->sbuf), in->in);
                in->buf_pos  = 0;
                if (in->buf_fill > 0) {
                    if (total_size <= in->buf_fill) {
                        memcpy(((char *)buf) + total, in->sbuf, total_size);
                        in->buf_pos = total_size;
                        total      += total_size;
                        break;
                    } else {
                        memcpy(((char *)buf) + total, in->sbuf, in->buf_fill);
                        total      += in->buf_fill;
                        total_size -= in->buf_fill;
                        in->buf_pos = in->buf_fill;
                    }
                }
            } while (in->buf_fill > 0);
        }
    }
    return total / size;
}

#ifdef __BIG_ENDIAN__
#ifndef swap16
#define swap16(x) (((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))
#endif
#ifndef swap32
#define swap32(x) (((x & 0x000000ff) << 24) | ((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8))
#endif

#error "Not tested on big endian. Please remove this error from main.cpp"
int concept_fwrite_int(const void *ptr, int size, int count, FILE * stream) {
    if (count == 1) {
        if (size == 2) {
            unsigned short n = *(unsigned short *)ptr;
            n = swap16(n);
            return concept_fwrite(&n, 2, 1, stream);
        } else
        if (size == 4) {
            unsigned int n = *(unsigned int *)ptr;
            n = swap32(n);
            return concept_fwrite(&n, 4, 1, stream);
        }
    } else
    if (count > 1) {
        int res = 0;
        if (size == 2) {
            unsigned short *ref = (unsigned short *)ptr;
            unsigned short *n = (unsigned short *)malloc(sizeof(unsigned short) * count);
            if (n) {
                for (int i = 0; i < count; i++)
                    n[i] = swap16(ref[i]);
                res = concept_fwrite(n, size, count, stream);
                free(n);
                return res;
            }
        } else
        if (size == 4) {
            unsigned int *ref = (unsigned int *)ptr;
            unsigned int *n = (unsigned int *)malloc(sizeof(unsigned int) * count);
            if (n) {
                for (int i = 0; i < count; i++)
                    n[i] = swap32(ref[i]);
                res = concept_fwrite(n, size, count, stream);
                free(n);
                return res;
            }
        }
    }
    return concept_fwrite(ptr, size, count, stream);
}

int concept_fread_int(void *buf, size_t size, size_t len, CachedFILE *in) {
    int res = concept_fread(buf, size, len, in);
    if (res > 0) {
        if (size == 2) {
            unsigned short *elements = (unsigned short *)buf;
            for (int i = 0; i < len; i++)
                elements[i] = swap16(elements[i]);
        } else
        if (size == 4) {
            unsigned int *elements = (unsigned int *)buf;
            for (int i = 0; i < len; i++)
                elements[i] = swap32(elements[i]);
        }
    }
    return res;
}
#endif

int NotifyParent(int pipe_out, int apid, int msg_id, AnsiString DATA) {
    if (RemoteNotifyParent) {
        return RemoteNotifyParent(pipe_out, apid, msg_id, DATA.Length(), DATA.c_str());
    }

    if ((apid <= 0) || (pipe_out <= 0)) {
        return 0;
    }

    int  len  = DATA.Length();
    char *ptr = DATA.c_str();
    int params[3];
    params[0] = apid;
    params[1] = msg_id;
    params[2] = len;

    write(pipe_out, params, sizeof(int) * 3);
    if (len) {
        while (len > 0) {
            int written = write(pipe_out, ptr, len);
            if (written > 0) {
                len -= written;
                ptr += written;
            } else
                break;
        }
    }
    //}
    return 1;
}

void DeturnatedPrint(void *PIF, char *text, int len, void *userdata) {
    AnsiString data;

    if (len > -1) {
        data.LoadBuffer(text, len);
    } else {
        data = text;
    }
    if (PIF)
        NotifyParent(((PIFAlizator *)PIF)->pipe_write, ((PIFAlizator *)PIF)->parent_apid, -10, data);
}

CONCEPT_DLL_API SetNotifyParent(NOTIFY_PARENT np) {
    RemoteNotifyParent = np;
    return 0;
}

CONCEPT_DLL_API SetDirectPipe(int direct_pipe) {
    cached_direct_pipe = direct_pipe;
    return 0;
}

CONCEPT_DLL_API SetArguments(int argc, char **argv) {
    PIFAlizator::argc = argc;
    PIFAlizator::argv = argv;

    return 0;
}

struct ThreadData {
    NOTIFY_PARENT RemoteNotifyParent;
    int           argc;
    char          **argv;
};

void *GetStaticData() {
#ifndef SINGLE_PROCESS_DLL
    ThreadData *td = new ThreadData();
    td->RemoteNotifyParent = RemoteNotifyParent;
    td->argc = PIFAlizator::argc;
    td->argv = PIFAlizator::argv;

    return td;
#else
    return 0;
#endif
}

void SetStaticData(void *data) {
#ifndef SINGLE_PROCESS_DLL
    ThreadData *td = (ThreadData *)data;
    RemoteNotifyParent = td->RemoteNotifyParent;
    PIFAlizator::argc  = td->argc;
    PIFAlizator::argv  = td->argv;

    delete td;
#endif
}

CONCEPT_DLL_API Concept_CachedInit(char *filename) {
    struct stat buf;
    char        lpBuffer[8192];
    AnsiString  accel_filename(filename);

    accel_filename += (char *)DEFAULT_BIN_EXTENSION;

#ifdef _WIN32
    AnsiString key;
    lpBuffer[8191] = 0;
    GetCurrentDirectory(8191, lpBuffer);
    int i = 0;
    while (lpBuffer[i]) {
        if (lpBuffer[i] == '\\')
            lpBuffer[i] = '/';
        i++;
    }
    key += lpBuffer;
    key += "/";
    key += filename;
#else
    AnsiString key;
    lpBuffer[8191] = 0;
    getcwd(lpBuffer, 8191);
    key += lpBuffer;
    key += "/";
    key += filename;
#endif
    FILE *in = fopen(accel_filename.c_str(), "rb");
    if (!in)
        return -2;
    fclose(in);
#ifdef _WIN32
    if (!stat(accel_filename.c_str(), &buf)) {
        key += ":";
        key += AnsiString((intptr_t)buf.st_size);
        key += ":";
        key += AnsiString((intptr_t)buf.st_mtime);
    }
#endif
    cache_buffer = SHPoolOpen(key.c_str());
    SHLockWait(key.c_str());
    if (cache_buffer) {
        return 0xFFFF;
    }
    if (!cache_buffer) {
        if (!SHLock(key.c_str()))
            return -1;
        int size = PIFAlizator::ComputeSharedSize(accel_filename.c_str());
        if (size > 0) {
            // 255 bytes reserved ...
            size        += 0xFF;
            cache_buffer = SHPoolAlloc(key.c_str(), size);
            // don't unlock the semaphore !!
            if (cache_buffer) {
                return size;
            }
        }
        SHUnLock();
    }
    return -1;
}

CONCEPT_DLL_API Concept_CachedDone(char *filename) {
    if (cache_buffer) {
        SHPoolFree(cache_buffer);
        cache_buffer = 0;
        SHUnLock();
    }
    return 0;
}

CONCEPT_DLL_API Concept_Compile(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent) {
    // programm internal form
    AnsiString S;

    if (S.LoadFile(filename)) {
        return -3;
    }

    SimpleStream SS(fp, sock);

    char is_cgi = 0;
    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }
#ifdef TIMMING
    intptr_t start = clock();
    SS.Print("PIF...");
#endif
    PIFAlizator PIF(inc_dir, lib_dir, &S, &SS, filename, debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY);
    PIF.SetPipe(pipe_in, pipe_out, apid, parent, cached_direct_pipe);
    PIF.Execute();
#ifdef TIMMING
    intptr_t total = clock() - start;
    SS.Print("done\nIn: ");
    SS.Print((double)total);
    SS.Print("\n");
    SS.Print("OPTIMIZE...");
    start = clock();
#endif
    bool build_package = false;
    if (!PIF.ErrorCount()) {
        PIF.Optimize();
        build_package = !PIF.CheckRunable();

        if ((PIF.Errors.Count() == 1) && (build_package)) {
            PIF.Errors.Delete(PIF.Errors.Count() - 1);
        }

        if (build_package) {
            SS.Print("Building package ...\n");
        }
    }
#ifdef TIMMING
    total = clock() - start;
    SS.Print("done\nIn: ");
    SS.Print((double)total);
    SS.Print("\n");
#endif
    if (!PIF.ErrorCount()) {
        if ((PIF.WarningCount()) && (!is_cgi)) {
            SS.Print(PIF.PRINT_WARNINGS());
            SS.Print("\n\nThere are warnings.\n");
        }
        AnsiString accel_filename = AnsiString(filename);
        accel_filename += build_package ? (char *)DEFAULT_PACK_EXTENSION : (char *)DEFAULT_BIN_EXTENSION;
        PIF.Serialize(accel_filename, build_package);
        // set file times ...
        struct stat buf;
        if (!stat(filename, &buf)) {
            struct utimbuf timebuf;
            timebuf.actime  = buf.st_mtime;
            timebuf.modtime = buf.st_mtime;

            // set the time for the accel file
            utime(accel_filename.c_str(), &timebuf);
        }
    } else {
        SS.Print(PIF.PRINT_ERRORS(is_cgi));
        PIFAlizator::Shutdown();
        return -2;
    }
    PIFAlizator::Shutdown();
    return 0;
}

CONCEPT_DLL_API LastResult() {
    return last_result;
}

CONCEPT_DLL_API Concept_Execute(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata) {
    ForeignNotify fp_notify = 0;

    if ((parent > 0) && (pipe_out > 0))
        fp_notify = DeturnatedPrint;

    SimpleStream SS(fp, sock, fp_notify, userdata);
    AnsiString   S;
    if (S.LoadFile(filename)) {
        NotifyParent(pipe_out, parent, -6, "File not found");
        SS.Print(AnsiString("File '") + AnsiString(filename) + "' is not available\n", -1, -2);
        return -3;
    }

    char is_cgi = 0;

    if (debug == -2) {
        is_cgi = 1;
        debug  = 1;
    }

    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }
    PIFAlizator PIF(inc_dir, lib_dir, &S, &SS, filename, debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY);
    PIF.SetPipe(pipe_in, pipe_out, apid, parent, cached_direct_pipe);
    NotifyParent(pipe_out, parent, -1, "Unserializing");
#ifdef TIMMING
    int start = clock();
#endif
    if ((!debug) && (!PIF.Unserialize(AnsiString(filename) + DEFAULT_BIN_EXTENSION))) {
        NotifyParent(pipe_out, parent, -1, "Optimizing");
        PIF.Optimize(0, true);
#ifdef TIMMING
        printf("Total time: %i\n", clock() - start);
#endif
        // in case we have some cache locking
        SHUnLock();
    } else {
        NotifyParent(pipe_out, parent, -1, "Compiling");
        PIF.Execute();
        if (!PIF.ErrorCount()) {
            NotifyParent(pipe_out, parent, -1, "Optimizing");
            PIF.Optimize();
        }
    }
    if (!PIF.ErrorCount()) {
        PIF.CheckRunable();
    }
    if (!PIF.ErrorCount()) {
        if ((PIF.WarningCount()) && (!is_cgi)) {
            SS.Print(PIF.PRINT_WARNINGS());
            SS.Print("\n\nThere are warnings.\n");
            NotifyParent(pipe_out, parent, -5, PIF.SerializeWarningsErrors(1));
        }
        NotifyParent(pipe_out, parent, -4, "Running");
        ClassCode *CC = (ClassCode *)PIF.GetStartingPoint();

        VariableDATA *STATIC_ENTRY = (VariableDATA *)VAR_ALLOC(&PIF);
        STATIC_ENTRY->CLASS_DATA  = 0;
        STATIC_ENTRY->LINKS       = 2;
        STATIC_ENTRY->NUMBER_DATA = 0;
        STATIC_ENTRY->TYPE        = VARIABLE_CLASS;
        STATIC_ENTRY->IS_PROPERTY_RESULT = 0;

        CC->CreateInstance(&PIF, STATIC_ENTRY, 0, 0, 0, NULL, 1);
        STATIC_ENTRY->CLASS_DATA = 0;

        VAR_FREE(STATIC_ENTRY);

        NotifyParent(pipe_out, parent, -6, "Done");
        last_result = PIF.last_result;
        if (PIF.ErrorCount()) {
            if (SS.AlreadyPrinted()) {
                SS.Print(PIF.PRINT_ERRORS(is_cgi ? 2 : 0));
            } else {
                SS.Print(PIF.PRINT_ERRORS(is_cgi));
            }
            if (!is_cgi) {
                SS.Print("There are RUN-TIME errors.", -1, -2);
            } else {
                SS.Print("\n\nThere are RUN-TIME errors.\n");
            }
            NotifyParent(pipe_out, parent, -2, PIF.SerializeWarningsErrors());
            PIFAlizator::Shutdown();
            return -1;
        }
    } else {
        if (SS.AlreadyPrinted()) {
            SS.Print(PIF.PRINT_ERRORS(is_cgi ? 2 : 0));
        } else {
            SS.Print(PIF.PRINT_ERRORS(is_cgi));
        }

        if (!is_cgi) {
            SS.Print("There are compile-time errors.", -1, -2);
        }

        NotifyParent(pipe_out, parent, -3, PIF.SerializeWarningsErrors());
        PIFAlizator::Shutdown();
        return -2;
    }
    PIFAlizator::Shutdown();
    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API SetCheckpointFunction(CHECK_POINT cp) {
    PIFAlizator::CheckPoint = cp;

    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_Execute2(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT cp, void *userdata) {
    PIFAlizator::CheckPoint = cp;

    return Concept_Execute(filename, inc_dir, lib_dir, fp, sock, debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY, pipe_in, pipe_out, apid, parent, userdata);
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_GetSocket() {
    return PIFAlizator::refSOCKET;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_ExecuteBuffer(char *buffer, int len, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, CHECK_POINT cp, void *result, void *module_list, void *userdata) {
    PIFAlizator::CheckPoint = cp;
    ForeignNotify fp_notify = 0;

    if ((parent > 0) && (pipe_out > 0))
        fp_notify = DeturnatedPrint;

    SimpleStream SS(fp, sock, fp_notify, userdata);
    AnsiString   S;
    S.LoadBuffer(buffer, len);

    char is_cgi = 0;

    if (debug == -2) {
        is_cgi = 1;
        debug  = 1;
    }

    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }
    PIFAlizator PIF(inc_dir, lib_dir, &S, &SS, "code", debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY, NULL, result, module_list);
    PIF.is_buffer = 1;
    PIF.SetPipe(pipe_in, pipe_out, apid, parent, cached_direct_pipe);
#ifdef TIMMING
    int start = clock();
#endif

    NotifyParent(pipe_out, parent, -1, "Compiling");
    PIF.Execute();
    if (!PIF.ErrorCount()) {
        NotifyParent(pipe_out, parent, -1, "Optimizing");
        PIF.Optimize();
    }
    if (!PIF.ErrorCount()) {
        PIF.CheckRunable();
    }
    if (!PIF.ErrorCount()) {
        if ((PIF.WarningCount()) && (!is_cgi)) {
            SS.Print(PIF.PRINT_WARNINGS());
            SS.Print("\n\nThere are warnings.\n");
            NotifyParent(pipe_out, parent, -5, PIF.SerializeWarningsErrors(1));
        }
        NotifyParent(pipe_out, parent, -4, "Running");
        ClassCode *CC = (ClassCode *)PIF.GetStartingPoint();

        VariableDATA *STATIC_ENTRY = (VariableDATA *)VAR_ALLOC(&PIF);
        STATIC_ENTRY->CLASS_DATA  = 0;
        STATIC_ENTRY->LINKS       = 2;
        STATIC_ENTRY->NUMBER_DATA = 0;
        STATIC_ENTRY->TYPE        = VARIABLE_CLASS;
        STATIC_ENTRY->IS_PROPERTY_RESULT = 0;

        CC->CreateInstance(&PIF, STATIC_ENTRY, 0, 0, 0, NULL, 1);
        STATIC_ENTRY->CLASS_DATA = 0;

        VAR_FREE(STATIC_ENTRY);

        NotifyParent(pipe_out, parent, -6, "Done");
        last_result = PIF.last_result;
        if (PIF.ErrorCount()) {
            if (SS.AlreadyPrinted()) {
                SS.Print(PIF.PRINT_ERRORS(is_cgi ? 2 : 0));
            } else {
                SS.Print(PIF.PRINT_ERRORS(is_cgi));
            }
            if (!is_cgi) {
                SS.Print("There are RUN-TIME errors.", -1, -2);
            } else {
                SS.Print("\n\nThere are RUN-TIME errors.\n");
            }
            NotifyParent(pipe_out, parent, -2, PIF.SerializeWarningsErrors());
            PIFAlizator::Shutdown();
            return -1;
        }
    } else {
        if (SS.AlreadyPrinted()) {
            SS.Print(PIF.PRINT_ERRORS(is_cgi ? 2 : 0));
        } else {
            SS.Print(PIF.PRINT_ERRORS(is_cgi));
        }

        if (!is_cgi) {
            SS.Print("There are compile-time errors.", -1, -2);
        }

        NotifyParent(pipe_out, parent, -3, PIF.SerializeWarningsErrors());
        PIFAlizator::Shutdown();
        return -2;
    }
    PIFAlizator::Shutdown();
    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API_HANDLER Concept_Execute3_Init(char *filename, char *inc_dir, char *lib_dir, ForeignPrint fp, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, void *userdata) {
    ForeignNotify fp_notify = 0;

    if ((parent > 0) && (pipe_out > 0))
        fp_notify = DeturnatedPrint;

    SimpleStream *SS = new SimpleStream(fp, sock, fp_notify, userdata);
    AnsiString   S;
    if (S.LoadFile(filename)) {
        NotifyParent(pipe_out, parent, -6, "File not found");
        SS->Print(AnsiString("File '") + AnsiString(filename) + "' is not available\n", -1, -2);
        return 0;
    }

    char is_cgi = 0;

    if (debug == -2) {
        is_cgi = 1;
        debug  = 1;
    }

    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }
    PIFAlizator *PIF = new PIFAlizator(inc_dir, lib_dir, &S, SS, filename, debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY);
    PIF->SetPipe(pipe_in, pipe_out, apid, parent, cached_direct_pipe);
    NotifyParent(pipe_out, parent, -1, "Unserializing");
    if ((!debug) && (!PIF->Unserialize(AnsiString(filename) + DEFAULT_BIN_EXTENSION))) {
        NotifyParent(pipe_out, parent, -1, "Optimizing");
        PIF->Optimize(0, true);
        // in case we have some cache locking
        SHUnLock();
    } else {
        NotifyParent(pipe_out, parent, -1, "Compiling");
        PIF->Execute();
        if (!PIF->ErrorCount()) {
            NotifyParent(pipe_out, parent, -1, "Optimizing");
            PIF->Optimize();
        }
    }
    if (!PIF->ErrorCount()) {
        PIF->CheckRunable();
    }
    if (!PIF->ErrorCount()) {
        if ((PIF->WarningCount()) && (!is_cgi)) {
            SS->Print(PIF->PRINT_WARNINGS());
            SS->Print("\n\nThere are warnings.\n");
            NotifyParent(pipe_out, parent, -5, PIF->SerializeWarningsErrors(1));
        }
        NotifyParent(pipe_out, parent, -4, "Running");

        ClassCode *CC = (ClassCode *)PIF->GetStartingPoint();

        Container *res = new Container;
        res->R1     = PIF;
        res->R2     = SS;
        res->R3     = CC;
        res->is_cgi = is_cgi;
        return res;
    } else {
        if (SS->AlreadyPrinted()) {
            SS->Print(PIF->PRINT_ERRORS(is_cgi ? 2 : 0));
        } else {
            SS->Print(PIF->PRINT_ERRORS(is_cgi));
        }

        if (!is_cgi) {
            SS->Print("There are compile-time errors.", -1, -2);
        }

        NotifyParent(pipe_out, parent, -3, PIF->SerializeWarningsErrors());
        delete PIF;
        delete SS;
        return 0;
    }
    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_Execute3_Run(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent) {
    Container *Cptr = (Container *)PTR;

    char is_cgi = 0;

    if (debug == -2) {
        is_cgi = 1;
        debug  = 1;
    }

    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }

    if (!Cptr)
        return -1;
    SimpleStream *SS  = (SimpleStream *)Cptr->R2;
    PIFAlizator  *PIF = (PIFAlizator *)Cptr->R1;
    ClassCode    *CC  = (ClassCode *)Cptr->R3;
    SS->Reinit(sock);

    PIF->Adjust(debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY);
    PIF->SetPipe(pipe_in, pipe_out, apid, parent, cached_direct_pipe);

    VariableDATA *STATIC_ENTRY = (VariableDATA *)VAR_ALLOC(PIF);
    STATIC_ENTRY->CLASS_DATA  = 0;
    STATIC_ENTRY->LINKS       = 2;
    STATIC_ENTRY->NUMBER_DATA = 0;
    STATIC_ENTRY->TYPE        = VARIABLE_CLASS;
    STATIC_ENTRY->IS_PROPERTY_RESULT = 0;

    CC->CreateInstance(PIF, STATIC_ENTRY, 0, 0, 0, 0 );
    STATIC_ENTRY->CLASS_DATA = 0;

    VAR_FREE(STATIC_ENTRY);

    NotifyParent(pipe_out, parent, -6, "Done");
    last_result = PIF->last_result;
    if (PIF->ErrorCount()) {
        if (SS->AlreadyPrinted()) {
            SS->Print(PIF->PRINT_ERRORS(Cptr->is_cgi ? 2 : 0));
        } else {
            SS->Print(PIF->PRINT_ERRORS(Cptr->is_cgi));
        }
        if (!Cptr->is_cgi) {
            SS->Print("There are RUN-TIME errors.", -1, -2);
        } else {
            SS->Print("\n\nThere are RUN-TIME errors.\n");
        }

        NotifyParent(pipe_out, parent, -2, PIF->SerializeWarningsErrors());
        PIF->Clear();
        return 0;
    }
    PIF->Clear();
    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_Execute3_RunClone(void *PTR, SOCKET sock, int debug, DEBUGGER_CALLBACK DEBUGGER_TRAP, void *DEBUGGER_RESERVED, char *SERVER_PUBLIC_KEY, char *SERVER_PRIVATE_KEY, char *CLIENT_PUBLIC_KEY, int pipe_in, int pipe_out, int apid, int parent, int direct_pipe) {
    Container *Cptr = (Container *)PTR;

    char is_cgi = 0;

    if (debug == -2) {
        is_cgi = 1;
        debug  = 1;
    }

    if (debug == -1) {
        is_cgi = 1;
        debug  = 0;
    }

    if (!Cptr)
        return -1;

    ForeignNotify fp_notify = 0;

    if ((parent > 0) && (pipe_out > 0))
        fp_notify = DeturnatedPrint;

    SimpleStream *SSParent  = (SimpleStream *)Cptr->R2;
    PIFAlizator  *PIFParent = (PIFAlizator *)Cptr->R1;
    SimpleStream SS(SSParent->fprint, sock, fp_notify, SSParent->userdata);
    AnsiString   S;
    PIFAlizator  *PIF = new PIFAlizator("", "", &S, &SS, "", debug, DEBUGGER_TRAP, DEBUGGER_RESERVED, SERVER_PUBLIC_KEY, SERVER_PRIVATE_KEY, CLIENT_PUBLIC_KEY, PIFParent);
    PIF->SetPipe(pipe_in, pipe_out, apid, parent, direct_pipe);

    ClassCode *CC = (ClassCode *)Cptr->R3;

    VariableDATA *STATIC_ENTRY = (VariableDATA *)VAR_ALLOC(PIF);
    STATIC_ENTRY->CLASS_DATA  = 0;
    STATIC_ENTRY->LINKS       = 2;
    STATIC_ENTRY->NUMBER_DATA = 0;
    STATIC_ENTRY->TYPE        = VARIABLE_CLASS;
    STATIC_ENTRY->IS_PROPERTY_RESULT = 0;

    CC->CreateInstance(PIF, STATIC_ENTRY, 0, 0, 0, NULL, 1);
    STATIC_ENTRY->CLASS_DATA = 0;

    VAR_FREE(STATIC_ENTRY);

    NotifyParent(pipe_out, parent, -6, "Done");
    last_result = PIF->last_result;
    if (PIF->ErrorCount()) {
        if (SS.AlreadyPrinted()) {
            SS.Print(PIF->PRINT_ERRORS(Cptr->is_cgi ? 2 : 0));
        } else {
            SS.Print(PIF->PRINT_ERRORS(Cptr->is_cgi));
        }
        if (!Cptr->is_cgi) {
            SS.Print("There are RUN-TIME errors.", -1, -2);
        } else {
            SS.Print("\n\nThere are RUN-TIME errors.\n");
        }

        NotifyParent(pipe_out, parent, -2, PIF->SerializeWarningsErrors());
        delete PIF;
        return 0;
    }
    delete PIF;
    return 0;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API Concept_Execute3_Done(void *PTR) {
    Container *Cptr = (Container *)PTR;

    if (!Cptr)
        return -1;
    SimpleStream *SS  = (SimpleStream *)Cptr->R2;
    PIFAlizator  *PIF = (PIFAlizator *)Cptr->R1;

    delete Cptr;
    delete SS;
    delete PIF;
    PIFAlizator::Shutdown();
    return 0;
}

//-----------------------------------------------------------------------------------
#ifdef _WIN32
//-----------------------------------------------------------------------------------
// DLL Entry Point ... nothing ...
int WINAPI DllEntryPoint(HINSTANCE hinst, uintptr_t reason, void *lpReserved) {
    return 1;
}

//-----------------------------------------------------------------------------------
#endif

