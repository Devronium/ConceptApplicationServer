#ifndef __CONCEPT_POOLS_H
#define __CONCEPT_POOLS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "semhh.h"
extern "C" {
    #include "simple/plainstring.h"
    #include "cacheio.h"
}
// #define SIMPLE_MULTI_THREADING
#define DISABLE_DEPRECATED_FEATURES
#define JIT_RUNTIME_CHECKS
#define OPTIONAL_SEPARATOR
#define MAX_RUNTIME_ERRORS      480

#if defined(_WIN32) || defined(__linux__)
// on FreeBSD dlmalloc is the default memory allocator
// works when using posix locks instead of spin locks
 #define USE_DLMALLOC
 #define USE_MEMORY_SPACE
#endif

#ifdef USE_BOOST_LIBRARY
 #include <boost/pool/object_pool.hpp>

 #define POOLED(PCLS)                                                                       \
    static boost::object_pool<PCLS> POOL;                                                   \
    inline void PCLS::operator delete(void *p, size_t size) { PCLS::POOL.free((PCLS *)p); } \
    inline void *PCLS::operator new(size_t size) { return PCLS::POOL.malloc(); }

 #define POOLED_IMPLEMENTATION(PCLS)    boost::object_pool<PCLS> PCLS::POOL;
#else
 #define POOLED(PCLS)
 #define POOLED_IMPLEMENTATION(PCLS)
#endif

#define POOL_BLOCK_SIZE           127
#define OBJECT_POOL_BLOCK_SIZE    124
#define ARRAY_POOL_BLOCK_SIZE     124
// 8k * sizeof(void *) buffer
#define BLOCK_STACK_SIZE          8192 //1024

#define POOL_OFFSET(type, field)  ((uintptr_t)&(((type *)0)->field))

void *AllocVAR(void *pif);
void *GetPOOLContext(void *refVAR);
void FreeVAR(void *refVAR);
void AllocMultipleVars(void **context, void *pif, int count, int offset);
void FreeMultipleVars(void **refVARs, int count);

void *AllocClassObject(void *PIF);
void FreeClassObject(void *refObject);

void *AllocArray(void *PIF, bool skip_top = false);
void FreeArray(void *refObject);

int ModuleCheckReachability(void *PIF);
int CheckReachability(void *PIF, bool skip_top = false);
void AddGCRoot(void *PIF, void *CONTEXT);
void RemoveGCRoot(void *PIF, void *CONTEXT);
int GetMemoryStatistics(void *PIF, void *RESULT);

#ifdef USE_DLMALLOC
 extern "C" {
     void *dlmalloc(size_t);
     void *dlcalloc(size_t, size_t);
     void  dlfree(void *);
     void *dlrealloc(void *, size_t);
     size_t dlmalloc_footprint(void);
     int dlmalloc_trim(size_t pad);

     struct dl_mallinfo {
        size_t arena;
        size_t ordblks;
        size_t smblks;
        size_t hblks;
        size_t hblkhd;
        size_t usmblks;
        size_t fsmblks;
        size_t uordblks;
        size_t fordblks;
        size_t keepcost;
     };

     struct dl_mallinfo dlmallinfo(void);
#ifdef USE_MEMORY_SPACE
     void *create_mspace(size_t capacity, int locked);
     size_t destroy_mspace(void *msp);
     void *mspace_malloc(void *msp, size_t bytes);
     void mspace_free(void *msp, void *mem);
     void *mspace_realloc(void *msp, void *mem, size_t newsize);
     void *mspace_calloc(void *msp, size_t n_elements, size_t elem_size);
     size_t mspace_footprint(void *msp);
     struct dl_mallinfo mspace_mallinfo(void *msp);
     int dlmallopt(int param_number, int value);
     int mspace_trim(void *msp, size_t pad);
#endif
 }

#ifdef USE_MEMORY_SPACE
 #define FAST_MALLOC(pif, size)          (pif ? mspace_malloc(((PIFAlizator *)(pif))->memory, size) : dlmalloc(size))
 #define FAST_CALLOC(pif, num, size)     (pif ? mspace_calloc(((PIFAlizator *)(pif))->memory, num, size) : dlcalloc(num, size))
 #define FAST_FREE(pif, ptr)             (pif ? mspace_free(((PIFAlizator *)(pif))->memory, ptr) : dlfree(ptr))
 #define FAST_REALLOC(pif, ptr, size)    (pif ? mspace_realloc(((PIFAlizator *)(pif))->memory, ptr, size) : dlrealloc(ptr, size))
 #define FAST_MALLINFO(pif)              mspace_mallinfo(((PIFAlizator *)(pif))->memory)
 #define FAST_FOOTPRINT(pif)             mspace_footprint(((PIFAlizator *)(pif))->memory)
 #define FAST_TRIM(pif)                  (pif ? mspace_trim(((PIFAlizator *)(pif))->memory, 0) : dlmalloc_trim(0))

#ifdef _WIN32
 #define FAST_MSPACE_CREATE(memory)      memory = create_mspace(0, 0)
#else
 // set page size to 64K. On most systems default max_mmap_count is 65530 and page size 4096 ~ 256MB of memory. 
 // With page size of 64k, we get about 4GB of memory.
 #define FAST_MSPACE_CREATE(memory)      dlmallopt(-2, 64 * 1024); memory = create_mspace(0, 0)
#endif
 #define FAST_MSPACE_DESTROY(memory)     destroy_mspace(memory)
#else
 #define FAST_MALLOC(pif, size)          dlmalloc(size)
 #define FAST_CALLOC(pif, size)          dlcalloc(num, size)
 #define FAST_FREE(pif, ptr)             dlfree(ptr)
 #define FAST_REALLOC(pif, ptr, size)    dlrealloc(ptr, size)
 #define FAST_MALLINFO(pif)              dlmallinfo()
 #define FAST_FOOTPRINT(pif)             dlmalloc_footprint()
 #define FAST_TRIM(pif)                  dlmalloc_trim(0)
#endif
#else
 #define FAST_MALLOC(pif, size)          malloc(size)
 #define FAST_CALLOC(pif, size)          calloc(size)
 #define FAST_FREE(pif, ptr)             free(ptr)
 #define FAST_REALLOC(pif, ptr, size)    realloc(ptr, size)
 #define FAST_MALLINFO(pif)              mallinfo()
 #define FAST_FOOTPRINT(pif)             -1
 #define FAST_TRIM(pif)
#endif

#ifdef SIMPLE_MULTI_THREADING
 #ifdef USE_DLMALLOC
  #define VAR_ALLOC(pif)                                 dlmalloc(sizeof(VariableDATA))
  #define VAR_FREE(refVAR)                               dlfree(refVAR)
 #else
  #define VAR_ALLOC(pif)                                 malloc(sizeof(VariableDATA))
  #define VAR_FREE(refVAR)                               free(refVAR)
 #endif
 #define POOL_CONTEXT(var)                               NULL
#else
 #define VAR_ALLOC(pif)                                  AllocVAR(pif)
 #define VAR_FREE(refVAR)                                FreeVAR(refVAR)

 #define POOL_BLOCK_ALLOC
 #define POOL_STACK
 #define BLOCK_VAR_ALLOC(context, pif, count, offset)    AllocMultipleVars((void **)context, pif, count, offset)
 #define BLOCK_VAR_FREE(context, count)                  FreeMultipleVars((void **)context, count)
 #define POOL_CONTEXT(var)                               GetPOOLContext(var)
#endif

#define OBJECT_MALLOC(pif, size)                         FAST_MALLOC(pif, size)
#define OBJECT_FREE(pif, ptr)                            FAST_FREE(pif, ptr)
#endif // __CONCEPT_POOLS_H
