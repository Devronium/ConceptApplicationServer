#include "odbcman.h"
#include "SQLStatament.h"

#define MAX_PATH    260

//AnsiString CSQLManager::LAST_ERROR;
//AnsiString CSQLManager::LAST_WARNING;

static AnsiString empty_string("");

int CSQLManager::DestroyEnviroment(SQLHANDLE _hEnv) {
    STATIC_CHECK_FAIL(SQLFreeHandle(SQL_HANDLE_ENV, _hEnv));
    return 0;
}

int CSQLManager::CreateEnviroment(SQLHANDLE *_hEnv) {
    SQLRETURN return_code = SQLAllocHandle(SQL_HANDLE_ENV,
                                           SQL_NULL_HANDLE,
                                           _hEnv);

    STATIC_CHECK_FAIL2(return_code);

    // setting ODBC version to 3.0
    return_code = SQLSetEnvAttr(*_hEnv,
                                SQL_ATTR_ODBC_VERSION,
                                (SQLPOINTER)SQL_OV_ODBC3,
                                SQL_IS_UINTEGER);
    STATIC_CHECK_FAIL2(return_code);
    return 0;
}

int CSQLManager::SetEnviroment(SQLHANDLE _hEnv) {
    if (_hEnv) {
        // enviroment already created by another connection
        hEnv = _hEnv;
        return 0;
    }
    return CSQLManager::CreateEnviroment(&hEnv);
}

int CSQLManager::Connect(const char *szDSN, const char *username, const char *password) {
    CHECK_ENVIROMENT;

    SQLRETURN return_code = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hConn);
    CHECK_FAIL(return_code);

    return_code = SQLSetConnectOption(hConn, SQL_ODBC_CURSORS, SQL_CUR_USE_ODBC);
    CHECK_FAIL_CONNECTION(return_code);

    return_code = SQLConnect(
        hConn,
        (SQLCHAR *)szDSN,                             /* String variable containing database name */
        SQL_NTS,                                      /* Database name is null-terminated string. */
        (SQLCHAR *)username,
        SQL_NTS,                                      /* User ID is null-terminated string. */
        (SQLCHAR *)password,
        SQL_NTS);                                     /* User password is null-terminated string. */


    CHECK_FAIL_CONNECTION(return_code);
    return 0;
}

int CSQLManager::Connect(const char *conn_string) {
    CHECK_ENVIROMENT;

    CHECK_ENVIROMENT;

    SQLRETURN return_code = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hConn);
    CHECK_FAIL(return_code);

    return_code = SQLSetConnectOption(hConn, SQL_ODBC_CURSORS, SQL_CUR_USE_DRIVER);
    //SQLSetConnectAttr(hConn,SQL_ATTR_ODBC_CURSORS, (SQLPOINTER)SQL_CUR_USE_IF_NEEDED, SQL_IS_INTEGER);
    CHECK_FAIL_CONNECTION(return_code);

    char OutConnectionString[1024];
    return_code = SQLDriverConnect(hConn,
                                   0,
                                   (SQLCHAR *)conn_string,
                                   SQL_NTS,
                                   (SQLCHAR *)OutConnectionString,
                                   1024,
                                   0,
                                   SQL_DRIVER_COMPLETE);

    //AnsiString error=CSQLManager::GetError(hEnv,hConn,0);
    CHECK_FAIL_CONNECTION(return_code);
    return 0;
}

int CSQLManager::Disconnect() {
    if (hConn) {
        SQLDisconnect(hConn);
        SQLFreeHandle(SQL_HANDLE_DBC, hConn);
        hConn = 0;
    }
    return 0;
}

CSQLManager::~CSQLManager() {
    Disconnect();
}

CSQLManager::CSQLManager(SQLHANDLE hEnv) {
    SetEnviroment(hEnv);
    hConn = 0;
}

AnsiString CSQLManager::GetDriver() {
    char  szDriverName[MAX_PATH] = "";
    short iLen = MAX_PATH;

    RETCODE rc;

    rc = SQLGetInfo(hConn, SQL_DRIVER_NAME, szDriverName, MAX_PATH, &iLen);
    if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
        return AnsiString(szDriverName);

    return AnsiString("unknown");
}

AnsiString CSQLManager::GetFirstDSN(SQLHANDLE hEnv) {
    CHECK_ENVIROMENT;

    RETCODE rc;
    short   cbData, cbData1;
    char    szDesc[1024];
    char    pszDSN[1024];
    int     lMax = 1024;
    pszDSN[0] = 0;

    rc = SQLDataSources(hEnv, SQL_FETCH_FIRST, (unsigned char *)pszDSN, lMax, &cbData,
                        (unsigned char *)szDesc, 1024, &cbData1);

    if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
        return AnsiString(pszDSN);

    return empty_string;
}

AnsiString CSQLManager::GetNextDSN(SQLHANDLE hEnv) {
    CHECK_ENVIROMENT;

    RETCODE     rc;
    short       cbData, cbData1;
    static char szDesc[1024];
    char        pszDSN[1024];
    int         lMax = 1024;
    pszDSN[0] = 0;

    rc = SQLDataSources(hEnv, SQL_FETCH_NEXT, (unsigned char *)pszDSN, lMax, &cbData,
                        (unsigned char *)szDesc, 1024, &cbData1);

    if (rc == SQL_NO_DATA_FOUND) {
        return empty_string;
    }

    if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
        return AnsiString(pszDSN);

    return empty_string;
}

int CSQLManager::SetTransactionModel(char auto_commit) {
    CHECK_ENVIROMENT;
    CHECK_CONNECTION;

    SQLRETURN rc;

    if (auto_commit)
        rc = SQLSetConnectOption(hConn, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
    else
        rc = SQLSetConnectOption(hConn, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);

    CHECK_FAIL(rc);
    return 0;
}

int CSQLManager::Commit() {
    CHECK_CONNECTION;
    SQLRETURN rc = SQLEndTran(SQL_HANDLE_DBC, hConn, SQL_COMMIT);
    CHECK_FAIL(rc);
    return 0;
}

int CSQLManager::Rollback() {
    CHECK_CONNECTION;
    SQLRETURN rc = SQLEndTran(SQL_HANDLE_DBC, hConn, SQL_ROLLBACK);
    CHECK_FAIL(rc);
    return 0;
}

int CSQLManager::FreeStatament(CSQLStatement *stmt) {
    stmt->ResetStmt();
    return Stataments.DeleteByData(stmt);
}

CSQLStatement *CSQLManager::CreateStatement() {
    CSQLStatement *stmt;

    stmt = new CSQLStatement(this);
    Stataments.Add(stmt, DATA_STATAMENT, 1);
    return stmt;
}

AnsiString CSQLManager::GetError() {
    return CSQLManager::GetError(hEnv, hConn, 0);
}

AnsiString CSQLManager::GetError(SQLHANDLE hEnv, SQLHANDLE hConn, SQLHANDLE hStmt) {
    unsigned char szSQLSTATE[10];
    SQLINTEGER    nErr;
    unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
    SWORD         cbmsg;
    AnsiString    error_msg;

    while (SQLError(hEnv, hConn, hStmt, szSQLSTATE, &nErr, msg, sizeof(msg), &cbmsg) == SQL_SUCCESS) {
        error_msg += (char *)"SQLSTATE=";
        error_msg += (char *)szSQLSTATE;
        error_msg += (char *)", Error=";
        error_msg += AnsiString(nErr);
        error_msg += (char *)", Text=";
        error_msg += (char *)msg;
        error_msg += (char *)"\n";
    }

    return error_msg;
}
