//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
extern "C" {
#include "dbf.h"
#include "bool.h"
}


//-------------------------//
// Local variables         //
//-------------------------//
static INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(DBF_MAJOR_VERSION)
    DEFINE_ECONSTANT(DBF_MINOR_VERSION)
    DEFINE_ECONSTANT(DBF_SVN_VERSION)
    DEFINE_ESCONSTANT(DBF_WEBSITE)
    DEFINE_ESCONSTANT(DBF_FORMAT_NAME)
    DEFINE_ESCONSTANT(DBF_LIB_NAME)
    DEFINE_ESCONSTANT(DBF_AUTHOR)

    DEFINE_ESCONSTANT(DBF_FILEEXT)
    DEFINE_ESCONSTANT(DBF_FILEEXT_MEMO)

    DEFINE_ECONSTANT(DBF_DBASE3_FIELDNAMELENGTH)
    DEFINE_ECONSTANT(DBF_DBASE4_FIELDNAMELENGTH)

    DEFINE_ECONSTANT(ENUM_dbf_charconv_oem_host)
    DEFINE_ECONSTANT(ENUM_dbf_charconv_off)
    DEFINE_ECONSTANT(ENUM_dbf_charconv_enumcount)
    DEFINE_ECONSTANT(ENUM_dbf_charconv_compatible)

    DEFINE_ECONSTANT(NAME_MAX)

    DEFINE_ECONSTANT(DBF_LEN_DATE)
    DEFINE_ECONSTANT(DBF_LEN_TIME)
    DEFINE_ECONSTANT(DBF_LEN_DATETIME)
    DEFINE_ECONSTANT(DBF_LEN_BOOLEAN)
    DEFINE_ECONSTANT(DBF_LEN_YEAR)

    DEFINE_ECONSTANT(DBASE_OUT_OF_MEM)
    DEFINE_ECONSTANT(DBASE_NO_FILE)
    DEFINE_ECONSTANT(DBASE_BAD_FORMAT)
    DEFINE_ECONSTANT(DBASE_INVALID_RECORD)
    DEFINE_ECONSTANT(DBASE_EOF)
    DEFINE_ECONSTANT(DBASE_BOF)
    DEFINE_ECONSTANT(DBASE_NOT_OPEN)
    DEFINE_ECONSTANT(DBASE_INVALID_FIELDNO)
    DEFINE_ECONSTANT(DBASE_INVALID_DATA)
    DEFINE_ECONSTANT(DBASE_SUCCESS)
    DEFINE_ECONSTANT(DBASE_NOT_FOUND)
    DEFINE_ECONSTANT(DBASE_WRITE_ERROR)
    DEFINE_ECONSTANT(DBASE_READ_ERROR)
    DEFINE_ECONSTANT(DBASE_INVALID_BLOCK_NO)
    DEFINE_ECONSTANT(DBASE_NO_MEMO_DATA)

    DEFINE_ECONSTANT(DBF_DATA_TYPE_CHAR)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_INTEGER)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_FLOAT)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_DATE)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_TIME)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_DATETIME)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_MEMO)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_BOOLEAN)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_ENUMCOUNT)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_LOGICAL)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_UNKNOWN)
    DEFINE_ECONSTANT(DBF_DATA_TYPE_ANY)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//

/*CONCEPT_FUNCTION_IMPL(DBFCreate,0)
    RETURN_NUMBER((SYS_INT)dbf_alloc());
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DBFOpen, 1, 4)
    T_STRING(0)

    BOOL editable = FALSE;
    dbf_charconv charconv    = ENUM_dbf_charconv_oem_host;
    char         *table_name = PARAM(0);
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        editable = (BOOL)PARAM_INT(1);

        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(2)
            charconv = (dbf_charconv)PARAM_INT(2);
        }

        if (PARAMETERS_COUNT > 3) {
            T_STRING(3)
            table_name = (char *)PARAM(3);
        }
    }

    DBF_HANDLE handle = dbf_open(PARAM(0), NULL, editable, charconv, table_name);
    RETURN_NUMBER((SYS_INT)handle);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFClose, 1)
    T_HANDLE(0)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    dbf_close(&handle);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFMove, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    BOOL res = dbf_setposition(handle, PARAM_INT(1));

    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFPutRecord, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    BOOL res = dbf_putrecord(handle, PARAM_INT(1));

    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFInsertRecord, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    BOOL res = dbf_putrecord(handle, PARAM_INT(1));

    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFAddRecord, 1)
    T_HANDLE(0)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_addrecord(handle);
    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFIsDeleted, 1)
    T_HANDLE(0)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_isrecorddeleted(handle);
    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFDelete, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_deleterecord(handle, PARAM_INT(1));
    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFFindField, 2)
    T_HANDLE(0)
    T_STRING(1)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    int res = dbf_findfield(handle, PARAM(1));
    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFLastError, 1)
    T_HANDLE(0)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    int res = dbf_getlasterror(handle);
    RETURN_NUMBER((INTEGER)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFLastErrorString, 1)
    T_HANDLE(0)
    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    const char *s = dbf_getlasterror_str(handle);
    RETURN_STRING(s);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFType, 1)
    T_NUMBER(0)

    const char *s = dbf_gettypetext((enum dbf_data_type)PARAM_INT(0));
    RETURN_STRING(s);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFInfo, 1)
    T_HANDLE(0)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    DBF_INFO dinfo;
    dbf_getinfo(handle, &dinfo);

    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "version", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.version);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.flags);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "fieldcount", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.fieldcount);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "recordcount", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.recordcount);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "lastupdate", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.lastupdate);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.flags);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "memo", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.memo);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "editable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.editable);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "modified", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.modified);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tablename", (INTEGER)VARIABLE_STRING, (char *)dinfo.tablename, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "format", (INTEGER)VARIABLE_STRING, (char *)dinfo.format, (NUMBER)0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFFieldInfo, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    DBF_FIELD_INFO dinfo;
    CREATE_ARRAY(RESULT);

    if (dbf_getfield_info(handle, PARAM_INT(1), &dinfo)) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "name", (INTEGER)VARIABLE_STRING, (char *)dinfo.name, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.type);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "length", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.length);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "decimals", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)dinfo.decimals);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFValue, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    static char temp[0xFFFF];

    int len = dbf_getfield(handle, dbf_getfieldptr(handle, PARAM_INT(1)), temp, sizeof(temp), DBF_DATA_TYPE_ANY);
    if (len > 0) {
        RETURN_BUFFER(temp, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFGet, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    static char temp[0xFFFF];

    int len = dbf_getfield(handle, dbf_getfieldptr(handle, PARAM_INT(1)), temp, sizeof(temp), DBF_DATA_TYPE_ANY);
    if (len > 0) {
        RETURN_BUFFER(temp, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFGetName, 2)
    T_HANDLE(0)
    T_STRING(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    static char temp[0xFFFF];

    int len = dbf_getfield(handle, dbf_getfieldptr_name(handle, PARAM(1)), temp, sizeof(temp), DBF_DATA_TYPE_ANY);
    if (len > 0) {
        RETURN_BUFFER(temp, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFUpdate, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_STRING(2)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);
    BOOL res = dbf_putfield(handle, dbf_getfieldptr(handle, PARAM_INT(1)), PARAM(2));

    RETURN_NUMBER((BOOL)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFUpdateName, 3)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_putfield(handle, dbf_getfieldptr_name(handle, PARAM(1)), PARAM(2));

    RETURN_NUMBER((BOOL)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFNull, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_isnull(handle, dbf_getfieldptr(handle, PARAM_INT(1)));

    RETURN_NUMBER((BOOL)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFNullName, 2)
    T_HANDLE(0)
    T_STRING(1)

    DBF_HANDLE handle = (DBF_HANDLE)PARAM_INT(0);

    BOOL res = dbf_isnull(handle, dbf_getfieldptr_name(handle, PARAM(1)));

    RETURN_NUMBER((BOOL)res);
END_IMPL
//-----------------------------------------------------//
