#ifdef _WIN32
 #define _WIN32_WINNT    0x0501
#endif
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include <stdlib.h>
#include "library.h"
#include <string.h>
#include <sys/stat.h>
#include "AnsiString.h"

#include "lmdb.h"
#ifdef _WIN32
 #include <direct.h>
#endif
#include <limits.h>
#ifndef max
 #define max(a, b)    a > b ? a : b;
#endif
#define MDB_INT_VAL_TYPE    int64_t

// moved into lmdb.c

/*extern "C" {
    static int mdb_cmp_int(const MDB_val *a, const MDB_val *b) {
            return (*(MDB_INT_VAL_TYPE *)a->mv_data < *(MDB_INT_VAL_TYPE *)b->mv_data) ? -1 : *(MDB_INT_VAL_TYPE *)a->mv_data > *(MDB_INT_VAL_TYPE *)b->mv_data;
    }
   }*/

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(MDB_FIXEDMAP)
    DEFINE_ECONSTANT(MDB_NOSUBDIR)
    DEFINE_ECONSTANT(MDB_NOSYNC)
    DEFINE_ECONSTANT(MDB_RDONLY)
    DEFINE_ECONSTANT(MDB_NOMETASYNC)
    DEFINE_ECONSTANT(MDB_WRITEMAP)
    DEFINE_ECONSTANT(MDB_MAPASYNC)
    DEFINE_ECONSTANT(MDB_NOTLS)
    DEFINE_ECONSTANT(MDB_NOLOCK)
    DEFINE_ECONSTANT(MDB_NORDAHEAD)
    DEFINE_ECONSTANT(MDB_NOMEMINIT)

    DEFINE_ECONSTANT(MDB_REVERSEKEY)
    DEFINE_ECONSTANT(MDB_DUPSORT)
    DEFINE_ECONSTANT(MDB_INTEGERKEY)
    DEFINE_ECONSTANT(MDB_DUPFIXED)
    DEFINE_ECONSTANT(MDB_INTEGERDUP)
    DEFINE_ECONSTANT(MDB_REVERSEDUP)
    DEFINE_ECONSTANT(MDB_CREATE)
    DEFINE_ECONSTANT(MDB_NOOVERWRITE)
    DEFINE_ECONSTANT(MDB_NODUPDATA)
    DEFINE_ECONSTANT(MDB_CURRENT)
    DEFINE_ECONSTANT(MDB_RESERVE)
    DEFINE_ECONSTANT(MDB_APPEND)
    DEFINE_ECONSTANT(MDB_APPENDDUP)
    DEFINE_ECONSTANT(MDB_MULTIPLE)
    DEFINE_ECONSTANT(MDB_CP_COMPACT)
    DEFINE_ECONSTANT(MDB_FIRST)
    DEFINE_ECONSTANT(MDB_FIRST_DUP)
    DEFINE_ECONSTANT(MDB_GET_BOTH)
    DEFINE_ECONSTANT(MDB_GET_BOTH_RANGE)
    DEFINE_ECONSTANT(MDB_GET_CURRENT)
    DEFINE_ECONSTANT(MDB_GET_MULTIPLE)
    DEFINE_ECONSTANT(MDB_LAST)
    DEFINE_ECONSTANT(MDB_LAST_DUP)
    DEFINE_ECONSTANT(MDB_NEXT)
    DEFINE_ECONSTANT(MDB_NEXT_DUP)
    DEFINE_ECONSTANT(MDB_NEXT_MULTIPLE)
    DEFINE_ECONSTANT(MDB_NEXT_NODUP)
    DEFINE_ECONSTANT(MDB_PREV)
    DEFINE_ECONSTANT(MDB_PREV_DUP)
    DEFINE_ECONSTANT(MDB_PREV_NODUP)
    DEFINE_ECONSTANT(MDB_SET)
    DEFINE_ECONSTANT(MDB_SET_KEY)
    DEFINE_ECONSTANT(MDB_SET_RANGE)
    DEFINE_ECONSTANT(MDB_SUCCESS)
    DEFINE_ECONSTANT(MDB_KEYEXIST)
    DEFINE_ECONSTANT(MDB_NOTFOUND)
    DEFINE_ECONSTANT(MDB_PAGE_NOTFOUND)
    DEFINE_ECONSTANT(MDB_CORRUPTED)
    DEFINE_ECONSTANT(MDB_PANIC)
    DEFINE_ECONSTANT(MDB_VERSION_MISMATCH)
    DEFINE_ECONSTANT(MDB_INVALID)
    DEFINE_ECONSTANT(MDB_MAP_FULL)
    DEFINE_ECONSTANT(MDB_DBS_FULL)
    DEFINE_ECONSTANT(MDB_READERS_FULL)
    DEFINE_ECONSTANT(MDB_TLS_FULL)
    DEFINE_ECONSTANT(MDB_TXN_FULL)
    DEFINE_ECONSTANT(MDB_CURSOR_FULL)
    DEFINE_ECONSTANT(MDB_PAGE_FULL)
    DEFINE_ECONSTANT(MDB_MAP_RESIZED)
    DEFINE_ECONSTANT(MDB_INCOMPATIBLE)
    DEFINE_ECONSTANT(MDB_BAD_RSLOT)
    DEFINE_ECONSTANT(MDB_BAD_TXN)
    DEFINE_ECONSTANT(MDB_BAD_VALSIZE)
    DEFINE_ECONSTANT(MDB_BAD_DBI)
    DEFINE_ECONSTANT(MDB_LAST_ERRCODE)
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapOpen, 1, 6)
    T_STRING(0)
    size_t map_size = 10485760;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        map_size = PARAM(1);
    }
    int flags = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        flags = PARAM_INT(2);
    }

    int maxdbs = 10;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(3)
        maxdbs = PARAM_INT(3);
    }

    int maxreaders = 8;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        maxreaders = PARAM_INT(4);
    }

    MDB_env *env = 0;
    mdb_env_create(&env);
    if (env) {
        mdb_env_set_maxreaders(env, maxreaders);
        mdb_env_set_mapsize(env, map_size);
        mdb_env_set_maxdbs(env, maxdbs);
#ifdef _WIN32
        mkdir(PARAM(0));
#else
        mkdir(PARAM(0), 0777L);
#endif
        int rc = mdb_env_open(env, PARAM(0), flags, 0664);
        if (rc != MDB_SUCCESS) {
            mdb_env_close(env);
            env = NULL;
        }
        if (PARAMETERS_COUNT > 5) {
            SET_NUMBER(5, rc);
        }
    } else {
        if (PARAMETERS_COUNT > 5) {
            SET_NUMBER(5, -101);
        }
    }
    RETURN_NUMBER((SYS_INT)env);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MapClose, 1)
    T_HANDLE(0)

    MDB_env * env = (MDB_env *)(SYS_INT)PARAM(0);
    mdb_env_close(env);
    SET_NUMBER(0, 0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapPut, 2, 6)
    T_HANDLE(0)
    T_ARRAY(1)
    char *database = NULL;
    int db_flags = MDB_DUPSORT;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        db_flags = PARAM(2);
    }

    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }
    int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        flags = PARAM_INT(4);
    }
    int use_int = 0;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        use_int = PARAM_INT(5);
    }

    MDB_txn          *txn;
    MDB_dbi          dbi;
    MDB_INT_VAL_TYPE val;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, 0, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    if (database)
        db_flags |= MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

//if (use_int)
//    mdb_set_compare(txn, dbi, mdb_cmp_int64);

    int     count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    MDB_val vkey, data;

    char **keys = NULL;
    if (count > 0) {
        keys = (char **)malloc(count * sizeof(char *));
        Invoke(INVOKE_ARRAY_KEYS, PARAMETER(1), keys, (INTEGER)count);
    }

    if (keys) {
        for (INTEGER i = 0; i < count; i++) {
            void *newpData = 0;
            char *key      = keys[i];
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            //Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(1), i, &key);
            if (key) {
                if (newpData) {
                    char    *szData;
                    INTEGER type;
                    NUMBER  nData;

                    Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if (type == VARIABLE_STRING) {
                        vkey.mv_size = strlen(key);
                        vkey.mv_data = key;
                        data.mv_size = nData;
                        data.mv_data = szData;
                        mdb_put(txn, dbi, &vkey, &data, flags);
                    } else
                    if (type == VARIABLE_NUMBER) {
                        vkey.mv_size = strlen(key);
                        vkey.mv_data = key;
                        if (use_int) {
                            val          = nData;
                            data.mv_size = sizeof(MDB_INT_VAL_TYPE);
                            data.mv_data = &val;
                        } else {
                            data.mv_size = sizeof(NUMBER);
                            data.mv_data = &nData;
                        }
                        mdb_put(txn, dbi, &vkey, &data, flags);
                    }
                }
            }
        }
        res = mdb_txn_commit(txn);
        free(keys);
    } else {
        mdb_txn_abort(txn);
        res = 0;
    }
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapPutMultiple, 2, 6)
    T_HANDLE(0)
    T_ARRAY(1)
    char *database = NULL;
    int db_flags = MDB_DUPSORT;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        db_flags = PARAM(2);
    }

    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }
    int flags = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        flags = PARAM_INT(4);
    }
    int use_int = 0;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        use_int = PARAM_INT(5);
    }

    MDB_txn          *txn;
    MDB_dbi          dbi;
    MDB_INT_VAL_TYPE val;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, 0, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    if (database)
        db_flags |= MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

//if (use_int)
//    mdb_set_compare(txn, dbi, mdb_cmp_int64);

    int     count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    MDB_val vkey, data;
    char    *szData;
    INTEGER type;
    NUMBER  nData;

    for (INTEGER i = 0; i < count; i++) {
        void *arr_var = NULL;
        int  count    = Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &arr_var);
        if (arr_var) {
            Invoke(INVOKE_GET_VARIABLE, arr_var, &type, &szData, &nData);
            if (type == VARIABLE_ARRAY) {
                int  count_var = Invoke(INVOKE_GET_ARRAY_COUNT, arr_var);
                char **keys    = NULL;
                if (count_var > 0) {
                    keys = (char **)malloc(count * sizeof(char *));
                    Invoke(INVOKE_ARRAY_KEYS, arr_var, keys, (INTEGER)count_var);
                }

                if (keys) {
                    for (INTEGER i = 0; i < count_var; i++) {
                        void *newpData = 0;
                        char *key      = keys[i];
                        Invoke(INVOKE_ARRAY_VARIABLE, arr_var, i, &newpData);
                        if (key) {
                            if (newpData) {
                                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                                if (type == VARIABLE_STRING) {
                                    vkey.mv_size = strlen(key);
                                    vkey.mv_data = key;
                                    data.mv_size = nData;
                                    data.mv_data = szData;
                                    mdb_put(txn, dbi, &vkey, &data, flags);
                                } else
                                if (type == VARIABLE_NUMBER) {
                                    vkey.mv_size = strlen(key);
                                    vkey.mv_data = key;
                                    if (use_int) {
                                        val          = nData;
                                        data.mv_size = sizeof(MDB_INT_VAL_TYPE);
                                        data.mv_data = &val;
                                    } else {
                                        data.mv_size = sizeof(NUMBER);
                                        data.mv_data = &nData;
                                    }

                                    mdb_put(txn, dbi, &vkey, &data, flags);
                                }
                            }
                        }
                    }
                    free(keys);
                }
            }
        }
    }
    res = mdb_txn_commit(txn);

    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGet, 3, 4)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = 0;
    if (database)
        db_flags = MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);
    res          = mdb_get(txn, dbi, &vkey, &data);
    if (((res == MDB_SUCCESS) || (res == MDB_KEYEXIST)) && (data.mv_size) && (data.mv_data)) {
        SET_BUFFER(2, data.mv_data, (NUMBER)data.mv_size);
    } else {
        SET_STRING(2, "");
    }

    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGetNumber, 3, 4)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = 0;
    if (database)
        db_flags = MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);
    res          = mdb_get(txn, dbi, &vkey, &data);
    if (((res == MDB_SUCCESS) || (res == MDB_KEYEXIST)) && (data.mv_size == sizeof(NUMBER)) && (data.mv_data)) {
        SET_NUMBER(2, *(NUMBER *)data.mv_data);
    } else {
        SET_NUMBER(2, 0);
    }

    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGetInt, 3, 4)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = 0;
    if (database)
        db_flags = MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);
    res          = mdb_get(txn, dbi, &vkey, &data);
    if (((res == MDB_SUCCESS) || (res == MDB_KEYEXIST)) && (data.mv_size == sizeof(MDB_INT_VAL_TYPE)) && (data.mv_data)) {
        SET_NUMBER(2, *(MDB_INT_VAL_TYPE *)data.mv_data);
    } else {
        SET_NUMBER(2, 0);
    }

    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapRemove, 2, 3)
    T_HANDLE(0)
    T_ARRAY(1)
    char *database = NULL;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2)
        if (PARAM_LEN(2))
            database = PARAM(2);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, 0, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = 0;
    if (database)
        db_flags = MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    int     count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    MDB_val vkey;
    for (INTEGER i = 0; i < count; i++) {
        void *newpData = 0;
        Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                vkey.mv_size = nData;
                vkey.mv_data = szData;
                mdb_del(txn, dbi, &vkey, NULL);
            }
        }
    }

    res = mdb_txn_commit(txn);
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MapError, 1)
    T_NUMBER(0)
    char *res = mdb_strerror(PARAM_INT(0));
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MapInfo, 1)
    T_HANDLE(0)
    MDB_env * env = (MDB_env *)(SYS_INT)PARAM(0);
    MDB_stat    mst;
    MDB_envinfo mei;
    mdb_env_stat(env, &mst);
    mdb_env_info(env, &mei);
    CREATE_ARRAY(RESULT);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Map address", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)mei.me_mapaddr);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Map size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mei.me_mapsize);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Page size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_psize);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Max pages", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(mei.me_mapsize / mst.ms_psize));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Used pages", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mei.me_last_pgno + 1);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Last transaction ID", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mei.me_last_txnid);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Max readers", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mei.me_maxreaders);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Used readers", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mei.me_numreaders);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Tree depth", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_depth);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Branch pages", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_branch_pages);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Leaf pages", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_leaf_pages);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Overflow pages", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_overflow_pages);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Entries", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)mst.ms_entries);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapRemoveValue, 2, 4)
    T_HANDLE(0)
    T_ARRAY(1)
    char *database = NULL;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2)
        if (PARAM_LEN(2))
            database = PARAM(2);
    }
    int use_int = 0;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(3)
        use_int = PARAM_INT(3);
    }

    MDB_txn          *txn;
    MDB_dbi          dbi;
    MDB_INT_VAL_TYPE val;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, 0, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = 0;
    if (database)
        db_flags = MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

//if (use_int)
//    mdb_set_compare(txn, dbi, mdb_cmp_int64);

    int     count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    MDB_val vkey, data;

    char **keys = NULL;
    if (count > 0) {
        keys = (char **)malloc(count * sizeof(char *));
        Invoke(INVOKE_ARRAY_KEYS, PARAMETER(1), keys, (INTEGER)count);
    }

    if (keys) {
        for (INTEGER i = 0; i < count; i++) {
            void *newpData = 0;
            char *key      = keys[i];
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), i, &newpData);
            //Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(1), i, &key);
            if (key) {
                if (newpData) {
                    char    *szData;
                    INTEGER type;
                    NUMBER  nData;

                    Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                    if (type == VARIABLE_STRING) {
                        vkey.mv_size = strlen(key);
                        vkey.mv_data = key;
                        data.mv_size = nData;
                        data.mv_data = szData;

                        mdb_del(txn, dbi, &vkey, &data);
                    } else
                    if (type == VARIABLE_NUMBER) {
                        vkey.mv_size = strlen(key);
                        vkey.mv_data = key;
                        if (use_int) {
                            data.mv_size = sizeof(MDB_INT_VAL_TYPE);
                            data.mv_data = &val;
                        } else {
                            data.mv_size = sizeof(NUMBER);
                            data.mv_data = &nData;
                        }

                        mdb_del(txn, dbi, &vkey, &data);
                    }
                }
            }
        }
        res = mdb_txn_commit(txn);
        free(keys);
    } else {
        mdb_txn_abort(txn);
        res = 0;
    }
    mdb_dbi_close(env, dbi);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGetAll, 2, 8)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;

    size_t        len    = 0;
    size_t        start  = 0;
    MDB_cursor_op op     = MDB_NEXT_DUP;
    MDB_cursor_op op2    = MDB_NEXT;
    MDB_cursor_op set_op = MDB_SET;
    MDB_val       limit_key;
    limit_key.mv_size = 0;
    limit_key.mv_data = 0;
    int cmp_flag = -1;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        if (PARAM_INT(2)) {
            op       = MDB_PREV_DUP;
            op2      = MDB_PREV;
            cmp_flag = 1;
        }
    }

    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        len = PARAM(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        start = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        if (PARAM_INT(6))
            set_op = MDB_SET_RANGE;
    }
    if (PARAMETERS_COUNT > 7) {
        T_STRING(7)
        limit_key.mv_data = PARAM(7);
        limit_key.mv_size = PARAM_LEN(7);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = MDB_DUPSORT;
    if (database)
        db_flags |= MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);

    MDB_cursor *cursor = NULL;
    res = mdb_cursor_open(txn, dbi, &cursor);
    CREATE_ARRAY(RESULT);
    if (res == MDB_SUCCESS) {
        int rc;
        if (vkey.mv_size)
            rc = mdb_cursor_get(cursor, &vkey, &data, set_op);
        else {
            if ((op != MDB_PREV) && (op != MDB_PREV_DUP))
                rc = mdb_cursor_get(cursor, &vkey, &data, MDB_NEXT_DUP);
        }
        if (rc == 0) {
            vkey.mv_size = PARAM_LEN(1);
            vkey.mv_data = PARAM(1);
            if ((op == MDB_PREV) || (op == MDB_PREV_DUP))
                mdb_cursor_get(cursor, &vkey, &data, MDB_LAST_DUP);
            size_t idx      = 0;
            size_t elements = 0;
            do {
                if ((set_op != MDB_SET) || (vkey.mv_size == PARAM_LEN(1))) {
                    if (idx >= start) {
                        if ((set_op != MDB_SET) && (PARAMETERS_COUNT > 7)) {
                            if (mdb_cmp(txn, dbi, &limit_key, &vkey) == cmp_flag)
                                break;
                        }
                        //if (PARAM_LEN(1)) {
                        if ((data.mv_data) && (data.mv_size > 0))
                            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_STRING, (char *)data.mv_data, (NUMBER)data.mv_size);
                        else
                            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);

                        /*} else {
                            AnsiString key;
                            key.LoadBuffer((char *)vkey.mv_data, vkey.mv_size);
                            if ((data.mv_data) && (data.mv_size > 0))
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY,RESULT,key.c_str(),(INTEGER)VARIABLE_STRING,(char *)data.mv_data,(NUMBER)data.mv_size);
                            else
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY,RESULT,key.c_str(),(INTEGER)VARIABLE_STRING,(char *)"",(NUMBER)0);
                           }*/
                        elements++;
                        if ((len > 0) && (elements >= len))
                            break;
                    }
                    idx++;
                }
                vkey.mv_size = PARAM_LEN(1);
                vkey.mv_data = PARAM(1);
            } while (((rc = mdb_cursor_get(cursor, &vkey, &data, op)) == 0) || ((set_op != MDB_SET) && ((rc = mdb_cursor_get(cursor, &vkey, &data, op2)) == 0)));
        }
        mdb_cursor_close(cursor);
    }
    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
//RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGetAllNumber, 2, 9)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;
    size_t        len    = 0;
    size_t        start  = 0;
    MDB_cursor_op op     = MDB_NEXT_DUP;
    MDB_cursor_op op2    = MDB_NEXT;
    MDB_cursor_op set_op = MDB_SET;
    MDB_val       limit_key;
    limit_key.mv_size = 0;
    limit_key.mv_data = 0;
    int cmp_flag = -1;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        if (PARAM_INT(2)) {
            op       = MDB_PREV;
            op2      = MDB_PREV;
            cmp_flag = 1;
        }
    }

    if (PARAMETERS_COUNT > 3) {
        T_STRING(3)
        if (PARAM_LEN(3))
            database = PARAM(3);
    }

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        len = PARAM(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        start = PARAM_INT(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        if (PARAM_INT(6))
            set_op = MDB_SET_RANGE;
    }
    if (PARAMETERS_COUNT > 7) {
        T_STRING(7)
        limit_key.mv_data = PARAM(7);
        limit_key.mv_size = PARAM_LEN(7);
    }
    int use_int = 0;
    if (PARAMETERS_COUNT > 8) {
        T_NUMBER(8)
        use_int = PARAM_INT(8);
    }

    MDB_txn          *txn;
    MDB_dbi          dbi;
    MDB_INT_VAL_TYPE val;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = MDB_DUPSORT;
    if (database)
        db_flags |= MDB_CREATE;

    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

//if (use_int)
//    mdb_set_compare(txn, dbi, mdb_cmp_int64);

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);

    MDB_cursor *cursor = NULL;
    res = mdb_cursor_open(txn, dbi, &cursor);
    CREATE_ARRAY(RESULT);
    if (res == MDB_SUCCESS) {
        int rc;
        if (vkey.mv_size)
            rc = mdb_cursor_get(cursor, &vkey, &data, set_op);
        else {
            if ((op != MDB_PREV) && (op != MDB_PREV_DUP))
                rc = mdb_cursor_get(cursor, &vkey, &data, MDB_NEXT_DUP);
        }
        if (rc == 0) {
            vkey.mv_size = PARAM_LEN(1);
            vkey.mv_data = PARAM(1);
            if ((op == MDB_PREV) || (op == MDB_PREV_DUP))
                mdb_cursor_get(cursor, &vkey, &data, MDB_LAST_DUP);
            size_t idx      = 0;
            size_t elements = 0;
            do {
                if ((set_op != MDB_SET) || (vkey.mv_size == PARAM_LEN(1))) {
                    if (idx >= start) {
                        if (data.mv_data) {
                            if (use_int) {
                                if (data.mv_size == sizeof(MDB_INT_VAL_TYPE)) {
                                    if ((set_op != MDB_SET) && (PARAMETERS_COUNT > 7)) {
                                        if (mdb_cmp(txn, dbi, &limit_key, &vkey) == cmp_flag)
                                            break;
                                    }
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_NUMBER, (char *)"", *((NUMBER *)data.mv_data));
                                    elements++;
                                }
                            } else
                            if (data.mv_size == sizeof(NUMBER)) {
                                if ((set_op != MDB_SET) && (PARAMETERS_COUNT > 7)) {
                                    if (mdb_cmp(txn, dbi, &limit_key, &vkey) == cmp_flag)
                                        break;
                                }
                                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_NUMBER, (char *)"", *((NUMBER *)data.mv_data));
                                elements++;
                            }
                        }
                        if ((len > 0) && (elements >= len))
                            break;
                    }
                    idx++;
                }
                vkey.mv_size = PARAM_LEN(1);
                vkey.mv_data = PARAM(1);
            } while (((rc = mdb_cursor_get(cursor, &vkey, &data, op)) == 0) || ((set_op != MDB_SET) && ((rc = mdb_cursor_get(cursor, &vkey, &data, op2)) == 0)));
        }
        mdb_cursor_close(cursor);
    }
    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
//RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
char **GetCharList2(void *arr, int **lens, INVOKE_CALL _Invoke) {
    INTEGER type      = 0;
    NUMBER  nr        = 0;
    void    *newpData = 0;
    char    **ret     = 0;

    int count = _Invoke(INVOKE_GET_ARRAY_COUNT, arr);

    ret        = new char * [count + 1];
    *lens      = new int[count + 1];
    ret[count] = 0;

    for (int i = 0; i < count; i++) {
        _Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            _Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_STRING) {
                ret[i]     = szData;
                (*lens)[i] = (int)nData;
            } else {
                delete[] *lens;
                delete[] ret;
                *lens = NULL;
                ret   = NULL;
                return NULL;
            }
        }
    }
    return ret;
}

//------------------------------------------------------------------------
int ParseKey(MDB_val *start_key, MDB_val *end_key, char *key, int len, int reversed = 0) {
    start_key->mv_data = 0;
    start_key->mv_size = 0;
    end_key->mv_data   = 0;
    end_key->mv_size   = 0;
    MDB_val *target_key = start_key;
    target_key->mv_data = key;
    int has_operator = 0;
    for (int i = 0; i < len; i++) {
        char c = key[i];
        switch (c) {
            case '<':
                if (reversed)
                    target_key = start_key;
                else
                    target_key = end_key;
                target_key->mv_data = key + i + 1;
                target_key->mv_size = 0;
                has_operator       |= 2;
                break;

            case '>':
                if (reversed)
                    target_key = end_key;
                else
                    target_key = start_key;
                target_key->mv_size = 0;
                target_key->mv_data = key + i + 1;
                has_operator       |= 1;
                break;

            default:
                target_key->mv_size++;
                break;
        }
    }
    return has_operator;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapGetAllNumberMI, 3, 7)
    T_HANDLE(0)
    T_ARRAY(1)
    T_ARRAY(2)

    CREATE_ARRAY(RESULT);
//char *database = NULL;
    size_t        len      = 0;
    size_t        start    = 0;
    MDB_cursor_op op       = MDB_NEXT_DUP;
    MDB_cursor_op op2      = MDB_NEXT;
    MDB_cursor_op set_op   = MDB_SET;
    int           cmp_flag = -1;

    int db_count = NULL;

    MDB_dbi *dbi_list = NULL;
    int     array_len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    if (array_len <= 0)
        return 0;

    db_count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    if (array_len != db_count)
        return 0;

    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(3)
        if (PARAM_INT(3)) {
            op       = MDB_PREV_DUP;
            op2      = MDB_PREV;
            cmp_flag = 1;
        }
    }

    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        len = PARAM(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(5)
        start = PARAM_INT(5);
    }
    int use_int  = 0;
    int datasize = sizeof(NUMBER);

    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(6)
        use_int  = PARAM_INT(6);
        datasize = sizeof(MDB_INT_VAL_TYPE);
    }

    MDB_txn *txn;
    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = MDB_DUPSORT | MDB_CREATE;

    char **databases = GetCharList(PARAMETER(2), Invoke);
    int  idx         = 0;
    dbi_list = (MDB_dbi *)malloc(sizeof(MDB_dbi) * db_count);
    while ((databases) && (databases[idx])) {
        char *database = databases[idx];
        dbi_list[idx] = 0;
        res           = mdb_dbi_open(txn, database, db_flags, &dbi_list[idx]);
        if (res != MDB_SUCCESS) {
            mdb_txn_abort(txn);
            RETURN_NUMBER(res);
            delete[] databases;
            free(dbi_list);
            return 0;
        }
        idx++;
    }
    if (databases) {
        delete[] databases;
        databases = NULL;
    }

    MDB_val vkey, vkey_saved, endkey, data;

    endkey.mv_size = 0;
    endkey.mv_data = 0;

    data.mv_size = 0;
    data.mv_data = 0;


    int  *lens  = NULL;
    char **keys = GetCharList2(PARAMETER(1), &lens, Invoke);
    if ((!keys) || (!lens)) {
        if (dbi_list) {
            for (int i = 0; i < db_count; i++) {
                if (dbi_list[i])
                    mdb_dbi_close(env, dbi_list[i]);
            }
            free(dbi_list);
        }
        return 0;
    }
// for each PARAM(1)
    MDB_cursor *cursor        = NULL;
    MDB_cursor **child_cursor = NULL;
    res = mdb_cursor_open(txn, dbi_list[0], &cursor);
    size_t skip = start;

//if (use_int)
//    mdb_set_compare(txn, dbi_list[0], mdb_cmp_int64);

    if (db_count > 1) {
        child_cursor = (MDB_cursor **)malloc(sizeof(MDB_cursor *) * db_count);
        for (int i = 0; i < db_count; i++)
            child_cursor[i] = 0;
    }

    if (res == MDB_SUCCESS) {
        int has_limit     = ParseKey(&vkey, &endkey, keys[0], lens[0], op == MDB_PREV_DUP);
        int has_limit_key = 0;
        if (has_limit & 1)
            set_op = MDB_SET_RANGE;
        if (has_limit & 2)
            has_limit_key = 1;
        vkey_saved.mv_data = vkey.mv_data;
        vkey_saved.mv_size = vkey.mv_size;
        int rc;
        if (vkey.mv_size)
            rc = mdb_cursor_get(cursor, &vkey, &data, set_op);
        else {
            if ((op != MDB_PREV) && (op != MDB_PREV_DUP))
                rc = mdb_cursor_get(cursor, &vkey, &data, MDB_NEXT_DUP);
        }
        if (rc == 0) {
            vkey.mv_data = vkey_saved.mv_data;
            vkey.mv_size = vkey_saved.mv_size;
            if ((op == MDB_PREV) || (op == MDB_PREV_DUP))
                mdb_cursor_get(cursor, &vkey, &data, MDB_LAST_DUP);

            size_t elements = 0;
            //vkey.mv_size = lens[0];
            //vkey.mv_data = keys[0];
            do {
                if ((data.mv_data) && (data.mv_size == datasize) && ((set_op != MDB_SET) || (vkey.mv_size == lens[0]))) {
                    if (has_limit_key) {
                        if (mdb_cmp(txn, dbi_list[0], &endkey, &vkey) == cmp_flag)
                            break;
                    }

                    MDB_val vkey2, endkey2, data2;
                    bool    add_item = true;
                    for (int i = 1; i < db_count; i++) {
                        data2.mv_size = data.mv_size;
                        data2.mv_data = data.mv_data;

                        MDB_cursor *target_cursor = child_cursor[i];
                        if (!target_cursor) {
                            //if (use_int)
                            //    mdb_set_compare(txn, dbi_list[i], mdb_cmp_int64);

                            mdb_cursor_open(txn, dbi_list[i], &target_cursor);
                            child_cursor[i] = target_cursor;
                        }
                        if (!target_cursor)
                            break;
                        // not efficient !!!
                        // rewrite
                        int has_limit     = ParseKey(&vkey2, &endkey2, keys[i], lens[i]);
                        int has_limit_key = 0;
                        if (has_limit & 2)
                            has_limit_key = 1;

                        if (has_limit) {
                            //vkey_saved2.mv_data = vkey2.mv_data;
                            //vkey_saved2.mv_size = vkey2.mv_size;

                            if (has_limit & 1)
                                res = mdb_cursor_get(target_cursor, &vkey2, &data2, MDB_SET_RANGE);
                            else
                                res = mdb_cursor_get(target_cursor, &vkey2, &data2, MDB_FIRST_DUP);

                            if (((res != MDB_SUCCESS) && (res != MDB_KEYEXIST)) || (!data.mv_size) || (!data.mv_data)) {
                                add_item = false;
                                break;
                            }

                            add_item = false;
                            do {
                                if ((has_limit_key) && (mdb_cmp(txn, dbi_list[i], &vkey2, &endkey2) == 1))
                                    break;
                                if (use_int) {
                                    if (*(MDB_INT_VAL_TYPE *)data2.mv_data == *(MDB_INT_VAL_TYPE *)data.mv_data) {
                                        add_item = true;
                                        break;
                                    }
                                } else
                                if (*(NUMBER *)data2.mv_data == *(NUMBER *)data.mv_data) {
                                    add_item = true;
                                    break;
                                }
                            } while (((rc = mdb_cursor_get(target_cursor, &vkey2, &data2, MDB_NEXT_DUP)) == 0) || ((rc = mdb_cursor_get(target_cursor, &vkey2, &data2, MDB_NEXT)) == 0));
                            if (!add_item)
                                break;
                        } else {
                            res = mdb_cursor_get(target_cursor, &vkey2, &data2, MDB_GET_BOTH);
                            if (((res != MDB_SUCCESS) && (res != MDB_KEYEXIST)) || (!data.mv_size) || (!data.mv_data)) {
                                add_item = false;
                                break;
                            }
                        }

                        /*res = mdb_get(txn, dbi_list[i], &vkey2, &data2);
                           if (((res != MDB_SUCCESS) && (res != MDB_KEYEXIST)) || (!data.mv_size) || (!data.mv_data)) {
                            add_item = false;
                            break;
                           }*/
                    }
                    if (add_item) {
                        if (skip) {
                            skip--;
                        } else {
                            if (use_int)
                                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(*((MDB_INT_VAL_TYPE *)data.mv_data)));
                            else
                                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)elements, (INTEGER)VARIABLE_NUMBER, (char *)"", *((NUMBER *)data.mv_data));
                            elements++;
                            if ((len > 0) && (elements >= len))
                                break;
                        }

                        if ((len > 0) && (elements >= len))
                            break;
                    }
                }
                //vkey.mv_size = lens[0];
                //vkey.mv_data = keys[0];
                //} while ((rc = mdb_cursor_get(cursor, &vkey, &data, op)) == 0);
            } while (((rc = mdb_cursor_get(cursor, &vkey, &data, op)) == 0) || ((set_op != MDB_SET) && ((rc = mdb_cursor_get(cursor, &vkey, &data, op2)) == 0)));
        }
        mdb_cursor_close(cursor);
    }
    if (child_cursor) {
        for (int i = 0; i < db_count; i++) {
            if (child_cursor[i])
                mdb_cursor_close(child_cursor[i]);
        }
        free(child_cursor);
        child_cursor = NULL;
    }
    mdb_txn_abort(txn);

    if (dbi_list) {
        for (int i = 0; i < db_count; i++) {
            if (dbi_list[i])
                mdb_dbi_close(env, dbi_list[i]);
        }
        free(dbi_list);
    }

    if (keys)
        delete[] keys;
    if (lens)
        delete[] lens;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MapDrop, 2)
    T_HANDLE(0)
    T_STRING(1)
    MDB_txn * txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, 0, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    res = mdb_dbi_open(txn, PARAM(1), 0, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }
// 1 = also close the dbi
    res = mdb_drop(txn, dbi, 1);
    mdb_txn_commit(txn);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MapCount, 2, 3)
    T_HANDLE(0)
    T_STRING(1)
    char *database = NULL;

    if (PARAMETERS_COUNT > 2) {
        T_STRING(2)
        if (PARAM_LEN(2))
            database = PARAM(2);
    }

    MDB_txn *txn;
    MDB_dbi dbi;

    MDB_env *env = (MDB_env *)(SYS_INT)PARAM(0);

    int res = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (res != MDB_SUCCESS) {
        RETURN_NUMBER(res);
        return 0;
    }
    int db_flags = MDB_DUPSORT;
    res = mdb_dbi_open(txn, database, db_flags, &dbi);
    if (res != MDB_SUCCESS) {
        mdb_txn_abort(txn);
        RETURN_NUMBER(res);
        return 0;
    }

    MDB_val vkey, data;

    data.mv_size = 0;
    data.mv_data = 0;

    vkey.mv_size = PARAM_LEN(1);
    vkey.mv_data = PARAM(1);
    MDB_cursor *cursor = NULL;
    res = mdb_cursor_open(txn, dbi, &cursor);

    size_t countp = 0;
    if (res == MDB_SUCCESS) {
        int rc;
        if (vkey.mv_size)
            rc = mdb_cursor_get(cursor, &vkey, &data, MDB_SET);
        if (rc == 0)
            res = mdb_cursor_count(cursor, &countp);
        mdb_cursor_close(cursor);
    }
    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
    if (res != 0) {
        RETURN_NUMBER(res);
    } else {
        RETURN_NUMBER(countp);
    }
END_IMPL
//------------------------------------------------------------------------
