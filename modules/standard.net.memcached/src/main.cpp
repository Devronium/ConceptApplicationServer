//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>

#include <libmemcached/memcached.h>

#ifdef _WIN32
WSADATA wsa;
#endif
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(MEMCACHED_DEFAULT_PORT)
    DEFINE_ECONSTANT(MEMCACHED_MAX_KEY)
    DEFINE_ECONSTANT(MEMCACHED_MAX_BUFFER)
    DEFINE_ECONSTANT(MEMCACHED_MAX_HOST_SORT_LENGTH)
    DEFINE_ECONSTANT(MEMCACHED_POINTS_PER_SERVER)
    DEFINE_ECONSTANT(MEMCACHED_POINTS_PER_SERVER_KETAMA)
    DEFINE_ECONSTANT(MEMCACHED_CONTINUUM_SIZE)
    DEFINE_ECONSTANT(MEMCACHED_STRIDE)
    DEFINE_ECONSTANT(MEMCACHED_DEFAULT_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_DEFAULT_CONNECT_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_CONTINUUM_ADDITION)
    DEFINE_ECONSTANT(MEMCACHED_PREFIX_KEY_MAX_SIZE)
    DEFINE_ECONSTANT(MEMCACHED_EXPIRATION_NOT_ADD)
    DEFINE_ECONSTANT(MEMCACHED_VERSION_STRING_LENGTH)
    DEFINE_ECONSTANT(MEMCACHED_SUCCESS)
    DEFINE_ECONSTANT(MEMCACHED_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_HOST_LOOKUP_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_BIND_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_WRITE_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_READ_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_UNKNOWN_READ_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_PROTOCOL_ERROR)
    DEFINE_ECONSTANT(MEMCACHED_CLIENT_ERROR)
    DEFINE_ECONSTANT(MEMCACHED_SERVER_ERROR)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_DATA_EXISTS)
    DEFINE_ECONSTANT(MEMCACHED_DATA_DOES_NOT_EXIST)
    DEFINE_ECONSTANT(MEMCACHED_NOTSTORED)
    DEFINE_ECONSTANT(MEMCACHED_STORED)
    DEFINE_ECONSTANT(MEMCACHED_NOTFOUND)
    DEFINE_ECONSTANT(MEMCACHED_MEMORY_ALLOCATION_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_PARTIAL_READ)
    DEFINE_ECONSTANT(MEMCACHED_SOME_ERRORS)
    DEFINE_ECONSTANT(MEMCACHED_NO_SERVERS)
    DEFINE_ECONSTANT(MEMCACHED_END)
    DEFINE_ECONSTANT(MEMCACHED_DELETED)
    DEFINE_ECONSTANT(MEMCACHED_VALUE)
    DEFINE_ECONSTANT(MEMCACHED_STAT)
    DEFINE_ECONSTANT(MEMCACHED_ITEM)
    DEFINE_ECONSTANT(MEMCACHED_ERRNO)
    DEFINE_ECONSTANT(MEMCACHED_FAIL_UNIX_SOCKET)
    DEFINE_ECONSTANT(MEMCACHED_NOT_SUPPORTED)
    DEFINE_ECONSTANT(MEMCACHED_FETCH_NOTFINISHED)
    DEFINE_ECONSTANT(MEMCACHED_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BUFFERED)
    DEFINE_ECONSTANT(MEMCACHED_BAD_KEY_PROVIDED)
    DEFINE_ECONSTANT(MEMCACHED_INVALID_HOST_PROTOCOL)
    DEFINE_ECONSTANT(MEMCACHED_SERVER_MARKED_DEAD)
    DEFINE_ECONSTANT(MEMCACHED_UNKNOWN_STAT_KEY)
    DEFINE_ECONSTANT(MEMCACHED_E2BIG)
    DEFINE_ECONSTANT(MEMCACHED_INVALID_ARGUMENTS)
    DEFINE_ECONSTANT(MEMCACHED_KEY_TOO_BIG)
    DEFINE_ECONSTANT(MEMCACHED_AUTH_PROBLEM)
    DEFINE_ECONSTANT(MEMCACHED_AUTH_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_AUTH_CONTINUE)
    DEFINE_ECONSTANT(MEMCACHED_MAXIMUM_RETURN)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_MODULA)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_CONSISTENT)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_RANDOM)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA_SPY)
    DEFINE_ECONSTANT(MEMCACHED_DISTRIBUTION_CONSISTENT_MAX)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_NO_BLOCK)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_TCP_NODELAY)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_HASH)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_KETAMA)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_CACHE_LOOKUPS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SUPPORT_CAS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_POLL_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_DISTRIBUTION)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_BUFFER_REQUESTS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_USER_DATA)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SORT_HOSTS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_VERIFY_KEY)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_RETRY_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_KETAMA_HASH)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_BINARY_PROTOCOL)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SND_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_RCV_TIMEOUT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_NOREPLY)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_USE_UDP)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_CORK)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_TCP_KEEPALIVE)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_TCP_KEEPIDLE)
    DEFINE_ECONSTANT(MEMCACHED_BEHAVIOR_MAX)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_PREFIX_KEY)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_USER_DATA)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_CLEANUP_FUNCTION)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_CLONE_FUNCTION)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_GET_FAILURE)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_DELETE_TRIGGER)
    DEFINE_ECONSTANT(MEMCACHED_CALLBACK_MAX)
    DEFINE_ECONSTANT(MEMCACHED_HASH_DEFAULT)
    DEFINE_ECONSTANT(MEMCACHED_HASH_MD5)
    DEFINE_ECONSTANT(MEMCACHED_HASH_CRC)
    DEFINE_ECONSTANT(MEMCACHED_HASH_FNV1_64)
    DEFINE_ECONSTANT(MEMCACHED_HASH_FNV1A_64)
    DEFINE_ECONSTANT(MEMCACHED_HASH_FNV1_32)
    DEFINE_ECONSTANT(MEMCACHED_HASH_FNV1A_32)
    DEFINE_ECONSTANT(MEMCACHED_HASH_HSIEH)
    DEFINE_ECONSTANT(MEMCACHED_HASH_MURMUR)
    DEFINE_ECONSTANT(MEMCACHED_HASH_JENKINS)
    DEFINE_ECONSTANT(MEMCACHED_HASH_CUSTOM)
    DEFINE_ECONSTANT(MEMCACHED_HASH_MAX)
    // DEFINE_ECONSTANT(MEMCACHED_CONNECTION_UNKNOWN)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_TCP)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_UDP)
    DEFINE_ECONSTANT(MEMCACHED_CONNECTION_UNIX_SOCKET)
    // DEFINE_ECONSTANT(MEMCACHED_CONNECTION_MAX)

#ifdef _WIN32
    WSAStartup(MAKEWORD(1, 0), &wsa);
#endif
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedCreate, 0)
    RETURN_NUMBER((SYS_INT)memcached_create(NULL))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedFree, 1)
    T_NUMBER(MemCachedFree, 0)
    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    if (memc) {
        memcached_quit(memc);
        memcached_free(memc);
    }
    RETURN_NUMBER(0)
    SET_NUMBER(0, (NUMBER)0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedResetServers, 1)
    T_HANDLE(MemCachedResetServers, 0)
    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    memcached_servers_reset(memc);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedAddServer, 3)
    T_HANDLE(MemCachedAddServer, 0)
    T_STRING(MemCachedAddServer, 1)
    T_NUMBER(MemCachedAddServer, 2)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_server_add(memc, PARAM(1), PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedAddServerUDP, 3)
    T_HANDLE(MemCachedAddServerUDP, 0)
    T_STRING(MemCachedAddServerUDP, 1)
    T_NUMBER(MemCachedAddServerUDP, 2)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_server_add_udp(memc, PARAM(1), PARAM_INT(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedAddServerUnixSocket, 2)
    T_HANDLE(MemCachedAddServerUnixSocket, 0)
    T_STRING(MemCachedAddServerUnixSocket, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_server_add_unix_socket(memc, PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedServerPush, 2)
    T_HANDLE(MemCachedServerPush, 0)
    T_ARRAY(MemCachedServerPush, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    memcached_server_st *servers = NULL;

    void *arr  = PARAMETER(1);
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
                char *server = 0;
                int  port    = 11211;
                Invoke(INVOKE_ARRAY_VARIABLE, newpData, 0, &newpData2);
                Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                if (type == VARIABLE_STRING)
                    server = szData;
                Invoke(INVOKE_ARRAY_VARIABLE, newpData, 1, &newpData2);
                Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                if (type == VARIABLE_NUMBER)
                    port = (INTEGER)nData;
                memcached_return_t rc;
                if ((port) && (server))
                    servers = memcached_server_list_append(servers, server, port, &rc);
            }
        }
    }

    RETURN_NUMBER(memcached_server_push(memc, servers))
    memcached_server_free(servers);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedSet, 3, 5)
    T_HANDLE(MemCachedSet, 0)
    T_STRING(MemCachedSet, 1)
    T_STRING(MemCachedSet, 2)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedSet, 3)
        settime = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedSet, 4)
        flags = PARAM_INT(4);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_set(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedAdd, 3, 5)
    T_HANDLE(MemCachedAdd, 0)
    T_STRING(MemCachedAdd, 1)
    T_STRING(MemCachedAdd, 2)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedAdd, 3)
        settime = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedAdd, 4)
        flags = PARAM_INT(4);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_add(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedReplace, 3, 5)
    T_HANDLE(MemCachedReplace, 0)
    T_STRING(MemCachedReplace, 1)
    T_STRING(MemCachedReplace, 2)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedReplace, 3)
        settime = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedReplace, 4)
        flags = PARAM_INT(4);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_replace(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedPrepend, 3, 5)
    T_HANDLE(MemCachedPrepend, 0)
    T_STRING(MemCachedPrepend, 1)
    T_STRING(MemCachedPrepend, 2)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedPrepend, 3)
        settime = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedPrepend, 4)
        flags = PARAM_INT(4);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_prepend(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedAppend, 3, 5)
    T_HANDLE(MemCachedAppend, 0)
    T_STRING(MemCachedAppend, 1)
    T_STRING(MemCachedAppend, 2)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedAppend, 3)
        settime = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedAppend, 4)
        flags = PARAM_INT(4);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_append(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedSetByKey, 4, 6)
    T_HANDLE(MemCachedSetByKey, 0)
    T_STRING(MemCachedSetByKey, 1)
    T_STRING(MemCachedSetByKey, 2)
    T_STRING(MemCachedSetByKey, 3)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedSetByKey, 4)
        settime = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MemCachedSetByKey, 5)
        flags = PARAM_INT(5);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_set_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), PARAM(3), PARAM_LEN(3), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedAddByKey, 4, 6)
    T_HANDLE(MemCachedAddByKey, 0)
    T_STRING(MemCachedAddByKey, 1)
    T_STRING(MemCachedAddByKey, 2)
    T_STRING(MemCachedAddByKey, 3)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedAddByKey, 4)
        settime = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MemCachedAddByKey, 5)
        flags = PARAM_INT(5);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_add_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), PARAM(3), PARAM_LEN(3), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedReplaceByKey, 4, 6)
    T_HANDLE(MemCachedReplaceByKey, 0)
    T_STRING(MemCachedReplaceByKey, 1)
    T_STRING(MemCachedReplaceByKey, 2)
    T_STRING(MemCachedReplaceByKey, 3)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedReplaceByKey, 4)
        settime = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MemCachedReplaceByKey, 5)
        flags = PARAM_INT(5);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_replace_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), PARAM(3), PARAM_LEN(3), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedPrependByKey, 4, 6)
    T_HANDLE(MemCachedPrependByKey, 0)
    T_STRING(MemCachedPrependByKey, 1)
    T_STRING(MemCachedPrependByKey, 2)
    T_STRING(MemCachedPrependByKey, 3)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedPrependByKey, 4)
        settime = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MemCachedPrependByKey, 5)
        flags = PARAM_INT(5);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_prepend_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), PARAM(3), PARAM_LEN(3), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedAppendByKey, 4, 6)
    T_HANDLE(MemCachedAppendByKey, 0)
    T_STRING(MemCachedAppendByKey, 1)
    T_STRING(MemCachedAppendByKey, 2)
    T_STRING(MemCachedAppendByKey, 3)

    time_t settime = 0;
    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(MemCachedAppendByKey, 4)
        settime = PARAM_INT(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MemCachedAppendByKey, 5)
        flags = PARAM_INT(5);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);

    RETURN_NUMBER(memcached_append_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), PARAM(3), PARAM_LEN(3), settime, flags));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedGet, 2, 4)
    T_HANDLE(MemCachedGet, 0)
    T_STRING(MemCachedGet, 1)

    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MemCachedGet, 2)
        flags = PARAM_INT(2);
    }

    memcached_st       *memc = (memcached_st *)PARAM_INT(0);
    size_t             len   = 0;
    memcached_return_t mem_error;
    char *ret = memcached_get(memc, PARAM(1), PARAM_LEN(1), &len, &flags, &mem_error);
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, flags);
    }
    if (PARAMETERS_COUNT > 3) {
        SET_NUMBER(3, mem_error);
    }
    if (len > 0) {
        RETURN_BUFFER(ret, len);
    } else {
        RETURN_STRING("");
    }
    if (ret)
        free(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedMGet, 2)
    T_HANDLE(MemCachedMGet, 0)
    T_ARRAY(MemCachedMGet, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    void *arr  = PARAMETER(1);
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    if (count) {
        char   **keys = new char * [count];
        size_t *lens  = new size_t[count];
        for (int i = 0; i < count; i++) {
            void *newpData;
            Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    keys[i] = szData;
                    lens[i] = (size_t)nData;
                } else {
                    keys[i] = 0;
                    lens[i] = 0;
                }
            }
        }
        memcached_return_t rc = memcached_mget(memc, keys, lens, count);
        char         *return_value;
        size_t       return_value_length;
        int          x = 0;
        char         return_key[MEMCACHED_MAX_KEY];
        size_t       return_key_length;
        unsigned int flags = 0;

        while ((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc))) {
            if (return_value_length)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, return_key, (INTEGER)VARIABLE_STRING, return_value, (NUMBER)return_value_length);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, return_key, (INTEGER)VARIABLE_STRING, "", (NUMBER)0);
            free(return_value);
            x++;
        }
        delete[] keys;
        delete[] lens;
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedMGetByKey, 3)
    T_HANDLE(MemCachedMGetByKey, 0)
    T_STRING(MemCachedMGetByKey, 1)
    T_ARRAY(MemCachedMGetByKey, 2)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    void *arr  = PARAMETER(2);
    int  count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    if (count) {
        char   **keys = new char * [count];
        size_t *lens  = new size_t[count];
        for (int i = 0; i < count; i++) {
            void *newpData;
            Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_STRING) {
                    keys[i] = szData;
                    lens[i] = (size_t)nData;
                } else {
                    keys[i] = 0;
                    lens[i] = 0;
                }
            }
        }
        memcached_return_t rc = memcached_mget_by_key(memc, PARAM(1), PARAM_LEN(1), keys, lens, count);
        char         *return_value;
        size_t       return_value_length;
        int          x = 0;
        char         return_key[MEMCACHED_MAX_KEY];
        size_t       return_key_length;
        unsigned int flags = 0;

        while ((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc))) {
            if (return_value_length)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, return_key, (INTEGER)VARIABLE_STRING, return_value, (NUMBER)return_value_length);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, return_key, (INTEGER)VARIABLE_STRING, "", (NUMBER)0);
            free(return_value);
            x++;
        }
        delete[] keys;
        delete[] lens;
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedGetByKey, 3, 5)
    T_HANDLE(MemCachedGetByKey, 0)
    T_STRING(MemCachedGetByKey, 1)
    T_STRING(MemCachedGetByKey, 2)

    unsigned int flags = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedGetByKey, 3)
        flags = PARAM_INT(3);
    }

    memcached_st       *memc = (memcached_st *)PARAM_INT(0);
    size_t             len   = 0;
    memcached_return_t mem_error;
    char *ret = memcached_get_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), &len, &flags, &mem_error);
    if (PARAMETERS_COUNT > 3) {
        SET_NUMBER(3, flags);
    }
    if (PARAMETERS_COUNT > 4) {
        SET_NUMBER(4, mem_error);
    }
    if (len > 0) {
        RETURN_BUFFER(ret, len);
    } else {
        RETURN_STRING("");
    }
    if (ret)
        free(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedDelete, 2, 3)
    T_HANDLE(MemCachedDelete, 0)
    T_STRING(MemCachedDelete, 1)

    time_t del_time = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MemCachedDelete, 2)
        del_time = PARAM_INT(2);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_delete(memc, PARAM(1), PARAM_LEN(1), del_time))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedDeleteByKey, 3, 4)
    T_HANDLE(MemCachedDeleteByKey, 0)
    T_STRING(MemCachedDeleteByKey, 1)
    T_STRING(MemCachedDeleteByKey, 2)

    time_t del_time = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MemCachedDeleteByKey, 3)
        del_time = PARAM_INT(3);
    }

    memcached_st *memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_delete_by_key(memc, PARAM(1), PARAM_LEN(1), PARAM(2), PARAM_LEN(2), del_time))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedError, 2)
    T_HANDLE(MemCachedError, 0)
    T_NUMBER(MemCachedError, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_STRING(memcached_strerror(memc, (memcached_return_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedIncrement, 2, 3)
    T_HANDLE(MemCachedIncrement, 0)
    T_STRING(MemCachedIncrement, 1)

    unsigned int offset = 1;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MemCachedIncrement, 2)
        offset = PARAM_INT(2);
    }
    uint64_t value;
    memcached_st       *memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_increment(memc, PARAM(1), PARAM_LEN(1), offset, &value))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MemCachedDecrement, 2, 3)
    T_HANDLE(MemCachedDecrement, 0)
    T_STRING(MemCachedDecrement, 1)

    unsigned int offset = 1;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MemCachedDecrement, 2)
        offset = PARAM_INT(2);
    }
    uint64_t value;
    memcached_st       *memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_decrement(memc, PARAM(1), PARAM_LEN(1), offset, &value))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedFlush, 2)
    T_HANDLE(MemCachedFlush, 0)
    T_NUMBER(MemCachedFlush, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_flush(memc, (time_t)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedBehaviorSet, 3)
    T_HANDLE(MemCachedBehaviorSet, 0)
    T_NUMBER(MemCachedBehaviorSet, 1)
    T_NUMBER(MemCachedBehaviorSet, 2)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_behavior_set(memc, (memcached_behavior_t)PARAM(1), (unsigned long long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MemCachedBehaviorGet, 2)
    T_HANDLE(MemCachedBehaviorGet, 0)
    T_NUMBER(MemCachedBehaviorGet, 1)

    memcached_st * memc = (memcached_st *)PARAM_INT(0);
    RETURN_NUMBER(memcached_behavior_get(memc, (memcached_behavior_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

