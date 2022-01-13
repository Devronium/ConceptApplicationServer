#define NO_DEPENDECIES
#define __STDC_LIMIT_MACROS
#ifdef _WIN32
 #define _WIN32_WINNT    0x0500
#endif
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include <string.h>
#include "AnsiString.h"
#include "library.h"
#include <stdlib.h>
#ifdef NO_DEPENDECIES
extern "C" {
 #include "libzip.c"
}
#else
 #include <zip.h>
#endif

INVOKE_CALL InvokePtr = 0;
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(ZIP_CREATE)
    DEFINE_ECONSTANT(ZIP_EXCL)
    DEFINE_ECONSTANT(ZIP_CHECKCONS)

    DEFINE_ECONSTANT(ZIP_FL_NOCASE)
    DEFINE_ECONSTANT(ZIP_FL_NODIR)
    DEFINE_ECONSTANT(ZIP_FL_COMPRESSED)
    DEFINE_ECONSTANT(ZIP_FL_UNCHANGED)

    DEFINE_ECONSTANT(ZIP_ER_OK)
    DEFINE_ECONSTANT(ZIP_ER_MULTIDISK)
    DEFINE_ECONSTANT(ZIP_ER_RENAME)
    DEFINE_ECONSTANT(ZIP_ER_CLOSE)
    DEFINE_ECONSTANT(ZIP_ER_SEEK)
    DEFINE_ECONSTANT(ZIP_ER_READ)
    DEFINE_ECONSTANT(ZIP_ER_WRITE)
    DEFINE_ECONSTANT(ZIP_ER_CRC)
    DEFINE_ECONSTANT(ZIP_ER_ZIPCLOSED)
    DEFINE_ECONSTANT(ZIP_ER_NOENT)
    DEFINE_ECONSTANT(ZIP_ER_EXISTS)
    DEFINE_ECONSTANT(ZIP_ER_OPEN)
    DEFINE_ECONSTANT(ZIP_ER_TMPOPEN)
    DEFINE_ECONSTANT(ZIP_ER_ZLIB)
    DEFINE_ECONSTANT(ZIP_ER_MEMORY)
    DEFINE_ECONSTANT(ZIP_ER_CHANGED)
    DEFINE_ECONSTANT(ZIP_ER_COMPNOTSUPP)
    DEFINE_ECONSTANT(ZIP_ER_EOF)
    DEFINE_ECONSTANT(ZIP_ER_INVAL)
    DEFINE_ECONSTANT(ZIP_ER_NOZIP)
    DEFINE_ECONSTANT(ZIP_ER_INTERNAL)
    DEFINE_ECONSTANT(ZIP_ER_INCONS)
    DEFINE_ECONSTANT(ZIP_ER_REMOVE)
    DEFINE_ECONSTANT(ZIP_ER_DELETED)

    DEFINE_ECONSTANT(ZIP_ET_NONE)
    DEFINE_ECONSTANT(ZIP_ET_SYS)
    DEFINE_ECONSTANT(ZIP_ET_ZLIB)

    DEFINE_ECONSTANT(ZIP_CM_DEFAULT)
    DEFINE_ECONSTANT(ZIP_CM_STORE)
    DEFINE_ECONSTANT(ZIP_CM_SHRINK)
    DEFINE_ECONSTANT(ZIP_CM_REDUCE_1)
    DEFINE_ECONSTANT(ZIP_CM_REDUCE_2)
    DEFINE_ECONSTANT(ZIP_CM_REDUCE_3)
    DEFINE_ECONSTANT(ZIP_CM_REDUCE_4)
    DEFINE_ECONSTANT(ZIP_CM_IMPLODE)
    DEFINE_ECONSTANT(ZIP_CM_DEFLATE)
    DEFINE_ECONSTANT(ZIP_CM_DEFLATE64)
    DEFINE_ECONSTANT(ZIP_CM_PKWARE_IMPLODE)
    DEFINE_ECONSTANT(ZIP_CM_BZIP2)

    DEFINE_ECONSTANT(ZIP_EM_NONE)
    DEFINE_ECONSTANT(ZIP_EM_TRAD_PKWARE)

    DEFINE_ECONSTANT(ZIP_EM_UNKNOWN)

    DEFINE_ECONSTANT(ZIP_SOURCE_OPEN)
    DEFINE_ECONSTANT(ZIP_SOURCE_READ)
    DEFINE_ECONSTANT(ZIP_SOURCE_CLOSE)
    DEFINE_ECONSTANT(ZIP_SOURCE_STAT)
    DEFINE_ECONSTANT(ZIP_SOURCE_ERROR)
    DEFINE_ECONSTANT(ZIP_SOURCE_FREE)

    DEFINE_SCONSTANT("zip_name_locate", "_zip_name_locate")
    DEFINE_SCONSTANT("zip_get_name", "_zip_get_name")
    DEFINE_SCONSTANT("zip_replace", "_zip_replace")
    DEFINE_SCONSTANT("zip_zip_unchange", "_zip_unchange")


    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_open, 3)
    T_STRING(_zip_open, 0)
    T_NUMBER(_zip_open, 1)

    int errorp = 0;

    char *safe_path = SafePath(PARAM(0), Invoke, PARAMETERS->HANDLER);
    RETURN_NUMBER((SYS_INT)zip_open(safe_path, PARAM_INT(1), &errorp))
    free(safe_path);

    SET_NUMBER(2, errorp);

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__zip_name_locate, 3)
    T_NUMBER(__zip_name_locate, 0)
    T_STRING(__zip_name_locate, 1)
    T_NUMBER(__zip_name_locate, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_name_locate: Invalid parameter");

    RETURN_NUMBER(zip_name_locate((zip *)PARAM_INT(0), PARAM(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_fopen, 3)
    T_NUMBER(_zip_fopen, 0)
    T_STRING(_zip_fopen, 1)
    T_NUMBER(_zip_fopen, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_fopen: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_fopen((zip *)PARAM_INT(0), PARAM(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_fopen_index, 3)
    T_NUMBER(_zip_fopen_index, 0)
    T_NUMBER(_zip_fopen_index, 1)
    T_NUMBER(_zip_fopen_index, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_fopen_index: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_fopen_index((zip *)PARAM_INT(0), PARAM_INT(1), PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_fread, 3)
    T_NUMBER(_zip_fread, 0)
//T_NUMBER(1)
    T_NUMBER(_zip_fread, 2)

    if ((!(PARAM_INT(0))) || (PARAM_INT(2) <= 0))
        FAIL_ERROR("_zip_fread: Invalid parameter");

    int nbytes = PARAM_INT(2);

    char *buf = new char[nbytes];

    int len = zip_fread((zip_file *)PARAM_INT(0), buf, nbytes);

    RETURN_NUMBER(len)

    if (len > 0) {
        SET_BUFFER(1, buf, len);
    } else {
        SET_STRING(1, "");
    }

    delete[] buf;
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_fclose, 1)
    T_NUMBER(_zip_fclose, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_fclose: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_fclose((zip_file *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_close, 1)
    T_NUMBER(_zip_close, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_close: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_close((zip *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_stat, 4)
    T_NUMBER(_zip_stat, 0)
    T_STRING(_zip_stat, 1)
    T_NUMBER(_zip_stat, 2)
//T_ARRAY(3)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_stat Invalid parameter");

    struct zip_stat sb;

    RETURN_NUMBER((SYS_INT)zip_stat((zip *)PARAM_INT(0), PARAM(1), PARAM_INT(2), &sb))

    Invoke(INVOKE_CREATE_ARRAY, PARAMETER(3));

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "name", (INTEGER)VARIABLE_STRING, (char *)sb.name, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "index", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.index);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "crc", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.crc);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.size);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "mtime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.mtime);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "comp_size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.comp_size);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "comp_method", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.comp_method);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(3), "encryption_method", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sb.encryption_method);

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_get_archive_comment, 2)
    T_NUMBER(_zip_get_archive_comment, 0)
    T_NUMBER(_zip_get_archive_comment, 1)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_get_archive_comment: Invalid parameter");

    int lenp        = 0;
    const char *str = zip_get_archive_comment((zip *)PARAM_INT(0), &lenp, PARAM_INT(1));
    if (str) {
        RETURN_BUFFER(str, lenp);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_get_file_comment, 3)
    T_NUMBER(_zip_get_file_comment, 0)
    T_NUMBER(_zip_get_file_comment, 1)
    T_NUMBER(_zip_get_file_comment, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_get_file_comment: Invalid parameter");

    int lenp        = 0;
    const char *str = zip_get_file_comment((zip *)PARAM_INT(0), PARAM_INT(1), &lenp, PARAM_INT(2));
    if (str) {
        RETURN_BUFFER(str, lenp);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__zip_get_name, 3)
    T_NUMBER(__zip_get_name, 0)
    T_NUMBER(__zip_get_name, 1)
    T_NUMBER(__zip_get_name, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("__zip_get_name: Invalid parameter");

    RETURN_STRING((char *)zip_get_name((zip *)PARAM_INT(0), PARAM_INT(1), PARAM_INT(2)));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_get_num_files, 1)
    T_NUMBER(_zip_get_num_files, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_get_num_files: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_get_num_files((zip *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_add, 3)
    T_NUMBER(_zip_add, 0)
    T_STRING(_zip_add, 1)
    T_NUMBER(_zip_add, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_add: Invalid parameter (1)");

    if (!(PARAM_INT(2)))
        FAIL_ERROR("_zip_add: Invalid parameter (3)");

    RETURN_NUMBER((SYS_INT)zip_add((zip *)PARAM_INT(0), PARAM(1), (zip_source *)PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__zip_replace, 3)
    T_NUMBER(__zip_replace, 0)
    T_NUMBER(__zip_replace, 1)
    T_NUMBER(__zip_replace, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("__zip_replace: Invalid parameter (1)");

    if (!(PARAM_INT(2)))
        FAIL_ERROR("__zip_replace: Invalid parameter (3)");

    RETURN_NUMBER((SYS_INT)zip_replace((zip *)PARAM_INT(0), PARAM_INT(1), (zip_source *)PARAM_INT(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_set_file_comment, 3)
    T_NUMBER(_zip_set_file_comment, 0)
    T_NUMBER(_zip_set_file_comment, 1)
    T_STRING(_zip_set_file_comment, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_set_file_comment: Invalid parameter (1)");

    RETURN_NUMBER((SYS_INT)zip_set_file_comment((zip *)PARAM_INT(0), PARAM_INT(1), PARAM(2), PARAM_LEN(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_source_buffer, 2)
    T_NUMBER(_zip_source_buffer, 0)
    T_STRING(_zip_source_buffer, 1)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_source_buffer: Invalid parameter (1)");
    SYS_INT res = 0;
    if (PARAM_LEN(1)) {
        char *buf = (char *)malloc(PARAM_LEN(1));
        if (buf) {
            memcpy(buf, PARAM(1), PARAM_LEN(1));
            res = (SYS_INT)zip_source_buffer((zip *)PARAM_INT(0), buf, PARAM_LEN(1), 1);
        } else
            res = (SYS_INT)zip_source_buffer((zip *)PARAM_INT(0), PARAM(1), PARAM_LEN(1), 0);
    } else
        res = (SYS_INT)zip_source_buffer((zip *)PARAM_INT(0), "", 0, 0);
    RETURN_NUMBER(res)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_source_file, 4)
    T_NUMBER(_zip_source_file, 0)
    T_STRING(_zip_source_file, 1)
    T_NUMBER(_zip_source_file, 2)
    T_NUMBER(_zip_source_file, 3)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_source_file: Invalid parameter (1)");

    RETURN_NUMBER((SYS_INT)zip_source_file((zip *)PARAM_INT(0), PARAM(1), PARAM_INT(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_source_filep, 4)
    T_NUMBER(_zip_source_filep, 0)
    T_NUMBER(_zip_source_filep, 1)
    T_NUMBER(_zip_source_filep, 2)
    T_NUMBER(_zip_source_filep, 3)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_source_filep: Invalid parameter (1)");

    if (!(PARAM_INT(1)))
        FAIL_ERROR("_zip_source_filep: Invalid parameter (2)");

    RETURN_NUMBER((SYS_INT)zip_source_filep((zip *)PARAM_INT(0), (FILE *)PARAM_INT(1), PARAM_INT(2), PARAM_INT(3)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_source_zip, 6)
    T_NUMBER(_zip_source_zip, 0)
    T_NUMBER(_zip_source_zip, 1)
    T_NUMBER(_zip_source_zip, 2)
    T_NUMBER(_zip_source_zip, 3)
    T_NUMBER(_zip_source_zip, 4)
    T_NUMBER(_zip_source_zip, 5)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_source_zip: Invalid parameter (1)");

    if (!(PARAM_INT(1)))
        FAIL_ERROR("_zip_source_zip: Invalid parameter (2)");

    RETURN_NUMBER((SYS_INT)zip_source_zip((zip *)PARAM_INT(0), (zip *)PARAM_INT(1), PARAM_INT(2), PARAM_INT(3), PARAM_INT(4), PARAM_INT(5)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_source_free, 1)
    T_NUMBER(_zip_source_free, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_source_free: Invalid parameter");

    zip_source_free((zip_source *)PARAM_INT(0));
    RETURN_NUMBER((SYS_INT)0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_rename, 3)
    T_NUMBER(_zip_rename, 0)
    T_NUMBER(_zip_rename, 1)
    T_STRING(_zip_rename, 2)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_rename: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_rename((zip *)PARAM_INT(0), PARAM_INT(1), PARAM(2)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_delete, 2)
    T_NUMBER(_zip_delete, 0)
    T_NUMBER(_zip_delete, 1)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_delete: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_delete((zip *)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(__zip_unchange, 2)
    T_NUMBER(__zip_unchange, 0)
    T_NUMBER(__zip_unchange, 1)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("__zip_unchange: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_unchange((zip *)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_unchange_all, 1)
    T_NUMBER(_zip_unchange_all, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_unchange_all: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_unchange_all((zip *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_unchange_archive, 1)
    T_NUMBER(_zip_unchange_archive, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_unchange_archive: Invalid parameter");

    RETURN_NUMBER((SYS_INT)zip_unchange_archive((zip *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_set_archive_comment, 2)
    T_NUMBER(_zip_set_archive_comment, 0)
    T_STRING(_zip_set_archive_comment, 1)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_set_archive_comment: Invalid parameter (1)");

    RETURN_NUMBER((SYS_INT)zip_set_archive_comment((zip *)PARAM_INT(0), PARAM(1), PARAM_LEN(1)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_strerror, 1)
    T_NUMBER(_zip_strerror, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_strerror: Invalid parameter");

    RETURN_STRING((char *)zip_strerror((zip *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_zip_file_strerror, 1)
    T_NUMBER(_zip_file_strerror, 0)

    if (!(PARAM_INT(0)))
        FAIL_ERROR("_zip_file_strerror: Invalid parameter");

    RETURN_STRING((char *)zip_file_strerror((zip_file *)PARAM_INT(0)))
END_IMPL
//---------------------------------------------------------------------------

