#include "SQLStatament.h"
#include <string.h>
#include <stdint.h>

#define MIN(a, b)    a < b ? a : b

CSQLStatement::CSQLStatement(CSQLManager *_Owner) {
    Owner      = _Owner;
    CursorName = AnsiString("Cursor") + AnsiString((long)(intptr_t) this);

    DATA      = 0;
    DataCount = 0;
    lastrow   = 0;

    hStmt = SQL_NULL_HSTMT;

    SQLAllocStmt(Owner->hConn, &hStmt);

    SQLSetStmtOption(hStmt, SQL_CONCURRENCY, SQL_CONCUR_VALUES);
    SQLSetStmtOption(hStmt, SQL_ASYNC_ENABLE, SQL_ASYNC_ENABLE_OFF);
    SQLSetStmtOption(hStmt, SQL_CURSOR_TYPE, SQL_CURSOR_KEYSET_DRIVEN);
    SQLSetStmtOption(hStmt, SQL_ROWSET_SIZE, 1);

    SQLSetStmtAttr(hStmt, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_IS_UINTEGER);


    SQLSetCursorName(hStmt, (SQLCHAR *)CursorName.c_str(), SQL_NTS);

#ifdef NO_BOUNDING
    bound_index = 0;
    list_cmd    = 0;
#endif
}

SQLLEN CSQLStatement::GetRowCount() {
    SQLLEN    iRetVal = -1;
    SQLRETURN rc      = SQLRowCount(hStmt, &iRetVal);

    CHECK_FAIL_STMT(rc);
    return iRetVal;
}

int CSQLStatement::GetCurrentRow() {
    int       lRetVal;
    SQLRETURN rc = SQLGetStmtOption(hStmt, SQL_ROW_NUMBER, (unsigned int *)&lRetVal);

    CHECK_FAIL_STMT(rc);
    return lRetVal;
}

int CSQLStatement::QueryPrepared(const char *szQuery) {
    SQLRETURN   rc;
    SQLSMALLINT iParamsCount = 0;

    lastrow = 0;

#ifdef NO_BOUNDING
    list_cmd = 0;
#endif

    rc = SQLPrepare(hStmt, (SQLCHAR *)szQuery, SQL_NTS);
    CHECK_FAIL_STMT(rc);

    rc = SQLNumParams(hStmt, &iParamsCount);
    CHECK_FAIL_STMT(rc);

    // let the driver decide
    if (iParamsCount > Parameters.Count())
        iParamsCount = Parameters.Count();

    if (iParamsCount > 0) {
        for (int i = 0; i < iParamsCount; i++) {
            SQLSMALLINT iScale;
            SQLULEN     iSqlSize;
            SQLSMALLINT iSqlType;
            SQLSMALLINT iNullable;

            rc = SQLDescribeParam(hStmt, i + 1,
                                  &iSqlType,
                                  &iSqlSize,
                                  &iScale,
                                  &iNullable);
            CHECK_FAIL_STMT(rc);

            char      is_blob = CHECK_LOB(iSqlType);
            Parameter *param  = (Parameter *)Parameters[i];
            param->is_blob = is_blob;
            if (/*is_blob*/ 0) {
                param->Indicators = SQL_DATA_AT_EXEC;
                rc = SQLBindParameter(hStmt,
                                      i + 1,
                                      SQL_PARAM_INPUT,
                                      SQL_C_CHAR,
                                      iSqlType,
                                      iSqlSize,
                                      iScale,
                                      (SQLCHAR *)param,
                                      0,
                                      &param->Indicators);
            } else {
                rc = SQLBindParameter(hStmt,
                                      i + 1,
                                      SQL_PARAM_INPUT,
                                      is_blob ? SQL_C_BINARY : SQL_C_CHAR,
                                      iSqlType,
                                      iSqlSize,
                                      iScale,
                                      (SQLPOINTER)param->DATA.c_str(),
                                      param->DATA.Length(),
                                      &param->Indicators);
            }
            CHECK_FAIL_STMT(rc);
        }
    }

    rc = SQLExecute(hStmt);

    while (rc == SQL_NEED_DATA) {
        Parameter *pToken = 0;
        rc = SQLParamData(hStmt, (SQLPOINTER *)&pToken);
        if (rc == SQL_NEED_DATA) {
            SQLRETURN rc2 = SQLPutData(hStmt, (SQLPOINTER)pToken->DATA.c_str(), pToken->DATA.Length());
            CHECK_FAIL_STMT(rc2);
        }
    }

    CHECK_FAIL_STMT(rc);
    return 0;
}

int CSQLStatement::Query(const char *szQuery) {
    lastrow = 0;
#ifdef NO_BOUNDING
    list_cmd = 0;
#endif
    SQLRETURN rc = SQLExecDirect(hStmt, (unsigned char *)szQuery, SQL_NTS);
    CHECK_FAIL_STMT(rc);
    return 0;
}

int CSQLStatement::ListTables(AnsiString szCatalog, AnsiString szSchema, AnsiString szName, AnsiString szType) {
    SQLRETURN rc = SQLTables(hStmt,
                             szCatalog.Length() ? (SQLCHAR *)szCatalog.c_str() : NULL,
                             szCatalog.Length(),
                             szSchema.Length() ? (SQLCHAR *)szSchema.c_str() : NULL,
                             szSchema.Length(),
                             szName.Length() ? (SQLCHAR *)szName.c_str() : NULL,
                             szName.Length(),
                             szType.Length() ? (SQLCHAR *)szType.c_str() : NULL,
                             szType.Length()
                             );

    CHECK_FAIL_STMT(rc);
#ifdef NO_BOUNDING
    list_cmd = 1;
#endif
    return 0;
}

int CSQLStatement::ListTypes() {
    SQLRETURN rc = SQLGetTypeInfo(hStmt, SQL_ALL_TYPES);

    CHECK_FAIL_STMT(rc);
#ifdef NO_BOUNDING
    list_cmd = 1;
#endif
    return 0;
}

int CSQLStatement::ListColumns(AnsiString szCatalog, AnsiString szSchema, AnsiString szName, AnsiString szType) {
    SQLRETURN rc = SQLColumns(hStmt,
                              szCatalog.Length() ? (SQLCHAR *)szCatalog.c_str() : NULL,
                              szCatalog.Length(),
                              szSchema.Length() ? (SQLCHAR *)szSchema.c_str() : NULL,
                              szSchema.Length(),
                              szName.Length() ? (SQLCHAR *)szName.c_str() : NULL,
                              szName.Length(),
                              szType.Length() ? (SQLCHAR *)szType.c_str() : NULL,
                              szType.Length()
                              );

    CHECK_FAIL_STMT(rc);
#ifdef NO_BOUNDING
    list_cmd = 1;
#endif
    return 0;
}

int CSQLStatement::ClearParameters() {
    Parameters.Clear();
    return 0;
}

int CSQLStatement::AddParameter(char *param, int size) {
    Parameter *nparam = new Parameter();

    nparam->DATA.LoadBuffer(param, size);
    nparam->Indicators = size ? size : -1;
    nparam->is_blob    = 0;
    Parameters.Add(nparam, DATA_PARAMETER);
    return 0;
}

int CSQLStatement::Free() {
    return Owner->FreeStatament(this);
}

int CSQLStatement::Describe(int index, AnsiString *Name, SQLSMALLINT *type, SQLULEN *size, SQLSMALLINT *scale, SQLSMALLINT *nullable) {
    char        name[MAX_PATH];
    SQLSMALLINT iLen;
    SQLRETURN   rc = SQLDescribeCol(hStmt,
                                    index,
                                    (unsigned char *)name, MAX_PATH, &iLen,
                                    type,
                                    size,
                                    scale,
                                    nullable);

    CHECK_FAIL_STMT(rc);
    name[iLen] = 0;
    *Name      = name;
    return 0;
}

int CSQLStatement::CountColumns() {
    SQLSMALLINT nColCount = 0;
    RETCODE     rc        = SQLNumResultCols(hStmt, &nColCount);

    CHECK_FAIL_STMT(rc);
    return nColCount;
}

int CSQLStatement::PrepareMove() {
    lastrow = 0;
    if ((DataCount) && (DATA)) {
        for (int i = 0; i < DataCount; i++) {
            int is_blob = CHECK_LOB(DATA[i].Type);
            DATA[i].Indicators = 0;
            DATA[i].Action     = DATA_BIND;
            if (is_blob) {
                delete DATA[i].Value;
                DATA[i].Size  = 0;
                DATA[i].Value = 0;
            }
        }
        return 0;
    }
    return -1;
}

int CSQLStatement::FreeResult() {
    if (DataCount) {
        if (DATA) {
            for (int i = 0; i < DataCount; i++) {
                if (DATA[i].Value)
                    delete[] DATA[i].Value;
            }
            delete[] DATA;
        }
        DataCount = 0;
    }
    //SQLFreeStmt(hStmt, SQL_RESET_PARAMS);
    //SQLFreeStmt(hStmt, SQL_UNBIND);
    return 0;
}

int CSQLStatement::BindResult() {
    int is_blob = 0;

    FreeResult();

    SQLSMALLINT nColCount = 0;
    RETCODE     rc        = SQLNumResultCols(hStmt, &nColCount);
    CHECK_FAIL_STMT(rc);


    if (nColCount < 1)
        return 1;

    DATA = new DataRecord[nColCount];
    if (!DATA)
        return 2;
    DataCount = nColCount;
#ifdef NO_BOUNDING
    bound_index = 0;
#endif

    for (int i = 0; i < nColCount; i++) {
        SQLSMALLINT iLen;
        SQLSMALLINT iNullable;

        DATA[i].Action     = DATA_BIND;
        DATA[i].Indicators = 0;

        rc = SQLDescribeCol(hStmt,
                            i + 1,
                            (unsigned char *)DATA[i].Name, MAX_PATH, &iLen,
                            &DATA[i].Type,
                            &DATA[i].Size,
                            &DATA[i].Scale,
                            &iNullable);


        SQLSMALLINT len;
        rc = SQLColAttribute(hStmt,
                             i + 1,
                             SQL_COLUMN_TABLE_NAME,
                             DATA[i].Table,
                             MAX_PATH,
                             &len,
                             0);

        CHECK_FAIL_STMT(rc);

        if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS)) {
            FreeResult();
            return 3;
        }

        is_blob = CHECK_LOB(DATA[i].Type);
        if (is_blob) {
            DATA[i].Size = 0;
        }

        // un caracter rezervat pt terminator
        DATA[i].Value = is_blob ? 0 : new char[++DATA[i].Size];

        if (is_blob) {
            //rc = SQLBindCol(hStmt, i + 1, SQL_C_CHAR, 0, 0, &DATA[i].Indicators);
            //GetError();
            rc = SQL_SUCCESS;
        } else {
            // daca nu am nici macar una "bounduita" ...
#ifdef NO_BOUNDING
            if ((!bound_index) || (list_cmd)) {
                rc          = SQLBindCol(hStmt, i + 1, SQL_C_CHAR, DATA[i].Value, DATA[i].Size, &DATA[i].Indicators);
                bound_index = i + 1;
            }
#else
            switch (DATA[i].Type) {
                case SQL_TYPE_TIMESTAMP:
                    // smalldatetime
                    if (DATA[i].Size == 17) {
                        rc = SQLBindCol(hStmt, i + 1, SQL_C_TIMESTAMP, DATA[i].Value, DATA[i].Size, &DATA[i].Indicators);
                        // atentie la break ! (e pus corect in if, pt ca daca nu
                        // are acea dimensiune, inseamna ca nu este smalldatetime)
                        break;
                    }

                default:
                    rc = SQLBindCol(hStmt, i + 1, SQL_C_CHAR, DATA[i].Value, DATA[i].Size, &DATA[i].Indicators);
            }
#endif
            DATA[i].Value[0] = 0;
            DATA[i].Value[DATA[i].Size - 1] = 0;
        }
        if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
            FreeResult();
            CHECK_FAIL_STMT(rc);
            return 4;
        }
    }
    return 0;
}

int CSQLStatement::FetchForward() {
    if (PrepareMove())
        return -2;

    SQLRETURN rc = SQLFetch(hStmt);

    CHECK_FAIL_STMT(rc);
    lastrow++;
    return 0;
}

int CSQLStatement::Fetch(short type, long parameter) {
    if (PrepareMove())
        return -2;
    SQLULEN      lRow    = 0;
    SQLUSMALLINT lStatus = 0;

    /*for (int i=0;i<DataCount;i++) {
        if (DATA[i].Value)
            DATA[i].Value[0]=0;
       }*/
    SQLRETURN rc = 0;


    rc      = SQLExtendedFetch(hStmt, type, parameter, &lRow, &lStatus);
    lastrow = lRow;


    //SQLRETURN rc=SQLFetch(hStmt);
    //SQLRETURN rc;
    //rc=SQLFetchScroll(hStmt, type, parameter);

    CHECK_FAIL_STMT(rc);
    return 0;
}

AnsiString bulk;
int CSQLStatement::GetColumnByIndex(long index, char **data, int *size) {
    *data = 0;
    *size = 0;
    if ((!DataCount) || (!DATA) || (index >= DataCount) || (index < 0)) {
        return -1;
    }

    char has_lob_data = 0;
#ifdef NO_BOUNDING
    if ((!CHECK_LOB(DATA[index].Type)) && (DATA[index].Value) && (index + 1 != bound_index) && (!list_cmd)) {
        if (DATA[index].Size < 0xFFFF) {
            SQLRETURN rc = SQLGetData(hStmt, index + 1, SQL_C_CHAR, DATA[index].Value, DATA[index].Size, &DATA[index].Indicators);
            CHECK_FAIL_STMT(rc);
            if (DATA[index].Indicators == SQL_NULL_DATA) {
                *data = 0;
                *size = 0;
                return 0;
            }

            if (DATA[index].Indicators > 0) {
                *data = DATA[index].Value;
                *size = DATA[index].Indicators + 1;
                return 0;
            }
        }
        return -6;
    } else
#endif
    if ((CHECK_LOB(DATA[index].Type)) && (!DATA[index].Value)) {
        char empty_char[1];

        SQLRETURN rc = SQLGetData(hStmt, index + 1, SQL_C_BINARY, empty_char, 0L, &DATA[index].Indicators);

        CHECK_FAIL_STMT(rc);
        if (rc == SQL_SUCCESS_WITH_INFO) {
            GetError();
        }

        if (DATA[index].Indicators == -1) {
            *data = 0;
            *size = 0;
            return 0;
        }
        SQLLEN Indicators = DATA[index].Indicators;

        // in indicators am size-ul
        //=====================================//
        if (DATA[index].Value)
            delete[] DATA[index].Value;
        //=====================================//

        DATA[index].Value = new char[Indicators + 1];

        if (!DATA[index].Value)
            return -1;
        DATA[index].Value[Indicators] = 0;

        DATA[index].Size     = Indicators + 1;
        DATA[index].Value[0] = 0;

        rc = SQLGetData(hStmt, index + 1, /*SQL_C_CHAR*/ SQL_C_BINARY, DATA[index].Value, DATA[index].Size, &Indicators);
        CHECK_FAIL_STMT(rc);
        //===============================//
        DATA[index].Size = Indicators;
        has_lob_data     = 1;
        //===============================//
    }

    if (DATA[index].Indicators == SQL_NULL_DATA) {
        *data = 0;
        *size = 0;
        return 0;
    }

    if ((DATA[index].Type == SQL_TYPE_TIMESTAMP) && (DATA[index].Value) && (DATA[index].Size == 17)) {
        TIMESTAMP_STRUCT *ts = (TIMESTAMP_STRUCT *)DATA[index].Value;
        bulk  = AnsiString((long)ts->year);
        bulk += "-";
        if (ts->month < 10)
            bulk += "0";
        bulk += AnsiString((long)ts->month);
        bulk += "-";
        if (ts->day < 10)
            bulk += "0";
        bulk += AnsiString((long)ts->day);
        bulk += " ";
        if (ts->hour < 10)
            bulk += "0";
        bulk += AnsiString((long)ts->hour);
        bulk += ":";
        if (ts->minute < 10)
            bulk += "0";
        bulk += AnsiString((long)ts->minute);
        bulk += ":";
        if (ts->second < 10)
            bulk += "0";
        bulk += AnsiString((long)ts->second);
        bulk += ".";
        if (ts->fraction < 100)
            bulk += "0";
        if (ts->fraction < 10)
            bulk += "0";
        if (ts->fraction)
            bulk += AnsiString((long)ts->fraction);
        else
            bulk += "0";
        bulk += " ";
        *data = (char *)bulk.c_str();
        *size = bulk.Length();

        return 0;
    }

    *data = DATA[index].Value;
    if ((DATA[index].Indicators >= 0) && (DATA[index].Indicators < DATA[index].Size))
        *size = has_lob_data ? DATA[index].Size : DATA[index].Indicators;//Size;
    else
        *size = DATA[index].Size;
    if (*size <= 0) {
        size  = 0;
        *data = 0;//"";
    }
    return 0;
}

int CSQLStatement::SetColumnByIndex(long index, char *data, int size) {
    if ((!DataCount) || (!DATA) || (index >= DataCount) || (index < 0))
        return -1;

    if (CHECK_LOB(DATA[index].Type)) {
        if (DATA[index].Value)
            delete[] DATA[index].Value;
        DATA[index].Size        = size + 1;
        DATA[index].Value       = new char[size + 1];
        DATA[index].Value[size] = 0;
        memcpy(DATA[index].Value, data, size);
    }
    DATA[index].Action = DATA_UPDATE;

    memset(DATA[index].Value, ' ', DATA[index].Size - 1);
    memcpy(DATA[index].Value, data, MIN((unsigned long)size, DATA[index].Size));

    return 0;

    /*AnsiString szUpdateQuery("UPDATE ");
       szUpdateQuery+=DATA[index].Table;
       szUpdateQuery+=(char *)" SET ";
       szUpdateQuery+=DATA[index].Name;
       szUpdateQuery+=(char *)" = ? WHERE CURRENT OF ";
       szUpdateQuery+=CursorName;

       CSQLStatement *temp=Owner->CreateStatement();
       temp->AddParameter(data,size);
       int result=temp->QueryPrepared(szUpdateQuery);
       temp->ClearParameters();
       temp->Free();

       // re-fetch current rowset !
       Fetch(SQL_FETCH_RELATIVE,0);

       return result;
     */
}

int CSQLStatement::Delete() {
    int result = 0;

    do {
        AnsiString    table;
        CSQLStatement *temp = Owner->CreateStatement();

        char not_first = 0;

        AnsiString sztemp = (char *)"";
        for (int i = 0; i < DataCount; i++) {
            if (DATA[i].Action != DATA_DELETE) {
                if (not_first) {
                    if (table != DATA[i].Table)
                        continue;
                    sztemp += (char *)", ";
                } else {
                    table     = DATA[i].Table;
                    not_first = 1;
                }
                DATA[i].Action = DATA_DELETE;
            }
        }
        if (!not_first)
            break;

        AnsiString szUpdateQuery("DELETE FROM ");
        szUpdateQuery += table;
        szUpdateQuery += " WHERE CURRENT OF ";
        szUpdateQuery += CursorName;

        result = temp->QueryPrepared(szUpdateQuery);
        temp->ClearParameters();
        temp->Free();
    } while (!result);


    // re-fetch current rowset !
    Fetch(SQL_FETCH_RELATIVE, 0);

    return result;
}

int CSQLStatement::Update() {
    int result = 0;

    do {
        AnsiString    table;
        CSQLStatement *temp = Owner->CreateStatement();

        char not_first = 0;

        AnsiString sztemp = (char *)"";
        for (int i = 0; i < DataCount; i++) {
            if (DATA[i].Action == DATA_UPDATE) {
                if (not_first) {
                    if (table != DATA[i].Table)
                        continue;
                    sztemp += (char *)", ";
                } else {
                    table     = DATA[i].Table;
                    not_first = 1;
                }
                sztemp += DATA[i].Name;
                sztemp += (char *)" = ? ";

                temp->AddParameter(DATA[i].Value, DATA[i].Size);
                DATA[i].Action = DATA_BIND;
            }
        }
        if (!not_first)
            break;

        AnsiString szUpdateQuery("UPDATE ");
        szUpdateQuery += table;
        szUpdateQuery += (char *)" SET ";
        szUpdateQuery += sztemp;
        szUpdateQuery += (char *)"WHERE CURRENT OF ";
        szUpdateQuery += CursorName;

        result = temp->QueryPrepared(szUpdateQuery);
        temp->ClearParameters();
        temp->Free();
    } while (!result);


    // re-fetch current rowset !
    Fetch(SQL_FETCH_RELATIVE, 0);

    return result;
}

int CSQLStatement::Lock(int lock) {
    if (!lastrow)
        return -1;
    SQLRETURN rc;
    rc = SQL_LOCK_RECORD(hStmt, lastrow, lock);
    //AnsiString error=CSQLManager::GetError(Owner->hEnv, Owner->hConn, hStmt);
    CHECK_FAIL_STMT(rc);
    return 0;
}

AnsiString CSQLStatement::GetError() {
    return CSQLManager::GetError(Owner->hEnv, Owner->hConn, hStmt);
}

int CSQLStatement::ResetStmt() {
    if (hStmt) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = 0;
    }
    return 0;
}

CSQLStatement::~CSQLStatement() {
    FreeResult();

    /*ClearParameters();
       if (hStmt)
        SQLFreeStmt(hStmt, SQL_CLOSE);*/
    //SQLFreeStmt(hStmt, SQL_CLOSE);

    /*if (hStmt) {
        std::cout << "Delete handle: "<< hStmt <<"...";
        //SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        std::cout << "done\n";
        hStmt=0;
       }*/
}
