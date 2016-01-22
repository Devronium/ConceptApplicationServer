#ifndef __ODBCMAN_H
#define __ODBCMAN_H

#ifdef _WIN32
 #include <windows.h>
#else
// to do
#endif

#include <sql.h>
#include <sqlext.h>
#include "AnsiString.h"
#include "AnsiList.h"

#define STATIC_CHECK_FAIL(result)        if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) { LAST_ERROR += GetError(_hEnv, 0, 0); return -1; }
#define STATIC_CHECK_FAIL2(result)       if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) { return -1; }

#define CHECK_FAIL(result)               if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) { LAST_ERROR += GetError(); return -1; } else if (result == SQL_SUCCESS_WITH_INFO) { LAST_WARNING += GetError(); }
#define CHECK_FAIL_CONNECTION(result)    if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) { LAST_ERROR += GetError(); SQLFreeHandle(SQL_HANDLE_DBC, hConn); hConn = 0; return -1; } else if (result == SQL_SUCCESS_WITH_INFO) { LAST_WARNING += GetError(); }

#define CHECK_FAIL_STMT(result)          if ((result != SQL_SUCCESS) && (result != SQL_SUCCESS_WITH_INFO)) { Owner->LAST_ERROR += GetError(); return -1; } else if (result == SQL_SUCCESS_WITH_INFO) { Owner->LAST_WARNING += GetError(); }

#define CHECK_ENVIROMENT    if (!hEnv) return -1;
#define CHECK_CONNECTION    if (!hConn) return -2;


class CSQLStatement;

class CSQLManager {
    friend class CSQLStatement;
private:
    SQLHANDLE hEnv;
    SQLHANDLE hConn;
    AnsiList  Stataments;
public:
    AnsiString LAST_ERROR;
    AnsiString LAST_WARNING;

    int DestroyEnviroment(SQLHANDLE _hEnv);
    static int CreateEnviroment(SQLHANDLE *_hEnv);

    int SetEnviroment(SQLHANDLE _hEnv = 0);

    int Connect(const char *szDSN, const char *username, const char *password);
    int Connect(const char *conn_string);

    int Disconnect();

    int SetTransactionModel(char auto_commit);

    int Commit();
    int Rollback();

    CSQLStatement *CreateStatement();
    int FreeStatament(CSQLStatement *);

    AnsiString GetDriver();
    static AnsiString GetFirstDSN(SQLHANDLE hEnv);
    static AnsiString GetNextDSN(SQLHANDLE hEnv);
    static AnsiString GetError(SQLHANDLE hEnv, SQLHANDLE hConn, SQLHANDLE hStmt);

    AnsiString GetError();

    CSQLManager(SQLHANDLE hEnv = 0);
    ~CSQLManager();
};
#endif // __ODBCMAN_H
