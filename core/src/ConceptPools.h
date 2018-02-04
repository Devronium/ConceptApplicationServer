#ifndef __CONCEPT_POOLS_H
#define __CONCEPT_POOLS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "semhh.h"

// #define SIMPLE_MULTI_THREADING
#define JIT_RUNTIME_CHECKS
#define OPTIONAL_SEPARATOR

#if defined(_WIN32) || defined(__linux__)
// on FreeBSD dlmalloc is the default memory allocator
// works when using posix locks instead of spin locks
 #define USE_DLMALLOC
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

#define concept_FILE             CachedFILE

struct CachedFILE {
    FILE *in;

    char sbuf [8192];
    int  buf_fill;
    int  buf_pos;
};

int cached_fread(void *buf, size_t size, size_t len, CachedFILE *in);
int cached_fseek(CachedFILE *in, intptr_t offset, int origin);
CachedFILE *cached_fopen(const char *filename, const char *mode);
int cached_fclose(CachedFILE *in);

#define concept_fread            cached_fread
#define concept_fread_buffer     cached_fread
#define concept_fwrite           fwrite
#define concept_fwrite_buffer    fwrite
#define concept_fseek            cached_fseek
#define concept_fopen            cached_fopen
#define concept_fclose           cached_fclose

#ifdef __BIG_ENDIAN__
int concept_fwrite_int(const void *ptr, int size, int count, FILE * stream);
int concept_fread_int(void *buf, size_t size, size_t len, CachedFILE *in);
#else
#define concept_fread_int        cached_fread
#define concept_fwrite_int       fwrite
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
}

 #define FAST_MALLOC(size)          dlmalloc(size)
 #define FAST_CALLOC(size)          dlcalloc(num, size)
 #define FAST_FREE(ptr)             dlfree(ptr)
 #define FAST_REALLOC(ptr, size)    dlrealloc(ptr, size)
#else
 #define FAST_MALLOC(size)          malloc(size)
 #define FAST_CALLOC(size)          calloc(size)
 #define FAST_FREE(ptr)             free(ptr)
 #define FAST_REALLOC(ptr, size)    realloc(ptr, size)
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
#endif // __CONCEPT_POOLS_H
