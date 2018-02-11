#ifndef __GARBAGE_COLLECTOR_H
#define __GARBAGE_COLLECTOR_H
#include <stdlib.h>
#include <stdint.h>
#include "ConceptPools.h"
#include "Codes.h"
//#include "semhh.h"

#define USE_HASHTABLE_GC

// no freing of the memory when the application closes (faster). Comment this if it should free.
//#define FAST_EXIT_NO_GC_CALL
typedef void (*foreach_slot)(void *data);

#ifdef USE_HASHTABLE_GC
    #include "khash.h"
    KHASH_SET_INIT_INT64(int64hashtable);
#else
typedef struct _GarbageElement {
    void            *DATA;
    _GarbageElement *NEXT;
    POOLED(_GarbageElement)
} GarbageElement;
#endif

class GarbageCollector {
#ifdef USE_HASHTABLE_GC
    khash_t (int64hashtable) *BASE;
#else
    GarbageElement *BASE;
#endif
public:
    POOLED(GarbageCollector)

    inline void Foreach(foreach_slot fs) {
#ifdef USE_HASHTABLE_GC
        if (!BASE)
            return;
        for (khint_t k = kh_begin(BASE); k != kh_end(BASE); ++k) {
            if (kh_exist(BASE, k))
                fs((void *)(uintptr_t)kh_key(BASE, k));
        }
#else
        GarbageElement *NODE = BASE;

        while (NODE) {
            if (NODE->DATA) {
                fs(NODE->DATA);
            }
            NODE = NODE->NEXT;
        }
#endif
    }

    inline bool IsReferenced(void *data) {
#ifdef USE_HASHTABLE_GC
        if (BASE) {
            khiter_t k = kh_get(int64hashtable, BASE, (uintptr_t)data);
            if ((k != kh_end(BASE)) && (kh_exist(BASE, k)))
                return true;

        }
        return false;
#else
        GarbageElement *NODE = BASE;

        while (NODE) {
            if (NODE->DATA == data)
                return true;
            NODE = NODE->NEXT;
        }
        return false;
#endif
    }

    inline void Reference(void *data) {
#ifdef USE_HASHTABLE_GC
        if (!BASE)
            BASE = kh_init(int64hashtable);

        int ret;
        khiter_t k = kh_put(int64hashtable, BASE, (uintptr_t)data, &ret);
#else
        if (!IsReferenced(data)) {
            GarbageElement *NODE = (GarbageElement *)malloc(sizeof(GarbageElement));

            NODE->NEXT = BASE;
            NODE->DATA = data;

            BASE = NODE;
            //}
        }
#endif
    }

    inline void *Unreference(void *data) {
#ifdef USE_HASHTABLE_GC
        if (BASE) {
            khiter_t k = kh_get(int64hashtable, BASE, (uintptr_t)data);
            intptr_t index = 0;
            if (k != kh_end(BASE))
                kh_del(int64hashtable, BASE, k);
            return data;
        }
#else
        GarbageElement *NODE      = BASE;
        GarbageElement *NODE_PREC = 0;

        while (NODE) {
            if (NODE->DATA == data) {
                if (NODE_PREC) {
                    NODE_PREC->NEXT = NODE->NEXT;
                } else {
                    BASE = NODE->NEXT;
                }

                free(NODE);
                return data;
            }
            NODE_PREC = NODE;
            NODE      = NODE->NEXT;
        }
#endif
        return 0;
    }

    void Call_All_Destructors(void *PIF);
    void EndOfExecution_SayBye_Objects();
    void EndOfExecution_SayBye_Arrays();
    void EndOfExecution_SayBye_Variables();

    GarbageCollector(void);
    ~GarbageCollector(void);
};
#endif //__GARBAGE_COLLECTOR_H
