//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include "Connection.h"
#include "Exception.h"
#include "Blob.h"
#include "DateTime.h"
#include "TimeStamp.h"
#include "Properties.h"
#include "SQLException.h"

using namespace NuoDB;
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
AnsiString  LastError;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    // starting up windows sockets
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
    DEFINE_ECONSTANT(SYNTAX_ERROR)
    DEFINE_ECONSTANT(FEATURE_NOT_YET_IMPLEMENTED)
    DEFINE_ECONSTANT(BUG_CHECK)
    DEFINE_ECONSTANT(COMPILE_ERROR)
    DEFINE_ECONSTANT(RUNTIME_ERROR)
    //DEFINE_ECONSTANT(IO_ERROR)
    DEFINE_ECONSTANT(NETWORK_ERROR)
    DEFINE_ECONSTANT(CONVERSION_ERROR)
    DEFINE_ECONSTANT(TRUNCATION_ERROR)
    DEFINE_ECONSTANT(CONNECTION_ERROR)
    DEFINE_ECONSTANT(DDL_ERROR)
    DEFINE_ECONSTANT(APPLICATION_ERROR)
    DEFINE_ECONSTANT(SECURITY_ERROR)
    DEFINE_ECONSTANT(DATABASE_CORRUPTION)
    DEFINE_ECONSTANT(VERSION_ERROR)
    DEFINE_ECONSTANT(LICENSE_ERROR)
    DEFINE_ECONSTANT(INTERNAL_ERROR)
    DEFINE_ECONSTANT(DEBUG_ERROR)
    DEFINE_ECONSTANT(LOST_BLOB)
    DEFINE_ECONSTANT(INCONSISTENT_BLOB)
    DEFINE_ECONSTANT(DELETED_BLOB)
    DEFINE_ECONSTANT(LOG_ERROR)
    DEFINE_ECONSTANT(DATABASE_DAMAGED)
    DEFINE_ECONSTANT(UPDATE_CONFLICT)
    DEFINE_ECONSTANT(NO_SUCH_TABLE)
    DEFINE_ECONSTANT(INDEX_OVERFLOW)
    DEFINE_ECONSTANT(UNIQUE_DUPLICATE)
    DEFINE_ECONSTANT(UNCOMMITTED_UPDATES)
    DEFINE_ECONSTANT(DEADLOCK)
    DEFINE_ECONSTANT(OUT_OF_MEMORY_ERROR)
    DEFINE_ECONSTANT(OUT_OF_RECORD_MEMORY_ERROR)
    DEFINE_ECONSTANT(LOCK_TIMEOUT)
    DEFINE_ECONSTANT(PLATFORM_ERROR)
    DEFINE_ECONSTANT(NO_SCHEMA)
    DEFINE_ECONSTANT(CONFIGURATION_ERROR)
    DEFINE_ECONSTANT(READ_ONLY_ERROR)
    DEFINE_ECONSTANT(THROWN_EXCEPTION)
    DEFINE_ECONSTANT(INVALID_TRANSACTION_ISOLATION)
    DEFINE_ECONSTANT(UNSUPPORTED_TRANSACTION_ISOLATION)
    DEFINE_ECONSTANT(INVALID_UTF8)

    /*DEFINE_ECONSTANT(PLATFORM_EXCEPTION)
       DEFINE_ECONSTANT(SQL_EXCEPTION)
       DEFINE_ECONSTANT(TRANSFORM_EXCEPTION)
       DEFINE_ECONSTANT(XML_EXCEPTION)
       DEFINE_ECONSTANT(DEBUG_EXCEPTION)
       DEFINE_ECONSTANT(TEST_EXCEPTION)*/
    DEFINE_ECONSTANT(TRANSACTION_NONE)
    DEFINE_ECONSTANT(TRANSACTION_READ_UNCOMMITTED)
    DEFINE_ECONSTANT(TRANSACTION_READ_COMMITTED)
    DEFINE_ECONSTANT(TRANSACTION_REPEATABLE_READ)
    DEFINE_ECONSTANT(TRANSACTION_SERIALIZABLE)

    DEFINE_ECONSTANT(CONNECTION_VERSION)
    DEFINE_ECONSTANT(DATABASEMETADATA_VERSION)
    DEFINE_ECONSTANT(STATEMENT_VERSION)
    DEFINE_ECONSTANT(STATEMENTMETADATA_VERSION)
    DEFINE_ECONSTANT(PREPAREDSTATEMENT_VERSION)
    DEFINE_ECONSTANT(RESULTSET_VERSION)
    DEFINE_ECONSTANT(RESULTSETMETADATA_VERSION)
    DEFINE_ECONSTANT(RESULTLIST_VERSION)
    DEFINE_ECONSTANT(CALLABLESTATEMENT_VERSION)
    DEFINE_ECONSTANT(RESULTSETMETADATA_VERSION)

    //DEFINE_ECONSTANT(ZERO_REPOSITORY_PLACE)
    DEFINE_ECONSTANT(SYNTAX_ERROR)
    DEFINE_ECONSTANT(FEATURE_NOT_YET_IMPLEMENTED)
    DEFINE_ECONSTANT(BUG_CHECK)
    DEFINE_ECONSTANT(COMPILE_ERROR)
    DEFINE_ECONSTANT(RUNTIME_ERROR)
    DEFINE_ECONSTANT(OCS_ERROR)
    DEFINE_ECONSTANT(NETWORK_ERROR)
    DEFINE_ECONSTANT(CONVERSION_ERROR)
    DEFINE_ECONSTANT(TRUNCATION_ERROR)
    DEFINE_ECONSTANT(CONNECTION_ERROR)
    DEFINE_ECONSTANT(DDL_ERROR)
    DEFINE_ECONSTANT(APPLICATION_ERROR)
    DEFINE_ECONSTANT(SECURITY_ERROR)
    DEFINE_ECONSTANT(DATABASE_CORRUPTION)
    DEFINE_ECONSTANT(VERSION_ERROR)
    DEFINE_ECONSTANT(LICENSE_ERROR)
    DEFINE_ECONSTANT(INTERNAL_ERROR)
    DEFINE_ECONSTANT(DEBUG_ERROR)
    DEFINE_ECONSTANT(LOST_BLOB)
    DEFINE_ECONSTANT(INCONSISTENT_BLOB)
    DEFINE_ECONSTANT(DELETED_BLOB)
    DEFINE_ECONSTANT(LOG_ERROR)
    DEFINE_ECONSTANT(DATABASE_DAMAGED)
    DEFINE_ECONSTANT(UPDATE_CONFLICT)
    DEFINE_ECONSTANT(NO_SUCH_TABLE)
    DEFINE_ECONSTANT(INDEX_OVERFLOW)
    DEFINE_ECONSTANT(UNIQUE_DUPLICATE)
    DEFINE_ECONSTANT(UNCOMMITTED_UPDATES)
    DEFINE_ECONSTANT(DEADLOCK)
    DEFINE_ECONSTANT(OUT_OF_MEMORY_ERROR)
    DEFINE_ECONSTANT(OUT_OF_RECORD_MEMORY_ERROR)
    DEFINE_ECONSTANT(LOCK_TIMEOUT)
    DEFINE_ECONSTANT(PLATFORM_ERROR)
    DEFINE_ECONSTANT(NO_SCHEMA)
    DEFINE_ECONSTANT(CONFIGURATION_ERROR)
    DEFINE_ECONSTANT(READ_ONLY_ERROR)
    //DEFINE_ECONSTANT(NO_GENERATED_KEYS)
    DEFINE_ECONSTANT(THROWN_EXCEPTION)
    DEFINE_ECONSTANT(INVALID_TRANSACTION_ISOLATION)
    DEFINE_ECONSTANT(UNSUPPORTED_TRANSACTION_ISOLATION)
    DEFINE_ECONSTANT(INVALID_UTF8)
    DEFINE_ECONSTANT(CONSTRAINT_ERROR)
    DEFINE_ECONSTANT(UPDATE_ERROR)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoDisconnect, 1)
    T_NUMBER(NuoDisconnect, 0) // pq
    LastError = "";

    RETURN_NUMBER(0);
    if (PARAM_INT(0)) {
        ((Connection *)PARAM_INT(0))->close();
        //delete ((Connection *)PARAM_INT(0));
        SET_NUMBER(0, 0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(NuoConnect, 2, 3)
    T_STRING(NuoConnect, 0)
    T_ARRAY(NuoConnect, 1)

    Connection * connection = createConnection();
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, "");
    }

    LastError = "";
    if (connection) {
        Properties *properties = connection->allocProperties();
        int        count       = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
        for (INTEGER i = 0; i < count; i++) {
            void *newpData = 0;
            char *key      = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(1), i, &key);
            if ((newpData) && (key)) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    properties->putValue(key, szData);
                } else
                if (type == VARIABLE_NUMBER) {
                    AnsiString tmp(nData);
                    properties->putValue(key, tmp.c_str());
                }
            }
        }
        try {
            connection->openDatabase(PARAM(0), properties);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            if (PARAMETERS_COUNT > 2)
                SET_STRING(2, LastError.c_str());

            connection->close();
            //delete properties;
            connection = 0;
        }
    }
    RETURN_NUMBER((SYS_INT)connection)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(NuoConnectCreate, 3, 4)
    T_STRING(NuoConnectCreate, 0)
    T_STRING(NuoConnectCreate, 1)
    T_ARRAY(NuoConnectCreate, 2)

    Connection * connection = createConnection();
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }

    LastError = "";
    if (connection) {
        Properties *properties = connection->allocProperties();
        int        count       = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
        for (INTEGER i = 0; i < count; i++) {
            void *newpData = 0;
            char *key      = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(2), i, &key);
            if ((newpData) && (key)) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    properties->putValue(key, szData);
                } else
                if (type == VARIABLE_NUMBER) {
                    AnsiString tmp(nData);
                    properties->putValue(key, tmp.c_str());
                }
            }
        }

        /*try {
            connection->createDatabase(PARAM(0), PARAM(1), properties);
           } catch (SQLException &exc) {
            LastError=(char *)exc.getText();
            if (PARAMETERS_COUNT>3)
                SET_STRING(3, LastError.c_str());

            connection->close();
            //delete properties;
            connection=0;
           }*/
    }
    RETURN_NUMBER((SYS_INT)connection)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoGetAutoCommit, 1)
    T_HANDLE(NuoGetAutoCommit, 0) // con

    LastError = "";
    int        result      = 0;
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        result = connection->getAutoCommit();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(result);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetAutoCommit, 2)
    T_HANDLE(NuoSetAutoCommit, 0) // con
    T_NUMBER(NuoSetAutoCommit, 1)

    LastError = "";
    bool       result      = PARAM_INT(1) ? true : false;
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        connection->setAutoCommit(result);
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(result);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoPrepareTransaction, 1)
    T_HANDLE(NuoPrepareTransaction, 0) // con

    LastError = "";
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        connection->prepareTransaction();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoGetIsolationLevel, 1)
    T_HANDLE(NuoGetIsolationLevel, 0) // con

    LastError = "";
    int        result      = 0;
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        result = connection->getTransactionIsolation();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(result);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetIsolationLevel, 2)
    T_HANDLE(NuoSetIsolationLevel, 0) // con
    T_NUMBER(NuoSetIsolationLevel, 1)

    LastError = "";
    int        result      = PARAM_INT(1);
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        connection->setTransactionIsolation(result);
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(result);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoRollback, 1)
    T_HANDLE(NuoRollback, 0) // con

    LastError = "";
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        connection->rollback();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoCommit, 1)
    T_HANDLE(NuoCommit, 0) // con

    LastError = "";
    Connection *connection = (Connection *)PARAM_INT(0);
    try {
        connection->commit();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoError, 0)
    RETURN_STRING(LastError.c_str());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoQuery, 2)
    T_STRING(NuoQuery, 0) // query
    T_HANDLE(NuoQuery, 1) // con

    LastError = "";
    if (PARAM_INT(1)) {
        Connection *connection = (Connection *)PARAM_INT(1);
        Statement  *result     = 0;
        ResultSet  *rs         = 0;
        try {
            result = connection->createStatement();
            rs     = result->executeQuery(PARAM(0));
        } catch (SQLException& exc) {
            int code = exc.getSqlcode();
            if (code == -5) {
                // statement is not a select
                try {
                    rs = 0;
                    result->executeUpdate(PARAM(0));
                } catch (SQLException& exc) {
                    LastError = (char *)exc.getText();
                }
            } else
                LastError = (char *)exc.getText();
        }
        //if (result)
        //    result->close();
        RETURN_NUMBER((SYS_INT)rs);
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(NuoPrepared, 2, 3)
    T_STRING(NuoPrepared, 0) // query
    T_HANDLE(NuoPrepared, 1) // con

    bool auto_gen_keys = false;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(NuoPrepared, 2)
        auto_gen_keys = PARAM_INT(2) ? true : false;
    }

    LastError = "";
    if (PARAM_INT(1)) {
        Connection        *connection = (Connection *)PARAM_INT(1);
        PreparedStatement *result     = 0;
        try {
            if (auto_gen_keys)
                result = connection->prepareStatement(PARAM(0), RETURN_GENERATED_KEYS);
            else
                result = connection->prepareStatement(PARAM(0));
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
        RETURN_NUMBER((SYS_INT)result);
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetParameter, 3)
    T_HANDLE(NuoSetParameter, 0)
    T_NUMBER(NuoSetParameter, 1)
    T_STRING(NuoSetParameter, 2)

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        try {
            int  len      = PARAM_LEN(2);
            char *data    = PARAM(2);
            bool do_bytes = false;
            //if (len<0xFF) {
            for (int i = 0; i < len; i++) {
                if (!data[i]) {
                    // has character 0
                    do_bytes = true;
                    break;
                }
            }
            //} else
            //    do_bytes=true;
            if (do_bytes)
                result->setBytes(PARAM_INT(1), len, data);
            else
                result->setString(PARAM_INT(1), data);
            RETURN_NUMBER(1);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetBlob, 3)
    T_HANDLE(NuoSetBlob, 0)
    T_NUMBER(NuoSetBlob, 1)
    T_HANDLE(NuoSetBlob, 2)

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        Blob              *lob    = (Blob *)PARAM_INT(2);
        try {
            result->setBlob(PARAM_INT(1), lob);
            RETURN_NUMBER(1);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetClob, 3)
    T_HANDLE(NuoSetClob, 0)
    T_NUMBER(NuoSetClob, 1)
    T_HANDLE(NuoSetClob, 2)

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        Clob              *lob    = (Clob *)PARAM_INT(2);
        try {
            result->setClob(PARAM_INT(1), lob);
            RETURN_NUMBER(1);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoParameterCount, 1)
    T_HANDLE(NuoParameterCount, 0)

    LastError = "";
    PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
    int res = 0;
    try {
        ParameterMetaData *meta = result->getParameterMetaData();
        if (meta)
            res = meta->getParameterCount();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoParameterMeta, 2)
    T_HANDLE(NuoParameterMeta, 0)
    T_NUMBER(NuoParameterMeta, 1)

    LastError = "";
    CREATE_ARRAY(RESULT);
    PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
    int index = PARAM_INT(1);
    try {
        ParameterMetaData *meta = result->getParameterMetaData();
        if (meta) {
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ParameterType", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getParameterType(index));
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Precision", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getPrecision(index));
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Scale", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getScale(index));
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "isNullable", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->isNullable(index));
            } catch (...) {
            }
        }
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetNull, 2)
    T_HANDLE(NuoSetNull, 0)
    T_NUMBER(NuoSetNull, 1)

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        try {
            result->setNull(PARAM_INT(1), 0);
            RETURN_NUMBER(1);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoBlobCreate, 1)
    T_HANDLE(NuoBlobCreate, 0)
    LastError = "";
    try {
        Blob *result = ((Connection *)PARAM_INT(0))->createBlob();
        RETURN_NUMBER((SYS_INT)result);
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoClobCreate, 1)
    T_HANDLE(NuoClobCreate, 0)
    LastError = "";
    try {
        Clob *result = ((Connection *)PARAM_INT(0))->createClob();
        RETURN_NUMBER((SYS_INT)result);
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoBlobWrite, 2)
    T_HANDLE(NuoBlobWrite, 0)
    T_STRING(NuoBlobWrite, 1)

    try {
        ((Blob *)PARAM_INT(0))->appendBytes(PARAM_LEN(1), (unsigned char *)PARAM(1));
        RETURN_NUMBER(PARAM_LEN(1));
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoClobWrite, 2)
    T_HANDLE(NuoClobWrite, 0)
    T_STRING(NuoClobWrite, 1)

    try {
        ((Clob *)PARAM_INT(0))->appendChars(PARAM_LEN(1), PARAM(1));
        RETURN_NUMBER(PARAM_LEN(1));
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoSetCursorName, 3)
    T_HANDLE(NuoSetCursorName, 0)
    T_NUMBER(NuoSetCursorName, 1)
    T_STRING(NuoSetCursorName, 2)

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        try {
            result->setCursorName(PARAM(2));
            RETURN_NUMBER(1);
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoExecute, 1)
    T_HANDLE(NuoExecute, 0) // con

    LastError = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        ResultSet         *rs     = 0;
        try {
            rs = result->executeQuery();
        } catch (SQLException& exc) {
            int code = exc.getSqlcode();
            if (code == -5) {
                // statement is not a select
                try {
                    rs = 0;
                    result->executeUpdate();
                } catch (SQLException& exc) {
                    LastError = (char *)exc.getText();
                }
            } else
                LastError = (char *)exc.getText();
        }
        //if (result)
        //    result->close();
        RETURN_NUMBER((SYS_INT)rs);
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoFinish, 1)
    T_NUMBER(NuoFinish, 0) // con

//LastError="";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);

        try {
            result->close();
            //delete result;
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
    }

    SET_NUMBER(0, 0);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoAnalyze, 2)
    T_HANDLE(NuoAnalyze, 0) // con
    T_NUMBER(NuoAnalyze, 1)

//LastError="";
    const char *res = "";
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        try {
            res = result->analyze(PARAM_INT(1));
            //delete result;
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
    }
    if (res) {
        RETURN_STRING(res)
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoCountRows, 1)
    T_HANDLE(NuoCountRows, 0) // con

//LastError="";
    int res = 0;
    if (PARAM_INT(0)) {
        PreparedStatement *result = (PreparedStatement *)PARAM_INT(0);
        try {
            res = result->getUpdateCount();
            //delete result;
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
    }
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoNonQuery, 2)
    T_STRING(NuoNonQuery, 0) // query
    T_HANDLE(NuoNonQuery, 1) // con

    LastError = "";
    if (PARAM_INT(1)) {
        Connection *connection = (Connection *)PARAM_INT(1);
        Statement  *result     = 0;
        int        rs          = 0;
        try {
            result = connection->createStatement();
            rs     = result->executeUpdate(PARAM(0));
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
        if (result)
            result->close();
        RETURN_NUMBER((SYS_INT)rs);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoCountColumns, 1)
    T_NUMBER(NuoCountColumns, 0)

    LastError = "";
    RETURN_NUMBER(0)
    if (PARAM_INT(0)) {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        try {
            ResultSetMetaData *meta = res->getMetaData();
            if (meta)
                RETURN_NUMBER(meta->getColumnCount());
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoColumnInfo, 2)
    T_HANDLE(NuoColumnInfo, 0) // con
    T_NUMBER(NuoColumnInfo, 1)

    LastError = "";
    if (PARAM_INT(0)) {
        CREATE_ARRAY(RESULT);

        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            int     index           = PARAM_INT(1);
            INTEGER count           = meta->getColumnCount();
            if ((meta) && (index <= count)) {
                const char *catalog   = meta->getCatalogName(index);
                int        size       = meta->getColumnDisplaySize(index);
                const char *label     = meta->getColumnLabel(index);
                const char *name      = meta->getColumnName(index);
                int        type       = meta->getColumnType(index);
                const char *type_name = meta->getColumnTypeName(index);
                int        maxlength  = meta->getCurrentColumnMaxLength(index);
                int        precision  = meta->getPrecision(index);
                int        scale      = meta->getScale(index);
                const char *schema    = meta->getSchemaName(index);
                const char *table     = meta->getTableName(index);

                int isNullable           = meta->isNullable(index);
                int isSigned             = meta->isSigned(index);
                int isReadOnly           = meta->isReadOnly(index);
                int isWritable           = meta->isWritable(index);
                int isDefinitelyWritable = meta->isDefinitelyWritable(index);
                int isCurrency           = meta->isCurrency(index);
                int isCaseSensitive      = meta->isCaseSensitive(index);
                int isAutoIncrement      = meta->isAutoIncrement(index);
                int isSearchable         = meta->isSearchable(index);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"CatalogName", (INTEGER)VARIABLE_STRING, (char *)catalog, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"ColumnDisplaySize", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)size);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"ColumnLabel", (INTEGER)VARIABLE_STRING, (char *)label, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"ColumnName", (INTEGER)VARIABLE_STRING, (char *)name, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"ColumnType", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)type);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"ColumnTypeName", (INTEGER)VARIABLE_STRING, (char *)type_name, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"CurrentColumnMaxLength", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)maxlength);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"Precision", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)precision);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"Scale", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)scale);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"Schema", (INTEGER)VARIABLE_STRING, (char *)schema, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"Table", (INTEGER)VARIABLE_STRING, (char *)table, (NUMBER)0);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isNullable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isNullable);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isSigned", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isSigned);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isReadOnly", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isReadOnly);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isWritable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isWritable);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isDefinitelyWritable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isDefinitelyWritable);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isCurrency", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isCurrency);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isCaseSensitive", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isCaseSensitive);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isAutoIncrement", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isAutoIncrement);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"isSearchable", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)isSearchable);
            }
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoInfo, 1)
    T_HANDLE(NuoInfo, 0) // con

    LastError = "";
    if (PARAM_INT(0)) {
        CREATE_ARRAY(RESULT);

        Connection       *res  = (Connection *)PARAM_INT(0);
        DatabaseMetaData *meta = 0;
        try {
            meta = res->getMetaData();
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }

        if (meta) {
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "URL", (INTEGER)VARIABLE_STRING, meta->getURL(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "UserName", (INTEGER)VARIABLE_STRING, meta->getUserName(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DatabaseProductName", (INTEGER)VARIABLE_STRING, meta->getDatabaseProductName(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DatabaseProductVersion", (INTEGER)VARIABLE_STRING, meta->getDatabaseProductVersion(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DriverName", (INTEGER)VARIABLE_STRING, meta->getDriverName(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DriverVersion", (INTEGER)VARIABLE_STRING, meta->getDriverVersion(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IdentifierQuoteString", (INTEGER)VARIABLE_STRING, meta->getIdentifierQuoteString(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SQLKeywords", (INTEGER)VARIABLE_STRING, meta->getSQLKeywords(), (NUMBER)0);
            } catch (...) {
            }

            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "NumericFunctions", (INTEGER)VARIABLE_STRING, meta->getNumericFunctions(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "StringFunctions", (INTEGER)VARIABLE_STRING, meta->getStringFunctions(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SystemFunctions", (INTEGER)VARIABLE_STRING, meta->getSystemFunctions(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "TimeDateFunctions", (INTEGER)VARIABLE_STRING, meta->getTimeDateFunctions(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SearchStringEscape", (INTEGER)VARIABLE_STRING, meta->getSearchStringEscape(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ExtraNameCharacters", (INTEGER)VARIABLE_STRING, meta->getExtraNameCharacters(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SchemaTerm", (INTEGER)VARIABLE_STRING, meta->getSchemaTerm(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ProcedureTerm", (INTEGER)VARIABLE_STRING, meta->getProcedureTerm(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "CatalogTerm", (INTEGER)VARIABLE_STRING, meta->getCatalogTerm(), (NUMBER)0);
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "CatalogSeparator", (INTEGER)VARIABLE_STRING, meta->getCatalogSeparator(), (NUMBER)0);
            } catch (...) {
            }

            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "allProceduresAreCallable", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->allProceduresAreCallable());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "allTablesAreSelectable", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->allTablesAreSelectable());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "isReadOnly", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->isReadOnly());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullsAreSortedHigh", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->nullsAreSortedHigh());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullsAreSortedLow", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->nullsAreSortedLow());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullsAreSortedAtStart", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->nullsAreSortedAtStart());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullsAreSortedAtEnd", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->nullsAreSortedAtEnd());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DatabaseMajorVersion", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getDatabaseMajorVersion());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DatabaseMinorVersion", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getDatabaseMinorVersion());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DriverMajorVersion", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getDriverMajorVersion());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DriverMinorVersion", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getDriverMinorVersion());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "usesLocalFiles", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->usesLocalFiles());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "usesLocalFilePerTable", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->usesLocalFilePerTable());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsMixedCaseIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsMixedCaseIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesUpperCaseIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesUpperCaseIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesLowerCaseIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesLowerCaseIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesMixedCaseIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesMixedCaseIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsMixedCaseQuotedIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsMixedCaseQuotedIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesUpperCaseQuotedIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesUpperCaseQuotedIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesLowerCaseQuotedIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesLowerCaseQuotedIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "storesMixedCaseQuotedIdentifiers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->storesMixedCaseQuotedIdentifiers());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsAlterTableWithAddColumn", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsAlterTableWithAddColumn());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsAlterTableWithDropColumn", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsAlterTableWithDropColumn());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsColumnAliasing", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsColumnAliasing());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nullPlusNonNullIsNull", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->nullPlusNonNullIsNull());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsConvert", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsConvert());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsTableCorrelationNames", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsTableCorrelationNames());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsDifferentTableCorrelationNames", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsDifferentTableCorrelationNames());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsExpressionsInOrderBy", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsExpressionsInOrderBy());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOrderByUnrelated", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOrderByUnrelated());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsGroupBy", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsGroupBy());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsGroupByUnrelated", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsGroupByUnrelated());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsGroupByBeyondSelect", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsGroupByBeyondSelect());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsLikeEscapeClause", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsLikeEscapeClause());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsMultipleResultSets", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsMultipleResultSets());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsMultipleTransactions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsMultipleTransactions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsNonNullableColumns", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsNonNullableColumns());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsMinimumSQLGrammar", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsMinimumSQLGrammar());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCoreSQLGrammar", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCoreSQLGrammar());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsExtendedSQLGrammar", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsExtendedSQLGrammar());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsANSI92EntryLevelSQL", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsANSI92EntryLevelSQL());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsANSI92IntermediateSQL", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsANSI92IntermediateSQL());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsANSI92FullSQL", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsANSI92FullSQL());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsIntegrityEnhancementFacility", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsIntegrityEnhancementFacility());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOuterJoins", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOuterJoins());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsFullOuterJoins", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsFullOuterJoins());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsLimitedOuterJoins", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsLimitedOuterJoins());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "isCatalogAtStart", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->isCatalogAtStart());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSchemasInDataManipulation", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSchemasInDataManipulation());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSchemasInProcedureCalls", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSchemasInProcedureCalls());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSchemasInTableDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSchemasInTableDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSchemasInIndexDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSchemasInIndexDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSchemasInPrivilegeDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSchemasInPrivilegeDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCatalogsInDataManipulation", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCatalogsInDataManipulation());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCatalogsInProcedureCalls", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCatalogsInProcedureCalls());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCatalogsInTableDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCatalogsInTableDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCatalogsInIndexDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCatalogsInIndexDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCatalogsInPrivilegeDefinitions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCatalogsInPrivilegeDefinitions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsPositionedDelete", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsPositionedDelete());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsPositionedUpdate", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsPositionedUpdate());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSelectForUpdate", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSelectForUpdate());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsStoredProcedures", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsStoredProcedures());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSubqueriesInComparisons", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSubqueriesInComparisons());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSubqueriesInExists", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSubqueriesInExists());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSubqueriesInIns", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSubqueriesInIns());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsSubqueriesInQuantifieds", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsSubqueriesInQuantifieds());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsCorrelatedSubqueries", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsCorrelatedSubqueries());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsUnion", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsUnion());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsUnionAll", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsUnionAll());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOpenCursorsAcrossCommit", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOpenCursorsAcrossCommit());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOpenCursorsAcrossRollback", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOpenCursorsAcrossRollback());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOpenStatementsAcrossCommit", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOpenStatementsAcrossCommit());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsOpenStatementsAcrossRollback", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsOpenStatementsAcrossRollback());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxCharLiteralLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxCharLiteralLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnsInGroupBy", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnsInGroupBy());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnsInIndex", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnsInIndex());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnsInOrderBy", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnsInOrderBy());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnsInSelect", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnsInSelect());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxColumnsInTable", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxColumnsInTable());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxConnections", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxConnections());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxCursorNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxCursorNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxIndexLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxIndexLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxSchemaNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxSchemaNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxProcedureNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxProcedureNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxCatalogNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxCatalogNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxRowSize", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxRowSize());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "doesMaxRowSizeIncludeBlobs", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->doesMaxRowSizeIncludeBlobs());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxStatementLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxStatementLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxStatements", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxStatements());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxTableNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxTableNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxTablesInSelect", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxTablesInSelect());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxUserNameLength", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getMaxUserNameLength());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "DefaultTransactionIsolation", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->getDefaultTransactionIsolation());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsTransactions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsTransactions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsDataDefinitionAndDataManipulationTransactions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsDataDefinitionAndDataManipulationTransactions());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "supportsDataManipulationTransactionsOnly", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->supportsDataManipulationTransactionsOnly());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dataDefinitionCausesTransactionCommit", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->dataDefinitionCausesTransactionCommit());
            } catch (...) {
            }
            try {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "dataDefinitionIgnoredInTransactions", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)meta->dataDefinitionIgnoredInTransactions());
            } catch (...) {
            }
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoValue, 2)
    T_HANDLE(NuoValue, 0)
    T_NUMBER(NuoValue, 1)

    LastError = "";

    RETURN_STRING("");
    int index = PARAM_INT(1);
    if (PARAM_INT(0)) {
        char *data = 0;
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();

            if (index <= count) {
                try {
                    Clob *lob = res->getClob(index);
                    if (lob) {
                        int len = lob->length();
                        CORE_NEW(len + 1, data);
                        if (data) {
                            lob->getChars(0, len, data);
                            data[len] = 0;

                            SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                        }
                    }
                } catch (...) {
                    if (data) {
                        CORE_DELETE(data);
                        data = 0;
                    }

                    Blob *lob = res->getBlob(index);
                    if (lob) {
                        int len = lob->length();
                        //unsigned char *data=0;
                        CORE_NEW(len + 1, data);
                        if (data) {
                            lob->getBytes(0, len, (unsigned char *)data);
                            data[len] = 0;
                            SetVariable(RESULT, (INTEGER)-1, (char *)data, (NUMBER)len);
                        }
                    }
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
                data = 0;
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoValueByName, 2)
    T_HANDLE(NuoValueByName, 0)
    T_STRING(NuoValueByName, 1)

    LastError = "";

    RETURN_STRING("");

    char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet *res = (ResultSet *)PARAM_INT(0);
            try {
                Clob *lob = res->getClob(PARAM(1));
                if (lob) {
                    int len = lob->length();
                    CORE_NEW(len + 1, data);
                    if (data) {
                        lob->getChars(0, len, data);
                        data[len] = 0;

                        SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                    }
                }
            } catch (...) {
                if (data) {
                    CORE_DELETE(data);
                    data = 0;
                }
                Blob *lob = res->getBlob(PARAM(1));
                if (lob) {
                    int len = lob->length();
                    //unsigned char *data=0;
                    CORE_NEW(len + 1, data);
                    if (data) {
                        lob->getBytes(0, len, (unsigned char *)data);
                        data[len] = 0;

                        SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                    }
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
                data = 0;
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoBlob, 4)
    T_HANDLE(NuoBlob, 0)
    T_NUMBER(NuoBlob, 1)
    T_NUMBER(NuoBlob, 2)
    T_NUMBER(NuoBlob, 3)

    LastError = "";

    RETURN_STRING("");
    int           index = PARAM_INT(1);
    unsigned char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();
            long start = PARAM_INT(2);
            long bytes = PARAM_INT(3);
            if (index <= count) {
                Blob *lob = res->getBlob(index);
                if (lob) {
                    int len = lob->length();
                    if (len > bytes)
                        len = bytes;

                    CORE_NEW(len + 1, data);
                    if (data) {
                        lob->getBytes(start, len, data);
                        data[len] = 0;

                        SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                    }
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoBlobByName, 4)
    T_HANDLE(NuoBlobByName, 0)
    T_STRING(NuoBlobByName, 1)
    T_NUMBER(NuoBlobByName, 2)
    T_NUMBER(NuoBlobByName, 3)

    LastError = "";

    RETURN_STRING("");
    unsigned char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();
            long start = PARAM_INT(2);
            long bytes = PARAM_INT(3);
            Blob *lob  = res->getBlob(PARAM(1));
            if (lob) {
                int len = lob->length();
                if (len > bytes)
                    len = bytes;

                CORE_NEW(len + 1, data);
                if (data) {
                    data[len] = 0;
                    lob->getBytes(start, len, data);

                    SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoClob, 4)
    T_HANDLE(NuoClob, 0)
    T_NUMBER(NuoClob, 1)
    T_NUMBER(NuoClob, 2)
    T_NUMBER(NuoClob, 3)

    LastError = "";

    RETURN_STRING("");
    int  index = PARAM_INT(1);
    char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();
            long start = PARAM_INT(2);
            long bytes = PARAM_INT(3);
            if (index <= count) {
                Clob *lob = res->getClob(index);
                if (lob) {
                    int len = lob->length();
                    if (len > bytes)
                        len = bytes;

                    CORE_NEW(len + 1, data);
                    if (data) {
                        lob->getChars(start, len, data);
                        data[len] = 0;

                        SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                    }
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoClobByName, 4)
    T_HANDLE(NuoClobByName, 0)
    T_STRING(NuoClobByName, 1)
    T_NUMBER(NuoClobByName, 2)
    T_NUMBER(NuoClobByName, 3)

    LastError = "";

    RETURN_STRING("");
    char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();
            long start = PARAM_INT(2);
            long bytes = PARAM_INT(3);
            Clob *lob  = res->getClob(PARAM(1));
            if (lob) {
                int len = lob->length();
                if (len > bytes)
                    len = bytes;

                CORE_NEW(len + 1, data);
                if (data) {
                    data[len] = 0;
                    lob->getChars(start, len, data);

                    SetVariable(RESULT, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoValues, 1)
    T_HANDLE(NuoValues, 0)

    LastError = "";
    CREATE_ARRAY(RESULT);
    char *data = 0;
    if (PARAM_INT(0)) {
        try {
            ResultSet         *res  = (ResultSet *)PARAM_INT(0);
            ResultSetMetaData *meta = res->getMetaData();
            INTEGER           count = meta->getColumnCount();
            for (int i = 1; i <= count; i++) {
                //const char *data=res->getString(i);
                data = 0;
                try {
                    Clob *lob = res->getClob(i);
                    if (lob) {
                        int        len   = lob->length();
                        const char *name = meta->getColumnName(i);
                        if (name) {
                            //char *data=new char[len+1];
                            //data[len]=0;
                            CORE_NEW(len + 1, data);
                            if (data) {
                                data[len] = 0;
                                lob->getChars(0, len, data);

                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)name, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                            }
                        }
                    }
                    //lob->release();
                } catch (...) {
                    if (data) {
                        CORE_DELETE(data);
                        data = 0;
                    }
                    Blob *lob = res->getBlob(i);
                    if (lob) {
                        int        len   = lob->length();
                        const char *name = meta->getColumnName(i);
                        if (name) {
                            //char *data=new char[len+1];
                            //data[len]=0;
                            CORE_NEW(len + 1, data);
                            if (data) {
                                data[len] = 0;
                                lob->getBytes(0, len, (unsigned char *)data);

                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)name, (INTEGER)/*VARIABLE_STRING*/ -1, (char *)data, (NUMBER)len);
                            }
                        }
                    }
                }
            }
        } catch (SQLException& exc) {
            if (data) {
                CORE_DELETE(data);
                data = 0;
            }
            LastError = (char *)exc.getText();
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoNext, 1)
    T_HANDLE(NuoNext, 0)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        RETURN_NUMBER(res->next());
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoFirst, 1)
    T_HANDLE(NuoFirst, 0)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        RETURN_NUMBER(res->first());
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoAbsolute, 2)
    T_HANDLE(NuoAbsolute, 0)
    T_NUMBER(NuoAbsolute, 1)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        RETURN_NUMBER(res->absolute(PARAM_INT(1)));
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoLast, 1)
    T_HANDLE(NuoLast, 0)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        RETURN_NUMBER(res->last());
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoPrevious, 1)
    T_HANDLE(NuoPrevious, 0)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        RETURN_NUMBER(res->previous());
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoRefresh, 1)
    T_HANDLE(NuoRefresh, 0)

    LastError = "";
    try {
        ResultSet *res = (ResultSet *)PARAM_INT(0);
        res->refreshRow();
    } catch (SQLException& exc) {
        LastError = (char *)exc.getText();
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NuoFreeResult, 1)
    T_NUMBER(NuoFreeResult, 0) // result

//LastError="";
    if (PARAM_INT(0)) {
        try {
            //Statement *s=((ResultSet *)PARAM_INT(0))->getStatement();
            //if (s)
            //    s->close();
            //else
            ((ResultSet *)PARAM_INT(0))->close();
            //delete ((ResultSet *)PARAM_INT(0));
        } catch (SQLException& exc) {
            LastError = (char *)exc.getText();
        }
        SET_NUMBER(0, 0);
    }

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//

