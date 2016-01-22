//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include "AnsiList.h"

#ifdef _WIN32
 #include <windows.h>
#else
 #include <unistd.h>
#endif

AnsiList DESCRIPTORS;


long do_nothing_because_some_compilers_make_some_optimisations(long param) {
// wait one millisecond (to keep the CPU relaxed)
#ifdef _WIN32
    Sleep(1);
#else
    usleep(1000);
#endif
    if (param)
        return param >= 1;
    return 0;
}

//----------------------------------------------------------------//
#define LOCK_OWNER(VAR)     (*(long *)((char *)VAR->buffer + 3 * sizeof(long)))
#define UNLOCK(VAR)         (*(long *)((char *)VAR->buffer + 3 * sizeof(long))) = 0
#define LOCK(VAR)           (*(long *)((char *)VAR->buffer + 3 * sizeof(long))) = (long)VAR
#define WAIT_LOCKED(VAR)    while ((LOCK_OWNER(VAR)) && (LOCK_OWNER(VAR) != (long)VAR)) do_nothing_because_some_compilers_make_some_optimisations(LOCK_OWNER(VAR));
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_create CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "share_create paramters error: handle share_create(Key,nSize,[, nRead_only_flag=false])");
    LOCAL_INIT;

    NUMBER     nKey = 0;
    char       *szKey;
    NUMBER     nReadOnly = 0;
    NUMBER     size;
    int        created = 0;
    AnsiString key;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szKey, &nKey);
    if (TYPE == VARIABLE_NUMBER)
        key = AnsiString(nKey);
    else
    if (TYPE == VARIABLE_STRING)
        key.LoadBuffer(szKey, (int)nKey);
    else
        key = AnsiString((long)szKey);

    //GET_CHECK_NUMBER(0, nKey, "share_create: Parameter 0 should be of STATIC NUMBER type(key)");
    GET_CHECK_NUMBER(1, size, "share_create: Parameter 1 should be of STATIC NUMBER type(size)");
    if (PARAMETERS_COUNT == 3) {
        // checking parameters type
        GET_CHECK_NUMBER(2, nReadOnly, "Parameter 2 should be of STATIC NUMBER type(read_only_flag)");
    }
#ifdef _WIN32
    HANDLE hFileMapping = 0;

    // try to open it first, maybe already created !
    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                                   FALSE,
                                   key.c_str()
                                   );

    if (!hFileMapping) {
        created      = 1;
        hFileMapping = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL,
                                         PAGE_READWRITE,
                                         0,
                                         (DWORD)size + sizeof(long) * 4,
                                         key.c_str()
                                         );
    }

    if (!hFileMapping)
        return (void *)"share_create: error creating shared memory mapping";

    void *buffer = MapViewOfFile(hFileMapping,
                                 (long)nReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE,
                                 0, 0, 0);

    if (!buffer) {
        CloseHandle(hFileMapping);
        return (void *)"share_create: error getting shared buffer";
    }
#else
    void *buffer;
    int  hFileMapping = shmget(ftok(key, 1), (long)size + sizeof(long) * 4, IPC_CREAT | 0666);
    if (hFileMapping != -1) {
        buffer = shmat(hFileMapping, 0, ((long)nReadOnly ? SHM_R : SHM_R) | SHM_W);
        if (buffer)
            created = 1;
    }
#endif

    // links
    if (created) {
        *(long *)buffer = 1;
        *(long *)((char *)buffer + sizeof(long))     = (long)size;
        *(long *)((char *)buffer + sizeof(long) * 2) = 0;
        *(long *)((char *)buffer + sizeof(long) * 3) = 0;
    } else {
        size = *(long *)((char *)buffer + sizeof(long));
        (*(long *)buffer)++;
    }

    SHARED_MEMORY_DESCRIPTOR *MD = new SHARED_MEMORY_DESCRIPTOR;
    MD->handle = hFileMapping;
    MD->buffer = buffer;
    MD->size   = (int)size;
    DESCRIPTORS.Add(MD, DATA_SH_DESC);

    RETURN_NUMBER(DESCRIPTORS.Count());
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_open CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "share_open paramters error: handle share_open(nKey,[, nRead_only_flag=false])");
    LOCAL_INIT;

    char   *szKey    = 0;
    NUMBER nKey      = 0;
    NUMBER nReadOnly = 0;
    int    created   = 0;

    AnsiString key;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szKey, &nKey);
    if (TYPE == VARIABLE_NUMBER)
        key = AnsiString(nKey);
    else
    if (TYPE == VARIABLE_STRING)
        key.LoadBuffer(szKey, (int)nKey);
    else
        key = AnsiString((long)szKey);

    //GET_CHECK_NUMBER(0, nKey, "share_open: Parameter 0 should be of STATIC NUMBER type(key)");
    if (PARAMETERS_COUNT == 2) {
        // checking parameters type
        GET_CHECK_NUMBER(1, nReadOnly, "share_open: Parameter 1 should be of STATIC NUMBER type(read_only_flag)");
    }
#ifdef _WIN32
    HANDLE hFileMapping = 0;

    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                                   FALSE,
                                   key.c_str()
                                   );

    if (!hFileMapping) {
        RETURN_NUMBER(0);
        return 0;
    }

    void *buffer = MapViewOfFile(hFileMapping,
                                 (long)nReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE,
                                 0, 0, 0);

    if (!buffer) {
        CloseHandle(hFileMapping);
        return (void *)"share_open: error getting shared buffer";
    }
#else
    void *buffer;
    int  hFileMapping = shmget((key_t)ftok(key, 1), 0, 0666);
    if (hFileMapping != -1) {
        buffer = shmat(hFileMapping, 0, (long)nReadOnly ? SHM_R : SHM_R | SHM_W);
        if (!buffer)
            return (void *)"share_open: error getting shared buffer";
    }
#endif

    // links
    (*(long *)buffer)++;

    SHARED_MEMORY_DESCRIPTOR *MD = new SHARED_MEMORY_DESCRIPTOR;
    MD->handle = hFileMapping;
    MD->buffer = buffer;
    MD->size   = *(long *)((char *)buffer + sizeof(long));
    DESCRIPTORS.Add(MD, DATA_SH_DESC);

    RETURN_NUMBER(DESCRIPTORS.Count());
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_link_count CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "share_link_count requires one parameter (shared_memory_handle)");
    LOCAL_INIT;

    NUMBER nIndex = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_link_count: Parameter 0 should be of STATIC NUMBER type(handle)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        RETURN_NUMBER(*(long *)(((SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1))->buffer));
    } else
        return (void *)"share_link_count: Invalid connection handle";
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_size CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "share_size requires one parameter (shared_memory_handle)");
    LOCAL_INIT;

    NUMBER nIndex = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_size: Parameter 0 should be of STATIC NUMBER type(handle)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        RETURN_NUMBER(((SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1))->size);
    } else
        return (void *)"share_size: Invalid connection handle";
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_set CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "share_set requires 2 parameters (shared_memory_handle, value)");
    LOCAL_INIT;

    NUMBER nIndex = 0;
    char   *data;
    NUMBER data_len;

    GET_CHECK_NUMBER(0, nIndex, "share_set: Parameter 0 should be of STATIC NUMBER type(handle)");
    GET_CHECK_BUFFER(1, data, data_len, "share_set: Parameter 1 should be of STATIC STRING type(value)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        SHARED_MEMORY_DESCRIPTOR *MD = (SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1);
        WAIT_LOCKED(MD);
        if (data_len > MD->size) {
            return (void *)"share_set: Data to big to fit in shared memory. Increase the 'size' parameter passed to the share_create function.";
        } else {
            memcpy((char *)MD->buffer + 4 * sizeof(long), data, (int)data_len);
            *(long *)((char *)MD->buffer + 2 * sizeof(long)) = (long)data_len;
            RETURN_NUMBER(0);
        }
    } else
        return (void *)"share_set: Invalid connection handle";
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_get CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "share_get requires 1 parameters (shared_memory_handle)");
    LOCAL_INIT;

    NUMBER nIndex = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_get: Parameter 0 should be of STATIC NUMBER type(handle)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        SHARED_MEMORY_DESCRIPTOR *MD = (SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1);
        WAIT_LOCKED(MD);
        int size = *(long *)((char *)MD->buffer + 2 * sizeof(long));

        if (size > MD->size)
            return (void *)"share_get: invalid shared buffer. Data corrupted";

        RETURN_BUFFER((char *)MD->buffer + 4 * sizeof(long), size);
    } else
        return (void *)"share_get: Invalid connection handle";
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_done CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "share_done requires one parameter (shared_memory_handle)");
    LOCAL_INIT;

    NUMBER nIndex = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_done: Parameter 0 should be of STATIC NUMBER type(handle)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        DESCRIPTORS.Delete((long)nIndex - 1);
        RETURN_NUMBER(0);
    } else
        return (void *)"share_done: Invalid connection handle";
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_lock CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "share_lock requires one to two parameters (shared_memory_handle[, if_locked_wait_unlock=false])");
    LOCAL_INIT;

    NUMBER nIndex = 0;
    NUMBER nBlock = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_lock: Parameter 0 should be of STATIC NUMBER type(handle)");

    if (PARAMETERS_COUNT > 1) {
        GET_CHECK_NUMBER(1, nBlock, "share_lock: Parameter 1 should be of STATIC NUMBER type(handle)");
    }

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        SHARED_MEMORY_DESCRIPTOR *MD = (SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1);
        if ((bool)(long)nBlock) {
            WAIT_LOCKED(MD);
        }
        if (!LOCK_OWNER(MD)) {
            LOCK(MD);
            RETURN_NUMBER(1);
        } else
            RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(0);
        return (void *)"share_lock: Invalid connection handle";
    }
    return 0;
}
//----------------------------------------------------------------//
CONCEPT_DLL_API CONCEPT_share_unlock CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "share_unlock requires 1 parameters (shared_memory_handle)");
    LOCAL_INIT;

    NUMBER nIndex = 0;

    GET_CHECK_NUMBER(0, nIndex, "share_unlock: Parameter 0 should be of STATIC NUMBER type(handle)");

    if ((nIndex > 0) && (nIndex <= DESCRIPTORS.Count())) {
        SHARED_MEMORY_DESCRIPTOR *MD = (SHARED_MEMORY_DESCRIPTOR *)DESCRIPTORS.Item((long)nIndex - 1);
        if ((long)MD == LOCK_OWNER(MD)) {
            UNLOCK(MD);
            RETURN_NUMBER(1);
        } else
            RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(0);
        return (void *)"share_unlock: Invalid connection handle";
    }
    return 0;
}
//----------------------------------------------------------------//
