//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <bson.h>
#include <mongoc.h>

#define CONCEPT_CLASS_TYPE    "classof()"
void do_array(bson_t *b, void *arr, bool is_oid = false, bool as_object = false, bool keep_types = false, bool look_for_eval = false);
void do_object(bson_t *b, void *pData, bool keep_types = false);

static INVOKE_CALL InvokePtr = 0;

#ifdef _WIN32
__declspec(thread) bson_error_t last_error;
__declspec(thread) bson_error_t last_error2;
#else
static __thread bson_error_t last_error;
static __thread bson_error_t last_error2;
#endif

void ResetError() {
    if (last_error.code)
        memset(&last_error, 0, sizeof(last_error));
    if (last_error2.code)
        memset(&last_error2, 0, sizeof(last_error2));
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    mongoc_init();
    ResetError();
    DEFINE_ECONSTANT(BSON_TYPE_EOD)
    DEFINE_ECONSTANT(BSON_TYPE_DOUBLE)
    DEFINE_ECONSTANT(BSON_TYPE_UTF8)
    DEFINE_ECONSTANT(BSON_TYPE_DOCUMENT)
    DEFINE_ECONSTANT(BSON_TYPE_ARRAY)
    DEFINE_ECONSTANT(BSON_TYPE_BINARY)
    DEFINE_ECONSTANT(BSON_TYPE_UNDEFINED)
    DEFINE_ECONSTANT(BSON_TYPE_OID)
    DEFINE_ECONSTANT(BSON_TYPE_BOOL)
    DEFINE_ECONSTANT(BSON_TYPE_DATE_TIME)
    DEFINE_ECONSTANT(BSON_TYPE_NULL)
    DEFINE_ECONSTANT(BSON_TYPE_REGEX)
    DEFINE_ECONSTANT(BSON_TYPE_DBPOINTER)
    DEFINE_ECONSTANT(BSON_TYPE_CODE)
    DEFINE_ECONSTANT(BSON_TYPE_SYMBOL)
    DEFINE_ECONSTANT(BSON_TYPE_CODEWSCOPE)
    DEFINE_ECONSTANT(BSON_TYPE_INT32)
    DEFINE_ECONSTANT(BSON_TYPE_TIMESTAMP)
    DEFINE_ECONSTANT(BSON_TYPE_INT64)
    DEFINE_ECONSTANT(BSON_TYPE_MAXKEY)
    DEFINE_ECONSTANT(BSON_TYPE_MINKEY)

    DEFINE_ECONSTANT(MONGOC_UPDATE_NONE)
    DEFINE_ECONSTANT(MONGOC_UPDATE_UPSERT)
    DEFINE_ECONSTANT(MONGOC_UPDATE_MULTI_UPDATE)

    DEFINE_ECONSTANT(MONGOC_REPLY_NONE)
    DEFINE_ECONSTANT(MONGOC_REPLY_CURSOR_NOT_FOUND)
    DEFINE_ECONSTANT(MONGOC_REPLY_QUERY_FAILURE)
    DEFINE_ECONSTANT(MONGOC_REPLY_SHARD_CONFIG_STALE)
    DEFINE_ECONSTANT(MONGOC_REPLY_AWAIT_CAPABLE)

    DEFINE_ECONSTANT(MONGOC_QUERY_NONE)
    DEFINE_ECONSTANT(MONGOC_QUERY_TAILABLE_CURSOR)
    DEFINE_ECONSTANT(MONGOC_QUERY_SLAVE_OK)
    DEFINE_ECONSTANT(MONGOC_QUERY_OPLOG_REPLAY)
    DEFINE_ECONSTANT(MONGOC_QUERY_NO_CURSOR_TIMEOUT)
    DEFINE_ECONSTANT(MONGOC_QUERY_AWAIT_DATA)
    DEFINE_ECONSTANT(MONGOC_QUERY_EXHAUST)
    DEFINE_ECONSTANT(MONGOC_QUERY_PARTIAL)

    DEFINE_ECONSTANT(MONGOC_INSERT_NONE)
    DEFINE_ECONSTANT(MONGOC_INSERT_CONTINUE_ON_ERROR)

    DEFINE_ECONSTANT(MONGOC_REMOVE_NONE)
    DEFINE_ECONSTANT(MONGOC_REMOVE_SINGLE_REMOVE)

    DEFINE_ECONSTANT(MONGOC_DELETE_NONE)
    DEFINE_ECONSTANT(MONGOC_DELETE_SINGLE_REMOVE)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        mongoc_cleanup();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoConnect, 1)
    T_STRING(MongoConnect, 0)
    ResetError();
    mongoc_client_t *client = mongoc_client_new(PARAM(0));
    RETURN_NUMBER((SYS_INT)client);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoDisconnect, 1)
    T_HANDLE(MongoDisconnect, 0)

    mongoc_client_t * client = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_client_destroy(client);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
void do_object(bson_t *b, void *pData, bool keep_types) {
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
                BSON_APPEND_UTF8(b, CONCEPT_CLASS_TYPE, class_name);
            for (int i = 0; i < members_count; i++) {
                if (flags[i] == 0) {
                    switch (types[i]) {
                        case VARIABLE_STRING:
                            if (strcmp(members[i], "__DBID"))
                                bson_append_utf8(b, members[i], strlen(members[i]), szValues[i], (int)nValues[i]);
                            //BSON_APPEND_BINARY(b, members[i], BSON_SUBTYPE_BINARY, (unsigned char *)szValues[i], (int)nValues[i]);
                            break;

                        case VARIABLE_NUMBER:
                            if (strcmp(members[i], "__DBID")) {
                                BSON_APPEND_DOUBLE(b, members[i], nValues[i]);
                            }
                            //bson_append_double(b, members[i], nValues[i]);
                            break;

                        case VARIABLE_ARRAY:
                            {
                                //bson_append_start_array(b, members[i]);
                                bson_t ar;
                                BSON_APPEND_ARRAY_BEGIN(b, members[i], &ar);
                                do_array(&ar, variable_data[i], false, false, keep_types);
                                bson_append_array_end(b, &ar);
                            }
                            break;

                        case VARIABLE_CLASS:
                            {
                                //bson_append_start_object(b, members[i]);
                                bson_t ar;
                                BSON_APPEND_DOCUMENT_BEGIN(b, members[i], &ar);
                                do_object(&ar, class_data[i], keep_types);
                                bson_append_document_end(b, &ar);
                            }
                            break;

                        default:
                            break;
                    }
                } else
                if (flags[i] == 0xFF) {
                    // is a null variable !
                    if (strcmp(members[i], "__DBID")) {
                        BSON_APPEND_INT32(b, members[i], 0);
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
void do_array(bson_t *b, void *arr, bool is_oid, bool as_object, bool keep_types, bool look_for_eval) {
    void *newpData;
    char *key;

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

            //bool loco_oid=false;

            /*if ((key) && (!strcmp(key, "$oid")))
                loco_oid=true;*/

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
                            szData = "";
                        bson_oid_init_from_string(oid, szData);

                        BSON_APPEND_OID(b, key, oid);
                    } else
                    if ((look_for_eval) && (key) && (key[0] == '%') && (key[1])) {
                        int  len    = (int)nData;
                        char *flags = "";
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
                        key++;
                        if (buf) {
                            BSON_APPEND_REGEX(b, key, buf, flags);
                            free(buf);
                        } else
                            BSON_APPEND_REGEX(b, key, szData, flags);
                    } else
                        //bson_append_string_n(b, key, szData, (int)nData);
                        bson_append_utf8(b, key, strlen(key), szData, (int)nData);
                    break;

                case VARIABLE_NUMBER:
                    BSON_APPEND_DOUBLE(b, key, nData);
                    break;

                case VARIABLE_ARRAY:
                    {
                        bool l_as_object = as_object;
                        if ((key) && (as_object) && (key[0] == '$') &&
                            ((!strcmp(key, "$and")) || (!strcmp(key, "$all")) || (!strcmp(key, "$or")) || (!strcmp(key, "$nor")) ||
                             (!strcmp(key, "$in")) || (!strcmp(key, "$nin")) || (!strcmp(key, "$mod")) || (!strcmp(key, "$each")) ||
                             (!strcmp(key, "$pullAll"))))
                            l_as_object = false;
                        bson_t ar;
                        if (l_as_object)
                            BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        else
                            BSON_APPEND_ARRAY_BEGIN(b, key, &ar);
                        do_array(&ar, newpData, loco_oid, as_object, keep_types, look_for_eval);
                        if (l_as_object)
                            bson_append_document_end(b, &ar);
                        else
                            bson_append_array_end(b, &ar);
                    }
                    break;

                case VARIABLE_CLASS:
                    {
                        bson_t ar;
                        BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        do_object(&ar, szData, keep_types);
                        bson_append_document_end(b, &ar);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------//
void BuildCond(bson_t *b, void *arr, bool as_object = false, bool look_for_eval = false) {
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            char *class_name = 0;
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
                        bson_t ar;
                        BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        do_object(&ar, szData);
                        bson_append_document_end(b, &ar);
                    } else
                    if (type == VARIABLE_ARRAY) {
                        bool l_as_object = as_object;
                        if ((key) && (as_object) && (key[0] == '$') &&
                            ((!strcmp(key, "$and")) || (!strcmp(key, "$all")) || (!strcmp(key, "$or")) || (!strcmp(key, "$nor")) ||
                             (!strcmp(key, "$in")) || (!strcmp(key, "$nin")) || (!strcmp(key, "$mod")) || (!strcmp(key, "$each")) ||
                             (!strcmp(key, "$pullAll"))))
                            l_as_object = false;

                        /*if (l_as_object) {
                            bson_append_start_object(b, key);
                           } else
                            bson_append_start_array(b, key);
                           do_array(b, newpData, is_oid, as_object, false, look_for_eval);
                           if (l_as_object)
                            bson_append_finish_object(b);
                           else
                            bson_append_finish_array(b);*/

                        bson_t ar;
                        if (l_as_object)
                            BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        else
                            BSON_APPEND_ARRAY_BEGIN(b, key, &ar);
                        do_array(&ar, newpData, is_oid, as_object, false, look_for_eval);
                        if (l_as_object)
                            bson_append_document_end(b, &ar);
                        else
                            bson_append_array_end(b, &ar);
                    } else
                    if (type == VARIABLE_STRING) {
                        if (is_oid) {
                            bson_oid_t oid[1];
                            if (!szData)
                                szData = "";
                            bson_oid_init_from_string(oid, szData);
                            BSON_APPEND_OID(b, key, oid);
                        } else
                        if (eval)
                            BSON_APPEND_CODE(b, key, szData);
                        else
                        if ((look_for_eval) && (key) && (key[0] == '%') && (key[1])) {
                            int  len    = (int)nData;
                            char *flags = "";
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
                            key++;
                            if (buf) {
                                BSON_APPEND_REGEX(b, key, buf, flags);
                                free(buf);
                            } else
                                BSON_APPEND_REGEX(b, key, szData, flags);
                            //bson_append_regex(b, ++key, szData, "i");
                        } else
                            bson_append_utf8(b, key, strlen(key), szData, (int)nData);
                        //bson_append_string_n(b, key, szData, (int)nData);
                    } else
                    if (type == VARIABLE_NUMBER)
                        BSON_APPEND_DOUBLE(b, key, nData);
                }
            }
        }
    }
}

//-----------------------------------------------------//
int BuildFields(bson_t *b, void *arr, bool as_object = false, bool add_type = false) {
    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            char *class_name = 0;
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
                        /*bson_append_start_object(b, key);
                           do_object(b, szData);
                           bson_append_finish_object(b);*/
                        bson_t ar;
                        BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        do_object(&ar, szData);
                        bson_append_document_end(b, &ar);
                    } else
                    if (type == VARIABLE_ARRAY) {
                        /*if (as_object)
                            bson_append_start_object(b, key);
                           else
                            bson_append_start_array(b, key);
                           do_array(b, newpData);
                           if (as_object)
                            bson_append_finish_object(b);
                           else
                            bson_append_finish_array(b);*/
                        bson_t ar;
                        if (as_object)
                            BSON_APPEND_DOCUMENT_BEGIN(b, key, &ar);
                        else
                            BSON_APPEND_ARRAY_BEGIN(b, key, &ar);
                        do_array(&ar, newpData);
                        if (as_object)
                            bson_append_document_end(b, &ar);
                        else
                            bson_append_array_end(b, &ar);
                    } else
                    if (type == VARIABLE_STRING)
                        //bson_append_string_n(b, key, szData, (int)nData);
                        bson_append_utf8(b, key, strlen(key), szData, (int)nData);
                    else
                    if (type == VARIABLE_NUMBER)
                        BSON_APPEND_DOUBLE(b, key, nData);
                } else {
                    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if ((type == VARIABLE_STRING) && (nData))
                        BSON_APPEND_INT32(b, szData, 1);
                }
            }
        }
        if (add_type)
            BSON_APPEND_INT32(b, CONCEPT_CLASS_TYPE, 1);
    }
    return count;
}

//-----------------------------------------------------//
const char *PeekClass(bson_iter_t *base) {
    bson_iter_t i;
    const char  *class_name = 0;

    //i.cur=base->cur;
    //i.first=base->first;

    // ugly !
    memcpy(&i, base, sizeof(bson_iter_t));

    while (bson_iter_next(&i)) {
        if (strcmp(CONCEPT_CLASS_TYPE, bson_iter_key(&i)) == 0) {
            uint32_t length;
            class_name = bson_iter_utf8(&i, &length);
            if ((class_name) && (!class_name[0]))
                class_name = 0;
            break;
        }
    }
    return class_name;
}

//-----------------------------------------------------//
void DoBSON(void *ConceptHandler, void *ARR, bson_iter_t *i, bool use_type = false) {
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

    while (bson_iter_next(i)) {
        bson_type_t t = bson_iter_type(i);
        if (t == 0)
            break;
        int            type    = -1;
        double         nData   = 0;
        const char     *szData = "";
        const char     *key    = bson_iter_key(i);
        uint32_t       length  = 0;
        bson_subtype_t subtype;
        switch (t) {
            case BSON_TYPE_DOUBLE:
                type  = VARIABLE_NUMBER;
                nData = bson_iter_double(i);
                break;

            case BSON_TYPE_UTF8:
                type = VARIABLE_STRING;
                // it has the null terminator
                szData = bson_iter_utf8(i, &length);
                nData  = length;
                if ((nData > 0) && (!szData[(int)nData - 1]))
                    nData--;
                break;

            case BSON_TYPE_DOCUMENT:
            case BSON_TYPE_ARRAY:
                {
                    bson_iter_t sub;
                    bson_t      bson;
                    //bson_iter_subiterator(i, sub);

                    const uint8_t *buf = NULL;
                    if (t == BSON_TYPE_ARRAY)
                        bson_iter_array(i, &length, &buf);
                    else
                        bson_iter_document(i, &length, &buf);

                    bson_init_static(&bson, buf, length);

                    if (bson_iter_init(&sub, &bson)) {
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
                            DoBSON(ConceptHandler, subarr, &sub, use_type);
                        }
                    }
                }
                break;

            case BSON_TYPE_BINARY:
                type = VARIABLE_STRING;
                //nData=bson_iter_bin(i);
                bson_iter_binary(i, &subtype, &length, (const uint8_t **)&szData);
                if (!length)
                    szData = "";
                break;

            case BSON_TYPE_OID:
                type      = VARIABLE_STRING;
                oidhex[0] = 0;
                bson_oid_to_string(bson_iter_oid(i), oidhex);
                szData = oidhex;
                break;

            case BSON_TYPE_BOOL:
                type  = VARIABLE_NUMBER;
                nData = bson_iter_bool(i);
                break;

            case BSON_TYPE_DATE_TIME:
                type  = VARIABLE_NUMBER;
                nData = bson_iter_date_time(i);
                break;

            case BSON_TYPE_NULL:
            case BSON_TYPE_UNDEFINED:
            case BSON_TYPE_DBPOINTER:
                type = VARIABLE_NUMBER;
                break;

            case BSON_TYPE_REGEX:
                type   = VARIABLE_STRING;
                szData = bson_iter_regex(i, NULL);
                break;

            case BSON_TYPE_CODE:
            case BSON_TYPE_CODEWSCOPE:
                type   = VARIABLE_STRING;
                szData = bson_iter_code(i, &length);
                if (!length)
                    szData = "";
                break;

            case BSON_TYPE_INT32:
                type  = VARIABLE_NUMBER;
                nData = bson_iter_int32(i);
                break;

            case BSON_TYPE_TIMESTAMP:
                {
                    type = VARIABLE_NUMBER;
                    uint32_t stamp;
                    uint32_t increment;
                    bson_iter_timestamp(i, &stamp, &increment);
                    nData = (int64_t)stamp * (int64_t)increment;
                }
                break;

            case BSON_TYPE_INT64:
                type  = VARIABLE_NUMBER;
                nData = bson_iter_int64(i);
                break;

            case BSON_TYPE_SYMBOL:
            case BSON_TYPE_EOD:
            default:
                type = VARIABLE_STRING;
                //szData=bson_iter_value(i);
                bson_iter_binary(i, &subtype, &length, (const uint8_t **)&szData);
                if (!length)
                    szData = "";
                break;
        }
        if ((type == VARIABLE_STRING) && (!szData))
            szData = "";
        if (((type == VARIABLE_NUMBER) || (type == VARIABLE_STRING)) && (key)) {
            if (class_name) {
                void *handler = 0;
                int  r        = Invoke(INVOKE_GET_CLASS_VARIABLE, ARR, (char *)key, &handler);
                if ((!IS_OK(r)) || (!handler)) {
                    if (t == BSON_TYPE_OID) {
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoFind, 4, 8)
    T_HANDLE(MongoFind, 0)
    T_STRING(MongoFind, 1)
    T_STRING(MongoFind, 2)

    mongoc_client_t * client = (mongoc_client_t *)(SYS_INT)PARAM(0);

    char *arr_data = 0;
    bool is_array  = false;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoFind: parameter 3 should be an array or an id (string)";

    int skip  = 0;
    int limit = 0;
    int flags = MONGOC_QUERY_SLAVE_OK;

    bson_t *b2  = NULL;
    bson_t *pb2 = NULL;

    if (PARAMETERS_COUNT > 4) {
        b2 = bson_new();
        T_ARRAY(MongoFind, 4)
        if (BuildFields(b2, PARAMETER(4), true, true))
            pb2 = b2;
    }

    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoFind, 5)
        limit = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoFind, 6)
        skip = PARAM_INT(6);
    }
    if (PARAMETERS_COUNT > 7) {
        T_NUMBER(MongoFind, 7)
        flags = PARAM_INT(7);
    }

    ResetError();
    bson_t *b = NULL;
    if (is_array) {
        b = bson_new();
        BuildCond(b, PARAMETER(3), true, true);
        char *str = bson_as_json(b, NULL);
        fprintf(stderr, "JSON: %s\n", str);
    } else {
        if ((arr_data) && (arr_data[0])) {
            if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
            } else {
                b = bson_new();
                bson_oid_t oid[1];
                bson_oid_init_from_string(oid, arr_data);
                BSON_APPEND_OID(b, "_id", oid);
            }
        }
    }

    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    mongoc_cursor_t     *cursor     = 0;
    if (collection) {
        if (PARAMETERS_COUNT > 4)
            cursor = mongoc_collection_find(collection, (mongoc_query_flags_t)flags, skip, limit, 0, b, pb2, NULL);
        else
            cursor = mongoc_collection_find(collection, (mongoc_query_flags_t)flags, skip, limit, 0, b, 0, NULL);

        mongoc_collection_destroy(collection);
    }
    if (b)
        bson_destroy(b);
    if (b2)
        bson_destroy(b2);
    RETURN_NUMBER((SYS_INT)cursor);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoCount, 4, 7)
    T_HANDLE(MongoCount, 0)
    T_STRING(MongoCount, 1)
    T_STRING(MongoCount, 2)

    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoCount: parameter 4 should be an array or an id (string)";

    int skip  = 0;
    int limit = 0;
    int flags = MONGOC_QUERY_NONE;

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoCount, 4)
        flags = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoCount, 5)
        skip = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoCount, 6)
        limit = PARAM_INT(6);
    }
    ResetError();
    int res = 0;
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    if (collection) {
        bson_t *b = NULL;
        //BuildCond(b, PARAMETER(3), true, true);
        if (is_array) {
            b = bson_new();
            BuildCond(b, PARAMETER(3), true, true);
        } else {
            if ((arr_data) && (arr_data[0])) {
                if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                    b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                } else {
                    b = bson_new();
                    bson_oid_t oid[1];
                    bson_oid_init_from_string(oid, arr_data);
                    BSON_APPEND_OID(b, "_id", oid);
                }
            }
        }

        res = mongoc_collection_count(collection, (mongoc_query_flags_t)flags, b, skip, limit, NULL, &last_error);
        if (b)
            bson_destroy(b);
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoRemove, 4, 5)
    T_HANDLE(MongoRemove, 0)
    T_STRING(MongoRemove, 1)
    T_STRING(MongoRemove, 2)

    ResetError();
    char *arr_data = 0;
    bool is_array  = false;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoRemove: parameter 4 should be an array or an id (string)";

    int skip  = 0;
    int limit = 0;
    int flags = MONGOC_REMOVE_NONE;

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoRemove, 4)
        flags = PARAM_INT(4);
    }

    int res = 0;
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    if (collection) {
        //bson_t *b = bson_new();
        //BuildCond(b, PARAMETER(3), true, true);
        bson_t *b = NULL;
        //BuildCond(b, PARAMETER(3), true, true);
        if (is_array) {
            b = bson_new();
            BuildCond(b, PARAMETER(3), true, true);
        } else {
            if ((arr_data) && (arr_data[0])) {
                if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                    b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                } else {
                    b = bson_new();
                    bson_oid_t oid[1];
                    bson_oid_init_from_string(oid, arr_data);
                    BSON_APPEND_OID(b, "_id", oid);
                }
            }
        }
        if (b) {
            res = mongoc_collection_remove(collection, (mongoc_remove_flags_t)flags, b, NULL, &last_error);
            bson_destroy(b);
        } else
            res = -1;
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoNext, 2, 4)
    T_HANDLE(MongoNext, 0)
    mongoc_cursor_t * cursor = (mongoc_cursor_t *)(SYS_INT)PARAM(0);
    const bson_t *doc     = NULL;
    int          as_json  = 0;
    int          use_type = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MongoNext, 2)
        use_type = PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MongoNext, 3)
        as_json = PARAM_INT(3);
    }
    int res = mongoc_cursor_next(cursor, &doc);
    if (res) {
        if (as_json) {
            size_t len  = 0;
            char   *str = bson_as_json(doc, &len);
            if (len > 0) {
                SET_BUFFER(1, str, len);
            } else {
                SET_STRING(1, str);
            }
            if (str)
                bson_free(str);
        } else {
            //SET_NUMBER(1, (SYS_INT)doc);
            bson_iter_t i;
            if (bson_iter_init(&i, doc)) {
                DoBSON(PARAMETERS->HANDLER, PARAMETER(1), &i, use_type);
            }
        }
    } else {
        if (as_json) {
            SET_STRING(1, "");
        } else {
            SET_NUMBER(1, 0);
        }
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MongoClose, 1)
    T_HANDLE(MongoClose, 0)
    mongoc_cursor_t * cursor = (mongoc_cursor_t *)(SYS_INT)PARAM(0);
    mongoc_cursor_destroy(cursor);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoInsert, 4, 7)
    T_HANDLE(MongoInsert, 0)
    T_STRING(MongoInsert, 1)
    T_STRING(MongoInsert, 2)

    int res = -2;
//lasterror=(char *)"";
    mongoc_client_t *client           = (mongoc_client_t *)(SYS_INT)PARAM(0);
    bool            keep_object_types = true;

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoInsert, 4)
        keep_object_types = (bool)PARAM_INT(4);
    }
    char    *szData;
    INTEGER type;
    NUMBER  nData;
    char    *key = "_id";
    if (PARAMETERS_COUNT > 6) {
        T_STRING(MongoInsert, 6)
        key = PARAM(6);
    }
    if (PARAMETERS_COUNT > 5) {
        SET_STRING(5, "");
    }
    ResetError();

    Invoke(INVOKE_GET_VARIABLE, PARAMETER(3), &type, &szData, &nData);
    switch (type) {
        case VARIABLE_CLASS:
            {
                mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
                if (collection) {
                    bson_t *b = bson_new();

                    if (PARAMETERS_COUNT > 5) {
                        bson_oid_t oid;
                        bson_oid_init(&oid, NULL);
                        bson_append_oid(b, key, strlen(key), &oid);
                    }
                    do_object(b, szData, keep_object_types);

                    res = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, b, NULL, &last_error);

                    if (PARAMETERS_COUNT > 5) {
                        bson_iter_t i;
                        bson_iter_init(&i, b);
                        while (bson_iter_next(&i)) {
                            if (bson_iter_type(&i) == BSON_TYPE_OID) {
                                char oidhex[0xFFF];
                                oidhex[0] = 0;
                                bson_oid_to_string(bson_iter_oid(&i), oidhex);
                                SET_STRING(5, oidhex);
                                break;
                            }
                        }
                    }
                    bson_destroy(b);
                    mongoc_collection_destroy(collection);
                }
            }
            break;

        case VARIABLE_ARRAY:
            {
                int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, PARAMETER(3));
                if (count > 0) {
                    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
                    if (collection) {
                        mongoc_bulk_operation_t *bulk = mongoc_collection_create_bulk_operation(collection, true, NULL);
                        if (bulk) {
                            for (int i = 0; i < count; i++) {
                                void *newpData;
                                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(3), i, &newpData);
                                if (newpData) {
                                    char    *szData;
                                    INTEGER type;
                                    NUMBER  nData;

                                    Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                                    if (type == VARIABLE_CLASS) {
                                        bson_t *b = bson_new();
                                        do_object(b, szData, keep_object_types);
                                        mongoc_bulk_operation_insert(bulk, b);
                                        bson_destroy(b);
                                    } else
                                    if (type == VARIABLE_ARRAY) {
                                        bson_t *b = bson_new();
                                        do_array(b, newpData, false, false, keep_object_types);
                                        mongoc_bulk_operation_insert(bulk, b);
                                        bson_destroy(b);
                                    }
                                }
                            }
                            if (PARAMETERS_COUNT > 5) {
                                bson_t reply;
                                res = mongoc_bulk_operation_execute(bulk, &reply, &last_error);
                                CREATE_ARRAY(PARAMETER(5));
                                bson_iter_t i;

                                if (bson_iter_init(&i, &reply))
                                    DoBSON(PARAMETERS->HANDLER, PARAMETER(5), &i, false);
                                bson_destroy(&reply);
                            } else
                                res = mongoc_bulk_operation_execute(bulk, NULL, &last_error);
                            mongoc_bulk_operation_destroy(bulk);
                        }
                        mongoc_collection_destroy(collection);
                    }
                }
            }
            break;

        case VARIABLE_STRING:
            {
                mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
                if (collection) {
                    bson_t *b = bson_new_from_json((uint8_t *)szData, (int)nData, &last_error2);
                    if (b) {
                        res = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, b, NULL, &last_error);

                        if (PARAMETERS_COUNT > 5) {
                            bson_iter_t i;
                            bson_iter_init(&i, b);
                            while (bson_iter_next(&i)) {
                                if (bson_iter_type(&i) == BSON_TYPE_OID) {
                                    char oidhex[0xFFF];
                                    oidhex[0] = 0;
                                    bson_oid_to_string(bson_iter_oid(&i), oidhex);
                                    SET_STRING(5, oidhex);
                                    break;
                                }
                            }
                        }
                        bson_destroy(b);
                        mongoc_collection_destroy(collection);
                    }
                }
            }
            break;
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdate, 5, 7)
    T_HANDLE(MongoUpdate, 0)
    T_STRING(MongoUpdate, 1)
    T_STRING(MongoUpdate, 2)
//T_ARRAY(2)
    char *arr_data = 0;
    bool is_array  = false;
    bool as_object = true;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdate: parameter 4 should be an array or an id (string)";
    T_ARRAY(MongoUpdate, 4)
    int flags = MONGOC_UPDATE_MULTI_UPDATE;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoUpdate, 5)
        flags = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoUpdate, 6)
        as_object = (bool)PARAM_INT(6);
    }
    ResetError();
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    int res = -2;
    if (collection) {
        bson_t *b = NULL;

        if (is_array) {
            b = bson_new();
            BuildCond(b, PARAMETER(3), as_object, true);
        } else {
            if ((arr_data) && (arr_data[0])) {
                if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                    b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                } else {
                    b = bson_new();
                    bson_oid_t oid[1];
                    bson_oid_init_from_string(oid, arr_data);
                    BSON_APPEND_OID(b, "_id", oid);
                }
            }

            if (PARAMETERS_COUNT <= 5) ;
            flags = MONGOC_UPDATE_NONE;
        }
        if (b) {
            bson_t *b2 = bson_new();
            BuildCond(b2, PARAMETER(4), true);

            res = mongoc_collection_update(collection, (mongoc_update_flags_t)flags, b, b2, NULL, &last_error);

            bson_destroy(b);
            bson_destroy(b2);
        }
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdateType, 5, 9)
    T_HANDLE(MongoUpdateType, 0)
    T_STRING(MongoUpdateType, 1)
    T_STRING(MongoUpdateType, 2)
//T_ARRAY(2)
    char *arr_data = 0;
    bool is_array = false;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdate: parameter 4 should be an array or an id (string)";

    T_STRING(MongoUpdateType, 4)
    T_NUMBER(MongoUpdateType, 5)
//T_STRING(5)
    bool keep_types = false;
    if (PARAMETERS_COUNT > 7) {
        T_NUMBER(MongoUpdateType, 7)
        keep_types = PARAM_INT(7);
    }

    int flags = MONGOC_UPDATE_MULTI_UPDATE;
    if (PARAMETERS_COUNT > 8) {
        T_NUMBER(MongoUpdateType, 8)
        flags = PARAM_INT(8);
    }
    ResetError();
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    int res = -2;
    if (collection) {
        bson_t *b = NULL;
        if (is_array) {
            b = bson_new();
            BuildCond(b, PARAMETER(3), true, true);
        } else {
            if ((arr_data) && (arr_data[0])) {
                if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                    b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                } else {
                    b = bson_new();
                    bson_oid_t oid[1];
                    bson_oid_init_from_string(oid, arr_data);
                    BSON_APPEND_OID(b, "_id", oid);
                }
            }
            if (PARAMETERS_COUNT <= 5) ;
            flags = MONGOC_UPDATE_NONE;
        }
        if (b) {
            bson_t *base_doc = bson_new();
            bson_t b2;
            BSON_APPEND_DOCUMENT_BEGIN(base_doc, "$set", &b2);
            char *str = PARAM(4);
            switch (PARAM_INT(5)) {
                case BSON_TYPE_EOD:
                    break;

                case BSON_TYPE_DOUBLE:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_DOUBLE(&b2, str, PARAM(6));
                    }
                    break;

                case BSON_TYPE_UTF8:
                    {
                        T_STRING(MongoUpdateType, 6)
                        bson_append_utf8(&b2, str, (int)strlen(str), PARAM(6), (int)PARAM_LEN(6));
                    }
                    break;

                case BSON_TYPE_DOCUMENT:
                    {
                        T_OBJECT(MongoUpdateType, 6)
                        bson_t ar;
                        BSON_APPEND_DOCUMENT_BEGIN(&b2, str, &ar);
                        do_object(&b2, PARAM(6), keep_types);
                        bson_append_document_end(&b2, &ar);
                    }
                    break;

                case BSON_TYPE_ARRAY:
                    {
                        T_ARRAY(MongoUpdateType, 6);
                        bson_t ar;
                        BSON_APPEND_ARRAY_BEGIN(&b2, str, &ar);
                        do_array(&ar, PARAMETER(6), false, false, keep_types);
                        bson_append_array_end(&b2, &ar);
                    }
                    break;

                case BSON_TYPE_BINARY:
                    {
                        T_STRING(MongoUpdateType, 6)
                        bson_append_binary(&b2, str, PARAM_INT(6), BSON_SUBTYPE_BINARY, (uint8_t *)PARAM(6), PARAM_LEN(6));
                    }
                    break;

                case BSON_TYPE_OID:
                    {
                        T_STRING(MongoUpdateType, 6)
                        bson_oid_t oid;
                        if (PARAM_LEN(6)) {
                            bson_oid_init_from_string(&oid, PARAM(6));
                            BSON_APPEND_OID(b, str, &oid);
                        }
                    }
                    break;

                case BSON_TYPE_BOOL:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_BOOL(&b2, str, PARAM_INT(6));
                    }
                    break;

                case BSON_TYPE_DATE_TIME:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_DATE_TIME(&b2, str, PARAM(6));
                    }
                    break;

                case BSON_TYPE_NULL:
                    BSON_APPEND_NULL(&b2, str);
                    break;

                case BSON_TYPE_REGEX:
                    {
                        T_STRING(MongoUpdateType, 6)
                        BSON_APPEND_REGEX(&b2, str, PARAM(6), "");
                    }
                    break;

                case BSON_TYPE_CODE:
                    {
                        T_STRING(MongoUpdateType, 6)
                        BSON_APPEND_CODE(&b2, str, PARAM(6));
                    }
                    break;

                case BSON_TYPE_SYMBOL:
                    {
                        T_STRING(MongoUpdateType, 6)
                        BSON_APPEND_SYMBOL(&b2, str, PARAM(6));
                    }
                    break;

                case BSON_TYPE_CODEWSCOPE:
                    {
                        T_STRING(MongoUpdateType, 6)
                        //BSON_APPEND_CODE_WITH_SCOPE(b2, str, scope, PARAM(5));
                    }
                    break;

                case BSON_TYPE_INT32:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_INT32(&b2, str, PARAM_INT(6));
                    }
                    break;

                case BSON_TYPE_TIMESTAMP:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_TIME_T(&b2, str, (time_t)PARAM_INT(6));
                    }
                    break;

                case BSON_TYPE_INT64:
                    {
                        T_NUMBER(MongoUpdateType, 6)
                        BSON_APPEND_INT64(&b2, str, (int64_t)PARAM(6));
                    }
                    break;

                case BSON_TYPE_UNDEFINED:
                    BSON_APPEND_UNDEFINED(&b2, str);

                default:
                    break;
            }
            bson_append_document_end(base_doc, &b2);
            res = mongoc_collection_update(collection, (mongoc_update_flags_t)flags, b, base_doc, NULL, &last_error);

            bson_destroy(b);
            bson_destroy(base_doc);
        }
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoUpdateObject, 4, 6)
    T_HANDLE(MongoUpdateObject, 0)
    T_STRING(MongoUpdateObject, 1)
    T_STRING(MongoUpdateObject, 2)
//T_ARRAY(2)
    char *arr_data = 0;
    bool is_array  = false;
    bool as_object = true;
    GET_ARRAY(3, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoUpdate: parameter 4 should be an array or an id (string)";
    T_OBJECT(MongoUpdateObject, 4)
    int flags = MONGOC_UPDATE_MULTI_UPDATE;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoUpdateObject, 5)
        flags = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoUpdateObject, 6)
        as_object = (bool)PARAM_INT(6);
    }
    ResetError();
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    int res = -2;
    if (collection) {
        bson_t *b = NULL;

        if (is_array) {
            b = bson_new();
            BuildCond(b, PARAMETER(3), as_object, true);
        } else {
            if ((arr_data) && (arr_data[0])) {
                if ((strchr(arr_data, '{')) || (strchr(arr_data, '['))) {
                    b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                } else {
                    b = bson_new();
                    bson_oid_t oid[1];
                    bson_oid_init_from_string(oid, arr_data);
                    BSON_APPEND_OID(b, "_id", oid);
                }
            }
            if (PARAMETERS_COUNT <= 5) ;
            flags = MONGOC_UPDATE_NONE;
        }
        if (b) {
            bson_t *b2 = bson_new();
            bson_t ar;
            BSON_APPEND_DOCUMENT_BEGIN(b2, "$set", &ar);
            do_object(&ar, PARAM(4), as_object);
            bson_append_document_end(b2, &ar);

            res = mongoc_collection_update(collection, (mongoc_update_flags_t)flags, b, b2, NULL, &last_error);

            bson_destroy(b);
            bson_destroy(b2);
        }
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSInit, 3)
    T_HANDLE(GridFSInit, 0)
    T_STRING(GridFSInit, 1)
    T_STRING(GridFSInit, 2)
    ResetError();
    mongoc_client_t *client = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_gridfs_t *gfs    = mongoc_client_get_gridfs(client, PARAM(1), PARAM(2), &last_error);

    RETURN_NUMBER((SYS_INT)gfs);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSDone, 1)
    T_HANDLE(GridFSDone, 0)

    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    mongoc_gridfs_destroy(gfs);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(GridFSStore, 3, 4)
    T_HANDLE(GridFSStore, 0)
    T_STRING(GridFSStore, 1)
    T_STRING(GridFSStore, 2)
    char *type = "application/octet-stream";

    mongoc_gridfs_t *gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    if (PARAMETERS_COUNT > 3) {
        T_STRING(GridFSStore, 3)
        type = PARAM(3);
    }
    int written = -1;
    if (PARAM_LEN(1) > 0) {
        bson_error_t dummy_error;
        mongoc_gridfs_remove_by_filename(gfs, PARAM(1), &dummy_error);

        mongoc_gridfs_file_opt_t opts = { 0 };

        opts.filename     = PARAM(1);
        opts.content_type = type;

        mongoc_gridfs_file_t *file = mongoc_gridfs_create_file(gfs, &opts);
        if (file) {
            mongoc_stream_t *stream = mongoc_stream_gridfs_new(file);
            if (stream) {
                written = mongoc_stream_write(stream, PARAM(2), PARAM_LEN(2), 3600000);
                mongoc_stream_destroy(stream);
            }
            mongoc_gridfs_file_destroy(file);
        }
        RETURN_NUMBER(written);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSGet, 2)
    T_HANDLE(GridFSGet, 0)
    T_STRING(GridFSGet, 1)
    ResetError();
    mongoc_gridfs_t      *gfs  = (mongoc_gridfs_t *)PARAM_INT(0);
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        int64_t len = mongoc_gridfs_file_get_length(file);
        if (len > 0) {
            char *buf = 0;
            CORE_NEW(len + 1, buf);
            if (buf) {
                buf[len] = 0;

                mongoc_stream_t *stream   = mongoc_stream_gridfs_new(file);
                char            *ptr      = buf;
                int64_t         remaining = len;
                if (stream) {
                    size_t size = 0;
                    while (remaining > 0) {
                        size_t s = mongoc_stream_read(stream, ptr, remaining, remaining, 3600000);
                        if (s <= 0)
                            break;

                        size      += s;
                        remaining -= s;
                        ptr       += s;
                    }
                    if (size > 0) {
                        SetVariable(RESULT, -1, buf, size);
                    } else {
                        CORE_DELETE(buf);
                        RETURN_STRING("");
                    }
                    mongoc_stream_destroy(stream);
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
        mongoc_gridfs_file_destroy(file);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSRemove, 2)
    T_HANDLE(GridFSRemove, 0)
    T_STRING(GridFSRemove, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    int res = mongoc_gridfs_remove_by_filename(gfs, PARAM(1), NULL);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSExists, 2)
    T_HANDLE(GridFSExists, 0)
    T_STRING(GridFSExists, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    int res = 0;
    ResetError();
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        mongoc_gridfs_file_destroy(file);
        res = 1;
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSSize, 2)
    T_HANDLE(GridFSSize, 0)
    T_STRING(GridFSSize, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    int64_t res = -1;
    ResetError();
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        res = mongoc_gridfs_file_get_length(file);
        mongoc_gridfs_file_destroy(file);
    }
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSType, 2)
    T_HANDLE(GridFSType, 0)
    T_STRING(GridFSType, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    ResetError();
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        const char *str = mongoc_gridfs_file_get_content_type(file);
        RETURN_STRING(str);
        mongoc_gridfs_file_destroy(file);
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSDate, 2)
    T_HANDLE(GridFSDate, 0)
    T_STRING(GridFSDate, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    int64_t res = -1;
    ResetError();
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        res = mongoc_gridfs_file_get_upload_date(file);
        mongoc_gridfs_file_destroy(file);
    }
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GridFSMD5, 2)
    T_HANDLE(GridFSMD5, 0)
    T_STRING(GridFSMD5, 1)
    mongoc_gridfs_t * gfs = (mongoc_gridfs_t *)PARAM_INT(0);
    ResetError();
    mongoc_gridfs_file_t *file = mongoc_gridfs_find_one_by_filename(gfs, PARAM(1), &last_error);
    if (file) {
        const char *md5_str = mongoc_gridfs_file_get_md5(file);
        RETURN_STRING(md5_str);
        mongoc_gridfs_file_destroy(file);
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoSimpleCommand, 3, 5)
    T_HANDLE(MongoSimpleCommand, 0)
    T_STRING(MongoSimpleCommand, 1)
    T_STRING(MongoSimpleCommand, 2)

    bool is_number = true;

    INTEGER type = 0;
    double  *ret = 0;
    char    *szData;
    NUMBER  nData;
    ResetError();
    mongoc_client_t *client = (mongoc_client_t *)(SYS_INT)PARAM(0);
    bson_t          *b      = bson_new_from_json((uint8_t *)PARAM(2), PARAM_LEN(2), &last_error2);
    int             res     = -2;
    if (b) {
        bson_t *out = bson_new();
        res = mongoc_client_command_simple(client, PARAM(1), b, NULL, out, &last_error);

        if (PARAMETERS_COUNT > 3) {
            bool use_types = false;
            if (PARAMETERS_COUNT > 4) {
                T_NUMBER(MongoSimpleCommand, 4)
                use_types = (bool)PARAM_INT(4);
            }

            bson_iter_t i;
            if (bson_iter_init(&i, out))
                DoBSON(PARAMETERS->HANDLER, PARAMETER(4), &i, use_types);
        }
        bson_destroy(out);
        bson_destroy(b);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoCommand, 3, 7)
    T_HANDLE(MongoCommand, 0)
    T_STRING(MongoCommand, 1)

    mongoc_client_t * client = (mongoc_client_t *)(SYS_INT)PARAM(0);
    ResetError();
    char *arr_data = 0;
    bool is_array  = false;
    GET_ARRAY(2, arr_data);
    if (TYPE == VARIABLE_ARRAY)
        is_array = true;
    else
    if (TYPE != VARIABLE_STRING)
        return (void *)"MongoCommand: parameter 3 should be an array or an id (string)";

    int skip  = 0;
    int limit = 0;
    int flags = 0;

    bson_t b2[1];
    bson_init(b2);

    bson_t *pb2 = 0;

    if (PARAMETERS_COUNT > 3) {
        T_ARRAY(MongoCommand, 3)
        if (BuildFields(b2, PARAMETER(3), true, true))
            pb2 = b2;
    }

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MongoCommand, 4)
        limit = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MongoCommand, 5)
        skip = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(MongoCommand, 6)
        flags = PARAM_INT(6);
    }

    bson_t *b = NULL;
    if (is_array) {
        b = bson_new();
        BuildCond(b, PARAMETER(2), true, true);
    } else {
        if ((arr_data) && (arr_data[0])) {
            if ((strchr(arr_data, '{')) || (strchr(arr_data, '[')) || (strchr(arr_data, '(')) || (strchr(arr_data, '.'))) {
                b = bson_new_from_json((uint8_t *)arr_data, strlen(arr_data), &last_error2);
                if (!b)
                    b = bson_new();
            } else {
                b = bson_new();
                bson_oid_t oid[1];
                bson_oid_init_from_string(oid, arr_data);
                BSON_APPEND_OID(b, "_id", oid);
            }
        }
    }

    mongoc_cursor_t *cursor = 0;
    if (PARAMETERS_COUNT > 4)
        cursor = mongoc_client_command(client, PARAM(1), (mongoc_query_flags_t)flags, skip, limit, 0, b, pb2, NULL);
    else
        cursor = mongoc_client_command(client, PARAM(1), (mongoc_query_flags_t)flags, skip, limit, 0, b, 0, NULL);

    if (b)
        bson_destroy(b);
    bson_destroy(b2);
    RETURN_NUMBER((SYS_INT)cursor);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoIndex, 4, 5)
    T_HANDLE(MongoIndex, 0)
    T_STRING(MongoIndex, 1)
    T_STRING(MongoIndex, 2)
    T_STRING(MongoIndex, 3)

    char *name = NULL;
    if (PARAMETERS_COUNT > 4) {
        T_STRING(MongoIndex, 4)
        name = PARAM(4);
    }
    ResetError();
    mongoc_client_t     *client     = (mongoc_client_t *)(SYS_INT)PARAM(0);
    mongoc_collection_t *collection = mongoc_client_get_collection(client, PARAM(1), PARAM(2));
    int res = -2;
    if (collection) {
        bson_t *b = bson_new_from_json((uint8_t *)PARAM(3), PARAM_LEN(3), &last_error2);
        if (b) {
            mongoc_index_opt_t opt;
            mongoc_index_opt_init(&opt);
            if (name)
                opt.name = name;

            res = mongoc_collection_create_index(collection, b, &opt, &last_error);

            bson_destroy(b);
        }
        mongoc_collection_destroy(collection);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MongoError, 0, 2)
    bson_error_t * err = NULL;
    if (last_error2.code)
        err = &last_error2;
    else
    if (last_error.code)
        err = &last_error;

    if (err) {
        RETURN_NUMBER(err->code);
        if (PARAMETERS_COUNT > 0) {
            SET_STRING(0, err->message);
        }
        if (PARAMETERS_COUNT > 1) {
            SET_NUMBER(1, err->domain);
        }
    } else {
        RETURN_NUMBER(0);
        if (PARAMETERS_COUNT > 0) {
            SET_STRING(0, "");
        }
        if (PARAMETERS_COUNT > 1) {
            SET_NUMBER(1, 0);
        }
    }
END_IMPL
//-----------------------------------------------------//

