#ifndef __GARBAGE_COLLECTOR_H
#define __GARBAGE_COLLECTOR_H
#include <stdlib.h>
#include "ConceptPools.h"
#include "Codes.h"
//#include "semhh.h"

#define USE_MAP_GC

// no freing of the memory when the application closes (faster). Comment this if it should free.
//#define FAST_EXIT_NO_GC_CALL
typedef void (*foreach_slot)(void *data);

#ifdef USE_MAP_GC
 #include <set>
#else
typedef struct _GarbageElement {
    void            *DATA;
    _GarbageElement *NEXT;
    POOLED(_GarbageElement)
} GarbageElement;
#endif

class GarbageCollector {
#ifdef USE_MAP_GC
    std::set<void *> *BASE;
#else
    GarbageElement *BASE;
#endif
public:
    POOLED(GarbageCollector)

    inline void Foreach(foreach_slot fs) {
#ifdef USE_MAP_GC
        if (!BASE)
            return;
        std::set<void *>::iterator end = BASE->end();
        for (std::set<void *>::iterator i = BASE->begin(); i != end; ++i)
            fs(*i);
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
#ifdef USE_MAP_GC
        if (BASE)
            return (BASE->find(data) != BASE->end());
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
#ifdef USE_MAP_GC
        if (!BASE)
            BASE = new std::set<void *>();

        BASE->insert(data);
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
#ifdef USE_MAP_GC
        if (BASE) {
            BASE->erase(data);
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

