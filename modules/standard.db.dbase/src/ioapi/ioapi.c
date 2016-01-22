/* ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   Version 1.01, May 8th, 2004

   Copyright (C) 1998-2004 Gilles Vollant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "ioapi.h"
#ifdef _WIN32
 #include <io.h>
#else
 #include <sys/stat.h>
ZPOS_T _filelength(int fd) {
    struct stat st;

    return (-1 != fstat(fd, &st)) ? st.st_size : -1;
}
#endif
/* local: I want to #include "ioapi.c" and 'call' xxxx_file_func inline, with no function call overhead. Troels */
#ifndef local
 #define local    __inline static
#endif


/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
 #define SEEK_CUR    1
#endif

#ifndef SEEK_END
 #define SEEK_END    2
#endif

#ifndef SEEK_SET
 #define SEEK_SET    0
#endif

local voidpf ZCALLBACK fopen_file_func OF((
                                              voidpf opaque,
                                              const char *filename,
                                              int mode));

local uLong ZCALLBACK fread_file_func OF((
                                             voidpf opaque,
                                             voidpf stream,
                                             void *buf,
                                             uLong size));

local uLong ZCALLBACK fwrite_file_func OF((
                                              voidpf opaque,
                                              voidpf stream,
                                              const void *buf,
                                              uLong size));

local ZPOS_T ZCALLBACK ftell_file_func OF((
                                              voidpf opaque,
                                              voidpf stream));

local long ZCALLBACK fseek_file_func OF((
                                            voidpf opaque,
                                            voidpf stream,
                                            ZOFF_T offset,
                                            int origin));

local int ZCALLBACK fclose_file_func OF((
                                            voidpf opaque,
                                            voidpf stream));

local int ZCALLBACK ferror_file_func OF((
                                            voidpf opaque,
                                            voidpf stream));


local voidpf ZCALLBACK fopen_file_func(opaque, filename, mode)
voidpf opaque;

const char *filename;
int        mode;
{
    FILE       *file       = NULL;
    const char *mode_fopen = NULL;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
        mode_fopen = "rb";
    else
    if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        mode_fopen = "r+b";
    else
    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        mode_fopen = "wb";

    /* TROELS BEGIN
       else
       if (mode & ZLIB_FILEFUNC_MODE_WRITE)
        mode_fopen = "a+";
       TROELS END */

    if ((filename != NULL) && (mode_fopen != NULL))
        file = fopen(filename, mode_fopen);

    return file;
}


local uLong ZCALLBACK fread_file_func(opaque, stream, buf, size)
voidpf opaque;

voidpf stream;
void   *buf;
uLong  size;
{
    uLong ret;
    ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}


local uLong ZCALLBACK fwrite_file_func(opaque, stream, buf, size)
voidpf opaque;

voidpf     stream;
const void *buf;
uLong      size;
{
    uLong ret;
    ret = (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}

local ZPOS_T ZCALLBACK ftell_file_func(opaque, stream)
voidpf opaque;

voidpf stream;
{
    ZPOS_T ret;
#if defined(__USE_FILE_OFFSET64) && defined(_WIN32)
    fgetpos((FILE *)stream, &ret);
#else
    ret = ftell((FILE *)stream);
#endif
    return ret;
}

local long ZCALLBACK fseek_file_func(opaque, stream, offset, origin)
voidpf opaque;

voidpf stream;
ZOFF_T offset;
int    origin;
{
    long ret = 0;
#ifdef xFILESIZE64
    fflush((FILE *)stream);
    ret = (_lseeki64(_fileno((FILE *)stream), offset, origin) == -1) ? -1 : 0;
#elif defined(__USE_FILE_OFFSET64) && defined(_WIN32)
    ZPOS_T pos;
    FILE   *file = (FILE *)stream;
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
 #ifdef _WIN32
            fgetpos(file, &pos);
            pos += offset;
 #else
            pos = ftell(file) + offset;
 #endif
            break;

        case ZLIB_FILEFUNC_SEEK_END:
 #ifdef _WIN32
            pos = _filelengthi64(_fileno(file));
 #else
            pos = _filelength(_fileno(file));
 #endif
            pos += offset;
            break;

        case ZLIB_FILEFUNC_SEEK_SET:
            pos = offset;
            break;

        default:
            return -1;
    }
    ret = fsetpos(file, &pos) ? -1 : 0;
#else
    int fseek_origin = 0;
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
            fseek_origin = SEEK_CUR;
            break;

        case ZLIB_FILEFUNC_SEEK_END:
            fseek_origin = SEEK_END;
            break;

        case ZLIB_FILEFUNC_SEEK_SET:
            fseek_origin = SEEK_SET;
            break;

        default:
            return -1;
    }
    ret = fseek((FILE *)stream, offset, fseek_origin) ? -1 : 0;
#endif
    return ret;
}

local int ZCALLBACK fclose_file_func(opaque, stream)
voidpf opaque;

voidpf stream;
{
    int ret;
    ret = fclose((FILE *)stream);
    return ret;
}

local int ZCALLBACK ferror_file_func(opaque, stream)
voidpf opaque;

voidpf stream;
{
    int ret;
    ret = ferror((FILE *)stream);
    return ret;
}

void fill_fopen_filefunc(pzlib_filefunc_def)
zlib_filefunc_def * pzlib_filefunc_def;

{
    pzlib_filefunc_def->zopen_file  = fopen_file_func;
    pzlib_filefunc_def->zread_file  = fread_file_func;
    pzlib_filefunc_def->zwrite_file = fwrite_file_func;
    pzlib_filefunc_def->ztell_file  = ftell_file_func;
    pzlib_filefunc_def->zseek_file  = fseek_file_func;
    pzlib_filefunc_def->zclose_file = fclose_file_func;
    pzlib_filefunc_def->zerror_file = ferror_file_func;
    pzlib_filefunc_def->opaque      = NULL;
}
