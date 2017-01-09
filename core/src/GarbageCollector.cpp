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
#ifdef USE_MAP_GC
    if (BASE) {
        std::set<void *>::iterator end = BASE->end();
        for (std::set<void *>::iterator i = BASE->begin(); i != end; ++i) {
            CompiledClass *ptr = (CompiledClass *)(*i);
            if (ptr->HasDestructor())
                ptr->Destroy((PIFAlizator *)PIF);
        }
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    while (NODE) {
        NODE2 = NODE->NEXT;
        CompiledClass *ptr = (CompiledClass *)NODE->DATA;

        if (ptr->HasDestructor()) {
            ptr->Destroy();
        }

        NODE = NODE2;
    }
#endif
}

void GarbageCollector::EndOfExecution_SayBye_Objects() {
#ifdef USE_MAP_GC
    if (BASE) {
        std::set<void *> *BASE2 = BASE;
        BASE = 0;
        std::set<void *>::iterator end = BASE2->end();
        for (std::set<void *>::iterator i = BASE2->begin(); i != end; ++i) {
            CompiledClass *ptr = (CompiledClass *)(*i);
            if (ptr) {
                ptr->LINKS = -1;
                if (ptr->_CONTEXT) {
                    FAST_FREE(ptr->_CONTEXT);
                    ptr->_CONTEXT = NULL;
                }
                delete ptr;
            }
        }
        BASE2->clear();
        delete BASE2;
    }
#else
    GarbageElement *NODE  = BASE;
    GarbageElement *NODE2 = 0;

    // to prevent recursive re-execution
    BASE = 0;
    while (NODE) {
        NODE2 = NODE->NEXT;
        CompiledClass *ptr = (CompiledClass *)NODE->DATA;
        free(NODE);
        NODE = NODE2;

        ptr->LINKS = -1;
        if (ptr->_CONTEXT) {
            FAST_FREE(ptr->_CONTEXT);
            ptr->_CONTEXT = NULL;
        }
        delete ptr;
    }
#endif
}

void GarbageCollector::EndOfExecution_SayBye_Arrays() {
#ifdef USE_MAP_GC
    if (BASE) {
        std::set<void *> *BASE2 = BASE;
        BASE = 0;
        std::set<void *>::iterator end = BASE2->end();
        for (std::set<void *>::iterator i = BASE2->begin(); i != end; ++i) {
            Array *ptr = (Array *)(*i);
            if (ptr) {
                ptr->LINKS = -1;
                delete ptr;
            }
        }
        BASE2->clear();
        delete BASE2;
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
#ifdef USE_MAP_GC
    if (BASE) {
        std::set<void *> *BASE2 = BASE;
        BASE = 0;

        std::set<void *>::iterator end = BASE2->end();
        for (std::set<void *>::iterator i = BASE2->begin(); i != end; ++i) {
            VariableDATA *ptr = (VariableDATA *)(*i);
            if (ptr) {
                if ((ptr->CLASS_DATA) && (ptr->TYPE == VARIABLE_STRING))
                    delete (AnsiString *)ptr->CLASS_DATA;
                VAR_FREE(ptr);
            }
        }
        BASE2->clear();
        delete BASE2;
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
#ifdef USE_MAP_GC
    if (BASE) {
        BASE->clear();
        delete BASE;
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
