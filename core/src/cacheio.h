#ifndef __CACHEIO_H
#define __CACHEIO_H

#include <stdio.h>

#define concept_FILE             CachedFILE

struct CachedFILE {
    FILE *in;

    char sbuf [8192];
    int  buf_fill;
    int  buf_pos;
};

void dtoa_milo_c_wrapper(double d, char *buf);

int cached_fread(void *buf, size_t size, size_t len, struct CachedFILE *in);
int cached_fseek(struct CachedFILE *in, intptr_t offset, int origin);
struct CachedFILE *cached_fopen(const char *filename, const char *mode);
int cached_fclose(struct CachedFILE *in);

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

#define FREAD_FAIL(buffer, size, elems, fin)  if (!concept_fread(buffer, size, elems, fin)) { concept_fclose(fin); return 0; }

#ifdef __BIG_ENDIAN__
#define FREAD_FAIL(buffer, size, elems, fin)  if (!concept_fread_int(buffer, size, elems, fin)) { concept_fclose(fin); return 0; }
#else
    #define FREAD_INT_FAIL  FREAD_FAIL
#endif

#define SKIP(bytes, fin)    concept_fseek(fin, bytes, SEEK_CUR);

#endif
