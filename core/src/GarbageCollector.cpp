#include "GarbageCollector.h"
#include "CompiledClass.h"
#include "ClassCode.h"
#include "Array.h"
#include "PIFAlizator.h"

POOLED_IMPLEMENTATION(_GarbageElement)
POOLED_IMPLEMENTATION(GarbageCollector)

GarbageCollector::GarbageCollector() {
    BASE = 0;
}

void GarbageCollector::Call_All_Destructors(void *PIF) {
#ifdef USE_HASHTABLE_GC
    if (BASE) {
        for (khint_t k = kh_begin(BASE); k != kh_end(BASE); ++k) {
            if (kh_exist(BASE, k)) {
                CompiledClass *ptr = (struct CompiledClass *)(uintptr_t)kh_key(BASE, k);
                if (CompiledClass_HasDestructor(ptr))
                    CompiledClass_Destroy(ptr, (PIFAlizator *)PIF);
            }
        }
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    while (NODE) {
        NODE2 = NODE->NEXT;
        CompiledClass *ptr = (struct CompiledClass *)NODE->DATA;

        if (CompiledClass_HasDestructor(ptr)) {
            CompiledClass_Destroy(PIF);
        }

        NODE = NODE2;
    }
#endif
}

void GarbageCollector::EndOfExecution_SayBye_Objects() {
#ifdef USE_HASHTABLE_GC
    if (BASE) {
        khash_t (int64hashtable) *BASE2 = BASE;
        BASE = 0;
        for (khint_t k = kh_begin(BASE2); k != kh_end(BASE2); ++k) {
            if (kh_exist(BASE2, k)) {
                CompiledClass *ptr = (struct CompiledClass *)(uintptr_t)kh_key(BASE2, k);
                if (ptr) {
                    ptr->LINKS = -1;
                    if (ptr->_CONTEXT) {
                        FAST_FREE(ptr->_CONTEXT);
                        ptr->_CONTEXT = NULL;
                    }
                    delete_CompiledClass(ptr);
                }
            }
        }
        kh_destroy(int64hashtable, BASE2);
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    // to prevent recursive re-execution
    BASE = 0;
    while (NODE) {
        NODE2 = NODE->NEXT;
        CompiledClass *ptr = (struct CompiledClass *)NODE->DATA;
        free(NODE);
        NODE = NODE2;

        ptr->LINKS = -1;
        if (ptr->_CONTEXT) {
            FAST_FREE(ptr->_CONTEXT);
            ptr->_CONTEXT = NULL;
        }
        delete_CompiledClass(ptr);
    }
#endif
}

void GarbageCollector::EndOfExecution_SayBye_Arrays() {
#ifdef USE_HASHTABLE_GC
    if (BASE) {
        khash_t (int64hashtable) *BASE2 = BASE;
        BASE = 0;
        for (khint_t k = kh_begin(BASE2); k != kh_end(BASE2); ++k) {
            if (kh_exist(BASE2, k)) {
                Array *ptr = (Array *)(uintptr_t)kh_key(BASE2, k);
                if (ptr) {
                    ptr->LINKS = -1;
                    delete ptr;
                }
            }
        }
        kh_destroy(int64hashtable, BASE2);
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    BASE = 0;
    while (NODE) {
        NODE2 = NODE->NEXT;
        Array *ptr = (Array *)NODE->DATA;
        free(NODE);
        NODE = NODE2;

        ptr->LINKS = -1;

        delete ptr;
    }
#endif
}

void GarbageCollector::EndOfExecution_SayBye_Variables() {
#ifdef USE_HASHTABLE_GC
    if (BASE) {
        khash_t (int64hashtable) *BASE2 = BASE;
        BASE = 0;
        for (khint_t k = kh_begin(BASE2); k != kh_end(BASE2); ++k) {
            if (kh_exist(BASE2, k)) {
                VariableDATA *ptr = (VariableDATA *)(uintptr_t)kh_key(BASE2, k);
                if (ptr) {
                    if ((ptr->CLASS_DATA) && (ptr->TYPE == VARIABLE_STRING)) {
                        plainstring_delete((struct plainstring *)ptr->CLASS_DATA);
                        ptr->CLASS_DATA = NULL;
                    }
                    VAR_FREE(ptr);
                }
            }
        }
        kh_destroy(int64hashtable, BASE2);
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    BASE = 0;
    while (NODE) {
        NODE2 = NODE->NEXT;
        VariableDATA *ptr = (VariableDATA *)NODE->DATA;
        free(NODE);
        NODE = NODE2;
        if (ptr)
            VAR_FREE(ptr);
    }
#endif
}

GarbageCollector::~GarbageCollector() {
#ifdef USE_HASHTABLE_GC
    if (BASE) {
        kh_destroy(int64hashtable, BASE);
        BASE = 0;
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    while (NODE) {
        NODE2 = NODE->NEXT;
        free(NODE);
        NODE = NODE2;
    }
#endif
}
