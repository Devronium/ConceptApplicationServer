//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <stdio.h>
extern "C" {
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
}

#define BOOLOID			16
#define BYTEAOID		17
#define CHAROID			18
#define NAMEOID			19
#define INT8OID			20
#define INT2OID			21
#define INT2VECTOROID	22
#define INT4OID			23
#define REGPROCOID		24
#define TEXTOID			25
#define OIDOID			26
#define TIDOID		27
#define XIDOID 28
#define CIDOID 29
#define OIDVECTOROID	30
#define XMLOID 142
#define PGNODETREEOID	194
#define POINTOID		600
#define LSEGOID			601
#define PATHOID			602
#define BOXOID			603
#define POLYGONOID		604
#define LINEOID			628
#define FLOAT4OID 700
#define FLOAT8OID 701
#define ABSTIMEOID		702
#define RELTIMEOID		703
#define TINTERVALOID	704
#define UNKNOWNOID		705
#define CIRCLEOID		718
#define CASHOID 790
#define MACADDROID 829
#define INETOID 869
#define CIDROID 650
#define INT4ARRAYOID		1007
#define TEXTARRAYOID		1009
#define FLOAT4ARRAYOID 1021
#define ACLITEMOID		1033
#define CSTRINGARRAYOID		1263
#define BPCHAROID		1042
#define VARCHAROID		1043
#define DATEOID			1082
#define TIMEOID			1083
#define TIMESTAMPOID	1114
#define TIMESTAMPTZOID	1184
#define INTERVALOID		1186
#define TIMETZOID		1266
#define BITOID	 1560
#define VARBITOID	  1562
#define NUMERICOID		1700
#define REFCURSOROID	1790
#define REGPROCEDUREOID 2202
#define REGOPEROID		2203
#define REGOPERATOROID	2204
#define REGCLASSOID		2205
#define REGTYPEOID		2206
#define REGTYPEARRAYOID 2211
#define TSVECTOROID		3614
#define GTSVECTOROID	3642
#define TSQUERYOID		3615
#define REGCONFIGOID	3734
#define REGDICTIONARYOID	3769
#define RECORDOID		2249
#define RECORDARRAYOID	2287
#define CSTRINGOID		2275
#define ANYOID			2276
#define ANYARRAYOID		2277
#define VOIDOID			2278
#define TRIGGEROID		2279
#define LANGUAGE_HANDLEROID		2280
#define INTERNALOID		2281
#define OPAQUEOID		2282
#define ANYELEMENTOID	2283
#define ANYNONARRAYOID	2776
#define ANYENUMOID		3500
#define FDW_HANDLEROID		3115

//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
#define PGSQL_SEEK_SET    SEEK_SET
#define PGSQL_SEEK_CUR    SEEK_CUR
#define PGSQL_SEEK_END    SEEK_END

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    // starting up windows sockets
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
    DEFINE_ECONSTANT(PGSQL_SEEK_SET)
    DEFINE_ECONSTANT(PGSQL_SEEK_END)
    DEFINE_ECONSTANT(PGSQL_SEEK_CUR)
    DEFINE_ECONSTANT(INV_READ)
    DEFINE_ECONSTANT(INV_WRITE)
    DEFINE_ECONSTANT(PG_COPYRES_ATTRS)
    DEFINE_ECONSTANT(PG_COPYRES_ATTRS)
    DEFINE_ECONSTANT(PG_COPYRES_TUPLES)
    DEFINE_ECONSTANT(PG_COPYRES_EVENTS)
    DEFINE_ECONSTANT(PG_COPYRES_NOTICEHOOKS)
    DEFINE_ECONSTANT(CONNECTION_OK)
    DEFINE_ECONSTANT(CONNECTION_BAD)
    DEFINE_ECONSTANT(CONNECTION_STARTED)
    DEFINE_ECONSTANT(CONNECTION_MADE)
    DEFINE_ECONSTANT(CONNECTION_AWAITING_RESPONSE)
    DEFINE_ECONSTANT(CONNECTION_AUTH_OK)
    DEFINE_ECONSTANT(CONNECTION_SETENV)
    DEFINE_ECONSTANT(CONNECTION_SSL_STARTUP)
    DEFINE_ECONSTANT(CONNECTION_NEEDED)
    DEFINE_ECONSTANT(PGRES_POLLING_FAILED)
    DEFINE_ECONSTANT(PGRES_POLLING_READING)
    DEFINE_ECONSTANT(PGRES_POLLING_WRITING)
    DEFINE_ECONSTANT(PGRES_POLLING_OK)
    DEFINE_ECONSTANT(PGRES_POLLING_ACTIVE)
    DEFINE_ECONSTANT(PGRES_EMPTY_QUERY)
    DEFINE_ECONSTANT(PGRES_COMMAND_OK)
    DEFINE_ECONSTANT(PGRES_TUPLES_OK)
    DEFINE_ECONSTANT(PGRES_COPY_OUT)
    DEFINE_ECONSTANT(PGRES_COPY_IN)
    DEFINE_ECONSTANT(PGRES_BAD_RESPONSE)
    DEFINE_ECONSTANT(PGRES_NONFATAL_ERROR)
    DEFINE_ECONSTANT(PGRES_FATAL_ERROR)
    DEFINE_ECONSTANT(PQTRANS_IDLE)
    DEFINE_ECONSTANT(PQTRANS_ACTIVE)
    DEFINE_ECONSTANT(PQTRANS_INTRANS)
    DEFINE_ECONSTANT(PQTRANS_INERROR)
    DEFINE_ECONSTANT(PQTRANS_UNKNOWN)
    DEFINE_ECONSTANT(PQERRORS_TERSE)
    DEFINE_ECONSTANT(PQERRORS_DEFAULT)
    DEFINE_ECONSTANT(PQERRORS_VERBOSE)

    DEFINE_ECONSTANT(BOOLOID)
    DEFINE_ECONSTANT(BYTEAOID)
    DEFINE_ECONSTANT(CHAROID)
    DEFINE_ECONSTANT(NAMEOID)
    DEFINE_ECONSTANT(INT8OID)
    DEFINE_ECONSTANT(INT2OID)
    DEFINE_ECONSTANT(INT2VECTOROID)
    DEFINE_ECONSTANT(INT4OID)
    DEFINE_ECONSTANT(REGPROCOID)
    DEFINE_ECONSTANT(TEXTOID)
    DEFINE_ECONSTANT(OIDOID)
    DEFINE_ECONSTANT(TIDOID)
    DEFINE_ECONSTANT(XIDOID)
    DEFINE_ECONSTANT(CIDOID)
    DEFINE_ECONSTANT(OIDVECTOROID)
    DEFINE_ECONSTANT(XMLOID)
    DEFINE_ECONSTANT(PGNODETREEOID)
    DEFINE_ECONSTANT(POINTOID)
    DEFINE_ECONSTANT(LSEGOID)
    DEFINE_ECONSTANT(PATHOID)
    DEFINE_ECONSTANT(BOXOID)
    DEFINE_ECONSTANT(POLYGONOID)
    DEFINE_ECONSTANT(LINEOID)
    DEFINE_ECONSTANT(FLOAT4OID)
    DEFINE_ECONSTANT(FLOAT8OID)
    DEFINE_ECONSTANT(ABSTIMEOID)
    DEFINE_ECONSTANT(RELTIMEOID)
    DEFINE_ECONSTANT(TINTERVALOID)
    DEFINE_ECONSTANT(UNKNOWNOID)
    DEFINE_ECONSTANT(CIRCLEOID)
    DEFINE_ECONSTANT(CASHOID)
    DEFINE_ECONSTANT(MACADDROID)
    DEFINE_ECONSTANT(INETOID)
    DEFINE_ECONSTANT(CIDROID)
    DEFINE_ECONSTANT(INT4ARRAYOID)
    DEFINE_ECONSTANT(TEXTARRAYOID)
    DEFINE_ECONSTANT(FLOAT4ARRAYOID)
    DEFINE_ECONSTANT(ACLITEMOID)
    DEFINE_ECONSTANT(CSTRINGARRAYOID)
    DEFINE_ECONSTANT(BPCHAROID)
    DEFINE_ECONSTANT(VARCHAROID)
    DEFINE_ECONSTANT(DATEOID)
    DEFINE_ECONSTANT(TIMEOID)
    DEFINE_ECONSTANT(TIMESTAMPOID)
    DEFINE_ECONSTANT(TIMESTAMPTZOID)
    DEFINE_ECONSTANT(INTERVALOID)
    DEFINE_ECONSTANT(TIMETZOID)
    DEFINE_ECONSTANT(BITOID)
    DEFINE_ECONSTANT(VARBITOID)
    DEFINE_ECONSTANT(NUMERICOID)
    DEFINE_ECONSTANT(REFCURSOROID)
    DEFINE_ECONSTANT(REGPROCEDUREOID)
    DEFINE_ECONSTANT(REGOPEROID)
    DEFINE_ECONSTANT(REGOPERATOROID)
    DEFINE_ECONSTANT(REGCLASSOID)
    DEFINE_ECONSTANT(REGTYPEOID)
    DEFINE_ECONSTANT(REGTYPEARRAYOID)
    DEFINE_ECONSTANT(TSVECTOROID)
    DEFINE_ECONSTANT(GTSVECTOROID)
    DEFINE_ECONSTANT(TSQUERYOID)
    DEFINE_ECONSTANT(REGCONFIGOID)
    DEFINE_ECONSTANT(REGDICTIONARYOID)
    DEFINE_ECONSTANT(RECORDOID)
    DEFINE_ECONSTANT(RECORDARRAYOID)
    DEFINE_ECONSTANT(CSTRINGOID)
    DEFINE_ECONSTANT(ANYOID)
    DEFINE_ECONSTANT(ANYARRAYOID)
    DEFINE_ECONSTANT(VOIDOID)
    DEFINE_ECONSTANT(TRIGGEROID)
    DEFINE_ECONSTANT(LANGUAGE_HANDLEROID)
    DEFINE_ECONSTANT(INTERNALOID)
    DEFINE_ECONSTANT(OPAQUEOID)
    DEFINE_ECONSTANT(ANYELEMENTOID)
    DEFINE_ECONSTANT(ANYNONARRAYOID)
    DEFINE_ECONSTANT(ANYENUMOID)
    DEFINE_ECONSTANT(FDW_HANDLEROID)
    /*DEFINE_ECONSTANT(TYPTYPE_BASE)
       DEFINE_ECONSTANT(TYPTYPE_COMPOSITE)
       DEFINE_ECONSTANT(TYPTYPE_DOMAIN)
       DEFINE_ECONSTANT(TYPTYPE_ENUM)
       DEFINE_ECONSTANT(TYPTYPE_PSEUDO)

       DEFINE_ECONSTANT(TYPCATEGORY_INVALID)
       DEFINE_ECONSTANT(TYPCATEGORY_ARRAY)
       DEFINE_ECONSTANT(TYPCATEGORY_BOOLEAN)
       DEFINE_ECONSTANT(TYPCATEGORY_COMPOSITE)
       DEFINE_ECONSTANT(TYPCATEGORY_DATETIME)
       DEFINE_ECONSTANT(TYPCATEGORY_ENUM)
       DEFINE_ECONSTANT(TYPCATEGORY_GEOMETRIC)
       DEFINE_ECONSTANT(TYPCATEGORY_NETWORK)
       DEFINE_ECONSTANT(TYPCATEGORY_NUMERIC)
       DEFINE_ECONSTANT(TYPCATEGORY_PSEUDOTYPE)
       DEFINE_ECONSTANT(TYPCATEGORY_STRING)
       DEFINE_ECONSTANT(TYPCATEGORY_TIMESPAN)
       DEFINE_ECONSTANT(TYPCATEGORY_USER)
       DEFINE_ECONSTANT(TYPCATEGORY_BITSTRING)
       DEFINE_ECONSTANT(TYPCATEGORY_UNKNOWN)*/

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
CONCEPT_FUNCTION_IMPL(PQConnect, 1)
    T_STRING(0)

    RETURN_NUMBER((SYS_INT)PQconnectdb(PARAM(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQLogin, 7)
    T_STRING(0) // host
    T_STRING(1) // port
    T_STRING(2) // options
    T_STRING(3) // tty
    T_STRING(4) // db
    T_STRING(5) // user
    T_STRING(6) // password

    char *host = PARAM_LEN(0) ? PARAM(0) : 0;
    char *port     = PARAM_LEN(1) ? PARAM(1) : 0;
    char *options  = PARAM_LEN(2) ? PARAM(2) : 0;
    char *tty      = PARAM_LEN(3) ? PARAM(3) : 0;
    char *flags    = PARAM_LEN(4) ? PARAM(4) : 0;
    char *user     = PARAM_LEN(5) ? PARAM(5) : 0;
    char *password = PARAM_LEN(6) ? PARAM(6) : 0;

    RETURN_NUMBER((SYS_INT)PQsetdbLogin(host, port, options, tty, flags, user, password))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQDisconnect, 1)
    T_NUMBER(0) // pq

    RETURN_NUMBER(0);

    if (PARAM_INT(0)) {
        PQfinish((PGconn *)PARAM_INT(0));
        SET_NUMBER(0, 0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQError, 1)
    T_NUMBER(0) // conn

    if (PARAM_INT(0)) {
        RETURN_STRING(PQerrorMessage((PGconn *)PARAM_INT(0)));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQQueryError, 1)
    T_NUMBER(0)

    if (PARAM_INT(0)) {
        RETURN_STRING(PQresultErrorMessage((PGresult *)PARAM_INT(0)));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQAffectedRows, 1)
    T_NUMBER(0)

    if (PARAM_INT(0)) {
        RETURN_STRING(PQcmdTuples((PGresult *)PARAM_INT(0)));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQQuery, 2)
    T_STRING(0) // query
    T_NUMBER(1) // con

    if (PARAM_INT(1)) {
        PGresult *result = PQexec((PGconn *)PARAM_INT(1), PARAM(0));

        RETURN_NUMBER((SYS_INT)result);
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQQueryParams, 3)
    T_STRING(0) // query
    T_ARRAY(1)  // params
    T_NUMBER(2) // con

    INTEGER type = 0;
    NUMBER nr        = 0;
    void   *newpData = 0;
    char   **ret     = 0;
    int    *lengths  = 0;

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    ret        = new char * [count + 1];
    lengths    = new int[count + 1];
    lengths[0] = 0;
    int *formats = new int[count + 1];
    ret[count] = 0;

    for (INTEGER i = 0; i < count; i++) {
        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            formats[i] = 0;
            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i]     = szData;
                lengths[i] = (int)nData;

                // check if printable. If not, set as binary
                int len = lengths[i];
                for (int k = 0; k < len; k++) {
                    if ((szData[k] < 32) || (szData[k] > 126) || (szData[k] == '\\')) {
                        formats[i] = 1;
                        break;
                    }
                }
            } else {
                ret[i]     = 0;
                lengths[i] = 0;
            }
            //formats[i]=1;
        }
    }

    if (PARAM_INT(1)) {
        PGresult *result = PQexecParams((PGconn *)PARAM_INT(2), PARAM(0), count, NULL, ret, lengths, formats, 0);
        RETURN_NUMBER((SYS_INT)result);
    } else {
        RETURN_NUMBER(0)
    }
    delete[] lengths;
    delete[] ret;
    delete[] formats;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQFreeResult, 1)
    T_NUMBER(0) // mysql result

    if (PARAM_INT(0)) {
        PQclear((PGresult *)PARAM_INT(0));
        SET_NUMBER(0, 0);
    }

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQCountColumns, 1)
    T_NUMBER(0) // mysql result

    if (PARAM_INT(0)) {
        RETURN_NUMBER(PQnfields((PGresult *)PARAM_INT(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQCountRows, 1)
    T_NUMBER(0) // mysql result

    if (PARAM_INT(0)) {
        RETURN_NUMBER(PQntuples((PGresult *)PARAM_INT(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQDescribeCol, 5)
    T_NUMBER(0) // index
    T_NUMBER(4) // mysql result

    if (PARAM_INT(4)) {
        int  res        = 0;
        long num_fields = PQnfields((PGresult *)PARAM_INT(4));
        int  index      = PARAM_INT(0);

        index--;
        if ((index < 0) || (index >= num_fields)) {
            RETURN_NUMBER(-1);
            return 0;
        }
        SET_STRING(1, PQfname((PGresult *)PARAM_INT(4), index));
        SET_NUMBER(2, PQftype((PGresult *)PARAM_INT(4), index));
        SET_NUMBER(3, PQfsize((PGresult *)PARAM_INT(4), index));

        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQIsNull, 3)
    T_NUMBER(0) // index
    T_NUMBER(1) // index
    T_NUMBER(2) // mysql result

    if (PARAM_INT(2)) {
        RETURN_NUMBER(PQgetisnull((PGresult *)PARAM_INT(2), PARAM_INT(0), PARAM_INT(1)))
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQLength, 3)
    T_NUMBER(0) // index
    T_NUMBER(1) // index
    T_NUMBER(2) // mysql result

    if (PARAM_INT(2)) {
        RETURN_NUMBER(PQgetlength((PGresult *)PARAM_INT(2), PARAM_INT(0), PARAM_INT(1)))
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQGetValue, 3)
    T_NUMBER(0) // index
    T_NUMBER(1) // index
    T_NUMBER(2) // mysql result

    if (PARAM_INT(2)) {
        int  len  = PQgetlength((PGresult *)PARAM_INT(2), PARAM_INT(0), PARAM_INT(1));
        int  type = PQftype((PGresult *)PARAM_INT(2), PARAM_INT(1));
        char *val = PQgetvalue((PGresult *)PARAM_INT(2), PARAM_INT(0), PARAM_INT(1));

        if (len > 0) {
            if (type == 17) { // bytea
                size_t to_len = 0;
                char   *data  = (char *)PQunescapeBytea((const unsigned char *)val, &to_len);
                if ((data) && (to_len > 0)) {
                    RETURN_BUFFER(data, to_len);
                } else {
                    RETURN_STRING("");
                }
                if (data)
                    PQfreemem(data);
            } else {
                RETURN_BUFFER(val, len)
            }
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQTable, 2)
    T_NUMBER(0)
    T_NUMBER(1) // mysql result

    if (PARAM_INT(1)) {
        RETURN_NUMBER(PQftable((PGresult *)PARAM_INT(1), PARAM_INT(0)))
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQTableCol, 2)
    T_NUMBER(0)
    T_NUMBER(1) // mysql result

    if (PARAM_INT(1)) {
        RETURN_NUMBER(PQftablecol((PGresult *)PARAM_INT(1), PARAM_INT(0)))
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQEscape, 2)
    T_STRING(0) // mysql res
    T_NUMBER(1) // mysql

    if (!PARAM_INT(1)) {
        RETURN_STRING("");
        return 0;
    }

    int len    = PARAM_LEN(0);
    int _error = 0;

    char *buffer = new char[len * 2 + 1];

    unsigned long len_new = PQescapeStringConn((PGconn *)PARAM_INT(1), buffer, PARAM(0), len, &_error);
    if (_error) {
        RETURN_STRING("");
    } else {
        RETURN_BUFFER(buffer, len_new);
    }
    delete[] buffer;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQEncryptPassword, 2)
    T_STRING(0)
    T_STRING(1)

    RETURN_STRING(PQencryptPassword(PARAM(0), PARAM(1)));
END_IMPL
//-----------------------------------------------------//
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_open, 3)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int

    RETURN_NUMBER((SYS_INT)lo_open((PGconn *)PARAM_INT(0), (unsigned int)PARAM(1), PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_close, 2)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int

    RETURN_NUMBER(lo_close((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_read, 4)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_STRING(2)     // char*
    T_NUMBER(3)     // size_t

    int len = PARAM_INT(3);
    char *buf = 0;
    CORE_NEW((len + 1), buf);

    buf[len] = 0;
    buf[0]   = 0;

    SET_STRING(2, "");
    int res = lo_read((PGconn *)PARAM_INT(0), (int)PARAM(1), buf, (size_t)len);
    if (res > 0) {
        //SET_BUFFER(2, buf, res);
        SetVariable(PARAMETER(2), -1, buf, res);
    } else
        delete[] buf;

    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_write, 3)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_STRING(2)     // char*

    RETURN_NUMBER(lo_write((PGconn *)PARAM_INT(0), (int)PARAM(1), (char *)PARAM(2), (size_t)PARAM_LEN(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_lseek, 4)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_NUMBER(2)     // int
    T_NUMBER(3)     // int

    RETURN_NUMBER(lo_lseek((PGconn *)PARAM_INT(0), (int)PARAM(1), (int)PARAM(2), (int)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_creat, 2)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int

    RETURN_NUMBER(lo_creat((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_create, 2)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int

    RETURN_NUMBER(lo_create((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_tell, 2)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int

    RETURN_NUMBER(lo_tell((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_truncate, 3)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_NUMBER(2)     // size_t

    RETURN_NUMBER(lo_truncate((PGconn *)PARAM_INT(0), (int)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_unlink, 2)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int

    RETURN_NUMBER(lo_unlink((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_import, 2)
    T_HANDLE(0)     // void*
    T_STRING(1)     // char*

    RETURN_NUMBER(lo_import((PGconn *)PARAM_INT(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_import_with_oid, 3)
    T_HANDLE(0)     // void*
    T_STRING(1)     // char*
    T_NUMBER(2)     // int

    RETURN_NUMBER(lo_import_with_oid((PGconn *)PARAM_INT(0), (char *)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(lo_export, 3)
    T_HANDLE(0)     // void*
    T_NUMBER(1)     // int
    T_STRING(2)     // char*

    RETURN_NUMBER(lo_export((PGconn *)PARAM_INT(0), (int)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQsetnonblocking, 2)
    T_HANDLE(0) // void*
    T_NUMBER(1) // int
    RETURN_NUMBER(PQsetnonblocking((PGconn *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQisnonblocking, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQisnonblocking((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQgetResult, 1)
    T_HANDLE(0)     // void*
    PGresult * result = PQgetResult((PGconn *)PARAM_INT(0));
    RETURN_NUMBER((SYS_INT)result);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQconsumeInput, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQconsumeInput((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQisBusy, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQisBusy((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQflush, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQflush((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQsocket, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQsocket((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQrequestCancel, 1)
    T_HANDLE(0)     // void*
    RETURN_NUMBER(PQrequestCancel((PGconn *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PQQueryParamsAsync, 3)
    T_STRING(0) // query
    T_ARRAY(1)  // params
    T_NUMBER(2) // con

    INTEGER type = 0;
    NUMBER nr        = 0;
    void   *newpData = 0;
    char   **ret     = 0;
    int    *lengths  = 0;

    int count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    ret        = new char * [count + 1];
    lengths    = new int[count + 1];
    lengths[0] = 0;
    int *formats = new int[count + 1];
    ret[count] = 0;

    for (INTEGER i = 0; i < count; i++) {
        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            formats[i] = 0;
            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i]     = szData;
                lengths[i] = (int)nData;

                // check if printable. If not, set as binary
                int len = lengths[i];
                for (int k = 0; k < len; k++) {
                    if ((szData[k] < 32) || (szData[k] > 126) || (szData[k] == '\\')) {
                        formats[i] = 1;
                        break;
                    }
                }
            } else {
                ret[i]     = 0;
                lengths[i] = 0;
            }
            //formats[i]=1;
        }
    }

    if (PARAM_INT(1)) {
        int res = PQsendQueryParams((PGconn *)PARAM_INT(2), PARAM(0), count, NULL, ret, lengths, formats, 0);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
    delete[] lengths;
    delete[] ret;
    delete[] formats;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(PQQueryAsync, 2)
    T_STRING(0) // query
    T_NUMBER(1) // con

    if (PARAM_INT(1)) {
        int res = PQsendQuery((PGconn *)PARAM_INT(1), PARAM(0));

        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
