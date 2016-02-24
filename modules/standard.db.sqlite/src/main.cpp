//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "sqlite3.h"
#include "SQLDB.h"
#include "SQLSTMT.h"
#include "AnsiString.h"
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
void WRAPFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    INVOKE_CALL Invoke = InvokePtr;

    void **PARAMETERS = new void * [argc + 1];

    PARAMETERS[argc] = 0;

    for (int i = 0; i < argc; i++) {
        sqlite3_value *param = argv[i];

        CREATE_VARIABLE(PARAMETERS[i]);

        switch (sqlite3_value_type(param)) {
            case     SQLITE_TEXT:
            case     SQLITE_BLOB:
                Invoke(INVOKE_SET_VARIABLE, PARAMETERS[i], (INTEGER)VARIABLE_STRING, (char *)sqlite3_value_text(param), (double)0);
                break;

            default:
                Invoke(INVOKE_SET_VARIABLE, PARAMETERS[i], (INTEGER)VARIABLE_NUMBER, (char *)"", sqlite3_value_double(param));
                break;
        }
    }

    void *EXCEPTION = 0;
    void *RES       = 0;

    void *data = sqlite3_user_data(context);

    Invoke(INVOKE_CALL_DELEGATE, data, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);

    for (int i = 0; i < argc; i++) {
        FREE_VARIABLE(PARAMETERS[i]);
    }

    if (RES) {
        INTEGER TYPE        = 0;
        char    *szvariable = 0;
        NUMBER  nValue      = 0;
        Invoke(INVOKE_GET_VARIABLE, RES, &TYPE, &szvariable, &nValue);
        if (TYPE == VARIABLE_STRING) {
            sqlite3_result_text(context, szvariable, (int)nValue, SQLITE_TRANSIENT);
        } else
        if (TYPE == VARIABLE_NUMBER) {
            sqlite3_result_double(context, nValue);
        } else
            Invoke(INVOKE_FREE_VARIABLE, RES);
    }
}

//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(SQLITE_ABORT)

    /*DEFINE_ECONSTANT(SQLITE_ACCESS_EXISTS)
       DEFINE_ECONSTANT(SQLITE_ACCESS_READ)
       DEFINE_ECONSTANT(SQLITE_ACCESS_READWRITE)*/
    DEFINE_ECONSTANT(SQLITE_ALTER_TABLE)
    DEFINE_ECONSTANT(SQLITE_ANALYZE)
    DEFINE_ECONSTANT(SQLITE_ANY)
    DEFINE_ECONSTANT(SQLITE_ATTACH)
    DEFINE_ECONSTANT(SQLITE_AUTH)
    DEFINE_ECONSTANT(SQLITE_BLOB)
    DEFINE_ECONSTANT(SQLITE_BUSY)
    DEFINE_ECONSTANT(SQLITE_CANTOPEN)
    DEFINE_ECONSTANT(SQLITE_CONSTRAINT)
    DEFINE_ECONSTANT(SQLITE_COPY)
    DEFINE_ECONSTANT(SQLITE_CORRUPT)
    DEFINE_ECONSTANT(SQLITE_CREATE_INDEX)
    DEFINE_ECONSTANT(SQLITE_CREATE_TABLE)
    DEFINE_ECONSTANT(SQLITE_CREATE_TEMP_INDEX)
    DEFINE_ECONSTANT(SQLITE_CREATE_TEMP_TABLE)
    DEFINE_ECONSTANT(SQLITE_CREATE_TEMP_TRIGGER)
    DEFINE_ECONSTANT(SQLITE_CREATE_TEMP_VIEW)
    DEFINE_ECONSTANT(SQLITE_CREATE_TRIGGER)
    DEFINE_ECONSTANT(SQLITE_CREATE_VIEW)
    DEFINE_ECONSTANT(SQLITE_CREATE_VTABLE)
    DEFINE_ECONSTANT(SQLITE_DELETE)
    DEFINE_ECONSTANT(SQLITE_DENY)
    DEFINE_ECONSTANT(SQLITE_DETACH)
    DEFINE_ECONSTANT(SQLITE_DONE)
    DEFINE_ECONSTANT(SQLITE_DROP_INDEX)
    DEFINE_ECONSTANT(SQLITE_DROP_TABLE)
    DEFINE_ECONSTANT(SQLITE_DROP_TEMP_INDEX)
    DEFINE_ECONSTANT(SQLITE_DROP_TEMP_TABLE)
    DEFINE_ECONSTANT(SQLITE_DROP_TEMP_TRIGGER)
    DEFINE_ECONSTANT(SQLITE_DROP_TEMP_VIEW)
    DEFINE_ECONSTANT(SQLITE_DROP_TRIGGER)
    DEFINE_ECONSTANT(SQLITE_DROP_VIEW)
    DEFINE_ECONSTANT(SQLITE_DROP_VTABLE)
    DEFINE_ECONSTANT(SQLITE_EMPTY)
    DEFINE_ECONSTANT(SQLITE_ERROR)
    //DEFINE_ECONSTANT(SQLITE_FCNTL_LOCKSTATE)
    DEFINE_ECONSTANT(SQLITE_FLOAT)
    DEFINE_ECONSTANT(SQLITE_FORMAT)
    DEFINE_ECONSTANT(SQLITE_FULL)
    DEFINE_ECONSTANT(SQLITE_FUNCTION)
    DEFINE_ECONSTANT(SQLITE_IGNORE)
    DEFINE_ECONSTANT(SQLITE_INSERT)
    DEFINE_ECONSTANT(SQLITE_INTEGER)
    DEFINE_ECONSTANT(SQLITE_INTERNAL)
    DEFINE_ECONSTANT(SQLITE_INTERRUPT)

    /*DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC16K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC1K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC2K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC32K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC4K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC512)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC64K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_ATOMIC8K)
       DEFINE_ECONSTANT(SQLITE_IOCAP_SAFE_APPEND)
       DEFINE_ECONSTANT(SQLITE_IOCAP_SEQUENTIAL)*/
    DEFINE_ECONSTANT(SQLITE_IOERR)
    DEFINE_ECONSTANT(SQLITE_IOERR_BLOCKED)
    DEFINE_ECONSTANT(SQLITE_IOERR_DELETE)
    DEFINE_ECONSTANT(SQLITE_IOERR_DIR_FSYNC)
    DEFINE_ECONSTANT(SQLITE_IOERR_FSTAT)
    DEFINE_ECONSTANT(SQLITE_IOERR_FSYNC)
    //DEFINE_ECONSTANT(SQLITE_IOERR_NOMEM)
    DEFINE_ECONSTANT(SQLITE_IOERR_RDLOCK)
    DEFINE_ECONSTANT(SQLITE_IOERR_READ)
    DEFINE_ECONSTANT(SQLITE_IOERR_SHORT_READ)
    DEFINE_ECONSTANT(SQLITE_IOERR_TRUNCATE)
    DEFINE_ECONSTANT(SQLITE_IOERR_UNLOCK)
    DEFINE_ECONSTANT(SQLITE_IOERR_WRITE)
    DEFINE_ECONSTANT(SQLITE_LOCKED)

    /*DEFINE_ECONSTANT(SQLITE_LOCK_EXCLUSIVE)
       DEFINE_ECONSTANT(SQLITE_LOCK_NONE)
       DEFINE_ECONSTANT(SQLITE_LOCK_PENDING)
       DEFINE_ECONSTANT(SQLITE_LOCK_RESERVED)
       DEFINE_ECONSTANT(SQLITE_LOCK_SHARED)*/
    DEFINE_ECONSTANT(SQLITE_MISMATCH)
    DEFINE_ECONSTANT(SQLITE_MISUSE)

    /*DEFINE_ECONSTANT(SQLITE_MUTEX_FAST)
       DEFINE_ECONSTANT(SQLITE_MUTEX_RECURSIVE)
       DEFINE_ECONSTANT(SQLITE_MUTEX_STATIC_LRU)
       DEFINE_ECONSTANT(SQLITE_MUTEX_STATIC_MASTER)
       DEFINE_ECONSTANT(SQLITE_MUTEX_STATIC_MEM)
       DEFINE_ECONSTANT(SQLITE_MUTEX_STATIC_MEM2)
       DEFINE_ECONSTANT(SQLITE_MUTEX_STATIC_PRNG)*/
    DEFINE_ECONSTANT(SQLITE_NOLFS)
    //DEFINE_ECONSTANT(SQLITE_NOMEM)
    DEFINE_ECONSTANT(SQLITE_NOTADB)
    DEFINE_ECONSTANT(SQLITE_NOTFOUND)
    DEFINE_ECONSTANT(SQLITE_NULL)
    DEFINE_ECONSTANT(SQLITE_OK)

    /*DEFINE_ECONSTANT(SQLITE_OPEN_CREATE)
       DEFINE_ECONSTANT(SQLITE_OPEN_DELETEONCLOSE)
       DEFINE_ECONSTANT(SQLITE_OPEN_EXCLUSIVE)
       DEFINE_ECONSTANT(SQLITE_OPEN_MAIN_DB)
       DEFINE_ECONSTANT(SQLITE_OPEN_MAIN_JOURNAL)
       DEFINE_ECONSTANT(SQLITE_OPEN_MASTER_JOURNAL)
       DEFINE_ECONSTANT(SQLITE_OPEN_READONLY)
       DEFINE_ECONSTANT(SQLITE_OPEN_READWRITE)
       DEFINE_ECONSTANT(SQLITE_OPEN_SUBJOURNAL)
       DEFINE_ECONSTANT(SQLITE_OPEN_TEMP_DB)
       DEFINE_ECONSTANT(SQLITE_OPEN_TEMP_JOURNAL)
       DEFINE_ECONSTANT(SQLITE_OPEN_TRANSIENT_DB)*/
    DEFINE_ECONSTANT(SQLITE_PERM)
    DEFINE_ECONSTANT(SQLITE_PRAGMA)
    DEFINE_ECONSTANT(SQLITE_PROTOCOL)
    DEFINE_ECONSTANT(SQLITE_RANGE)
    DEFINE_ECONSTANT(SQLITE_READ)
    DEFINE_ECONSTANT(SQLITE_READONLY)
    DEFINE_ECONSTANT(SQLITE_REINDEX)
    DEFINE_ECONSTANT(SQLITE_ROW)
    DEFINE_ECONSTANT(SQLITE_SCHEMA)
    DEFINE_ECONSTANT(SQLITE_SELECT)
    //DEFINE_ECONSTANT(SQLITE_STATIC)

    /*DEFINE_ECONSTANT(SQLITE_SYNC_DATAONLY)
       DEFINE_ECONSTANT(SQLITE_SYNC_FULL)
       DEFINE_ECONSTANT(SQLITE_SYNC_NORMAL)*/
    DEFINE_ECONSTANT(SQLITE_TOOBIG)
    DEFINE_ECONSTANT(SQLITE_TRANSACTION)
    //DEFINE_ECONSTANT(SQLITE_TRANSIENT)
    DEFINE_ECONSTANT(SQLITE_UPDATE)
    DEFINE_ECONSTANT(SQLITE_UTF16)
    DEFINE_ECONSTANT(SQLITE_UTF16BE)
    DEFINE_ECONSTANT(SQLITE_UTF16LE)
    DEFINE_ECONSTANT(SQLITE_UTF16_ALIGNED)
    DEFINE_ECONSTANT(SQLITE_UTF8)
    AnsiString ver((char *)"\"");
    ver += SQLITE_VERSION;
    ver += (char *)"\"";
    DEFINE_SCONSTANT("SQLITE_VERSION", ver)
    DEFINE_ECONSTANT(SQLITE_VERSION_NUMBER)

    DEFINE_ECONSTANT(SQLITE_INTEGER)
    DEFINE_ECONSTANT(SQLITE_FLOAT)
    DEFINE_ECONSTANT(SQLITE_BLOB)
    DEFINE_ECONSTANT(SQLITE_NULL)
    DEFINE_ECONSTANT(SQLITE3_TEXT)
    sqlite3_initialize();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        sqlite3_shutdown();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBOpen, 1)
    T_STRING(0)

    SQLDB * db = new SQLDB();
    if (db->Open(PARAM(0))) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER((intptr_t)db);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBClose, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if ((!db) || (!db->Close())) {
        RETURN_NUMBER(false);
    } else {
        delete db;
        RETURN_NUMBER(true);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBError, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_STRING("");
    } else {
        char *res = (char *)db->Error();
        RETURN_STRING(res ? res : "");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBErrorCode, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(db->ErrorCode());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBChanges, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(db->Changes());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBLastRowID, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(db->LastRowID());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBNewStmt, 1)
    T_NUMBER(0)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(0);
    } else {
        SQLSTMT *stmt = new SQLSTMT(db);
        RETURN_NUMBER((intptr_t)stmt);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFree, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(false);
    } else {
        delete stmt;
        RETURN_NUMBER(true);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBColumns, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->CountColumns());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBFirst, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);
    if (!stmt) {
        RETURN_NUMBER(0);
    } else {
        int res = stmt->First();
        RETURN_NUMBER((res == SQLITE_ROW) /*|| (res==SQLITE_DONE)*/);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBReset, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(stmt->Reset() != -1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBNext, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(stmt->Next() == SQLITE_ROW);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBData, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(0);
    } else {
        switch (stmt->ColumnType(PARAM_INT(1))) {
            case SQLITE_FLOAT:
            case SQLITE_INTEGER:
                {
                    RETURN_NUMBER(stmt->GetColumn(PARAM_INT(1)));
                }
                break;

            case SQLITE_BLOB:
                {
                    int  len  = 0;
                    char *res = (char *)stmt->GetColumnBlob(PARAM_INT(1), &len);
                    if ((res) && (len > 0)) {
                        RETURN_BUFFER(res, len);
                    } else {
                        RETURN_STRING("");
                    }
                }
                break;

            default:
                {
                    char *res = (char *)stmt->GetColumnText(PARAM_INT(1));
                    RETURN_STRING(res ? res : "");
                }
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBDataRead, 6)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
//T_NUMBER(3)
    T_NUMBER(4)
    T_NUMBER(5)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        char *ptr = 0;
        int  size = stmt->ReadColumnBLOB(PARAM_INT(1), PARAM_INT(2), &ptr, PARAM_INT(4), PARAM_INT(5));
        if ((size > 0) && (ptr)) {
            SET_BUFFER(3, ptr, size);
            delete[] ptr;
        }
        RETURN_NUMBER(size);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBDataWrite, 5)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_STRING(3)
    T_NUMBER(4)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->WriteColumnBLOB(PARAM_INT(1), PARAM_INT(2), PARAM(3), PARAM_LEN(3), PARAM_INT(4)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBQuery, 2)
    T_NUMBER(0)
    T_STRING(1)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->Prepare(PARAM(1), PARAM_LEN(1)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBClearBindings, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->ClearBindings());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBColumnDeclType, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_STRING("");
    } else {
        char *res = (char *)stmt->ColumnDeclType(PARAM_INT(1));
        RETURN_STRING(res ? res : "");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBColumnName, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_STRING("");
    } else {
        char *res = (char *)stmt->ColumnName(PARAM_INT(1));
        RETURN_STRING(res ? res : "");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBColumnType, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->ColumnType(PARAM_INT(1)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBWrap, 2)
    T_NUMBER(0)
    T_DELEGATE(1)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(0);
    } else {
        char *class_name  = 0;
        char *member_name = 0;
        Invoke(INVOKE_GET_DELEGATE_NAMES, PARAMETER(1), &class_name, &member_name);
        int params = Invoke(INVOKE_COUNT_DELEGATE_PARAMS, PARAMETER(1));
        if ((member_name) && (params >= 0)) {
            db->WrapFunction(member_name, WRAPFunc, PARAMETER(1), params);
            LOCK_VARIABLE(PARAMETER(1));
        }
        RETURN_NUMBER(1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBBind, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_STRING(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->Parameter(PARAM_INT(1), PARAM(2), PARAM_LEN(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBNameBind, 3)
    T_NUMBER(0)
    T_STRING(1)
    T_STRING(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->Parameter(PARAM(1), PARAM(2), PARAM_LEN(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBBLOBBind, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->Parameter(PARAM_INT(1), PARAM_INT(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBParamCount, 1)
    T_NUMBER(0)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->ParamCount());
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBBLOBNameBind, 3)
    T_NUMBER(0)
    T_STRING(1)
    T_NUMBER(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->Parameter(PARAM(1), PARAM_INT(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBBLOBNameDataBind, 3)
    T_NUMBER(0)
    T_STRING(1)
    T_STRING(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->BlobParameter(PARAM(1), PARAM(2), PARAM_LEN(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBBLOBDataBind, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_STRING(2)

    SQLSTMT * stmt = (SQLSTMT *)PARAM_INT(0);

    if (!stmt) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(stmt->BlobParameter(PARAM_INT(1), PARAM(2), PARAM_LEN(2)));
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DBTimeout, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    SQLDB * db = (SQLDB *)PARAM_INT(0);

    if (!db) {
        RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(db->SetTimeout(PARAM_INT(1)));
    }
END_IMPL
//-----------------------------------------------------//
