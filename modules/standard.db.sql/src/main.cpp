//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiList.h"
#include "AnsiString.h"
#include "odbcman.h"
#include "SQLStatament.h"
#include <sql.h>
#include <sqlext.h>
#include "semhh.h"

#define RESET_ERRORS

//-------------------------//
// Local variables         //
//-------------------------//

SQLHANDLE InstanceEnviroment = 0;             // Default ODBC enviroment, auto-created
                                              // by this library

HHSEM    sem;
AnsiList ConnectionList;                    // Connection Lists : a list where all
                                            // connections can be retrieved

///////// Memory management functions ////////////////////////////
//---------------------------------------------------------------------------
// Called automaticaly by Concept Interpreter on Library Load
CONCEPT_DLL_API ON_CREATE_CONTEXT(SYS_INT _MANAGEMENT_UNIQUE_ID, void *HANDLER, INVOKE_CALL Invoke) {
    if (CSQLManager::CreateEnviroment(&InstanceEnviroment) != 0) {
        InstanceEnviroment = 0;
        return (void *)ERR_ENVIROMENT;
    }
    DEFINE_ICONSTANT("ODBC_CHAR", SQL_CHAR)
    DEFINE_ICONSTANT("ODBC_VARCHAR", SQL_VARCHAR)
    DEFINE_ICONSTANT("ODBC_LONGVARCHAR", SQL_LONGVARCHAR)
    DEFINE_ICONSTANT("ODBC_WCHAR", SQL_WCHAR)
    DEFINE_ICONSTANT("ODBC_WVARCHAR", SQL_WVARCHAR)
    DEFINE_ICONSTANT("ODBC_WLONGVARCHAR", SQL_WLONGVARCHAR)
    DEFINE_ICONSTANT("ODBC_DECIMAL", SQL_DECIMAL)
    DEFINE_ICONSTANT("ODBC_NUMERIC", SQL_NUMERIC)
    DEFINE_ICONSTANT("ODBC_SMALLINT", SQL_SMALLINT)
    DEFINE_ICONSTANT("ODBC_INTEGER", SQL_INTEGER)
    DEFINE_ICONSTANT("ODBC_REAL", SQL_REAL)
    DEFINE_ICONSTANT("ODBC_FLOAT", SQL_FLOAT)
    DEFINE_ICONSTANT("ODBC_DOUBLE", SQL_DOUBLE)
    DEFINE_ICONSTANT("ODBC_BIT", SQL_BIT)
    DEFINE_ICONSTANT("ODBC_TINYINT", SQL_TINYINT)
    DEFINE_ICONSTANT("ODBC_BIGINT", SQL_BIGINT)
    DEFINE_ICONSTANT("ODBC_BINARY", SQL_BINARY)
    DEFINE_ICONSTANT("ODBC_VARBINARY", SQL_VARBINARY)
    DEFINE_ICONSTANT("ODBC_LONGVARBINARY", SQL_LONGVARBINARY)
    DEFINE_ICONSTANT("ODBC_TYPE_DATE", SQL_TYPE_DATE)
    DEFINE_ICONSTANT("ODBC_TYPE_TIME", SQL_TYPE_TIME)
    DEFINE_ICONSTANT("ODBC_TYPE_TIMESTAMP", SQL_TYPE_TIMESTAMP)
    DEFINE_ICONSTANT("ODBC_INTERVAL_MONTH", SQL_INTERVAL_MONTH)
    DEFINE_ICONSTANT("ODBC_INTERVAL_YEAR", SQL_INTERVAL_YEAR)
    DEFINE_ICONSTANT("ODBC_INTERVAL_YEAR_TO_MONTH", SQL_INTERVAL_YEAR_TO_MONTH)
    DEFINE_ICONSTANT("ODBC_INTERVAL_DAY", SQL_INTERVAL_DAY)
    DEFINE_ICONSTANT("ODBC_INTERVAL_HOUR", SQL_INTERVAL_HOUR)
    DEFINE_ICONSTANT("ODBC_INTERVAL_MINUTE", SQL_INTERVAL_MINUTE)
    DEFINE_ICONSTANT("ODBC_INTERVAL_SECOND", SQL_INTERVAL_SECOND)
    DEFINE_ICONSTANT("ODBC_INTERVAL_DAY_TO_HOUR", SQL_INTERVAL_DAY_TO_HOUR)
    DEFINE_ICONSTANT("ODBC_INTERVAL_DAY_TO_MINUTE", SQL_INTERVAL_DAY_TO_MINUTE)
    DEFINE_ICONSTANT("ODBC_INTERVAL_DAY_TO_SECOND", SQL_INTERVAL_DAY_TO_SECOND)
    DEFINE_ICONSTANT("ODBC_INTERVAL_HOUR_TO_MINUTE", SQL_INTERVAL_HOUR_TO_MINUTE)
    DEFINE_ICONSTANT("ODBC_INTERVAL_HOUR_TO_SECOND", SQL_INTERVAL_HOUR_TO_SECOND)
    DEFINE_ICONSTANT("ODBC_INTERVAL_MINUTE_TO_SECOND", SQL_INTERVAL_MINUTE_TO_SECOND)
    DEFINE_ICONSTANT("ODBC_GUID", SQL_GUID)
    seminit(sem, 1);
    return 0;
}

//---------------------------------------------------------------------------
// Called automaticaly by Concept Interpreter on Library Unload
CONCEPT_DLL_API ON_DESTROY_CONTEXT(SYS_INT _MANAGEMENT_UNIQUE_ID, void *HANDLER, INVOKE_CALL Invoke) {
    // Fails on FreeBSD !!!

    /*if ((InstanceEnviroment) && (CSQLManager::DestroyEnviroment(InstanceEnviroment)!=0))
        return (void *)ERR_FREE_ENVIROMENT;
       ConnectionList.Clear();*/
    semdel(sem);
    return 0;
}

///////// End of memory management functions /////////////////////
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCConnect CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(3, "ODBCConnect requires 3 parameters : szDSN, szUSER, szPASS");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // DSN
    char *szDSN;
    // username
    char *szUSER;
    // password
    char *szPASS;
    // connection class
    CSQLManager *Connection;

    // checking parameters type
    GET_CHECK_STRING(0, szDSN, "Parameter 0 should be of STATIC STRING type");
    GET_CHECK_STRING(1, szUSER, "Parameter 1 should be of STATIC STRING type");
    GET_CHECK_STRING(2, szPASS, "Parameter 2 should be of STATIC STRING type");

    // creating new Connection
    Connection = new CSQLManager(InstanceEnviroment);

    if (Connection->Connect(szDSN, szUSER, szPASS)) {
        delete Connection;
        RETURN_NUMBER(0);
        return 0;
    }

    // adding to connection list
    semp(sem);
    ConnectionList.Add(Connection, DATA_CONNECTION);
    int res = ConnectionList.Count();
    semv(sem);
    // returning the index in ConnectionList
    RETURN_NUMBER(res);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCDriverConnect CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(1, "ODBCDriverConnect requires one parameter : szCONNECTION_STRING");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // DSN
    char *szCONNECTION_STRING;
    // connection class
    CSQLManager *Connection;

    // checking parameters type
    GET_CHECK_STRING(0, szCONNECTION_STRING, "Parameter 0 should be of STATIC STRING type");

    // creating new Connection
    Connection = new CSQLManager(InstanceEnviroment);

    if (Connection->Connect(szCONNECTION_STRING)) {
        delete Connection;
        RETURN_NUMBER(0);
        return 0;
    }

    // adding to connection list
    semp(sem);
    ConnectionList.Add(Connection, DATA_CONNECTION);
    int res = ConnectionList.Count();
    semv(sem);
    // returning the index in ConnectionList
    RETURN_NUMBER(res);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCDisconnect CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCDisconnect has only one optional parameter (nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;

    if (PARAMETERS_COUNT) {
        // checking parameters type
        GET_CHECK_NUMBER(0, nConnectionID, "Parameter 0 should be of STATIC NUMBER type");
    }

    // AnsiList performs delete on ConnectionList;
    // returning 1 (true) if it was a valid index, or 0 if not
    semp(sem);
    if (ConnectionList.Delete((int)nConnectionID - 1)) {
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
    semv(sem);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFirstDSN CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(0, "ODBCFirstDSN takes no parameters");

    AnsiString res = CSQLManager::GetFirstDSN(InstanceEnviroment);
    RETURN_STRING(res.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCNextDSN CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(0, "ODBCNextDSN takes no parameters");

    AnsiString res = CSQLManager::GetNextDSN(InstanceEnviroment);
    RETURN_STRING(res.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCDriver CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCGetDriver has only one optional parameter (nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;

    if (PARAMETERS_COUNT) {
        // checking parameters type
        GET_CHECK_NUMBER(0, nConnectionID, "Parameter 0 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    AnsiString res = Connection->GetDriver();
    RETURN_STRING(res.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCAutoCommit CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "ODBCAutoCommit has 2 paramaters (one is optional : nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // model
    NUMBER nModel = 0;
    // connection class
    CSQLManager *Connection = 0;

    GET_CHECK_NUMBER(0, nModel, "Parameter 0 should be of STATIC NUMBER type");
    if (PARAMETERS_COUNT > 1) {
        // checking parameters type
        GET_CHECK_NUMBER(1, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    RETURN_NUMBER(Connection->SetTransactionModel((char)nModel));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCCommit CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCCommit has only one optional parameter (nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;

    if (PARAMETERS_COUNT > 1) {
        // checking parameters type
        GET_CHECK_NUMBER(1, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    RETURN_NUMBER(Connection->Commit());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCRollback CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCRollback has only one optional parameter (nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;

    if (PARAMETERS_COUNT > 1) {
        // checking parameters type
        GET_CHECK_NUMBER(1, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    RETURN_NUMBER(Connection->Rollback());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCExecuteQuery CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "ODBCExecuteQuery has 2 paramaters (one is optional : nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;
    RESET_ERRORS

    // connection ID
    NUMBER nConnectionID = 1;
    // model
    char *szQuery = 0;
    // connection class
    CSQLManager *Connection = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_STRING(0, szQuery, "Parameter 0 should be of STATIC STRING type");
    if (PARAMETERS_COUNT > 1) {
        // checking parameters type
        GET_CHECK_NUMBER(1, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    Stmt = Connection->CreateStatement();

    if (Stmt->Query(szQuery)) {
        Connection->FreeStatament(Stmt);
        delete Stmt;
        RETURN_NUMBER(0);
    } else
        RETURN_NUMBER((long)Stmt);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCStatement CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCStatement has one optional paramater: nConnection_id");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    RESET_ERRORS

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;


    if (PARAMETERS_COUNT) {
        // checking parameters type
        GET_CHECK_NUMBER(0, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    Stmt = Connection->CreateStatement();

    RETURN_NUMBER((long)Stmt);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCExecute CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(2, "ODBCExecute has 2 paramaters: szQuery_string, nStatement");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    RESET_ERRORS

    // model
    char   *szQuery   = 0;
    NUMBER nStatement = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;


    GET_CHECK_STRING(0, szQuery, "Parameter 0 should be of STATIC STRING type");
    GET_CHECK_NUMBER(1, nStatement, "Parameter 1 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatement;
    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->QueryPrepared(szQuery));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCListTables CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(4, 5, "ODBCListTables has 5 paramaters (one is optional : nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;
    // parameters for ListTables
    char *szCatalog;
    char *szSchema;
    char *szName;
    char *szType;

    GET_CHECK_STRING(0, szCatalog, "Parameter 0 should be of STATIC STRING type");
    GET_CHECK_STRING(1, szSchema, "Parameter 1 should be of STATIC STRING type");
    GET_CHECK_STRING(2, szName, "Parameter 2 should be of STATIC STRING type");
    GET_CHECK_STRING(3, szType, "Parameter 3 should be of STATIC STRING type");
    if (PARAMETERS_COUNT > 4) {
        // checking parameters type
        GET_CHECK_NUMBER(4, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    Stmt = Connection->CreateStatement();

    if (Stmt->ListTables(szCatalog, szSchema, szName, szType)) {
        Connection->FreeStatament(Stmt);
        delete Stmt;
        RETURN_NUMBER(0);
    } else
        RETURN_NUMBER((long)Stmt);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCListTypes CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCListTypes has one optional paramater  : [nConnection_id]");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // connection class
    CSQLManager *Connection = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    if (PARAMETERS_COUNT) {
        // checking parameters type
        GET_CHECK_NUMBER(0, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    Stmt = Connection->CreateStatement();

    if (Stmt->ListTypes()) {
        Connection->FreeStatament(Stmt);
        delete Stmt;
        RETURN_NUMBER(0);
    } else
        RETURN_NUMBER((long)Stmt);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCListColumns CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(4, 5, "ODBCListColumns has 5 paramaters (one is optional : nConnection_id)");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // connection ID
    NUMBER nConnectionID = 1;
    // model
    CSQLManager *Connection = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;
    // parameters for ListTables
    char *szCatalog;
    char *szSchema;
    char *szName;
    char *szColumn;

    GET_CHECK_STRING(0, szCatalog, "Parameter 0 should be of STATIC STRING type");
    GET_CHECK_STRING(1, szSchema, "Parameter 1 should be of STATIC STRING type");
    GET_CHECK_STRING(2, szName, "Parameter 2 should be of STATIC STRING type");
    GET_CHECK_STRING(3, szColumn, "Parameter 3 should be of STATIC STRING type");
    if (PARAMETERS_COUNT > 4) {
        // checking parameters type
        GET_CHECK_NUMBER(4, nConnectionID, "Parameter 1 should be of STATIC NUMBER type");
    }

    semp(sem);
    Connection = (CSQLManager *)ConnectionList[(int)nConnectionID - 1];
    semv(sem);
    if (!Connection)
        return (void *)"Invalid connection descriptor";

    Stmt = Connection->CreateStatement();

    if (Stmt->ListColumns(szCatalog, szSchema, szName, szColumn)) {
        Connection->FreeStatament(Stmt);
        delete Stmt;
        RETURN_NUMBER(0);
    } else
        RETURN_NUMBER((long)Stmt);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFreeQuery CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(1, "ODBCFreeQuery takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    if (Stmt->Free()) {
        // setting statament handle to 0
        SET_NUMBER(0, 0);
        RETURN_NUMBER(1);
    } else
        RETURN_NUMBER(0);
    delete Stmt;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCCountColumns CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(1, "ODBCCountColumns takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->CountColumns());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCCountRows CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(1, "ODBCCountRows takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->GetRowCount());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCPosition CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(1, "ODBCPosition takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->GetCurrentRow());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCDescribeCol CONCEPT_API_PARAMETERS  {
    PARAMETERS_CHECK(7, "ODBCDescribeCol takes 7 parameters: column_index, var name, var type, var size, var scale, var nullable, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    NUMBER nIndex     = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nIndex, "Parameter 0 should be a STATIC NUMBER");
    CHECK_STATIC_PARAM(1, "Parameter 1 should not be an instance of a class");
    CHECK_STATIC_PARAM(2, "Parameter 2 should not be an instance of a class");
    CHECK_STATIC_PARAM(3, "Parameter 3 should not be an instance of a class");
    CHECK_STATIC_PARAM(4, "Parameter 4 should not be an instance of a class");
    CHECK_STATIC_PARAM(5, "Parameter 5 should not be an instance of a class");
    GET_CHECK_NUMBER(6, nStatament, "Parameter 0 should be of STATIC NUMBER type");


    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    AnsiString  Name;
    SQLSMALLINT type     = 0;
    SQLULEN     size     = 0;
    SQLSMALLINT scale    = 0;
    SQLSMALLINT nullable = 0;

    RETURN_NUMBER(Stmt->Describe((int)nIndex, &Name, &type, &size, &scale, &nullable));
    SET_STRING(1, Name.c_str());
    SET_NUMBER(2, type);
    SET_NUMBER(3, size);
    SET_NUMBER(4, scale);
    SET_NUMBER(5, nullable);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCBindResult CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCBindResult takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->BindResult());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFreeResult CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFreeResult takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(-1);
        return 0;
    }

    RETURN_NUMBER(Stmt->FreeResult());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchFirst CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFetchFirst takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_FIRST));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchLast CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFetchLast takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_LAST));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchNext CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFetchNext takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_NEXT));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchForward CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFetchForward takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->FetchForward());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchPrior CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCFetchPrior takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_PRIOR));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCFetchAbsolute CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "ODBCFetchAbsolute takes 2 parameters: row_number, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // row number
    NUMBER nRow = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nRow, "Parameter 0 should be of STATIC NUMBER type");
    GET_CHECK_NUMBER(1, nStatament, "Parameter 1 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_ABSOLUTE, (long)nRow));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCSkip CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "ODBCSkip takes 2 parameters: row_number, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // row number
    NUMBER nRow = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nRow, "Parameter 0 should be of STATIC NUMBER type");
    GET_CHECK_NUMBER(1, nStatament, "Parameter 1 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Fetch(SQL_FETCH_RELATIVE, (long)nRow));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCColumnGet CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "ODBCColumnGet takes 2 parameters: column_number, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // row number
    NUMBER nRow = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nRow, "Parameter 0 should be of STATIC NUMBER type");
    GET_CHECK_NUMBER(1, nStatament, "Parameter 1 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    char *data = 0;
    int  size;
    if (Stmt->GetColumnByIndex((long)nRow, &data, &size)) {
        RETURN_STRING("");
    } else {
        if (size > 0) {
            RETURN_BUFFER(data, size);
        } else {
            RETURN_STRING("");
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCColumnSet CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "ODBCColumnSet takes 3 parameters: row_number, value, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // row number
    NUMBER nRow = 0;
    // new value
    char   *szValue  = 0;
    NUMBER nValueLen = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nRow, "Parameter 0 should be of STATIC NUMBER type");
    GET_CHECK_BUFFER(1, szValue, nValueLen, "Parameter 1 should be of STATIC STRING type");
    GET_CHECK_NUMBER(2, nStatament, "Parameter 2 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    if (Stmt->SetColumnByIndex((long)nRow, szValue, (long)nValueLen)) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(1);
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCUpdate CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCUpdate takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Update());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCDelete CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCDelete takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Delete());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCLock CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCLock takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Lock(SQL_LOCK_EXCLUSIVE));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCUnlock CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCUnlock takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(!Stmt->Lock(SQL_LOCK_UNLOCK));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCError CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCError takes one optional parameter: connection_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nConnection = 1;

    if (PARAMETERS_COUNT) {
        GET_CHECK_NUMBER(0, nConnection, "Parameter 0 should be of STATIC NUMBER type");
    }

    semp(sem);
    CSQLManager *Connection = (CSQLManager *)ConnectionList[(int)nConnection - 1];
    semv(sem);

    if (Connection) {
        RETURN_STRING(Connection->LAST_ERROR.c_str());
        Connection->LAST_ERROR = (char *)"";
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCWarning CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(0, 1, "ODBCWarning takes one optional parameter: connection_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nConnection = 1;

    if (PARAMETERS_COUNT) {
        GET_CHECK_NUMBER(0, nConnection, "Parameter 0 should be of STATIC NUMBER type");
    }

    semp(sem);
    CSQLManager *Connection = (CSQLManager *)ConnectionList[(int)nConnection - 1];
    semv(sem);

    if (Connection) {
        RETURN_STRING(Connection->LAST_ERROR.c_str());
        Connection->LAST_ERROR = (char *)"";
    } else {
        RETURN_STRING("");
    }
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCAddParameter CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "ODBCAddParameters takes two parameters: parameter_data, statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    RESET_ERRORS

    // statament handle
    NUMBER nStatament = 0;
    // model
    char   *buffer = 0;
    NUMBER len;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_BUFFER(0, buffer, len, "Parameter 0 should be of STATIC STRING type");
    GET_CHECK_NUMBER(1, nStatament, "Parameter 1 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(!Stmt->AddParameter(buffer, (long)len));
    }

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ODBCClearParameters CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ODBCClearParameters takes one parameter: statament_handle");

    // Concept Library Local variabiles (declaration and initialization)
    // You should use this "inline" if you want to use GET_/SET_* macro
    LOCAL_INIT;

    // statament handle
    NUMBER nStatament = 0;
    // statemtn class
    CSQLStatement *Stmt = 0;

    GET_CHECK_NUMBER(0, nStatament, "Parameter 0 should be of STATIC NUMBER type");

    Stmt = (CSQLStatement *)(long)nStatament;

    if (!Stmt) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(!Stmt->ClearParameters());
    }
    return 0;
}
