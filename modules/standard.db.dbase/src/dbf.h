/* dbf.h */
/* Copyright (c) 2007-2012 by Troels K. All rights reserved. */
/* License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt */
/*  Partially based on Turbo C source code by Mark Sadler.      */
/*  Partially based on MFC source code by www.pablosoftwaresolutions.com 2002   */

#ifndef __DBF_H__
#define __DBF_H__

#ifndef __BOOL_H__
 #include "bool.h"
#endif

#ifndef EXTERN_C
 #ifdef __cplusplus
  #define EXTERN_C    extern "C"
 #else
  #define EXTERN_C    extern
 #endif
#endif

struct zlib_filefunc_def_s;
struct _DBF_FIELD_DATA;
struct _DBF_DATA;
typedef struct _DBF_DATA * DBF_HANDLE;
struct tm;

#define DBF_MAJOR_VERSION             1
#define DBF_MINOR_VERSION             2
#define DBF_SVN_VERSION               323
#define DBF_WEBSITE                   "http://sf.net/projects/dbase"
#define DBF_FORMAT_NAME               "dBASE"
#define DBF_LIB_NAME                  "dbflib"
#define DBF_AUTHOR                    "Troels K"

#define DBF_FILEEXT                   "dbf"
#define DBF_FILEEXT_MEMO              "dbt"

#define DBF_DBASE3_FIELDNAMELENGTH    11
#define DBF_DBASE4_FIELDNAMELENGTH    32

enum dbf_charconv {
    ENUM_dbf_charconv_oem_host,
    ENUM_dbf_charconv_off,
    ENUM_dbf_charconv_enumcount,
    ENUM_dbf_charconv_compatible = ENUM_dbf_charconv_oem_host
};

#ifndef NAME_MAX
 #define NAME_MAX                      255
#endif

#define DBF_LEN_DATE                   8
#define DBF_LEN_TIME                   9
#define DBF_LEN_DATETIME               (DBF_LEN_DATE + DBF_LEN_TIME)
#define DBF_LEN_BOOLEAN                1
#define DBF_LEN_YEAR                   4

/* error codes */
#define      DBASE_OUT_OF_MEM          100   /*   not enough memory error message */
#define      DBASE_NO_FILE             101   /*   file not found error message */
#define      DBASE_BAD_FORMAT          102   /*   file is not a dBASE III file */
#define      DBASE_INVALID_RECORD      103   /*   requested record does not exist */
#define      DBASE_EOF                 104   /*  the end of the file */
#define      DBASE_BOF                 105   /*  the begin of the file */
#define      DBASE_NOT_OPEN            106   /*  file is not open */
#define      DBASE_INVALID_FIELDNO     107   /*  invalid field number */
#define      DBASE_INVALID_DATA        108   /*   invalid data error message */
#define      DBASE_SUCCESS             109   /*   operation succesfull */
#define      DBASE_NOT_FOUND           110   /*   record not found */
#define      DBASE_WRITE_ERROR         111   /*  write error */
#define      DBASE_READ_ERROR          112   /*  read error */
#define      DBASE_INVALID_BLOCK_NO    113   /*  invalid memo block number */
#define      DBASE_NO_MEMO_DATA        114   /*  no memo data available */

#define DBF_FIELD                      struct _DBF_FIELD_DATA

enum dbf_data_type {
    DBF_DATA_TYPE_CHAR,
        DBF_DATA_TYPE_INTEGER,
        DBF_DATA_TYPE_FLOAT,
        DBF_DATA_TYPE_DATE,
        DBF_DATA_TYPE_TIME,     /* non standard */
        DBF_DATA_TYPE_DATETIME, /* non standard */
        DBF_DATA_TYPE_MEMO,
        DBF_DATA_TYPE_BOOLEAN,
        DBF_DATA_TYPE_ENUMCOUNT,
        DBF_DATA_TYPE_LOGICAL = DBF_DATA_TYPE_BOOLEAN,
    DBF_DATA_TYPE_UNKNOWN = -1,
    DBF_DATA_TYPE_ANY     = -2
};

typedef unsigned int   dbf_uint;

typedef struct _DBF_FIELD_INFO {
    char               name[DBF_DBASE4_FIELDNAMELENGTH]; // 10 chars + zero terminator
    enum dbf_data_type type;
    int                length;
    dbf_uint           decimals;
} DBF_FIELD_INFO;

EXTERN_C DBF_HANDLE  dbf_alloc(void);
EXTERN_C DBF_HANDLE  dbf_create_attach(void *stream, struct zlib_filefunc_def_s *, const DBF_FIELD_INFO *array, dbf_uint array_count, enum dbf_charconv charconv, void *memo);
EXTERN_C DBF_HANDLE  dbf_attach(void *stream, struct zlib_filefunc_def_s *, BOOL editable, enum dbf_charconv, void *memo, const char *tablename);
EXTERN_C void *dbf_detach(DBF_HANDLE *);
EXTERN_C DBF_HANDLE  dbf_open(const char *file, struct zlib_filefunc_def_s *, BOOL editable, enum dbf_charconv, const char *tablename);
EXTERN_C DBF_HANDLE  dbf_create(const char *file, const DBF_FIELD_INFO *array, dbf_uint array_count, enum dbf_charconv charconv, const char *tablename);

EXTERN_C void dbf_getfileapi(DBF_HANDLE, struct zlib_filefunc_def_s *);
EXTERN_C void *dbf_getmemofile(DBF_HANDLE);
EXTERN_C BOOL dbf_iseditable(DBF_HANDLE);
EXTERN_C BOOL dbf_ismodified(DBF_HANDLE);

//EXTERN_C BOOL        dbf_memo_attach   (DBF_HANDLE, void* stream);
EXTERN_C BOOL dbf_memo_create(DBF_HANDLE, void *stream);
EXTERN_C BOOL dbf_memo_open(DBF_HANDLE, void *stream);

EXTERN_C void        dbf_close(DBF_HANDLE *);

EXTERN_C void     dbf_close_memo(DBF_HANDLE);
EXTERN_C void     dbf_write_header(DBF_HANDLE);
EXTERN_C void     dbf_write_header_memo(DBF_HANDLE);
EXTERN_C dbf_uint dbf_getposition(DBF_HANDLE);
EXTERN_C dbf_uint dbf_getrecordcount(DBF_HANDLE);
EXTERN_C dbf_uint dbf_getfieldcount(DBF_HANDLE);

typedef struct _DBF_INFO {
    int      version;
    int      flags;
    dbf_uint fieldcount;
    dbf_uint recordcount;
    time_t   lastupdate;
    BOOL     memo;
    BOOL     editable;
    BOOL     modified;
    char     tablename[DBF_DBASE4_FIELDNAMELENGTH + 1];
    char     format[40];
} DBF_INFO;

EXTERN_C void dbf_getinfo(DBF_HANDLE, DBF_INFO *);
EXTERN_C BOOL        dbf_isvaliddate(const char *buf, enum dbf_data_type);
EXTERN_C BOOL        dbf_parsedate(const char *, struct tm *, int *ms, enum dbf_data_type);

EXTERN_C BOOL            dbf_setposition(DBF_HANDLE, dbf_uint index);
EXTERN_C BOOL            dbf_putrecord(DBF_HANDLE, dbf_uint index);
EXTERN_C BOOL            dbf_addrecord(DBF_HANDLE);
EXTERN_C BOOL            dbf_insertrecord(DBF_HANDLE, dbf_uint index);
EXTERN_C BOOL            dbf_isrecorddeleted(DBF_HANDLE);
EXTERN_C BOOL            dbf_deleterecord(DBF_HANDLE, BOOL do_delete);
EXTERN_C int             dbf_getfield(DBF_HANDLE, const DBF_FIELD *, char *buf, int buf_len, enum dbf_data_type);
EXTERN_C BOOL            dbf_putfield(DBF_HANDLE, const DBF_FIELD *, const char *buf);
EXTERN_C int             dbf_findfield(DBF_HANDLE, const char *fieldname);
EXTERN_C BOOL            dbf_isnull(DBF_HANDLE, const DBF_FIELD *);
EXTERN_C const DBF_FIELD *dbf_getfieldptr(DBF_HANDLE, dbf_uint field);
EXTERN_C const DBF_FIELD *dbf_getfieldptr_name(DBF_HANDLE, const char *field);
EXTERN_C BOOL            dbf_move_prepare(DBF_HANDLE);
EXTERN_C BOOL            dbf_getfield_numeric(DBF_HANDLE, const DBF_FIELD *, int *);
EXTERN_C BOOL            dbf_getfield_float(DBF_HANDLE, const DBF_FIELD *, double *);
EXTERN_C BOOL            dbf_getfield_bool(DBF_HANDLE, const DBF_FIELD *, BOOL *);
EXTERN_C BOOL            dbf_putfield_bool(DBF_HANDLE, const DBF_FIELD *, BOOL);
EXTERN_C BOOL            dbf_putfield_float(DBF_HANDLE, const DBF_FIELD *, double);

EXTERN_C BOOL dbf_getfield_time(DBF_HANDLE, const DBF_FIELD *, time_t *, int *ms);
EXTERN_C BOOL dbf_putfield_time(DBF_HANDLE, const DBF_FIELD *, time_t, int ms, enum dbf_data_type);

EXTERN_C BOOL dbf_getfield_tm(DBF_HANDLE, const DBF_FIELD *, struct tm *, int *ms);
EXTERN_C BOOL dbf_putfield_tm(DBF_HANDLE, const DBF_FIELD *, const struct tm *, int ms, enum dbf_data_type);

EXTERN_C BOOL dbf_putfield_numeric(DBF_HANDLE, const DBF_FIELD *, int);

EXTERN_C BOOL dbf_getfield_info(DBF_HANDLE, dbf_uint index, DBF_FIELD_INFO *);
EXTERN_C enum dbf_data_type dbf_getfield_type(DBF_HANDLE, const DBF_FIELD *);

EXTERN_C BOOL        dbf_copy(DBF_HANDLE handle,
                              void       *stream,
                              void       *stream_memo,
                              struct zlib_filefunc_def_s *,
                              BOOL       include_records,
                              BOOL       include_deleted_records);

EXTERN_C int        dbf_getlasterror(DBF_HANDLE);
EXTERN_C const char *dbf_getlasterror_str(DBF_HANDLE);
EXTERN_C void        dbf_getmemofilename(const char *file_dbf, char *buf, int buf_len);
EXTERN_C const char *dbf_gettypetext(enum dbf_data_type);
EXTERN_C enum        dbf_data_type dbf_gettype_int2ext(char type);
EXTERN_C char        dbf_gettype_ext2int(enum dbf_data_type);
#endif /* __DBF_H__ */
