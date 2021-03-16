//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
extern "C" {
    #ifndef _WIN32
        #define MONGO_HAVE_STDINT
    #endif
    #include <mongo.h>
    #include <bson.h>
    #include <gridfs.h>
}

#ifdef _WIN32
 #include <windows.h> //I've ommited this line.
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS    11644473600000000Ui64
 #else
  #define DELTA_EPOCH_IN_MICROSECS    11644473600000000ULL
 #endif

struct timezone {
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    FILETIME         ft;
    unsigned __int64 tmpres = 0;
    static int       tzflag;

    if (NULL != tv) {
        GetSystemTimeAsFileTime(&ft);

        tmpres  |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres  |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres     -= DELTA_EPOCH_IN_MICROSECS;
        tmpres     /= 10; /*convert into microseconds*/
        tv->tv_sec  = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz) {
        if (!tzflag) {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime     = _daylight;
    }
    return 0;
}

#else
 #include <sys/time.h>
#endif


#define CONCEPT_CLASS_TYPE    "classof()"
#define BSON_NUMBER(b, key, nData)                          \
    if (floor(nData) == nData) {                            \
        if ((nData >= 0x7FFFFFFF) || (nData < -0x7FFFFFFE)) \
            bson_append_long(b, key, (int64_t)nData);       \
        else                                                \
            bson_append_int(b, key, (int)nData);            \
    } else                                                  \
        bson_append_double(b, key, nData);

//-------------------------//
// Local variables         //
//-------------------------//
static INVOKE_CALL InvokePtr = 0;
static AnsiString  lasterror;

void do_array(bson *b, void *arr, bool is_oid = false, bool as_object = false, bool keep_types = false, bool look_for_eval = false);
void do_object(bson *b, void *pData, bool keep_types = false);

//-----------------------------------------------------//
int my_oid_fuzz_func(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    srand(tv.tv_usec);
    return rand();
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    // starting up windows sockets
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
    DEFINE_ECONSTANT(MONGO_OK);
    DEFINE_ECONSTANT(MONGO_ERROR);

    DEFINE_ECONSTANT(MONGO_CONN_SUCCESS);
    DEFINE_ECONSTANT(MONGO_CONN_NO_SOCKET);
    DEFINE_ECONSTANT(MONGO_CONN_FAIL);
    DEFINE_ECONSTANT(MONGO_CONN_ADDR_FAIL);
    DEFINE_ECONSTANT(MONGO_CONN_NOT_MASTER);
    DEFINE_ECONSTANT(MONGO_CONN_BAD_SET_NAME);
    DEFINE_ECONSTANT(MONGO_CONN_NO_PRIMARY);

    DEFINE_ECONSTANT(MONGO_IO_ERROR);
    DEFINE_ECONSTANT(MONGO_READ_SIZE_ERROR);
    DEFINE_ECONSTANT(MONGO_COMMAND_FAILED);
    DEFINE_ECONSTANT(MONGO_CURSOR_EXHAUSTED);
    DEFINE_ECONSTANT(MONGO_CURSOR_INVALID);
    DEFINE_ECONSTANT(MONGO_CURSOR_PENDING);
    DEFINE_ECONSTANT(MONGO_BSON_INVALID);
    DEFINE_ECONSTANT(MONGO_BSON_NOT_FINISHED);

    DEFINE_ECONSTANT(MONGO_CURSOR_MUST_FREE);
    DEFINE_ECONSTANT(MONGO_CURSOR_QUERY_SENT);

    DEFINE_ECONSTANT(MONGO_INDEX_UNIQUE);
    DEFINE_ECONSTANT(MONGO_INDEX_DROP_DUPS);
    DEFINE_ECONSTANT(MONGO_INDEX_BACKGROUND);
    DEFINE_ECONSTANT(MONGO_INDEX_SPARSE);
    DEFINE_ECONSTANT(MONGO_UPDATE_UPSERT);
    DEFINE_ECONSTANT(MONGO_UPDATE_MULTI);
    DEFINE_ECONSTANT(MONGO_UPDATE_BASIC);
    DEFINE_ECONSTANT(MONGO_TAILABLE);
    DEFINE_ECONSTANT(MONGO_SLAVE_OK);
    DEFINE_ECONSTANT(MONGO_NO_CURSOR_TIMEOUT);
    DEFINE_ECONSTANT(MONGO_AWAIT_DATA);
    DEFINE_ECONSTANT(MONGO_EXHAUST);
    DEFINE_ECONSTANT(MONGO_PARTIAL);
    DEFINE_ECONSTANT(MONGO_OP_MSG);
    DEFINE_ECONSTANT(MONGO_OP_UPDATE);
    DEFINE_ECONSTANT(MONGO_OP_INSERT);
    DEFINE_ECONSTANT(MONGO_OP_QUERY);
    DEFINE_ECONSTANT(MONGO_OP_GET_MORE);
    DEFINE_ECONSTANT(MONGO_OP_DELETE);
    DEFINE_ECONSTANT(MONGO_OP_KILL_CURSORS);

    DEFINE_ECONSTANT(BSON_OK)
    DEFINE_ECONSTANT(BSON_ERROR)
    DEFINE_ECONSTANT(BSON_SIZE_OVERFLOW)

    DEFINE_ECONSTANT(BSON_VALID)
    DEFINE_ECONSTANT(BSON_NOT_UTF8)
    DEFINE_ECONSTANT(BSON_FIELD_HAS_DOT)
    DEFINE_ECONSTANT(BSON_FIELD_INIT_DOLLAR)
    DEFINE_ECONSTANT(BSON_ALREADY_FINISHED)

    DEFINE_ECONSTANT(BSON_BIN_BINARY)
    DEFINE_ECONSTANT(BSON_BIN_FUNC)
    DEFINE_ECONSTANT(BSON_BIN_BINARY_OLD)
    DEFINE_ECONSTANT(BSON_BIN_UUID)
    DEFINE_ECONSTANT(BSON_BIN_MD5)
    DEFINE_ECONSTANT(BSON_BIN_USER)

    DEFINE_ECONSTANT(BSON_EOO)
    DEFINE_ECONSTANT(BSON_DOUBLE)
    DEFINE_ECONSTANT(BSON_STRING)
    DEFINE_ECONSTANT(BSON_OBJECT)
    DEFINE_ECONSTANT(BSON_ARRAY)
    DEFINE_ECONSTANT(BSON_BINDATA)
    DEFINE_ECONSTANT(BSON_UNDEFINED)
    DEFINE_ECONSTANT(BSON_OID)
    DEFINE_ECONSTANT(BSON_BOOL)
    DEFINE_ECONSTANT(BSON_DATE)
    DEFINE_ECONSTANT(BSON_NULL)
    DEFINE_ECONSTANT(BSON_REGEX)
    DEFINE_ECONSTANT(BSON_DBREF)
    DEFINE_ECONSTANT(BSON_CODE)
    DEFINE_ECONSTANT(BSON_SYMBOL)
    DEFINE_ECONSTANT(BSON_CODEWSCOPE)
    DEFINE_ECONSTANT(BSON_INT)
    DEFINE_ECONSTANT(BSON_TIMESTAMP)
    DEFINE_ECONSTANT(BSON_LONG)

    //ConceptHandler=HANDLER;

    bson_set_oid_fuzz(my_oid_fuzz_func);
    return 0;
}
//-----------------------------------------------------//

/*void ArrayToBson(void *arr, char *key, INVOKE_CALL _Invoke, bson *b) {
    INTEGER type=0;
    NUMBER nr=0;
    void *newpData=0;
    double *ret=0;
    char *szData;
    NUMBER nData;


    int count=_Invoke(INVOKE_GET_ARRAY_COUNT,arr);
    if (count<2)
        return;

    _Invoke(INVOKE_ARRAY_VARIABLE,arr,(INTEGER)0,&newpData);
    int btype=-1;
    char *str=0;
    if (newpData) {
        _Invoke(INVOKE_GET_VARIABLE,newpData,&type,&szData,&nData);
        if (type==VARIABLE_NUMBER)
            type=(int)nData;
    }
    _Invoke(INVOKE_ARRAY_VARIABLE,arr,(INTEGER)1,&newpData);
    if (newpData) {
        _Invoke(INVOKE_GET_VARIABLE,newpData,&type,&szData,&nData);
        if (type==VARIABLE_STRING)
            str=szData;
    }

    char *opts="";
    // read element #3 !
    if (count>3) {
        _Invoke(INVOKE_ARRAY_VARIABLE,arr,(INTEGER)2,&newpData);
        if (newpData) {
            _Invoke(INVOKE_GET_VARIABLE,newpData,&type,&szData,&nData);
            if (type==VARIABLE_STRING)
                opts=szData;
        }
    }

    szData=0;
    type=VARIABLE_NUMBER;
    nData=0;
    newpData=0;
    if (count>2) {
        _Invoke(INVOKE_ARRAY_VARIABLE,arr,(INTEGER)2,&newpData);
        if (newpData) {
            _Invoke(INVOKE_GET_VARIABLE,newpData,&type,&szData,&nData);
            if (type!=VARIABLE_STRING)
                szData=0;
        }
    }

    switch (btype) {
        case BSON_EOO:
            break;
        case BSON_DOUBLE:
            bson_append_double(b, str, nData);
            break;
        case BSON_STRING:
            if (!szData)
                 szData=(char *)"";
            bson_append_string_n(b, str, szData, (int)nData);
            break;
        case BSON_OBJECT:
            break;
        case BSON_ARRAY:
            {
                bson *b2=(bson *)malloc(sizeof(bson));
                bson_init(b2);
                ArrayToBson(newpData, _Invoke, b2);
                bson_finish(b2);
            }
            break;
        case BSON_BINDATA:
            bson_append_binary(b, str, btype, szData, (int)nData);
            break;
        case BSON_OID:
            break;
        case BSON_BOOL:
            bson_append_bool(b, str, (bson_bool_t)nData);
            break;
        case BSON_DATE:
            bson_append_date(b, str, (bson_date_t)nData);
            break;
        case BSON_NULL:
            bson_append_null(b, str);
            break;
        case BSON_REGEX:
            if (!szData)
                 szData=(char *)"";
            bson_append_regex(b, str, szData, opts);
            break;
        case BSON_DBREF:
            break;
        case BSON_CODE:
            if (!szData)
                 szData=(char *)"";
            bson_append_code_n(b, str, szData, (int)nData);
            break;
        case BSON_SYMBOL:
            if (!szData)
                 szData=(char *)"";
            bson_append_symbol_n(b, str, szData, (int)nData);
            break;
        case BSON_CODEWSCOPE:
            break;
        case BSON_INT:
            bson_append_int(b, str, (int)nData);
            break;
        case BSON_TIMESTAMP:
            bson_append_time_t(b, str, (time_t)nData);
            break;
        case BSON_LONG:
            bson_append_long(b, str, (int64_t)nData);
            break;
        case BSON_UNDEFINED:
        default:
            break;
    }
   }*/
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//-----------------------------------------------------//
void do_object(bson *b, void *pData, bool keep_types) {
    char *class_name   = 0;
    int  members_count = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);

    if (members_count > 0) {
        char          **members       = new char * [members_count];
        unsigned char *flags          = new unsigned char[members_count];
        char          *access         = new char[members_count];
        char          *types          = new char[members_count];
        char          **szValues      = new char * [members_count];
        NUMBER        *nValues        = new NUMBER[members_count];
        void          **class_data    = new void * [members_count];
        void          **variable_data = new void * [members_count];

        int result = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);

        if (IS_OK(result)) {
            if (keep_types)
                bson_append_string(b, CONCEPT_CLASS_TYPE, class_name);
            for (int i = 0; i < members_count; i++) {
                if (flags[i] == 0) {
                    switch (types[i]) {
                        case VARIABLE_STRING:
                            if (strcmp(members[i], "__DBID"))
                                bson_append_string_n(b, members[i], szValues[i], (int)nValues[i]);
                            break;

                        case VARIABLE_NUMBER:
                            if (strcmp(members[i], "__DBID")) {
                                BSON_NUMBER(b, members[i], nValues[i]);
                            }
                            //bson_append_double(b, members[i], nValues[i]);
                            break;

                        case VARIABLE_ARRAY:
                            {
                                bson_append_start_array(b, members[i]);
                                do_array(b, variable_data[i], false, false, keep_types);
                                bson_append_finish_array(b);
                            }
                            break;

                        case VARIABLE_CLASS:
                            {
                                bson_append_start_object(b, members[i]);
                                do_object(b, class_data[i], keep_types);
                                bson_append_finish_object(b);
                            }
                            break;

                        default:
                            break;
                    }
                } else
                if (flags[i] == 0xFF) {
                    // is a null variable !
                    if (strcmp(members[i], "__DBID")) {
                        bson_append_int(b, members[i], 0);
                    }
                }
            }
        }
        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;
    }
}

//-----------------------------------------------------//
void do_array(bson *b, void *arr, bool is_oid, bool as_object, bool keep_types, bool look_for_eval) {
    void *newpData;
    const char *key;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    for (int i = 0; i < count; i++) {
        newpData = 0;
        key      = 0;
        bool loco_oid = is_oid;
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
        AnsiString idx((long)i);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            if ((key) && (!loco_oid) && (!strcmp(key, "_id")))
                loco_oid = true;

            if ((key) && (key[0] == '&')) {
                key++;
                loco_oid = true;
            }

            if (!key)
                key = idx.c_str();

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            switch (type) {
                case VARIABLE_STRING:
                    if (loco_oid) {
                        bson_oid_t oid[1];
                        if (!szData)
                            szData = (char *)empty_string;
                        bson_oid_from_string(oid, szData);

                        bson_append_oid(b, key, oid);
                    } else
                    if ((look_for_eval) && (key) && (key[0] == '%') && (key[1])) {
                        int  len    = (int)nData;
                        const char *flags = empty_string;
                        char c[2];
                        c[1] = 0;
                        char *buf = 0;
                        if ((szData) && (len > 1) && (szData[0] == '/') && (szData[len - 2] == '/')) {
                            c[0] = szData[len - 1];
                            if ((c[0] == 'i') || (c[0] == 'm') || (c[0] == 'x') || (c[0] == 's')) {
                                flags        = c;
                                buf          = (char *)malloc(len - 2);
                                buf[len - 3] = 0;
                                memcpy(buf, szData + 1, len - 3);
                            }
                        }
                        if (buf) {
                            bson_append_regex(b, ++key, buf, flags);
                            free(buf);
                        } else
                            bson_append_regex(b, ++key, szData, flags);
                    } else
                        bson_append_string_n(b, key, szData, (int)nData);
                    break;

                case VARIABLE_NUMBER:
                    BSON_NUMBER(b, key, nData);
                    break;

                case VARIABLE_ARRAY:
                    {
                        bool l_as_object = as_object;
                        if ((key) && (as_object) && (key[0] == '$') &&
                            ((!strcmp(key, "$and")) || (!strcmp(key, "$all")) || (!strcmp(key, "$or")) || (!strcmp(key, "$nor")) ||
                             (!strcmp(key, "$in")) || (!strcmp(key, "$nin")) || (!strcmp(key, "$mod")) || (!strcmp(key, "$each")) ||
                             (!strcmp(key, "$pullAll"))))
                            l_as_object = false;

                        if (l_as_object)
                            bson_append_start_object(b, key);
                        else
                            bson_append_start_array(b, key);
                        do_array(b, newpData, loco_oid, as_object, keep_types, look_for_eval);
                        if (l_as_object)
                            bson_append_finish_object(b);
                        else
                            bson_append_finish_array(b);
                    }
                    break;

                case VARIABLE_CLASS:
                    {
                        bson_append_start_object(b, key);
                        do_object(b, szData, keep_types);
                        bson_append_finish_object(b);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------//
void BuildCond(bson *b, void *arr, bool as_object = false, bool look_for_eval = false) {
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            void *newpData;
            InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char *key = 0;
                InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
                bool is_oid = false;
                bool eval   = false;

                if ((look_for_eval) && (key)) {
                    if (!strcmp(key, "eval"))
                        eval = true;
                    else
                    if (!strcmp(key, "reduce"))
                        eval = true;
                    else
                    if (!strcmp(key, "map"))
                        eval = true;
                    else

                    /*if (!strcmp(key, "mapreduce"))
                        eval=true;
                       else*/
                    if (!strcmp(key, "finalize"))
                        eval = true;
                }

                if ((key) && ((!strcmp(key, "_id")) /* || (!strcmp(key, "$oid"))*/))
                    is_oid = true;

                if ((key) && (key[0] == '&')) {
                    key++;
                    is_oid = true;
                }

                if (key) {
                    char    *szData;
                    INTEGER type;
                    NUMBER  nData;

                    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if (type == VARIABLE_CLASS) {
                        bson_append_start_object(b, key);
                        do_object(b, szData);
                        bson_append_finish_object(b);
                    } else
                    if (type == VARIABLE_ARRAY) {
                        bool l_as_object = as_object;
                        if ((key) && (as_object) && (key[0] == '$') &&
                            ((!strcmp(key, "$and")) || (!strcmp(key, "$all")) || (!strcmp(key, "$or")) || (!strcmp(key, "$nor")) ||
                             (!strcmp(key, "$in")) || (!strcmp(key, "$nin")) || (!strcmp(key, "$mod")) || (!strcmp(key, "$each")) ||
                             (!strcmp(key, "$pullAll"))))
                            l_as_object = false;

                        if (l_as_object) {
                            bson_append_start_object(b, key);
                        } else
                            bson_append_start_array(b, key);
                        do_array(b, newpData, is_oid, as_object, false, look_for_eval);
                        if (l_as_object)
                            bson_append_finish_object(b);
                        else
                            bson_append_finish_array(b);
                    } else
                    if (type == VARIABLE_STRING) {
                        if (is_oid) {
                            bson_oid_t oid[1];
                            if (!szData)
                                szData = (char *)empty_string;
                            bson_oid_from_string(oid, szData);
                            bson_append_oid(b, key, oid);
                        } else
                        if (eval)
                            bson_append_code_n(b, key, szData, (int)nData);
                        else
                        if ((look_for_eval) && (key) && (key[0] == '%') && (key[1])) {
                            int  len    = (int)nData;
                            const char *flags = empty_string;
                            char c[2];
                            c[1] = 0;
                            char *buf = 0;
                            if ((szData) && (len > 1) && (szData[0] == '/') && (szData[len - 2] == '/')) {
                                c[0] = szData[len - 1];
                                if ((c[0] == 'i') || (c[0] == 'm') || (c[0] == 'x') || (c[0] == 's')) {
                                    flags        = c;
                                    buf          = (char *)malloc(len - 2);
                                    buf[len - 3] = 0;
                                    memcpy(buf, szData + 1, len - 3);
                                }
                            }
                            if (buf) {
                                bson_append_regex(b, ++key, buf, flags);
                                free(buf);
                            } else
                                bson_append_regex(b, ++key, szData, flags);
                            //bson_append_regex(b, ++key, szData, "i");
                        } else
                            bson_append_string_n(b, key, szData, (int)nData);
                    } else
                    if (type == VARIABLE_NUMBER)
                        bson_append_double(b, key, nData);
                }
            }
        }
    }
}

//-----------------------------------------------------//
int BuildFields(bson *b, void *arr, bool as_object = false, bool add_type = false) {
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            void *newpData;
            InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *key = 0;
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
                if (key) {
                    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if (type == VARIABLE_CLASS) {
                        bson_append_start_object(b, key);
                        do_object(b, szData);
                        bson_append_finish_object(b);
                    } else
                    if (type == VARIABLE_ARRAY) {
                        if (as_object)
                            bson_append_start_object(b, key);
                        else
                            bson_append_start_array(b, key);
                        do_array(b, newpData);
                        if (as_object)
                            bson_append_finish_object(b);
                        else
                            bson_append_finish_array(b);
                    } else
                    if (type == VARIABLE_STRING)
                        bson_append_string_n(b, key, szData, (int)nData);
                    else
                    if (type == VARIABLE_NUMBER)
                        bson_append_double(b, key, nData);
                } else {
                    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if ((type == VARIABLE_STRING) && (nData))
                        bson_append_int(b, szData, 1);
                }
            }
        }
        if (add_type)
            bson_append_int(b, CONCEPT_CLASS_TYPE, 1);
    }
    return count;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoConnect, 0, 3)
    const char *host = "127.0.0.1";
    int port = 27017;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(MongoConnect, 1)
        host = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MongoConnect, 2)
        port = PARAM_INT(2);
    }
    mongo *conn = (mongo *)malloc(sizeof(mongo));
    mongo_init(conn);
    mongo_client(conn, host, port);
    if (PARAMETERS_COUNT > 0) {
        SET_NUMBER(0, conn->err);
    }
    switch (conn->err) {
        case MONGO_CONN_NO_SOCKET:
        case MONGO_CONN_FAIL:
        case MONGO_CONN_ADDR_FAIL:
        case MONGO_CONN_NO_PRIMARY:
            lasterror = (char *)conn->errstr;
            mongo_destroy(conn);
            free(conn);
            conn = 0;
            break;
    }
    RETURN_NUMBER((SYS_INT)conn)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoConnectReplica, 3)
    T_STRING(MongoConnectReplica, 1)
    T_ARRAY(MongoConnectReplica, 2)

    mongo * conn = (mongo *)malloc(sizeof(mongo));
    mongo_replica_set_init(conn, PARAM(1));

    void *arr  = PARAMETER(2);
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    for (int i = 0; i < count; i++) {
        void *newpData;
        Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_ARRAY) {
                void *newpData2;
                const char *server = "127.0.0.1";
                int  port    = 27017;
                Invoke(INVOKE_ARRAY_VARIABLE, newpData, (int)0, &newpData2);
                Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    server = szData;
                Invoke(INVOKE_ARRAY_VARIABLE, newpData, (int)1, &newpData2);
                Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                if (type == VARIABLE_NUMBER) {
                    port = (INTEGER)nData;
                    if (port <= 0)
                        port = 27017;
                }

                if ((port) && (server))
                    mongo_replica_set_add_seed(conn, server, port);
            }
        }
    }

    mongo_replica_set_client(conn);
    SET_NUMBER(0, conn->err);

    switch (conn->err) {
        case MONGO_CONN_NO_SOCKET:
        case MONGO_CONN_FAIL:
        case MONGO_CONN_ADDR_FAIL:
        case MONGO_CONN_NO_PRIMARY:
            mongo_destroy(conn);
            free(conn);
            conn = 0;
            break;
    }
    RETURN_NUMBER((SYS_INT)conn)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoDisconnect, 1)
    T_NUMBER(MongoDisconnect, 0)
    mongo * conn = (mongo *)PARAM_INT(0);
//lasterror=(char *)"";
    if (conn) {
        mongo_destroy(conn);
        free(conn);
        SET_NUMBER(0, 0);
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoError, 0, 1)
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(MongoError, 0);
        mongo *conn = (mongo *)PARAM_INT(0);
        if (conn) {
            RETURN_STRING(conn->lasterrstr);
            return 0;
        }
    }

    RETURN_BUFFER(lasterror.c_str(), lasterror.Length());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoDropDb, 2)
    T_HANDLE(MongoDropDb, 0)
    T_STRING(MongoDropDb, 1)
//lasterror=(char *)"";
    mongo * conn = (mongo *)PARAM_INT(0);
    int res = mongo_cmd_drop_db(conn, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoAddUser, 4)
    T_HANDLE(MongoAddUser, 0)
    T_STRING(MongoAddUser, 1)
    T_STRING(MongoAddUser, 2)
    T_STRING(MongoAddUser, 3)
//lasterror=(char *)"";
    mongo * conn = (mongo *)PARAM_INT(0);
    int res = mongo_cmd_add_user(conn, PARAM(1), PARAM(2), PARAM(3));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoAuthenticate, 4)
    T_HANDLE(MongoAuthenticate, 0)
    T_STRING(MongoAuthenticate, 1)
    T_STRING(MongoAuthenticate, 2)
    T_STRING(MongoAuthenticate, 3)
//lasterror=(char *)"";
    mongo * conn = (mongo *)PARAM_INT(0);
    int res = mongo_cmd_authenticate(conn, PARAM(1), PARAM(2), PARAM(3));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoInsert, 3, 6)
    T_HANDLE(MongoInsert, 0)
    T_STRING(MongoInsert, 1)

    int res = -2;
//lasterror=(char *)"";
    mongo *conn             = (mongo *)PARAM_INT(0);
    bool  keep_object_types = true;

    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MongoInsert, 3)
        keep_object_types = (bool)PARAM_INT(3);
    }
    char    *szData;
    INTEGER type;
    NUMBER  nData;
    const char *key = "_id";
    if (PARAMETERS_COUNT > 4) {
        T_STRING(MongoInsert, 4)
        key = PARAM(4);
    }
    if (PARAMETERS_COUNT > 5) {
        SET_STRING(5, "");
    }


    Invoke(INVOKE_GET_VARIABLE, PARAMETER(2), &type, &szData, &nData);
    switch (type) {
        case VARIABLE_CLASS:
            {
                bson b[1];
                bson_init(b);

                if ((key) && (key[0]))
                    bson_append_new_oid(b, key);
                do_object(b, szData, keep_object_types);
                bson_finish(b);

                res = mongo_insert(conn, PARAM(1), b, NULL);

                if (PARAMETERS_COUNT > 5) {
                    bson_iterator i[1];
                    bson_iterator_init(i, b);
                    if (bson_iterator_next(i)) {
                        char oidhex[0xFFF];
                        oidhex[0] = 0;
                        bson_oid_to_string(bson_iterator_oid(i), oidhex);
                        SET_STRING(5, oidhex);
                    }
                }

                bson_destroy(b);
            }
            break;

        case VARIABLE_ARRAY:
            {
                int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
                if (count > 0) {
                    bson **ps = ( bson ** )malloc(sizeof(bson *) * count);
                    for (int i = 0; i < count; i++) {
                        bson *p = ( bson * )malloc(sizeof(bson));
                        bson_init(p);
                        void *newpData;
                        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &newpData);
                        if (newpData) {
                            char    *szData;
                            INTEGER type;
                            NUMBER  nData;

                            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                            if (type == VARIABLE_CLASS) {
                                if ((key) && (key[0]))
                                    bson_append_new_oid(p, key);
                                do_object(p, szData, keep_object_types);
                            } else
                            if (type == VARIABLE_ARRAY) {
                                if (key)
                                    bson_append_new_oid(p, key);
                                do_array(p, newpData, false, false, keep_object_types);
                            }
                        }
                        bson_finish(p);
                        ps[i] = p;
                    }
                    res = mongo_insert_batch(conn, PARAM(1), (const bson **)ps, count, NULL, 0);
                    if (PARAMETERS_COUNT > 5) {
                        CREATE_ARRAY(PARAMETER(5));
                    }
                    for (int i = 0; i < count; i++) {
                        if (PARAMETERS_COUNT > 5) {
                            bson_iterator it[1];
                            bson_iterator_init(it, ps[i]);
                            if (bson_iterator_next(it)) {
                                char oidhex[0xFFF];
                                oidhex[0] = 0;
                                bson_oid_to_string(bson_iterator_oid(it), oidhex);
                                InvokePtr(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(5), (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)oidhex, (NUMBER)0);
                            }
                        }
                        bson_destroy(ps[i]);
                        free(ps[i]);
                    }
                    free(ps);
                }
            }
            break;
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoSetOpTimeout, 2)
    T_HANDLE(MongoSetOpTimeout, 0)
    T_NUMBER(MongoSetOpTimeout, 1)
//lasterror=(char *)"";
    mongo * conn = (mongo *)PARAM_INT(0);
    int res = mongo_set_op_timeout(conn, PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoCheckConnection, 1)
    T_HANDLE(MongoCheckConnection, 0)
    mongo * conn = (mongo *)PARAM_INT(0);
    int res = mongo_check_connection(conn);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoCount, 4)
    T_HANDLE(MongoCount, 0)
    T_STRING(MongoCount, 1)
    T_STRING(MongoCount, 2)
//T_ARRAY(3)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoCount: parameter4 should be an array or an id (string)";

    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(3), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
    }
    bson_finish(b);

    int64_t res = mongo_count(conn, PARAM(1), PARAM(2), b);
    bson_destroy(b);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdate, 4, 6)
    T_HANDLE(MongoUpdate, 0)
    T_STRING(MongoUpdate, 1)
//T_ARRAY(2)
    char *arr_data = 0;
    bool is_array  = false;
    bool as_object = true;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdate: parameter 3 should be an array or an id (string)";
    T_ARRAY(MongoUpdate, 3)
    int flags = MONGO_UPDATE_MULTI;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoUpdate, 4)
        flags = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoUpdate, 5)
        as_object = (bool)PARAM_INT(5);
    }
    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), as_object, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
        if (PARAMETERS_COUNT <= 4) ;
        flags = MONGO_UPDATE_BASIC;
    }
    bson_finish(b);

    bson b2[1];
    bson_init(b2);
    BuildCond(b2, PARAMETER(3), true);
    bson_finish(b2);

    int res = mongo_update(conn, PARAM(1), b, b2, flags, NULL);
    bson_destroy(b);
    bson_destroy(b2);

    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdateType, 4, 8)
    T_HANDLE(MongoUpdateType, 0)
    T_STRING(MongoUpdateType, 1)
//T_ARRAY(2)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdate: parameter 3 should be an array or an id (string)";

    T_STRING(MongoUpdateType, 3)
    T_NUMBER(MongoUpdateType, 4)
//T_STRING(5)
    bool keep_types = false;
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoUpdateType, 6)
        keep_types = PARAM_INT(6);
    }

    int flags = MONGO_UPDATE_MULTI;
    if (PARAMETERS_COUNT > 7) {
        T_NUMBER(MongoUpdateType, 7)
        flags = PARAM_INT(7);
    }
    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
        if (PARAMETERS_COUNT <= 4) ;
        flags = MONGO_UPDATE_BASIC;
    }
    bson_finish(b);

    bson b2[1];
    bson_init(b2);
    bson_append_start_object(b2, "$set");
    char *str = PARAM(3);
    switch (PARAM_INT(4)) {
        case BSON_EOO:
            break;

        case BSON_DOUBLE:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_double(b2, str, PARAM(5));
            }
            break;

        case BSON_STRING:
            {
                T_STRING(MongoUpdateType, 5)
                bson_append_string_n(b2, str, PARAM(5), (int)PARAM_LEN(5));
            }
            break;

        case BSON_OBJECT:
            {
                T_OBJECT(MongoUpdateType, 5)
                bson_append_start_object(b2, str);
                do_object(b2, PARAM(5), keep_types);
                bson_append_finish_object(b2);
            }
            break;

        case BSON_ARRAY:
            {
                T_ARRAY(MongoUpdateType, 5);
                bson_append_start_array(b2, str);
                do_array(b2, PARAMETER(5), false, false, keep_types);
                bson_append_finish_array(b2);
            }
            break;

        case BSON_BINDATA:
            {
                T_STRING(MongoUpdateType, 5)
                bson_append_binary(b2, str, PARAM_INT(4), PARAM(5), PARAM_LEN(5));
            }
            break;

        case BSON_OID:
            {
                T_STRING(MongoUpdateType, 5)
                bson_oid_t oid[1];
                if (PARAM_LEN(5))
                    bson_oid_from_string(oid, PARAM(5));
                bson_append_oid(b2, str, oid);
            }
            break;

        case BSON_BOOL:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_bool(b2, str, (bson_bool_t)PARAM_INT(5));
            }
            break;

        case BSON_DATE:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_date(b2, str, (bson_date_t)PARAM_INT(5));
            }
            break;

        case BSON_NULL:
            bson_append_null(b2, str);
            break;

        case BSON_REGEX:
            {
                T_STRING(MongoUpdateType, 5)
                bson_append_regex(b2, str, PARAM(5), "");
            }
            break;

        case BSON_DBREF:
            break;

        case BSON_CODE:
            {
                T_STRING(MongoUpdateType, 5)
                bson_append_code_n(b2, str, PARAM(5), (int)PARAM_LEN(5));
            }
            break;

        case BSON_SYMBOL:
            {
                T_STRING(MongoUpdateType, 5)
                bson_append_symbol_n(b2, str, PARAM(5), (int)PARAM_LEN(5));
            }
            break;

        case BSON_CODEWSCOPE:
            break;

        case BSON_INT:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_int(b2, str, PARAM_INT(5));
            }
            break;

        case BSON_TIMESTAMP:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_time_t(b2, str, (time_t)PARAM_INT(5));
            }
            break;

        case BSON_LONG:
            {
                T_NUMBER(MongoUpdateType, 5)
                bson_append_long(b2, str, (int64_t)PARAM(5));
            }
            break;

        case BSON_UNDEFINED:
            bson_append_undefined(b2, str);

        default:
            break;
    }
    bson_append_finish_object(b2);
    bson_finish(b2);
    int res = mongo_update(conn, PARAM(1), b, b2, flags, NULL);
    bson_destroy(b);
    bson_destroy(b2);

    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdateObject, 4, 6)
    T_HANDLE(MongoUpdateObject, 0)
    T_STRING(MongoUpdateObject, 1)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdateObject: parameter 3 should be an array or an id (string)";

//T_ARRAY(2)
    T_OBJECT(MongoUpdateObject, 3)
    bool as_object = false;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoUpdateObject, 4)
        as_object = (bool)PARAM_INT(4);
    }

    int flags = MONGO_UPDATE_MULTI;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoUpdateObject, 5)
        flags = PARAM_INT(5);
    }
    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
        if (PARAMETERS_COUNT <= 5) ;
        flags = MONGO_UPDATE_BASIC;
    }
    bson_finish(b);

    bson b2[1];
    bson_init(b2);
    bson_append_start_object(b2, "$set");
    do_object(b2, PARAM(3), as_object);
    bson_append_finish_object(b2);
    bson_finish(b2);

    int res = mongo_update(conn, PARAM(1), b, b2, flags, NULL);
    bson_destroy(b);
    bson_destroy(b2);

    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoRemove, 3)
    T_HANDLE(MongoRemove, 0)
    T_STRING(MongoRemove, 1)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoRemove: parameter 3 should be an array or an id (string)";
//T_ARRAY(2)

    mongo *conn = (mongo *)PARAM_INT(0);

/*bson b[1];
   bson_init(b);
   BuildCond(b, PARAMETER(2), true);
   bson_finish(b);*/
    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
    }
    bson_finish(b);
    int res = mongo_remove(conn, PARAM(1), b, NULL);
    bson_destroy(b);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoFind, 3, 7)
    T_HANDLE(MongoFind, 0)
    T_STRING(MongoFind, 1)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoFind: parameter 3 should be an array or an id (string)";

    int skip  = 0;
    int limit = 0;
    int flags = 0;

    bson b2[1];
    bson_init(b2);

    bson *pb2 = 0;

    if (PARAMETERS_COUNT > 3) {
        T_ARRAY(MongoFind, 3)
        if (BuildFields(b2, PARAMETER(3), true, true))
            pb2 = b2;
    }
    bson_finish(b2);

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoFind, 4)
        limit = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoFind, 5)
        skip = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoFind, 6)
        flags = PARAM_INT(6);
    }
    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
    }
    bson_finish(b);

    mongo_cursor *cursor = 0;
    if (PARAMETERS_COUNT > 3)
        cursor = mongo_find(conn, PARAM(1), b, pb2, skip, limit, flags);
    else
        cursor = mongo_find(conn, PARAM(1), b, 0, skip, limit, flags);
    bson_destroy(b);
    bson_destroy(b2);
    RETURN_NUMBER((SYS_INT)cursor);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoNext, 1)
    T_HANDLE(MongoNext, 0)
    mongo_cursor * cursor = (mongo_cursor *)PARAM_INT(0);

    int res = mongo_cursor_next(cursor);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoClose, 1)
    T_HANDLE(MongoClose, 0)
    mongo_cursor * cursor = (mongo_cursor *)PARAM_INT(0);

    int res = mongo_cursor_destroy(cursor);
//free(cursor);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
const char *PeekClass(bson_iterator *base) {
    bson_iterator i;
    const char    *class_name = 0;

    i.cur   = base->cur;
    i.first = base->first;

    while (bson_iterator_next(&i)) {
        if (strcmp(CONCEPT_CLASS_TYPE, bson_iterator_key(&i)) == 0) {
            class_name = bson_iterator_string(&i);
            if ((class_name) && (!class_name[0]))
                class_name = 0;
            break;
        }
    }
    return class_name;
}

//-----------------------------------------------------//
void DoBSON(void *ConceptHandler, void *ARR, bson_iterator *i, bool use_type = false) {
    //bson_iterator i[1];
    char        oidhex[0xFFF];
    const char  *class_name = 0;
    INVOKE_CALL Invoke      = InvokePtr;

    if (!use_type) {
        CREATE_ARRAY(ARR);
    } else {
        class_name = PeekClass(i);
        if (class_name) {
            if (!IS_OK(Invoke(INVOKE_CREATE_OBJECT, ConceptHandler, ARR, class_name))) {
                // class does not exists
                class_name = 0;
            }
        }
        if (!class_name)
            CREATE_ARRAY(ARR);
    }

    while (bson_iterator_next(i)) {
        bson_type t = bson_iterator_type(i);
        if (t == 0)
            break;
        int        type    = -1;
        double     nData   = 0;
        const char *szData = "";
        const char *key    = bson_iterator_key(i);
        switch (t) {
            case BSON_DOUBLE:
                type  = VARIABLE_NUMBER;
                nData = bson_iterator_double(i);
                break;

            case BSON_STRING:
                type  = VARIABLE_STRING;
                nData = bson_iterator_string_len(i);
                // it has the null terminator
                szData = bson_iterator_string(i);
                if ((nData > 0) && (!szData[(int)nData - 1]))
                    nData--;
                break;

            case BSON_OBJECT:
            case BSON_ARRAY:
                {
                    bson_iterator sub[1];
                    bson_iterator_subiterator(i, sub);
                    void *subarr = 0;
                    if (class_name) {
                        if (!IS_OK(Invoke(INVOKE_GET_CLASS_VARIABLE, ARR, (char *)key, &subarr)))
                            subarr = 0;
                    } else {
                        if (!IS_OK(InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, ARR, (char *)key, &subarr)))
                            subarr = 0;
                    }

                    if (subarr) {
                        //InvokePtr(INVOKE_CREATE_ARRAY,subarr);
                        DoBSON(ConceptHandler, subarr, sub, use_type);
                    }
                }
                break;

            case BSON_BINDATA:
                type  = VARIABLE_STRING;
                nData = bson_iterator_bin_len(i);
                //if (nData>0)
                //    nData--;
                szData = nData ? bson_iterator_bin_data(i) : "";
                break;

            case BSON_OID:
                type      = VARIABLE_STRING;
                oidhex[0] = 0;
                bson_oid_to_string(bson_iterator_oid(i), oidhex);
                szData = oidhex;
                break;

            case BSON_BOOL:
                type  = VARIABLE_NUMBER;
                nData = bson_iterator_bool(i);
                break;

            case BSON_DATE:
                type  = VARIABLE_NUMBER;
                nData = bson_iterator_date(i);
                break;

            case BSON_NULL:
            case BSON_UNDEFINED:
            case BSON_DBREF:
                type = VARIABLE_NUMBER;
                break;

            case BSON_REGEX:
                type   = VARIABLE_STRING;
                szData = bson_iterator_regex(i);
                break;

            case BSON_CODE:
            case BSON_CODEWSCOPE:
                type   = VARIABLE_STRING;
                szData = bson_iterator_code(i);
                break;

            case BSON_INT:
                type  = VARIABLE_NUMBER;
                nData = bson_iterator_int(i);
                break;

            case BSON_TIMESTAMP:
                {
                    type = VARIABLE_NUMBER;
                    bson_timestamp_t stamp = bson_iterator_timestamp(i);
                    nData = *(int64_t *)&stamp;
                }
                break;

            case BSON_LONG:
                type  = VARIABLE_NUMBER;
                nData = bson_iterator_long(i);
                break;

            case BSON_SYMBOL:
            case BSON_EOO:
            default:
                type   = VARIABLE_STRING;
                szData = bson_iterator_value(i);
                break;
        }
        if ((type == VARIABLE_STRING) && (!szData))
            szData = "";
        if (((type == VARIABLE_NUMBER) || (type == VARIABLE_STRING)) && (key)) {
            if (class_name) {
                void *handler = 0;
                int  r        = Invoke(INVOKE_GET_CLASS_VARIABLE, ARR, (char *)key, &handler);
                if ((!IS_OK(r)) || (!handler)) {
                    if (t == BSON_OID) {
                        if (!IS_OK(Invoke(INVOKE_GET_CLASS_VARIABLE, ARR, (char *)"__DBID", &handler)))
                            handler = 0;
                    } else
                        handler = 0;
                }
                if (handler)
                    Invoke(INVOKE_SET_VARIABLE, handler, (INTEGER)type, szData, nData);
            } else
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)key, (INTEGER)type, (char *)szData, (NUMBER)nData);
        }
    }
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoData, 1, 2)
    T_HANDLE(MongoData, 0)
    bool use_type = false;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(MongoData, 1)
        use_type = (bool)PARAM_INT(1);
    }
    mongo_cursor  *cursor = (mongo_cursor *)PARAM_INT(0);
    const bson    *b      = mongo_cursor_bson(cursor);
    bson_iterator i[1];
    bson_iterator_init(i, b);
    DoBSON(PARAMETERS->HANDLER, RESULT, i, use_type);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoFindOne, 3, 6)
    T_HANDLE(MongoFindOne, 0)
    T_STRING(MongoFindOne, 1)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoFindOne: parameter 3 should be an array or an id (string)";

    bool use_type = false;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoFindOne, 5);
        use_type = (bool)PARAM_INT(5);
    }

    bson b2[1];
    bson_init(b2);
    bson *pb2 = 0;
    if (PARAMETERS_COUNT > 3) {
        T_ARRAY(MongoFindOne, 3)
        if (BuildFields(b2, PARAMETER(3), true, use_type))
            pb2 = b2;
    }
    bson_finish(b2);

    mongo *conn = (mongo *)PARAM_INT(0);

    bson b[1];
    bson_init(b);
    if (is_array) {
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            bson_oid_t oid[1];
            bson_oid_from_string(oid, arr_data);
            bson_append_oid(b, "_id", oid);
        }
    }
    bson_finish(b);

    int  res = 0;
    bson out[1];
    if (PARAMETERS_COUNT > 3)
        res = mongo_find_one(conn, PARAM(1), b, 0, out);
    else
        res = mongo_find_one(conn, PARAM(1), b, 0, out);
    bson_destroy(b);
    bson_destroy(b2);
    if (PARAMETERS_COUNT > 4) {
        if (res) {
            SET_NUMBER(4, 0);
        } else {
            bson_iterator i[1];
            bson_iterator_init(i, out);
            DoBSON(PARAMETERS->HANDLER, PARAMETER(4), i, use_type);
            bson_destroy(out);
        }
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoIsMaster, 1, 2)
    T_HANDLE(MongoIsMaster, 0)
    mongo * conn = (mongo *)PARAM_INT(0);
    bson out[1];
    int  res = mongo_cmd_ismaster(conn, out);
    if (PARAMETERS_COUNT > 1) {
        CREATE_ARRAY(PARAMETER(1));
        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(1), i);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoLastError, 2, 3)
    T_HANDLE(MongoLastError, 0)
    T_STRING(MongoLastError, 1)
    mongo * conn = (mongo *)PARAM_INT(0);
    bson out;
    int  res = mongo_cmd_get_last_error(conn, PARAM(1), &out);
    if (PARAMETERS_COUNT > 2) {
        CREATE_ARRAY(PARAMETER(2));
        bson_iterator i;
        bson_iterator_init(&i, &out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(2), &i);
        bson_destroy(&out);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoPrevError, 2, 3)
    T_HANDLE(MongoPrevError, 0)
    T_STRING(MongoPrevError, 1)
    mongo * conn = (mongo *)PARAM_INT(0);
    bson out;
    int  res = mongo_cmd_get_prev_error(conn, PARAM(1), &out);
    if (PARAMETERS_COUNT > 2) {
        CREATE_ARRAY(PARAMETER(2));
        bson_iterator i;
        bson_iterator_init(&i, &out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(2), &i);
        bson_destroy(&out);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoDropCollection, 3, 4)
    T_HANDLE(MongoDropCollection, 0)
    T_STRING(MongoDropCollection, 1)
    T_STRING(MongoDropCollection, 2)
    mongo * conn = (mongo *)PARAM_INT(0);
    bson out[1];
    int  res = mongo_cmd_drop_collection(conn, PARAM(1), PARAM(2), out);
    if (PARAMETERS_COUNT > 3) {
        CREATE_ARRAY(PARAMETER(3));
        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(3), i);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoSimpleCommand, 4, 6)
    T_HANDLE(MongoSimpleCommand, 0)
    T_STRING(MongoSimpleCommand, 1)
    T_STRING(MongoSimpleCommand, 2)

    bool is_number = true;

    INTEGER type = 0;
    char    *szData;
    NUMBER  nData;

    Invoke(INVOKE_GET_VARIABLE, PARAMETER(3), &type, &szData, &nData);
    if (type == VARIABLE_STRING)
        is_number = false;

    mongo *conn = (mongo *)PARAM_INT(0);
    bson  out[1];

    int res;
    if (is_number)
        res = mongo_simple_int_command(conn, PARAM(1), PARAM(2), (int)nData, out);
    else
        res = mongo_simple_str_command(conn, PARAM(1), PARAM(2), szData, out);

    if (PARAMETERS_COUNT > 4) {
        bool use_types = false;
        if (PARAMETERS_COUNT > 5) {
            T_NUMBER(MongoSimpleCommand, 5)
            use_types = (bool)PARAM_INT(5);
        }

        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(4), i, use_types);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoCommand, 3, 5)
    T_HANDLE(MongoCommand, 0)
    T_STRING(MongoCommand, 1)
    T_ARRAY(MongoCommand, 2)

    mongo * conn = (mongo *)PARAM_INT(0);
    bson b[1];
    bson_init(b);
    BuildCond(b, PARAMETER(2), true, true);
    bson_finish(b);

    bson out[1];
    int  res = mongo_run_command(conn, PARAM(1), b, out);

    bson_destroy(b);
    if (PARAMETERS_COUNT > 3) {
        bool use_types = false;
        if (PARAMETERS_COUNT > 4) {
            T_NUMBER(MongoCommand, 4)
            use_types = (bool)PARAM_INT(4);
        }
        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(3), i, use_types);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoIndex, 3, 5)
    T_HANDLE(MongoIndex, 0)
    T_STRING(MongoIndex, 1)
    T_ARRAY(MongoIndex, 2)
    int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MongoIndex, 3);
        flags = PARAM_INT(3);
    }

    mongo *conn = (mongo *)PARAM_INT(0);
    bson  b[1];
    bson_init(b);
    BuildCond(b, PARAMETER(2), true, true);
    bson_finish(b);

    bson out[1];
    int  res = mongo_create_index(conn, PARAM(1), b, flags, out);

    bson_destroy(b);
    if (PARAMETERS_COUNT > 4) {
        CREATE_ARRAY(PARAMETER(4));
        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(4), i);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoSimpleIndex, 3, 5)
    T_HANDLE(MongoSimpleIndex, 0)
    T_STRING(MongoSimpleIndex, 1)
    T_STRING(MongoSimpleIndex, 2)
    int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MongoSimpleIndex, 3);
        flags = PARAM_INT(3);
    }

    mongo *conn = (mongo *)PARAM_INT(0);

    bson out[1];
    int  res = mongo_create_simple_index(conn, PARAM(1), PARAM(2), flags, out);

    if (PARAMETERS_COUNT > 4) {
        CREATE_ARRAY(PARAMETER(4));
        bson_iterator i[1];
        bson_iterator_init(i, out);
        DoBSON(PARAMETERS->HANDLER, PARAMETER(4), i);
    }
    bson_destroy(out);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSInit, 3)
    T_HANDLE(GridFSInit, 0)
    T_STRING(GridFSInit, 1)
    T_STRING(GridFSInit, 2)

    mongo * conn = (mongo *)PARAM_INT(0);
    gridfs *gfs = (gridfs *)malloc(sizeof(gridfs));

    if (gridfs_init(conn, PARAM(1), PARAM(2), gfs) != MONGO_OK) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER((SYS_INT)gfs);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSDone, 1)
    T_HANDLE(GridFSDone, 0)

    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfs_destroy(gfs);
    free(gfs);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(GridFSStore, 3, 4)
    T_HANDLE(GridFSStore, 0)
    T_STRING(GridFSStore, 1)
    T_STRING(GridFSStore, 2)
    const char *type = "application/octet-stream";

    gridfs *gfs = (gridfs *)PARAM_INT(0);
    if (PARAMETERS_COUNT > 3) {
        T_STRING(GridFSStore, 3)
        type = PARAM(3);
    }
    if (PARAM_LEN(1) > 0) {
        gridfs_remove_filename(gfs, PARAM(1));
        int res = gridfs_store_buffer(gfs, PARAM(2), PARAM_LEN(2), PARAM(1), type);
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSGet, 2)
    T_HANDLE(GridFSGet, 0)
    T_STRING(GridFSGet, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        int len = gridfile_get_contentlength(gfile);
        if (len > 0) {
            char *buf = 0;
            CORE_NEW(len + 1, buf);
            buf[len] = 0;
            if (buf) {
                int size = gridfile_read(gfile, len, buf);
                if (size > 0) {
                    SetVariable(RESULT, -1, buf, size);
                } else {
                    CORE_DELETE(buf);
                    RETURN_STRING("");
                }
            } else {
                RETURN_STRING("");
            }
        } else {
            RETURN_STRING("");
        }
        gridfile_destroy(gfile);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSRemove, 2)
    T_HANDLE(GridFSRemove, 0)
    T_STRING(GridFSRemove, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfs_remove_filename(gfs, PARAM(1));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSExists, 2)
    T_HANDLE(GridFSExists, 0)
    T_STRING(GridFSExists, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    int      res = 0;
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        res = gridfile_exists(gfile);
        gridfile_destroy(gfile);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSSize, 2)
    T_HANDLE(GridFSSize, 0)
    T_STRING(GridFSSize, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    RETURN_NUMBER(0);
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        int len = gridfile_get_contentlength(gfile);
        RETURN_NUMBER(len);
        gridfile_destroy(gfile);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSType, 2)
    T_HANDLE(GridFSType, 0)
    T_STRING(GridFSType, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    RETURN_STRING("");
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        const char *c = gridfile_get_contenttype(gfile);
        if (c) {
            RETURN_STRING(c);
        }
        gridfile_destroy(gfile);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSDate, 2)
    T_HANDLE(GridFSDate, 0)
    T_STRING(GridFSDate, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    RETURN_NUMBER(0);
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        SYS_INT date = gridfile_get_uploaddate(gfile);
        RETURN_NUMBER(date);
        gridfile_destroy(gfile);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSMD5, 2)
    T_HANDLE(GridFSMD5, 0)
    T_STRING(GridFSMD5, 1)
    gridfs * gfs = (gridfs *)PARAM_INT(0);
    gridfile gfile[1];
    RETURN_STRING("");
    if (gridfs_find_filename(gfs, PARAM(1), gfile) == MONGO_OK) {
        const char *c = gridfile_get_md5(gfile);
        if (c) {
            RETURN_STRING(c);
        }
        gridfile_destroy(gfile);
    }
END_IMPL
//-----------------------------------------------------//

