//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
extern "C" {
//#include <my_global.h>
#include <mysql.h>
//#include "mysql/mysql.h"
}

#define SEND_BUF_SIZE    0x4000 //256k buffer

//-------------------------//
// Local variables         //
//-------------------------//
static INVOKE_CALL InvokePtr = 0;

//__declspec(thread) MYSQL_BIND *mybind;
//__declspec(thread) long unsigned int *mylens;
//__thread  MYSQL_BIND *mybind;
//__thread  long unsigned int *mylens;

#ifdef _WIN32
static int is_init = 0;
#endif
//-----------------------------------------------------//

struct WrappedSTMT {
    MYSQL_STMT        *stmt;
    MYSQL_BIND        *bind;
    char              **buffers;
    long unsigned int *lens;
    my_bool           *nulls;
    int               fields;

    MYSQL_BIND        *mybind;
    long unsigned int *mylens;
};


#define FREE_STMT(STMT)                \
    if (STMT->bind)                    \
        delete[] STMT->bind;           \
    if (STMT->buffers) {               \
        int len = STMT->fields;        \
        for (int i = 0; i < len; i++)  \
            delete[] STMT->buffers[i]; \
        delete[] STMT->buffers;        \
    }                                  \
    if (STMT->lens)                    \
        delete[] STMT->lens;           \
    if (STMT->nulls)                   \
        delete[] STMT->nulls;          \
    if (STMT->mybind)                  \
        delete[] STMT->mybind;         \
    if (STMT->mylens)                  \
        delete[] STMT->mylens;         \
    STMT->mylens  = NULL;              \
    STMT->bind    = NULL;              \
    STMT->buffers = NULL;              \
    STMT->lens    = NULL;              \
    STMT->nulls   = NULL;              \
    STMT->fields  = 0;                 \
    STMT->mybind  = NULL;              \
    STMT->mylens  = NULL;

#define DONE_STMT(STMT) \
    FREE_STMT(STMT);    \
    delete STMT;

//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    //master_mysql=mysql_init(NULL);
    //mysql_init(NULL);
    //mysql_thread_init();
#ifdef _WIN32
    // starting up windows sockets
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
    is_init = 1;
#endif
    DEFINE_ECONSTANT(MYSQL_INIT_COMMAND)
    DEFINE_ECONSTANT(MYSQL_READ_DEFAULT_FILE)
    DEFINE_ECONSTANT(MYSQL_READ_DEFAULT_GROUP)
    DEFINE_ECONSTANT(MYSQL_SET_CHARSET_DIR)
    DEFINE_ECONSTANT(MYSQL_SET_CHARSET_NAME)
    DEFINE_ECONSTANT(MYSQL_SHARED_MEMORY_BASE_NAME)
    DEFINE_ECONSTANT(MYSQL_OPT_GUESS_CONNECTION)
    DEFINE_ECONSTANT(MYSQL_OPT_LOCAL_INFILE)
    DEFINE_ECONSTANT(MYSQL_OPT_NAMED_PIPE)
    DEFINE_ECONSTANT(MYSQL_OPT_USE_EMBEDDED_CONNECTION)
    DEFINE_ECONSTANT(MYSQL_OPT_USE_REMOTE_CONNECTION)
    DEFINE_ECONSTANT(MYSQL_OPT_USE_RESULT)
    DEFINE_ECONSTANT(MYSQL_OPT_CONNECT_TIMEOUT)
    DEFINE_ECONSTANT(MYSQL_OPT_PROTOCOL)
    DEFINE_ECONSTANT(MYSQL_OPT_READ_TIMEOUT)
    DEFINE_ECONSTANT(MYSQL_OPT_WRITE_TIMEOUT)
    DEFINE_ECONSTANT(MYSQL_OPT_RECONNECT)
    DEFINE_ECONSTANT(MYSQL_OPT_SSL_VERIFY_SERVER_CERT)
    DEFINE_ECONSTANT(MYSQL_REPORT_DATA_TRUNCATION)
    DEFINE_ECONSTANT(MYSQL_SECURE_AUTH)

    DEFINE_ECONSTANT(NOT_NULL_FLAG)
    DEFINE_ECONSTANT(PRI_KEY_FLAG)
    DEFINE_ECONSTANT(UNIQUE_KEY_FLAG)
    DEFINE_ECONSTANT(MULTIPLE_KEY_FLAG)
    DEFINE_ECONSTANT(BLOB_FLAG)
    DEFINE_ECONSTANT(UNSIGNED_FLAG)
    DEFINE_ECONSTANT(ZEROFILL_FLAG)
    DEFINE_ECONSTANT(BINARY_FLAG)
    DEFINE_ECONSTANT(ENUM_FLAG)
    DEFINE_ECONSTANT(AUTO_INCREMENT_FLAG)
    DEFINE_ECONSTANT(TIMESTAMP_FLAG)
    DEFINE_ECONSTANT(SET_FLAG)
    DEFINE_ECONSTANT(NO_DEFAULT_VALUE_FLAG)
    DEFINE_ECONSTANT(ON_UPDATE_NOW_FLAG)
    DEFINE_ECONSTANT(NUM_FLAG)
    DEFINE_ECONSTANT(PART_KEY_FLAG)
    DEFINE_ECONSTANT(GROUP_FLAG)
    DEFINE_ECONSTANT(UNIQUE_FLAG)
    DEFINE_ECONSTANT(BINCMP_FLAG)
    DEFINE_ECONSTANT(GET_FIXED_FIELDS_FLAG)
    DEFINE_ECONSTANT(FIELD_IN_PART_FUNC_FLAG)
    DEFINE_ECONSTANT(FIELD_IN_ADD_INDEX)
    DEFINE_ECONSTANT(FIELD_IS_RENAMED)

    DEFINE_ECONSTANT(REFRESH_GRANT)
    DEFINE_ECONSTANT(REFRESH_LOG)
    DEFINE_ECONSTANT(REFRESH_TABLES)
    DEFINE_ECONSTANT(REFRESH_HOSTS)
    DEFINE_ECONSTANT(REFRESH_STATUS)
    DEFINE_ECONSTANT(REFRESH_THREADS)
    DEFINE_ECONSTANT(REFRESH_SLAVE)
    DEFINE_ECONSTANT(REFRESH_MASTER)
    DEFINE_ECONSTANT(REFRESH_READ_LOCK)
    DEFINE_ECONSTANT(REFRESH_FAST)

    DEFINE_ECONSTANT(REFRESH_QUERY_CACHE)
    DEFINE_ECONSTANT(REFRESH_QUERY_CACHE_FREE)
    DEFINE_ECONSTANT(REFRESH_DES_KEY_FILE)
    DEFINE_ECONSTANT(REFRESH_USER_RESOURCES)

    DEFINE_ECONSTANT(CLIENT_LONG_PASSWORD)
    DEFINE_ECONSTANT(CLIENT_FOUND_ROWS)
    DEFINE_ECONSTANT(CLIENT_LONG_FLAG)
    DEFINE_ECONSTANT(CLIENT_CONNECT_WITH_DB)
    DEFINE_ECONSTANT(CLIENT_NO_SCHEMA)
    DEFINE_ECONSTANT(CLIENT_COMPRESS)
    DEFINE_ECONSTANT(CLIENT_ODBC)
    DEFINE_ECONSTANT(CLIENT_LOCAL_FILES)
    DEFINE_ECONSTANT(CLIENT_IGNORE_SPACE)
    DEFINE_ECONSTANT(CLIENT_PROTOCOL_41)
    DEFINE_ECONSTANT(CLIENT_INTERACTIVE)
    DEFINE_ECONSTANT(CLIENT_SSL)
    DEFINE_ECONSTANT(CLIENT_IGNORE_SIGPIPE)
    DEFINE_ECONSTANT(CLIENT_TRANSACTIONS)
    DEFINE_ECONSTANT(CLIENT_RESERVED)
    DEFINE_ECONSTANT(CLIENT_SECURE_CONNECTION)
    DEFINE_ECONSTANT(CLIENT_MULTI_STATEMENTS)
    DEFINE_ECONSTANT(CLIENT_MULTI_RESULTS)

    DEFINE_ECONSTANT(CLIENT_SSL_VERIFY_SERVER_CERT)
    DEFINE_ECONSTANT(CLIENT_REMEMBER_OPTIONS)
    DEFINE_ECONSTANT(CLIENT_ALL_FLAGS)

    DEFINE_ECONSTANT(CLIENT_BASIC_FLAGS)

    DEFINE_ECONSTANT(SERVER_STATUS_IN_TRANS)
    DEFINE_ECONSTANT(SERVER_STATUS_AUTOCOMMIT)
    DEFINE_ECONSTANT(SERVER_MORE_RESULTS_EXISTS)
    DEFINE_ECONSTANT(SERVER_QUERY_NO_GOOD_INDEX_USED)
    DEFINE_ECONSTANT(SERVER_QUERY_NO_INDEX_USED)
    DEFINE_ECONSTANT(SERVER_STATUS_CURSOR_EXISTS)
    DEFINE_ECONSTANT(SERVER_STATUS_LAST_ROW_SENT)
    DEFINE_ECONSTANT(SERVER_STATUS_DB_DROPPED)
    DEFINE_ECONSTANT(SERVER_STATUS_NO_BACKSLASH_ESCAPES)
    DEFINE_ECONSTANT(SERVER_STATUS_METADATA_CHANGED)
    DEFINE_ECONSTANT(SERVER_STATUS_CLEAR_SET)
    DEFINE_ECONSTANT(MYSQL_ERRMSG_SIZE)
    DEFINE_ECONSTANT(NET_READ_TIMEOUT)
    DEFINE_ECONSTANT(NET_WRITE_TIMEOUT)
    DEFINE_ECONSTANT(NET_WAIT_TIMEOUT)

    DEFINE_ECONSTANT(ONLY_KILL_QUERY)
    DEFINE_ECONSTANT(MAX_TINYINT_WIDTH)
    DEFINE_ECONSTANT(MAX_SMALLINT_WIDTH)
    DEFINE_ECONSTANT(MAX_MEDIUMINT_WIDTH)
    DEFINE_ECONSTANT(MAX_INT_WIDTH)
    DEFINE_ECONSTANT(MAX_BIGINT_WIDTH)
    DEFINE_ECONSTANT(MAX_CHAR_WIDTH)
    DEFINE_ECONSTANT(MAX_BLOB_WIDTH)

    DEFINE_ECONSTANT(MYSQL_TYPE_DECIMAL)
    DEFINE_ECONSTANT(MYSQL_TYPE_TINY)
    DEFINE_ECONSTANT(MYSQL_TYPE_SHORT)
    DEFINE_ECONSTANT(MYSQL_TYPE_LONG)
    DEFINE_ECONSTANT(MYSQL_TYPE_FLOAT)
    DEFINE_ECONSTANT(MYSQL_TYPE_DOUBLE)
    DEFINE_ECONSTANT(MYSQL_TYPE_NULL)
    DEFINE_ECONSTANT(MYSQL_TYPE_TIMESTAMP)
    DEFINE_ECONSTANT(MYSQL_TYPE_LONGLONG)
    DEFINE_ECONSTANT(MYSQL_TYPE_INT24)
    DEFINE_ECONSTANT(MYSQL_TYPE_DATE)
    DEFINE_ECONSTANT(MYSQL_TYPE_TIME)
    DEFINE_ECONSTANT(MYSQL_TYPE_DATETIME)
    DEFINE_ECONSTANT(MYSQL_TYPE_YEAR)
    DEFINE_ECONSTANT(MYSQL_TYPE_NEWDATE)
    DEFINE_ECONSTANT(MYSQL_TYPE_VARCHAR)
    DEFINE_ECONSTANT(MYSQL_TYPE_BIT)
    DEFINE_ECONSTANT(MYSQL_TYPE_NEWDECIMAL)
    DEFINE_ECONSTANT(MYSQL_TYPE_ENUM)
    DEFINE_ECONSTANT(MYSQL_TYPE_SET)
    DEFINE_ECONSTANT(MYSQL_TYPE_TINY_BLOB)
    DEFINE_ECONSTANT(MYSQL_TYPE_MEDIUM_BLOB)
    DEFINE_ECONSTANT(MYSQL_TYPE_LONG_BLOB)
    DEFINE_ECONSTANT(MYSQL_TYPE_BLOB)
    DEFINE_ECONSTANT(MYSQL_TYPE_VAR_STRING)
    DEFINE_ECONSTANT(MYSQL_TYPE_STRING)
    DEFINE_ECONSTANT(MYSQL_TYPE_GEOMETRY)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    //mysql_server_end();

    /*if (master_mysql) {
        mysql_close(master_mysql);
        master_mysql=NULL;
       }*/
    //mysql_thread_end();
    if (!HANDLER) {
        mysql_library_end();
#ifdef _WIN32
        is_init = 0;
        WSACleanup();
#endif
    }
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLInit, 0)
#ifdef _WIN32
    if (!is_init) {
        WSADATA data;
        WSAStartup(MAKEWORD(2, 2), &data);
        is_init = 1;
    }
#endif

    MYSQL * r_mysql = mysql_init(NULL);
//if (!master_mysql)
//	master_mysql=r_mysql;
    RETURN_NUMBER((long)r_mysql);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLLibraryDone, 0)
    mysql_library_end();
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLLibraryInit, 2)
    T_ARRAY(0)
    T_ARRAY(1)

    int num_elements = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    char **server_options = GetCharList(PARAMETER(0), Invoke);
    int  num_grp          = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    char **server_groups  = GetCharList(PARAMETER(1), Invoke);
    if (num_grp > 0) {
        if (!server_groups[num_grp - 1][0]) {
            server_groups[num_grp - 1] = NULL;
        }
    }

//RETURN_NUMBER((long)mysql_server_init(num_elements, server_options, server_groups));
    if (!num_elements) {
        RETURN_NUMBER((long)mysql_library_init(0, NULL, server_groups));
    } else {
        RETURN_NUMBER((long)mysql_library_init(num_elements, server_options, server_groups));
    }
    if (server_options)
        delete[] server_options;
    if (server_groups)
        delete[] server_groups;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLOptions, 3)
    T_NUMBER(0)

    MYSQL * mysql;
    int          param_index = 0;
    mysql_option mopt;

//if (PARAMETERS_COUNT==3) {
    T_NUMBER(1)
    param_index = 1;
    mysql       = (MYSQL *)(SYS_INT)PARAM(0);
    mopt        = (mysql_option)(PARAM_INT(1));
//}
    mopt = (mysql_option)(PARAM_INT(0));
    param_index++;
    switch (mopt) {
        case MYSQL_INIT_COMMAND:
        //case MYSQL_OPT_SET_CLIENT_IP:
        case MYSQL_READ_DEFAULT_FILE:
        case MYSQL_READ_DEFAULT_GROUP:
        case MYSQL_SET_CHARSET_DIR:
        case MYSQL_SET_CHARSET_NAME:
        case MYSQL_SHARED_MEMORY_BASE_NAME:
            {
                T_STRING(param_index);
                RETURN_NUMBER(mysql_options(mysql, mopt, (const char *)PARAM(param_index)));
            }
            break;

        case MYSQL_OPT_COMPRESS:
        case MYSQL_OPT_GUESS_CONNECTION:
        case MYSQL_OPT_LOCAL_INFILE:
        case MYSQL_OPT_NAMED_PIPE:
        case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
        case MYSQL_OPT_USE_REMOTE_CONNECTION:
        case MYSQL_OPT_USE_RESULT:
            {
                T_NUMBER(param_index);
                RETURN_NUMBER(mysql_options(mysql, mopt, (const char *)PARAM_INT(param_index)));
            }
            break;

        case MYSQL_OPT_CONNECT_TIMEOUT:
        case MYSQL_OPT_PROTOCOL:
        case MYSQL_OPT_READ_TIMEOUT:
        case MYSQL_OPT_WRITE_TIMEOUT:
            {
                T_NUMBER(param_index);
                unsigned int param = PARAM_INT(param_index);
                RETURN_NUMBER(mysql_options(mysql, mopt, (const char *)&param));
                SET_NUMBER(param_index, param);
            }
            break;

        case MYSQL_OPT_RECONNECT:
        case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
        case MYSQL_REPORT_DATA_TRUNCATION:
        case MYSQL_SECURE_AUTH:
            {
                T_NUMBER(param_index);
                my_bool param = (my_bool)PARAM_INT(param_index);
                RETURN_NUMBER(mysql_options(mysql, mopt, (const char *)&param));
                SET_NUMBER(param_index, param);
            }
            break;
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLConnect, 8)
    T_STRING(0) // host
    T_STRING(1) // user
    T_STRING(2) // password
    T_STRING(3) // db
    T_NUMBER(4) // port
    T_STRING(5) // unix socket
    T_NUMBER(6) // client_flag

#ifdef _WIN32
    if (!is_init) {
        WSADATA data;

        WSAStartup(MAKEWORD(2, 2), &data);
        is_init = 1;
    }
#endif
    char *host = PARAM(0);
    char *user     = PARAM(1);
    char *password = PARAM(2);
    char *db       = PARAM(3);
    if (!PARAM_LEN(0))
        host = NULL;
    if (!PARAM_LEN(1))
        user = NULL;
    if (!PARAM_LEN(2))
        password = NULL;
    if (!PARAM_LEN(3))
        db = NULL;

    char *mysocket = PARAM(5);
    if ((mysocket) && (mysocket[0] == 0))
        mysocket = NULL;

//MYSQL *r_mysql=master_mysql;
//if (PARAMETERS_COUNT==8) {
    T_NUMBER(7)
    MYSQL * r_mysql = (MYSQL *)(SYS_INT)PARAM(7);
//}
    if (!r_mysql) {
        r_mysql = mysql_init(NULL);
        //if (!master_mysql)
        //	master_mysql=r_mysql;
    }

    long res = (long)mysql_real_connect(r_mysql, host, user, password, db, PARAM_INT(4), mysocket, PARAM_INT(6));
    if (!res) {
        mysql_close(r_mysql);
        //if (r_mysql==master_mysql)
        //	master_mysql=NULL;
    }
    RETURN_NUMBER(res);
//std::cerr << mysql_error(mysql);
//mysql_close(mysql);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLDisconnect, 1)
    T_NUMBER(0) // mysql

    RETURN_NUMBER(0);

    if (PARAM_INT(0)) {
        //if ((MYSQL *)PARAM_INT(0)==master_mysql)
        //	master_mysql=0;
        mysql_close((MYSQL *)(SYS_INT)PARAM(0));
        SET_NUMBER(0, 0);
    }

END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLAutoCommit, 2)
    T_NUMBER(0) // flag
    T_NUMBER(1) // mysql


    if (PARAM_INT(1)) {
        RETURN_NUMBER(mysql_autocommit((MYSQL *)(SYS_INT)PARAM(1), PARAM_INT(0)));
    } else {
        RETURN_NUMBER(-1);
    }

END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLCommit, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_commit((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLRollback, 1)
    T_NUMBER(0) // mysql


    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_commit((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLError, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_STRING(mysql_error((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        //if (master_mysql) {
        //    RETURN_STRING(mysql_error(master_mysql));
        //} else {
        RETURN_STRING("No connection");
        //}
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLSTMTError, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        //WrappedSTMT stmt=new WrappedSTMT;
        //RETURN_STRING(mysql_stmt_error((MYSQL_STMT *)PARAM_INT(0)));
        RETURN_STRING(mysql_stmt_error(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLWarningCount, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_warning_count((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLListTables, 2)
    T_STRING(0) // wildcard
    T_NUMBER(1) // mysql

    if (PARAM_INT(1)) {
        if (PARAM_LEN(0)) {
            RETURN_NUMBER((long)mysql_list_tables((MYSQL *)(SYS_INT)PARAM(1), NULL));
        } else {
            RETURN_NUMBER((long)mysql_list_tables((MYSQL *)(SYS_INT)PARAM(1), PARAM(0)));
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLListColumns, 3)
    T_STRING(0) // table
    T_STRING(1) // wildcard
    T_NUMBER(2) // mysql

    if (PARAM_INT(2)) {
        char *table = NULL;
        char *wild  = NULL;

        if (PARAM_LEN(0))
            table = PARAM(0);
        if (PARAM_LEN(1))
            wild = PARAM(1);

        RETURN_NUMBER((long)mysql_list_fields((MYSQL *)(SYS_INT)PARAM(2), table, wild));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLListDatabases, 2)
    T_STRING(0) // wildcard
    T_NUMBER(1) // mysql

    if (PARAM_INT(1)) {
        RETURN_NUMBER((long)mysql_list_dbs((MYSQL *)(SYS_INT)PARAM(1), PARAM(0)));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFreeResult, 1)
    T_NUMBER(0) // mysql result

/*if (mybind) {
        delete[] mybind;
        mybind=0;
   }
   if (mylens) {
        delete[] mylens;
        mylens=0;
   }*/

    if (PARAM_INT(0)) {
        mysql_free_result((MYSQL_RES *)(SYS_INT)PARAM(0));
    }

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLCountColumns, 1)
    T_NUMBER(0) // mysql result

    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_num_fields((MYSQL_RES *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLCountRows, 1)
    T_NUMBER(0) // mysql result

    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_num_rows((MYSQL_RES *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLDescribeCol, 7)
    T_NUMBER(0) // index
    T_NUMBER(6) // mysql result

    if (PARAM_INT(6)) {
        int  res        = 0;
        long num_fields = mysql_num_fields((MYSQL_RES *)(SYS_INT)PARAM(6));
        int  index      = PARAM_INT(0);

        index--;
        if ((index < 0) || (index >= num_fields)) {
            RETURN_NUMBER(-1);
            return 0;
        }
        MYSQL_FIELD *fields = mysql_fetch_fields((MYSQL_RES *)(SYS_INT)PARAM(6));

        SET_STRING(1, fields[index].name);
        SET_NUMBER(2, fields[index].type);
        SET_NUMBER(3, fields[index].length);
        SET_NUMBER(4, fields[index].decimals);
        SET_NUMBER(5, (!(IS_NOT_NULL(fields[index].flags))));

        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLDescribeCol2, 7)
    T_NUMBER(0) // index
    T_NUMBER(6) // mysql result

    if (PARAM_INT(6)) {
        int  res        = 0;
        long num_fields = mysql_num_fields((MYSQL_RES *)(SYS_INT)PARAM(6));
        int  index      = PARAM_INT(0);

        index--;
        if ((index < 0) || (index >= num_fields)) {
            RETURN_NUMBER(-1);
            return 0;
        }
        MYSQL_FIELD *fields = mysql_fetch_fields((MYSQL_RES *)(SYS_INT)PARAM(6));

        SET_STRING(1, fields[index].name);
        SET_NUMBER(2, fields[index].type);
        SET_NUMBER(3, fields[index].length);
        SET_NUMBER(4, fields[index].decimals);
        SET_NUMBER(5, fields[index].flags);

        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLStatement, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        WrappedSTMT *ws = new WrappedSTMT;

        ws->bind    = NULL;
        ws->buffers = NULL;
        ws->lens    = NULL;
        ws->nulls   = NULL;
        ws->mybind  = NULL;
        ws->mylens  = NULL;
        ws->stmt    = mysql_stmt_init((MYSQL *)(SYS_INT)PARAM(0));
        if (!ws->stmt) {
            DONE_STMT(ws);
            ws = NULL;
        }
        RETURN_NUMBER((long)ws);
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFreeQuery, 1)
    T_NUMBER(0) // stmt

/*if (mybind) {
        delete[] mybind;
        mybind=0;
   }
   if (mylens) {
        delete[] mylens;
        mylens=0;
   }*/

    if (PARAM_INT(0)) {
        RETURN_NUMBER(mysql_stmt_close(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt));
        DONE_STMT(((WrappedSTMT *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLExecute, 2)
    T_STRING(0) // query
    T_NUMBER(1) // stmt

    if (PARAM_INT(1)) {
        int res = mysql_stmt_prepare(((WrappedSTMT *)(SYS_INT)PARAM(1))->stmt, PARAM(0), PARAM_LEN(0));

        if (res) {
            RETURN_NUMBER(res);
            return 0;
        }
        res = mysql_stmt_execute(((WrappedSTMT *)(SYS_INT)PARAM(1))->stmt);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLPrepare, 2)
    T_STRING(0) // query
    T_NUMBER(1) // stmt

    if (PARAM_INT(1)) {
        int res = mysql_stmt_prepare(((WrappedSTMT *)(SYS_INT)PARAM(1))->stmt, PARAM(0), PARAM_LEN(0));

        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLCountParams, 1)
    T_NUMBER(0) // stmt

    if (PARAM_INT(0)) {
        int res = mysql_stmt_param_count(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt);

        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLPreparedExecute, 1)
    T_NUMBER(0) // stmt

    if (PARAM_INT(0)) {
        int res = mysql_stmt_execute(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt);

        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLBindParameters, 2)
    T_NUMBER(0) // stmt
    T_ARRAY(1)

    void *pData = PARAMETER(1);

    WrappedSTMT *ws = (WrappedSTMT *)(SYS_INT)PARAM(0);
    if (ws) {
        if (ws->mybind) {
            delete[] ws->mybind;
            ws->mybind = 0;
        }
        if (ws->mylens) {
            delete[] ws->mylens;
            ws->mylens = 0;
        }

        int count      = Invoke(INVOKE_GET_ARRAY_COUNT, pData);
        int parameters = mysql_stmt_param_count(ws->stmt);
        if (parameters != count) {
            RETURN_NUMBER(0);
            return 0;
        }

        ws->mybind = new MYSQL_BIND[parameters];
        memset(ws->mybind, 0, sizeof(MYSQL_BIND) * parameters);

        ws->mylens = new long unsigned int[parameters];
        char has_long_data    = 0;
        long unsigned int len = 0;
        for (int i = 0; i < parameters; i++) {
            ws->mylens[i] = 0;
            void    *newpData = 0;
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            //Invoke(INVOKE_ARRAY_VARIABLE,pData,i,&newpData);
            //Invoke(INVOKE_GET_VARIABLE,newpData,&type,&szData,&nData);
            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(1), i, &type, &szData, &nData);
            if (type != VARIABLE_STRING) {
                if (ws->mybind) {
                    delete[] ws->mybind;
                    ws->mybind = 0;
                }
                if (ws->mylens) {
                    delete[] ws->mylens;
                    ws->mylens = 0;
                }
                RETURN_NUMBER(-1);
                return 0;
            }
            len           = (long unsigned int)nData;
            ws->mylens[i] = len;

            ws->mybind[i].buffer_type   = MYSQL_TYPE_STRING;
            ws->mybind[i].buffer        = szData;
            ws->mybind[i].buffer_length = ws->mylens[i];      //STRING_SIZE;
            ws->mybind[i].is_null       = 0;
            ws->mybind[i].length        = &ws->mylens[i];

            if (len > SEND_BUF_SIZE) {
                ws->mybind[i].buffer        = 0;
                ws->mybind[i].buffer_length = 0;
                has_long_data = 1;

                /*int buf_size=8192;
                   int res=0;
                   do {
                    res=mysql_stmt_send_long_data(((WrappedSTMT *)PARAM_INT(0))->stmt, i, szData, buf_size);
                    szData+=buf_size;
                    len-=buf_size;
                    if ((len>0) && (len<buf_size))
                        buf_size=len;
                   } while (len>0);
                 */
            }
        }

        RETURN_NUMBER(mysql_stmt_bind_param(ws->stmt, ws->mybind));

        if (has_long_data) {
            for (int i = 0; i < parameters; i++) {
                long unsigned int len       = 0;
                void              *newpData = 0;
                char              *szData;
                INTEGER           type;
                NUMBER            nData;

                Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(1), i, &type, &szData, &nData);
                len = (long unsigned int)nData;
                if ((type == VARIABLE_STRING) && (len > SEND_BUF_SIZE)) {
                    has_long_data = 1;
                    int buf_size = SEND_BUF_SIZE;
                    int res      = 0;
                    do {
                        res     = mysql_stmt_send_long_data(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt, i, szData, buf_size);
                        szData += buf_size;
                        len    -= buf_size;
                        if ((len > 0) && (len < buf_size))
                            buf_size = len;
                    } while (len > 0);
                }
            }
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MySQLBindStatement, 1, 2)
    T_NUMBER(0) // stmt

    int store_result = 0;
    if (PARAMETERS_COUNT == 2) {
        T_NUMBER(1)
        store_result = PARAM_INT(1);
    }

    if (PARAM_INT(0)) {
        WrappedSTMT *ws     = (WrappedSTMT *)(SYS_INT)PARAM(0);
        MYSQL_RES   *result = mysql_stmt_result_metadata(ws->stmt);
        if (result) {
            FREE_STMT(ws);
            //int len=mysql_stmt_num_rows(ws->stmt);
            int len = mysql_num_fields(result);
            ws->fields = len;
            if (len) {
                mysql_field_seek(result, 0);
                ws->bind = new MYSQL_BIND[len];

                memset(ws->bind, 0, sizeof(MYSQL_BIND) * len);

                ws->lens    = new long unsigned int[len];
                ws->nulls   = new my_bool[len];
                ws->buffers = new char * [len];
                MYSQL_FIELD *fields = mysql_fetch_fields(result);
                //MYSQL_FIELD *field;
                for (int i = 0; i < len; i++) {
                    //field= mysql_fetch_field(result);

                    int max_size = fields[i].max_length;
                    //int max_size=field->max_length;
                    if (max_size <= 0) {
                        max_size = fields[i].length;
                        // blob ?
                        if ((max_size > 0xFFFFF) || (max_size < 0))
                            max_size = 0xFFFFF;
                    }
                    //max_size=field->length;

                    ws->buffers[i]           = new char [max_size + 1];
                    ws->buffers[i][max_size] = 0;
                    ws->buffers[i][0]        = 0;

                    ws->lens[i] = max_size + 1;

                    ws->bind[i].buffer_type   = MYSQL_TYPE_STRING;
                    ws->bind[i].buffer        = ws->buffers[i];
                    ws->bind[i].buffer_length = ws->lens[i];
                    ws->bind[i].is_null       = &ws->nulls[i];
                    ws->bind[i].length        = &ws->lens[i];
                }
            }
            int res = mysql_stmt_bind_result(ws->stmt, ws->bind);
            if ((!res) && (store_result))
                res = mysql_stmt_store_result(ws->stmt);
            mysql_free_result(result);
            RETURN_NUMBER(res);
        } else
            RETURN_NUMBER(-1);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLSTMTResult, 1)
    T_NUMBER(0) // stmt

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_stmt_result_metadata(((WrappedSTMT *)(SYS_INT)PARAM(0))->stmt));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//

/*
   CONCEPT_FUNCTION_IMPL(ODBCFetchAbsolute, 2)
    T_NUMBER(0) // row index
    T_NUMBER(1) // stmt

    if (PARAM_INT(1)) {
        INTEGER index=PARAM_INT(0);
        MYSQL_STMT *stmt=(MYSQL_STMT *)PARAM_INT(1);
        int total=mysql_stmt_num_rows(stmt);

        if ((index<0) || (index>=total)) {
            RETURN_NUMBER(0)
            return 0;
        }
        mysql_stmt_data_seek(stmt, index);
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0)
    }
   END_IMPL
   //-----------------------------------------------------//
   CONCEPT_FUNCTION_IMPL(MySQLPosition, 1)
    T_NUMBER(0) // stmt

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_stmt_row_tell((MYSQL_STMT *)PARAM_INT(0)));
    } else {
        RETURN_NUMBER(-1)
    }
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchForward, 1)
    T_NUMBER(0) // stmt

    if (PARAM_INT(0)) {
        WrappedSTMT *ws    = (WrappedSTMT *)(SYS_INT)PARAM(0);
        int         result = mysql_stmt_fetch(ws->stmt);

        if (result == MYSQL_DATA_TRUNCATED) {
            // to do
            int len = ws->fields;
            for (int i = 0; i < len; i++) {
                int f_len = ws->lens[i];
                if (f_len > ws->bind[i].buffer_length) {
                    if (ws->buffers[i])
                        delete[] ws->buffers[i];
                    ws->buffers[i] = new char[f_len + 1];
                    char *target = ws->buffers[i];
                    target[0]     = 0;
                    target[f_len] = 0;
                    ws->lens[i]   = f_len + 1;

                    ws->bind[i].buffer        = ws->buffers[i];
                    ws->bind[i].buffer_length = ws->lens[i];
                    mysql_stmt_fetch_column(ws->stmt, &ws->bind[i], i, 0);
                }
            }
        }
        RETURN_NUMBER(((result == 0) || (result == MYSQL_DATA_TRUNCATED)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL

/*
   //-----------------------------------------------------//
   CONCEPT_FUNCTION_IMPL(MySQLQuery,2)
    T_STRING(0) // query
    T_NUMBER(1) // mysql

    if (PARAM_INT(1)) {
        int result=mysql_real_query(((MYSQL *)PARAM_INT(1)), PARAM(0), PARAM_LEN(0));
        RETURN_NUMBER(result);
    } else {
        RETURN_NUMBER(-1)
    }
   END_IMPL
   //-----------------------------------------------------//

   CONCEPT_FUNCTION_IMPL(MySQLColumnGet, 2)
    T_NUMBER(0) // col index
    T_NUMBER(1) // stmt

    if (PARAM_INT(1)) {
        INTEGER index=PARAM_INT(0);
        MYSQL_STMT *stmt=(MYSQL_STMT *)PARAM_INT(1);
        mysql_stmt_fetch(stmt);
        MYSQL_RES *res=mysql_stmt_result_metadata(stmt);
        if (!res) {
            RETURN_STRING(0);
            return 0;
        }
        int total=mysql_num_fields(res);

        if ((index<0) || (index>=total)) {
            RETURN_STRING("")
            return 0;
        }

        long unsigned real_length= 0;
        MYSQL_BIND bind;
        bind.buffer_type=
        bind.length=&real_length;
        bind.buffer_length=0;
        bind.buffer=0;
   std::cout << "Before !\n";
   std::cout.flush();
        mysql_stmt_fetch_column(stmt, &bind, index, 0);
   std::cout << "After !\n";
   std::cout.flush();
        /*int current=mysql_field_tell(res);

        if (current!=index)
            mysql_field_seek(res, index);

        MYSQL_ROW row;
        MYSQL_FIELD *field=mysql_fetch_field(res);
        if (field) {
            int data=new char [field.length];
            mysql_

            RETURN_BUFFER(data, field.length);
            delete[] data;
        }
    } else {
        RETURN_STRING("")
    }
   END_IMPL
   //-----------------------------------------------------//
 */
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLQuery, 2)
    T_STRING(0) // query
    T_NUMBER(1) // mysql

    if (PARAM_INT(1)) {
        int result = mysql_real_query(((MYSQL *)(SYS_INT)PARAM(1)), PARAM(0), PARAM_LEN(0));

        RETURN_NUMBER(result);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLSendQuery, 2)
    T_STRING(0) // query
    T_NUMBER(1) // mysql

    if (PARAM_INT(1)) {
        int result = mysql_send_query(((MYSQL *)(SYS_INT)PARAM(1)), PARAM(0), PARAM_LEN(0));

        RETURN_NUMBER(result);
    } else {
        RETURN_NUMBER(-1)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchRow, 1)
    T_NUMBER(0)
    MYSQL_ROW row;
    MYSQL_RES    *result    = (MYSQL_RES *)(SYS_INT)PARAM(0);
    unsigned int num_fields = mysql_num_fields(result);

    row = mysql_fetch_row(result);
    if (!row) {
        RETURN_NUMBER(0);
    } else {
        INTEGER       index    = 0;
        unsigned long *lengths = mysql_fetch_lengths(result);
        if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
            return (void *)"Failed to INVOKE_CREATE_ARRAY";

        for (index = 0; index < num_fields; index++) {
            if ((row[index]) && (lengths[index]))
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, (char *)row[index], (NUMBER)lengths[index]);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLStoreResult, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_store_result((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLUseResult, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_use_result((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLReadQueryResult, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_read_query_result((MYSQL *)(SYS_INT)PARAM(0)));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLColumnGet, 2)
    T_NUMBER(0) // col index
    T_NUMBER(1) // mysql res

    if (PARAM_INT(1)) {
        INTEGER   index = PARAM_INT(0);
        MYSQL_RES *res  = (MYSQL_RES *)(SYS_INT)PARAM(1);

        if (!res) {
            RETURN_STRING("");
            return 0;
        }
        int total = mysql_num_fields(res);

        if ((index < 0) || (index >= total)) {
            RETURN_STRING("")
            return 0;
        }
        MYSQL_ROW_OFFSET old_pos = mysql_row_tell(res);

        MYSQL_ROW     row      = mysql_fetch_row(res);
        unsigned long *lengths = mysql_fetch_lengths(res);
        if ((!row) || (!lengths)) {
            mysql_row_seek(res, old_pos);
            RETURN_STRING("");
            return 0;
        }

        if (row[index]) {
            //int len=(int)lengths[index];
            MYSQL_FIELD *field = mysql_fetch_field_direct(res, index);
            mysql_row_seek(res, old_pos);
            if ((lengths[index]) && ((!field) || (field->type != MYSQL_TYPE_NULL))) {
                RETURN_BUFFER(row[index], (int)lengths[index]);
            } else {
                RETURN_STRING("")
            }
        } else {
            mysql_row_seek(res, old_pos);
            RETURN_STRING("")
        }
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLSTMTColumnGet, 2)
    T_NUMBER(0) // col index
    T_NUMBER(1) // mysql stmt

    if (PARAM_INT(1)) {
        INTEGER     index = PARAM_INT(0);
        WrappedSTMT *res  = (WrappedSTMT *)(SYS_INT)PARAM(1);

        if ((!res) || (!res->buffers) || (!res->stmt)) {
            RETURN_STRING("");
            return 0;
        }

        if ((index < 0) || (index >= res->fields)) {
            RETURN_STRING("")
            return 0;
        }

        if ((res->bind) && (res->bind[index].is_null_value)) {
            RETURN_STRING("")
            return 0;
        }

        if ((res->buffers[index]) && (!res->nulls[index]) && (res->lens[index] > 0)) {
            RETURN_BUFFER(res->buffers[index], res->lens[index]);
        } else {
            RETURN_STRING("")
        }
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//

/*CONCEPT_FUNCTION_IMPL(ODBCFetchAbsolute, 2)
    T_NUMBER(0) // row index
    T_NUMBER(1) // stmt

    if (PARAM_INT(1)) {
        INTEGER index=PARAM_INT(0);
        MYSQL_STMT *stmt=(MYSQL_STMT *)PARAM_INT(1);
        int total=mysql_stmt_num_rows(stmt);

        if ((index<0) || (index>=total)) {
            RETURN_NUMBER(0)
            return 0;
        }
        mysql_stmt_data_seek(stmt, index);
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0)
    }
   END_IMPL
   //-----------------------------------------------------//
   CONCEPT_FUNCTION_IMPL(MySQLPosition, 1)
    T_NUMBER(0) // mysql

    if (PARAM_INT(0)) {
        RETURN_NUMBER((long)mysql_row_tell((MYSQL_RES *)PARAM_INT(0)));
    } else {
        RETURN_NUMBER(-1)
    }
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchFirst, 1)
    T_NUMBER(0) // mysql res

    if (PARAM_INT(0)) {
        int index = mysql_num_rows((MYSQL_RES *)(SYS_INT)PARAM(0));

        if (index > 0) {
            mysql_data_seek((MYSQL_RES *)(SYS_INT)PARAM(0), 0);
            RETURN_NUMBER(1);
        } else {
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchLast, 1)
    T_NUMBER(0) // mysql res

    if (PARAM_INT(0)) {
        int index = mysql_num_rows((MYSQL_RES *)(SYS_INT)PARAM(0));

        if (index > 0) {
            mysql_data_seek((MYSQL_RES *)(SYS_INT)PARAM(0), index - 1);
            RETURN_NUMBER(1);
        } else {
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchAbsolute, 2)
    T_NUMBER(1) // index
    T_NUMBER(0) // mysql res

    if (PARAM_INT(1)) {
        int total = mysql_num_rows((MYSQL_RES *)(SYS_INT)PARAM(1));
        int index = PARAM_INT(0);

        if ((index >= 0) && (index < total)) {
            mysql_data_seek((MYSQL_RES *)(SYS_INT)PARAM(1), index);
            RETURN_NUMBER(1);
        } else {
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFetchNext, 1)
    T_NUMBER(0) // mysql res

    if (PARAM_INT(0)) {
        MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)(SYS_INT)PARAM(0));

        // twice ...
        if (row) {
            MYSQL_ROW_OFFSET old_pos = mysql_row_tell((MYSQL_RES *)(SYS_INT)PARAM(0));
            row = mysql_fetch_row((MYSQL_RES *)(SYS_INT)PARAM(0));
            if (row) {
                mysql_row_seek((MYSQL_RES *)(SYS_INT)PARAM(0), old_pos);
                RETURN_NUMBER(1)
            } else {
                RETURN_NUMBER(0)
            }
        } else {
            RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLEscape, 2)
    T_STRING(0) // mysql res
    T_NUMBER(1) // mysql

    if (!PARAM_INT(1)) {
        RETURN_STRING("");
        return 0;
    }

    int len = PARAM_LEN(0);

    char *buffer = new char[len * 2 + 1];

    unsigned long len_new = mysql_real_escape_string((MYSQL *)(SYS_INT)PARAM(1), buffer, PARAM(0), len);
    RETURN_BUFFER(buffer, len_new);
    delete[] buffer;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MySQLFD, 1)
    T_NUMBER(0) // mysql res

    MYSQL * mysql = (MYSQL *)(SYS_INT)PARAM(0);
    if (mysql) {
        RETURN_NUMBER(mysql->net.fd);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
