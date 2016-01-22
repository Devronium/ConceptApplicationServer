#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...
#include "stdlibrary.h"


#ifndef _WIN32
//#include <machine/param.h>
 #include <sys/types.h>
 #include <sys/ipc.h>
 #include <sys/shm.h>
#endif

extern "C" {
CONCEPT_DLL_API CONCEPT_share_create     CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_open       CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_link_count CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_size       CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_set        CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_get        CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_done       CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_lock       CONCEPT_API_PARAMETERS;
CONCEPT_DLL_API CONCEPT_share_unlock     CONCEPT_API_PARAMETERS;
}

class SHARED_MEMORY_DESCRIPTOR {
public:
#ifdef _WIN32
    HANDLE handle;
#else
    int handle;
#endif
    void *buffer;
    int  size;

    SHARED_MEMORY_DESCRIPTOR() {
        handle = 0;
        size   = 0;
        buffer = 0;
    }

    ~SHARED_MEMORY_DESCRIPTOR() {
        if (buffer) {
            if (!(--(*(long *)buffer))) {
#ifdef _WIN32
                UnmapViewOfFile(buffer);
                CloseHandle(handle);
#else
                shmdt(buffer);
                shmctl(handle, IPC_RMID, 0);
#endif
                buffer = 0;
                handle = 0;
#ifdef _WIN32
            }
#else
            } else {
                shmdt(buffer);
                buffer = 0;
                handle = 0;
            }
#endif
        }
    }
};
//----------------------------------------------------------------//
#endif // __LIBRARY_H
