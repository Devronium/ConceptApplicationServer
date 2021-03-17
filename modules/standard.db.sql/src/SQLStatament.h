#ifndef __SQLSTATAMENT_H
#define __SQLSTATAMENT_H

//#define NO_BOUNDING

#include "odbcman.h"

#ifndef SQL_TYPE_ORACLE_BLOB
 #define SQL_TYPE_ORACLE_BLOB    -402
#endif
#ifndef SQL_TYPE_ORACLE_CLOB
 #define SQL_TYPE_ORACLE_CLOB    -401
#endif

#define CHECK_LOB(type)                \
    ((type == SQL_LONGVARBINARY) ||    \
     (type == SQL_LONGVARBINARY) ||    \
     (type == SQL_LONGVARCHAR) ||      \
     (type == SQL_TYPE_ORACLE_BLOB) || \
     (type == SQL_TYPE_ORACLE_CLOB))   \
    ?                                  \
    1                                  \
    :                                  \
    0


#define MAX_PATH       260

#define DATA_BIND      0x00
#define DATA_UPDATE    0x01
#define DATA_DELETE    0x02
// data structure ...
typedef struct _DataRecord {
    char        Name[MAX_PATH];
    char        Table[MAX_PATH];
    char        *Value;
    char        Action;
    SQLULEN     Size;
    SQLSMALLINT Type;
    SQLSMALLINT Scale;
    SQLLEN      Indicators;
} DataRecord;

typedef struct _Parameter {
    AnsiString DATA;
    char       is_blob;
    SQLLEN     Indicators;
} Parameter;

class CSQLStatement
{
private:
    SQLHANDLE   hStmt;
    CSQLManager *Owner;
    int         index;

    long lastrow;

    DataRecord *DATA;
    int        DataCount;

    AnsiString CursorName;

    AnsiList Parameters;

    SQLUSMALLINT RowStatusArray[10];

#ifdef NO_BOUNDING
    short int bound_index;
    bool      list_cmd;
#endif

    int PrepareMove();

public:
    int Free();

    int AddParameter(char *szValue);

    int ListTables(AnsiString szCatalog, AnsiString szSchema, AnsiString szName, AnsiString szType);
    int ListColumns(AnsiString szCatalog, AnsiString szSchema, AnsiString szName, AnsiString szType);
    int ListTypes();

    int AddParameter(char *param, int size);
    int ClearParameters();

    int QueryPrepared(const char *szQuery);

    int Query(const char *szQuery);
    int BindResult();
    int Fetch(short type, long parameter = 1);
    int FetchForward();
    int FreeResult();

    SQLLEN GetRowCount();
    int GetCurrentRow();

    int GetColumnByIndex(long index, char **data, int *size);
    int SetColumnByIndex(long index, char *data, int size);

    int Update();
    int Delete();
    int Lock(int lock);

    int Describe(int index, AnsiString *Name, SQLSMALLINT *type, SQLULEN *size, SQLSMALLINT *scale, SQLSMALLINT *nullable);
    int CountColumns();

    AnsiString GetError();

    int ResetStmt();

    CSQLStatement(CSQLManager *_Owner);
    ~CSQLStatement();
};
#endif // __SQLSTATAMENT_H
