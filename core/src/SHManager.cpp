#include "SHManager.h"
#include <string.h>

#ifdef _WIN32
 #include <windows.h>
#else
 #include <sys/types.h>
 #include <sys/ipc.h>
 #include <sys/shm.h>
 #include <sys/sem.h>
 #include <unistd.h>
#endif
#include <stdint.h>

#ifdef _WIN32
HANDLE hMasterFileMapping          = 0;
HANDLE hMasterSemaphoreFileMapping = 0;
#else
int hMasterFileMapping          = -1;
int hMasterSemaphoreFileMapping = -1;
#endif
void *SemData = 0;

char *_top_ptr  = 0;
bool is_created = false;
int  t_size     = 0;

bool SHIsCreated() {
    return is_created;
}

bool SHIsPooled() {
    if (_top_ptr) {
        return true;
    }
    return false;
}

static int poz = 0;
void *SHAlloc(int size) {
#ifdef ARM_ADJUST_SIZE
    size = ARM_ADJUST_SIZE(size);
#endif
    poz += size;
    char *ptr = _top_ptr;
    if (!_top_ptr) {
        return 0;
    }

    _top_ptr += size;
    t_size   += size;

    if (is_created) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void *SHPoolAlloc(char *key, int size, int nReadOnly) {
    _top_ptr = 0;
#ifdef _WIN32
    hMasterFileMapping = 0;
    HANDLE hFileMapping = 0;

    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
                                     NULL,
                                     PAGE_READWRITE,
                                     0,
                                     (DWORD)size,
                                     key
                                     );

    if (!hFileMapping) {
        return 0;
    }

    void *buffer = MapViewOfFile(hFileMapping,
                                 (intptr_t)nReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE,
                                 0, 0, 0);

    if (!buffer) {
        CloseHandle(hFileMapping);
        return 0;
    }
#else
    hMasterFileMapping = -1;
    void  *buffer      = 0;
    key_t m_key        = ftok(key, 'C');
    int   hFileMapping = shmget(m_key, (intptr_t)size, IPC_CREAT | 0666);
    if (hFileMapping != -1)
        buffer = shmat(hFileMapping, 0, ((intptr_t)nReadOnly ? SHM_R : SHM_R) | SHM_W);
#endif

    hMasterFileMapping = hFileMapping;
    _top_ptr           = (char *)buffer;
    is_created         = true;
    return buffer;
}

void *SHPoolOpen(char *key, int nReadOnly) {
    hMasterFileMapping = 0;
#ifdef _WIN32
    HANDLE hFileMapping = 0;

    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                                   FALSE,
                                   key
                                   );

    if (!hFileMapping) {
        return 0;
    }

    void *buffer = MapViewOfFile(hFileMapping,
                                 (intptr_t)nReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE,
                                 0, 0, 0);

    if (!buffer) {
        CloseHandle(hFileMapping);
        return 0;
    }
#else
    hMasterFileMapping = -1;
    void  *buffer      = 0;
    key_t m_key        = ftok(key, 'C');
    int   hFileMapping = shmget(m_key, 0, 0666);
    if (hFileMapping != -1) {
        buffer = shmat(hFileMapping, 0, ((intptr_t)nReadOnly ? SHM_R : SHM_R) | SHM_W);
    }
#endif

    hMasterFileMapping = hFileMapping;
    _top_ptr           = (char *)buffer;
    return buffer;
}

void SHPoolFree(void *pBuf) {
    _top_ptr = 0;
    if (!pBuf) {
        return;
    }

#ifdef _WIN32
    UnmapViewOfFile(pBuf);
    CloseHandle(hMasterFileMapping);
    hMasterFileMapping = 0;
#else
    shmdt(pBuf);
    if (hMasterFileMapping > -1) {
        struct shmid_ds shm_data;
        if (shmctl(hMasterFileMapping, IPC_STAT, &shm_data) != -1) {
            if (shm_data.shm_nattch <= 1) {
                shmctl(hMasterFileMapping, IPC_RMID, NULL);
            }
        }
    }
    hMasterFileMapping = -1;
#endif
}

bool SHLock(char *key) {
    SemData = 0;
    int size = 1;
#ifdef _WIN32
    char key2 [0xFFF + 11];
    strncpy(key2, key, 0xFFF);
    strcat(key2, ".semaphore");
    hMasterSemaphoreFileMapping = 0;
    HANDLE hFileMapping = 0;

    hFileMapping = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL,
                                     PAGE_READWRITE,
                                     0,
                                     (DWORD)size,
                                     key2
                                     );

    if (!hFileMapping) {
        return false;
    }

    void *buffer = MapViewOfFile(hFileMapping,
                                 FILE_MAP_READ | FILE_MAP_WRITE,
                                 0, 0, 0);

    if (!buffer) {
        CloseHandle(hFileMapping);
        return false;
    }
#else
    char *key2 = key;
    hMasterSemaphoreFileMapping = -1;
    void  *buffer      = 0;
    key_t m_key        = ftok(key2, 'S');
    int   hFileMapping = shmget(m_key, (intptr_t)size, IPC_CREAT | 0666);
    if (hFileMapping != -1) {
        buffer = shmat(hFileMapping, 0, SHM_R | SHM_W);
        shmctl(hFileMapping, IPC_RMID, NULL);
    }
#endif

    hMasterSemaphoreFileMapping = hFileMapping;
    SemData = buffer;
    return true;
}

void SHUnLock() {
    if (!SemData) {
        return;
    }

#ifdef _WIN32
    UnmapViewOfFile(SemData);
    if (hMasterSemaphoreFileMapping) {
        CloseHandle(hMasterSemaphoreFileMapping);
    }
    hMasterSemaphoreFileMapping = 0;
#else
    shmdt(SemData);
    if (hMasterSemaphoreFileMapping > -1) {
        struct shmid_ds shm_data;
        if (shmctl(hMasterSemaphoreFileMapping, IPC_STAT, &shm_data) != -1) {
            if (shm_data.shm_nattch <= 0) {
                shmctl(hMasterSemaphoreFileMapping, IPC_RMID, NULL);
            }
        }
    }
    hMasterSemaphoreFileMapping = -1;
#endif
}

void SHLockWait(char *key) {
#ifdef _WIN32
    char key2 [0xFFF + 11];
    strncpy(key2, key, 0xFFF);
    strcat(key2, ".semaphore");

    HANDLE hFileMapping = 0;

    do {
        hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                                       FALSE,
                                       key2
                                       );
        if (hFileMapping) {
            CloseHandle(hFileMapping);
            Sleep(10);
        }
    } while (hFileMapping);
#else
    char  *key2        = key;
    int   hFileMapping = -1;
    key_t m_key        = ftok(key, 'S');
    do {
        hFileMapping = shmget(m_key, 0, 0666);
        if (hFileMapping != -1) {
            shmctl(hMasterSemaphoreFileMapping, IPC_RMID, NULL);
            usleep(10000);
        }
    } while (hFileMapping != -1);
#endif
}

