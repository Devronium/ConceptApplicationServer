//#define GO_DEBUG

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#ifdef _WIN32
 #include <windows.h>
 #include <pthread.h>
#else
 #include <pthread.h>
 #include <unistd.h>
 #include <sched.h>
 #include <sys/time.h>
 #define DWORD     long
 #define LPVOID    void *
 #define POSIX_SEMAPHORES
#endif

#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#ifdef USE_STD_QUEUE
#include <queue>
#endif
#include "semhh.h"
#include <map>
#include <string>

#ifdef POSIX_SEMAPHORES
 #include <semaphore.h>
#endif

struct Container {
    void *DELEGATE;
    void *CONTEXT;
    INTEGER *SPINLOCK;
};

struct WorkerContainer {
    void        *worker;
    // void        *variable;
    AnsiString  class_name;
    AnsiString  data;
    INVOKE_CALL Invoke;
};

#ifdef POSIX_SEMAPHORES
#define QUEUE_LOCK(sm)      if (sem_wait(&sm)) perror("sem_wait");
#define QUEUE_UNLOCK(sm)    if (sem_post(&sm)) perror("sem_post");
#define QUEUE_DONE(sm)      sem_destroy(&sm)
#define QUEUE_CREATE(sm)    if (sem_init(&sm, 0, 1)) perror("sem_init");
#define QUEUE_SEMAPHORE     sem_t
#else
#define QUEUE_LOCK(sm)      semp(sm)
#define QUEUE_UNLOCK(sm)    semv(sm)
#define QUEUE_DONE(sm)      semdel(sm)
#define QUEUE_CREATE(sm)    seminit(sm, 1)
#define QUEUE_SEMAPHORE     HHSEM
#endif

#ifndef USE_STD_QUEUE
struct SimpleQueueNODE {
    void *DATA;
    void *NEXT;
};

class SimpleQueue {
    SimpleQueueNODE *ROOT;
    SimpleQueueNODE *LAST;
    int node_count;
public:
    SimpleQueue() {
        ROOT = NULL;
        LAST = NULL;
        node_count = 0;
    }

    void push(void *data) {
        SimpleQueueNODE *NODE = new SimpleQueueNODE();
        NODE->NEXT = NULL;
        NODE->DATA = data;
        if (LAST)
            LAST->NEXT = NODE;
        else
            ROOT = NODE;
        LAST = NODE;
        node_count++;
    }

    void push_front(void *data) {
        SimpleQueueNODE *NODE = new SimpleQueueNODE();
        NODE->NEXT = ROOT;
        NODE->DATA = data;
        if (!ROOT)
            LAST = NODE;
        ROOT = NODE;
        node_count++;
    }

    void *front() {
        if (ROOT)
            return ROOT->DATA;
        return NULL;
    }

    int size() {
        return node_count;
    }

    int empty() {
        if (node_count <= 0)
            return 1;
        return 0;
    }

    void pop() {
        if (ROOT) {
            node_count--;
            SimpleQueueNODE *NODE = ROOT;
            ROOT = (SimpleQueueNODE *)ROOT->NEXT;
            if (LAST == NODE)
                LAST = NULL;
            delete NODE;
        }
    }
};
#endif

class CondWait {
private:
#ifdef _WIN32
    HANDLE cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
public:
    CondWait() {
#ifdef _WIN32
        cond = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
#endif
    }

    int wait(int timeout_ms = 0) {
#ifdef _WIN32
        if (timeout_ms > 0)
            return WaitForSingleObject(cond, timeout_ms);
        return WaitForSingleObject(cond, INFINITE);
#else
        if (timeout_ms > 0) {
            struct timeval now;
            gettimeofday(&now, NULL);
            struct timespec delay;
            delay.tv_sec  = now.tv_sec + timeout_ms / 1000;
            delay.tv_nsec = now.tv_usec * 1000 + (timeout_ms % 1000000) * 1000000;
            if (delay.tv_nsec >= 1000000000) {
                delay.tv_nsec -= 1000000000;
                delay.tv_sec++;
            }
            return pthread_cond_timedwait(&cond, &mutex, &delay);
        }
        return pthread_cond_wait(&cond, &mutex);
#endif
    }

    void notify() {
#ifdef _WIN32
        SetEvent(cond);
#else
        pthread_cond_signal(&cond);
#endif
    }

    ~CondWait() {
#ifdef _WIN32
        CloseHandle(cond);
#else
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
#endif
    }
};

#define RWLOCK_TYPE pthread_rwlock_t *
class RWLock {
private:
    QUEUE_SEMAPHORE access_lock;
    std::map<std::string, RWLOCK_TYPE> locks;

    RWLOCK_TYPE Get(const char *lock_name) {
        QUEUE_LOCK(access_lock);
        RWLOCK_TYPE lock = locks[lock_name];
        if (!lock) {
            lock = (RWLOCK_TYPE)malloc(sizeof(pthread_rwlock_t));
            if (lock) {
                memset(lock, 0, sizeof(pthread_rwlock_t));
                if (!pthread_rwlock_init(lock, NULL)) {
                    locks[lock_name] = lock;
                } else {
                    free(lock);
                    lock = NULL;
                }
            }
        }
        QUEUE_UNLOCK(access_lock);
        return lock;
    }
public:
    RWLock() {
        QUEUE_CREATE(access_lock);
    }

    int ReadLock(const char *lock_name, int non_blocking = 0) {
        RWLOCK_TYPE lock = Get(lock_name);
        if (!lock)
            return 0;

        if (non_blocking) {
            if (pthread_rwlock_tryrdlock(lock))
                return 0;
        } else {
            if (pthread_rwlock_rdlock(lock))
                return 0;
        }

        return 1;
    }

    int WriteLock(const char *lock_name, int non_blocking = 0) {
        RWLOCK_TYPE lock = Get(lock_name);
        if (!lock)
            return 0;
        
        if (non_blocking) {
            if (pthread_rwlock_trywrlock(lock))
                return 0;
        } else {
            if (pthread_rwlock_wrlock(lock))
                return 0;
        }

        return 1;
    }

    int ReadUnlock(const char *lock_name) {
        RWLOCK_TYPE lock = Get(lock_name);
        if (!lock)
            return 0;
        
        if (pthread_rwlock_unlock(lock))
            return 0;

        return 1;
    }

    int WriteUnlock(const char *lock_name) {
        RWLOCK_TYPE lock = Get(lock_name);
        if (!lock)
            return 0;
        
        if (pthread_rwlock_unlock(lock))
            return 0;

        return 1;
    }

    int DestroyLock(const char *lock_name) {
        QUEUE_LOCK(access_lock);
        RWLOCK_TYPE lock = locks[lock_name];
        if (lock)
            locks.erase(lock_name);
        QUEUE_UNLOCK(access_lock);
        if (lock) {
            pthread_rwlock_destroy(lock);
            free(lock);
            return 1;
        }
        return 0;
    }

    ~RWLock() {
        QUEUE_LOCK(access_lock);
        for (std::map<std::string, RWLOCK_TYPE>::iterator it = locks.begin(); it != locks.end(); ++it) {
            RWLOCK_TYPE lock = it->second;
            if (lock) {
                pthread_rwlock_destroy(lock);
                free(lock);
            }
        }
        QUEUE_UNLOCK(access_lock);
        QUEUE_DONE(access_lock);
    }
};

class ShareContext {
private:
    QUEUE_SEMAPHORE share_lock;
    std::map<std::string, std::map<std::string, AnsiString> *> share_data;
    int links;
public:
    ShareContext() {
        links = 0;
        QUEUE_CREATE(share_lock);
    }

    void Retain() {
        QUEUE_LOCK(share_lock);
        links++;
        QUEUE_UNLOCK(share_lock);
    }

    int Release() {
        QUEUE_LOCK(share_lock);
        links--;
        QUEUE_UNLOCK(share_lock);
        return links;
    }

    int SetKey(char *master_key, char *key, char *value, int len) {
        int size = 0;
        if ((master_key) && (key)) {
            QUEUE_LOCK(share_lock);
            std::map<std::string, AnsiString> *map = share_data[master_key];
            if (!map) {
                map = new std::map<std::string, AnsiString>();
                share_data[master_key] = map;
            }
            size = map->size();
            (*map)[key].LoadBuffer(value, len);
            QUEUE_UNLOCK(share_lock);
        }
        return size;
    }

    AnsiString *GetKey(char *master_key, char *key) {
        AnsiString *data = NULL;
        if ((master_key) && (key)) {
            QUEUE_LOCK(share_lock);
            std::map<std::string, AnsiString> *map = share_data[master_key];
            if (!map) {
                QUEUE_UNLOCK(share_lock);
                return data;
            }
            data = &(*map)[key];
            QUEUE_UNLOCK(share_lock);
        }
        return data;
    }

    AnsiString *GetSetKey(char *master_key, char *key, char *value, int len) {
        AnsiString *data = NULL;
        if ((master_key) && (key)) {
            QUEUE_LOCK(share_lock);
            std::map<std::string, AnsiString> *map = share_data[master_key];
            if (!map) {
                map = new std::map<std::string, AnsiString>();
                share_data[master_key] = map;
            }
            data = &(*map)[key];
            if ((!data) || (!data->Length()))
                (*map)[key].LoadBuffer(value, len);
            QUEUE_UNLOCK(share_lock);
        }
        return data;
    }

    int RemoveKey(char *master_key, char *key = NULL) {
        int size = 0;
        if (master_key) {
            QUEUE_LOCK(share_lock);
            std::map<std::string, AnsiString> *map = share_data[master_key];
            if (!map) {
                QUEUE_UNLOCK(share_lock);
                return size;
            }
            if (key)
                map->erase(key);
            else
                share_data.erase(master_key);
            QUEUE_UNLOCK(share_lock);
            size = map->size();
            if (!key)
                delete map;
        }
        return size;
    }

    ~ShareContext() {
        QUEUE_LOCK(share_lock);
        for (std::map<std::string, std::map<std::string, AnsiString> *>::iterator it = share_data.begin(); it != share_data.end(); ++it) {
            std::map<std::string, AnsiString> *map = it->second;
            if (map)
                delete map;
        }
        QUEUE_UNLOCK(share_lock);
        QUEUE_DONE(share_lock);
    }
};

class ThreadMetaContainer {
private:
#ifdef USE_STD_QUEUE
    std::queue<AnsiString *> input_data;
    std::queue<AnsiString *> output_data;
#else
    SimpleQueue input_data;
    SimpleQueue output_data;
#endif
    QUEUE_SEMAPHORE input_lock;
    QUEUE_SEMAPHORE output_lock;

    int input_cond_wait;
    int output_cond_wait;
    CondWait input_cond;
    CondWait output_cond;
public:
    ShareContext *sharecontext;
    RWLock rwlock;
    void *worker;

    int AddInput(char *S, int len, int priority = 0) {
        AnsiString *temp = new AnsiString();

        temp->LoadBuffer(S, len);
        QUEUE_LOCK(input_lock);
#if USE_STD_QUEUE
        input_data.push(temp);
#else
        if (priority > 0)
            input_data.push_front(temp);
        else
            input_data.push(temp);
#endif
        int size = input_data.size();
        if ((size == 1) && (input_cond_wait))
            input_cond.notify();
        QUEUE_UNLOCK(input_lock);
        return size;
    }

    int AddOutput(char *S, int len) {
        AnsiString *temp = new AnsiString();

        temp->LoadBuffer(S, len);
        QUEUE_LOCK(output_lock);
        output_data.push(temp);
        int size = output_data.size();
        if ((size == 1) && (output_cond_wait))
            output_cond.notify();
        QUEUE_UNLOCK(output_lock);
        return size;
    }

    int GetInput(AnsiString **S, int locking = 0) {
        QUEUE_LOCK(input_lock);
        int size = input_data.size();

        if (size > 0) {
            *S = (AnsiString *)input_data.front();
            input_data.pop();
        }
        if (locking)
            input_cond_wait = 1;
        QUEUE_UNLOCK(input_lock);
        if ((locking) && (!size)) {
            input_cond.wait(locking);
            return GetInput(S);
        }
        return size;
    }

    int GetOutput(AnsiString **S, int locking = 0) {
        QUEUE_LOCK(output_lock);
        int size = output_data.size();

        if (size > 0) {
            *S = (AnsiString *)output_data.front();
            output_data.pop();
        }
        if (locking)
            output_cond_wait = 1;
        QUEUE_UNLOCK(output_lock);
        if ((locking) && (!size)) {
            output_cond.wait(locking);
            return GetOutput(S);
        }
        return size;
    }

    ThreadMetaContainer(void *worker) {
        QUEUE_CREATE(input_lock);
        QUEUE_CREATE(output_lock);
        this->input_cond_wait = 0;
        this->output_cond_wait = 0;
        this->worker = worker;
        this->sharecontext = NULL;
    }

    void SetShareContext(void *context) {
        if (sharecontext) {
            if (sharecontext->Release() <= 0)
                delete sharecontext;
        }
        sharecontext = (ShareContext *)context;
        if (sharecontext)
            sharecontext->Retain();
    }

    ~ThreadMetaContainer() {
        QUEUE_LOCK(input_lock);
        while (!output_data.empty()) {
            AnsiString *S = (AnsiString *)output_data.front();
            output_data.pop();
            if (S)
                delete S;
        }
        while (!input_data.empty()) {
            AnsiString *S = (AnsiString *)input_data.front();
            input_data.pop();
            if (S)
                delete S;
        }
        SetShareContext(NULL);
        QUEUE_UNLOCK(input_lock);
        QUEUE_DONE(input_lock);
        QUEUE_DONE(output_lock);
    }
};

void thread_destroy_metadata(void *data, void *handler) {
    if (data)
        delete (ThreadMetaContainer *)data;
}

INVOKE_CALL LocalInvoker;
static int  MultiThreaded = 0;
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    LocalInvoker  = Invoke;
    MultiThreaded = Invoke(INVOKE_MULTITHREADED);
    DEFINE_ICONSTANT("RWLOCK_READ", 1);
    DEFINE_ICONSTANT("RWLOCK_WRITE", 2);
    DEFINE_ICONSTANT("RWUNLOCK_READ", 3);
    DEFINE_ICONSTANT("RWUNLOCK_WRITE", 4);
    DEFINE_ICONSTANT("RWLOCK_DONE", 5);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
#ifdef _WIN32
DWORD WINAPI ThreadFunction(LPVOID DPARAM) {
#else
LPVOID ThreadFunction(LPVOID DPARAM) {
#endif
    Container *cdc      = (Container *)DPARAM;
    void      *DELEGATE = cdc->DELEGATE;
    void      *context  = cdc->CONTEXT;
    INTEGER   *spin_lock = cdc->SPINLOCK;
    delete cdc;

    void *RES       = 0;
    void *EXCEPTION = 0;
    LocalInvoker(INVOKE_SET_THREAD_DATA, context);
    LocalInvoker(INVOKE_CALL_DELEGATE_THREAD_SPINLOCK, DELEGATE, &RES, &EXCEPTION, spin_lock, (INTEGER)0);
    LocalInvoker(INVOKE_FREE_VARIABLE, RES);
    LocalInvoker(INVOKE_FREE_VARIABLE, EXCEPTION);
    LocalInvoker(INVOKE_FREE_VARIABLE, DELEGATE);
    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_RunThread CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(1, 2, "RunThread parameters error : RunThread(delegate, detached=false)");
    if (!MultiThreaded) {
        return (void *)"RunThread must be called from multi-threaded Concept core. You are now running a non-thread safe version of Concept.";
    }
    LOCAL_INIT;

    char   *cls;
    NUMBER mid;
    void   *context    = 0;
    NUMBER ndetachable = 0;

    GET_CHECK_DELEGATE(0, cls, mid, "RunThread : delegate should be of DELEGATE type");
    if (PARAMETERS_COUNT > 1) {
        GET_CHECK_NUMBER(1, ndetachable, "RunThread : detached should be of NUMBER type");
    }

    // LocalInvoker(INVOKE_DYNAMIC_LOCK,PARAMETER(0));
    LocalInvoker(INVOKE_LOCK_VARIABLE, PARAMETER(0));
    LocalInvoker(INVOKE_GET_THREAD_DATA, &context);

    Container *cdc = new Container();
    INTEGER spin_lock = 1;
    cdc->DELEGATE = PARAMETER(0);
    cdc->CONTEXT  = context;
    cdc->SPINLOCK = &spin_lock;
#ifdef _WIN32
    DWORD  threadID = 0;
    HANDLE thandle  = CreateThread(NULL, 0, ThreadFunction, (LPVOID)cdc, 0, &threadID);
    if (!thandle)
        spin_lock = 0;
    if ((thandle) && ((int)ndetachable))
        CloseHandle(thandle);
    threadID = (long)thandle;
#else
    pthread_t threadID = 0;
    if ((int)ndetachable) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&threadID, &attr, ThreadFunction, (LPVOID)cdc)) {
            threadID = 0;
            spin_lock = 0;
        }
    } else {
        if (pthread_create(&threadID, NULL, ThreadFunction, (LPVOID)cdc)) {
            threadID = 0;
            spin_lock = 0;
        }
    }
#endif
    while (spin_lock == 1) {
#ifdef _WIN32
        // ugly windows (should use YieldProcessor() instead)
        Sleep(1);
#else
        sched_yield();
#endif
    }

    RETURN_NUMBER((double)(long)threadID);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_KillThread CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "KillThread parameters error : KillThread(threadID)");

    LOCAL_INIT;
    NUMBER nThreadID;

    GET_CHECK_NUMBER(0, nThreadID, "KillThread : threadID should be of STATIC NUMBER type");

#ifdef _WIN32
    TerminateThread((HANDLE)(long)nThreadID, 0);
#else
    pthread_cancel((pthread_t)(long)nThreadID);
#endif
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ThreadIsActive CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "ThreadIsActive parameters error : ThreadIsActive(threadID)");

    LOCAL_INIT;
    NUMBER nThreadID;

    GET_CHECK_NUMBER(0, nThreadID, "ThreadIsActive : threadID should be of STATIC NUMBER type");

#ifdef _WIN32
    DWORD result = WaitForSingleObject((HANDLE)(long)nThreadID, 0);
    if (result == WAIT_OBJECT_0) {
        RETURN_NUMBER(0);
    } else
    if (result == WAIT_TIMEOUT) {
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(-1);
    }
#else
    int res = pthread_kill((pthread_t)(long)nThreadID, 0);
    if (res == 0) {
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_WaitThread CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "WaitThread parameters error : WaitThread(threadID)");

    LOCAL_INIT;
    NUMBER nThreadID;

    GET_CHECK_NUMBER(0, nThreadID, "WaitThread : threadID should be of STATIC NUMBER type");

#ifdef _WIN32
    WaitForSingleObject((HANDLE)(long)nThreadID, INFINITE);
    CloseHandle((HANDLE)(long)nThreadID);
#else
    void *data;
    pthread_join((pthread_t)(long)nThreadID, &data);
#endif
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__Sleep CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "Sleep : Sleep(miliseconds)");

    LOCAL_INIT;
    NUMBER nSeconds;

    GET_CHECK_NUMBER(0, nSeconds, "Sleep : 'miliseconds' should be of STATIC NUMBER type");

#ifdef _WIN32
    Sleep((long)nSeconds);
#else
    usleep((long)nSeconds * 1000);
#endif
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_semcreate, 0)
#ifdef POSIX_SEMAPHORES
    sem_t * sm = (sem_t *)malloc(sizeof(sem_t));
#else
    HHSEM *sm = (HHSEM *)malloc(sizeof(HHSEM));
#endif
    RETURN_NUMBER((uintptr_t)sm)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_semdone, 1)
    T_HANDLE(_semdone, 0)
#ifdef POSIX_SEMAPHORES
    sem_t * sm = (sem_t *)(uintptr_t)PARAM(0);
    sem_destroy(sm);
#else
    HHSEM *sm = (HHSEM *)(uintptr_t)PARAM(0);
    semdel((*sm));
#endif
    free(sm);
    SET_NUMBER(0, 0)

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_seminit, 2)
    T_HANDLE(_seminit, 0)
    T_NUMBER(_seminit, 1)
#ifdef POSIX_SEMAPHORES
    sem_t *sm = (sem_t *)(uintptr_t)PARAM(0);
    int err = sem_init(sm, 0, 1);
    RETURN_NUMBER(err);
#else
    HHSEM *sm = (HHSEM *)(uintptr_t)PARAM(0);
    seminit((*sm), PARAM_INT(1));
    RETURN_NUMBER(0)
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_semp, 1)
    T_HANDLE(_semp, 0)

#ifdef POSIX_SEMAPHORES
    sem_t *sm = (sem_t *)(uint64_t)PARAM(0);
    int err = sem_wait(sm);
    RETURN_NUMBER(err);
#else
    HHSEM *sm = (HHSEM *)(uintptr_t)PARAM(0);
    semp((*sm));
    RETURN_NUMBER(0)
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_semv, 1)
    T_HANDLE(_semv, 0)

#ifdef POSIX_SEMAPHORES
    sem_t * sm = (sem_t *)(uintptr_t)PARAM(0);
    int err = sem_post(sm);
    RETURN_NUMBER(err);
#else
    HHSEM *sm = (HHSEM *)(uintptr_t)PARAM(0);
    semv((*sm));
    RETURN_NUMBER(0)
#endif
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsThreaded, 0)
    int res = Invoke(INVOKE_MULTITHREADED);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(Greenlets, 1)
    T_ARRAY(Greenlets, 0)

    std::vector<void *> *items = new std::vector<void *>;
    INTEGER count  = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    void    *prec  = 0;
    void    *first = 0;
    void    *arr   = PARAMETER(0);
    for (INTEGER i = 0; i < count; i++) {
        void *var = 0;
        Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &var);
        if (var) {
            INTEGER type    = 0;
            char    *szData = 0;
            NUMBER  nData   = 0;

            Invoke(INVOKE_GET_VARIABLE, var, &type, &szData, &nData);
            if (type == VARIABLE_DELEGATE) {
                void *ref = 0;
                Invoke(INVOKE_GREENTHREAD, var, &ref);
                if (ref) {
                    Invoke(INVOKE_LOCK_VARIABLE, var);
                    items->push_back(ref);
                    if (prec)
                        Invoke(INVOKE_GREENLINK, prec, ref);
                    prec = ref;
                    if (!first)
                        first = ref;
                    Invoke(INVOKE_GREENLINK, ref, first);
                }
            }
        }
    }

    RETURN_NUMBER((uintptr_t)items);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GreenletAdd, 2)
    T_HANDLE(GreenletAdd, 0)
    T_DELEGATE(GreenletAdd, 1)

    std::vector<void *> *items = (std::vector<void *> *)(uintptr_t) PARAM(0);
    void *ref = 0;
    int  res  = Invoke(INVOKE_GREENTHREAD, PARAMETER(1), &ref);
    if (ref) {
        void *first = items->front();
        // check if run-time insert is possible
        if (!IS_OK(Invoke(INVOKE_GREENINSERT, first, ref))) {
            void *prec = items->back();
            items->push_back(ref);
            if (prec)
                Invoke(INVOKE_GREENLINK, prec, ref);
            if (!first)
                first = ref;
            Invoke(INVOKE_GREENLINK, ref, first);
        }
        Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    }
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GreenLoop, 1)
    T_HANDLE(GreenLoop, 0)

    std::vector<void *> *items = (std::vector<void *> *)(uintptr_t) PARAM(0);
    void *first = items->front();
    void *last  = items->back();
    int  res    = -2;
    if (first)
        res = Invoke(INVOKE_GREENLOOP, first, last);

    delete items;
    SET_NUMBER(0, 0);
    RETURN_NUMBER(res);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(yield, 0)
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
#ifdef _WIN32
DWORD WINAPI WorkerFunction(LPVOID DPARAM) {
#else
LPVOID WorkerFunction(LPVOID DPARAM) {
#endif
    WorkerContainer *cdc       = (WorkerContainer *)DPARAM;
    void            *worker    = cdc->worker;
    //void            *variable  = cdc->variable;
    AnsiString      class_name = cdc->class_name;
    AnsiString      data       = cdc->data;
    INVOKE_CALL     Invoke     = cdc->Invoke;
    delete cdc;

    void *CONTEXT[2];
    void *parameter = NULL;

    void *variable = NULL;
    Invoke(INVOKE_CREATE_VARIABLE_2, worker, &variable);
    CONTEXT[0] = variable;
    Invoke(INVOKE_CREATE_VARIABLE_2, worker, &parameter);
    CONTEXT[1] = parameter;
    Invoke(INVOKE_SET_VARIABLE, parameter, (INTEGER)VARIABLE_STRING, data.c_str(), (NUMBER)data.Length());

    Invoke(INVOKE_CREATE_OBJECT_2, worker, variable, class_name.c_str(), CONTEXT, (INTEGER)2, (INTEGER)-1);
    Invoke(INVOKE_FREE_VARIABLE, variable);
    Invoke(INVOKE_FREE_VARIABLE, parameter);

    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(CreateWorker, 3, 4)
    T_STRING(CreateWorker, 0)
    SET_NUMBER(1, 0);
    T_STRING(CreateWorker, 2)
    ShareContext *sharecontext = NULL;
    void *worker = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(CreateWorker, 3);
        sharecontext = (ShareContext *)(SYS_INT)PARAM(3);
    }
    Invoke(INVOKE_CREATE_WORKER, PARAMETERS->HANDLER, &worker);
    if (worker) {
        //void *variable = NULL;
        //Invoke(INVOKE_CREATE_VARIABLE_2, worker, &variable);
        //if (variable) {
            ThreadMetaContainer *tmc = new ThreadMetaContainer(worker);
            Invoke(INVOKE_SETPROTODATA, worker, (int)2, tmc, thread_destroy_metadata);

            WorkerContainer *cdc = new WorkerContainer();
            cdc->worker     = worker;
            //cdc->variable   = variable;
            cdc->class_name = PARAM(0);
            cdc->data.LoadBuffer(PARAM(2), PARAM_LEN(2));
            cdc->Invoke = Invoke;
#ifdef _WIN32
            DWORD  threadID = 0;
            HANDLE thandle  = CreateThread(NULL, 0, WorkerFunction, (LPVOID)cdc, 0, &threadID);
            threadID = (long)thandle;
            if (!thandle) {
                delete cdc;
                cdc = NULL;
                //LocalInvoker(INVOKE_FREE_VARIABLE, variable);
                //variable = NULL;
            }
#else
            pthread_t threadID = 0;
            if (pthread_create(&threadID, NULL, WorkerFunction, (LPVOID)cdc)) {
                threadID = 0;
                delete cdc;
                cdc = NULL;
                //LocalInvoker(INVOKE_FREE_VARIABLE, variable);
                //variable = NULL;
            }
#endif
            SET_NUMBER(1, (uintptr_t)threadID);
        //}
        //if (!variable) {
        //    Invoke(INVOKE_FREE_WORKER, worker);
        //    return (void *)"CreateWorker: Class not found";
        //}
        if (sharecontext)
            tmc->SetShareContext(sharecontext);
    }

    RETURN_NUMBER((uintptr_t)worker);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(AddWorkerData, 2, 3)
    T_HANDLE(AddWorkerData, 0)
    T_STRING(AddWorkerData, 1)

    int priority = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(AddWorkerData, 2);
        priority = PARAM_INT(2);
    }
    void *worker = (void *)(uintptr_t)PARAM(0);
    ThreadMetaContainer *tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, worker, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    int size = tmc->AddInput(PARAM(1), PARAM_LEN(1), priority);
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddWorkerResultData, 1)
    T_STRING(AddWorkerResultData, 0)

    ThreadMetaContainer * tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";
    int size = tmc->AddOutput(PARAM(0), PARAM_LEN(0));
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(GetWorkerResultData, 2, 3)
    T_HANDLE(GetWorkerResultData, 0)

    void *worker = (void *)(uintptr_t)PARAM(0);
    ThreadMetaContainer *tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, worker, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    int locking = 0;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(GetWorkerResultData, 2);
        locking = PARAM_INT(2);
    }

    AnsiString *temp = NULL;
    int        size  = tmc->GetOutput(&temp, locking);
    if ((temp) && (temp->Length())) {
        SET_BUFFER(1, temp->c_str(), temp->Length());
    } else {
        SET_STRING(1, "");
    }
    if (temp)
        delete temp;
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(GetWorkerData, 1, 2)
    ThreadMetaContainer * tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    int locking = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(GetWorkerData, 1);
        locking = PARAM_INT(1);
    }
    AnsiString *temp = NULL;
    int size  = tmc->GetInput(&temp, locking);
    if ((temp) && (temp->Length())) {
        SET_BUFFER(0, temp->c_str(), temp->Length());
    } else {
        SET_STRING(0, "");
    }
    if (temp)
        delete temp;
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(FreeWorker, 1)
    T_NUMBER(FreeWorker, 0)

    void *worker = (void *)(uintptr_t)PARAM(0);
    if (worker) {
        Invoke(INVOKE_FREE_WORKER, worker);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CurrentWorker, 0)
    ThreadMetaContainer * tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";
    RETURN_NUMBER((uintptr_t)tmc->worker);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WorkerSharedSet, 3, 4)
    T_STRING(WorkerSharedSet, 0)
    T_STRING(WorkerSharedSet, 1)
    T_STRING(WorkerSharedSet, 2)

    ShareContext *sharecontext = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(WorkerSharedGet, 3);
        sharecontext = (ShareContext *)(SYS_INT)PARAM(3);
    }
    int size = 0;
    if (!sharecontext) {
        ThreadMetaContainer * tmc = NULL;
        Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
        if (!tmc)
            return (void *)"Using a worker function on a non-worker";
        if (!tmc->sharecontext)
            return (void *)"No shared context set";

        sharecontext = tmc->sharecontext;
    }
    size = sharecontext->SetKey(PARAM(0), PARAM(1), PARAM(2), PARAM_LEN(2));
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WorkerSharedGet, 2, 3)
    T_STRING(WorkerSharedGet, 0)
    T_STRING(WorkerSharedGet, 1)

    ShareContext *sharecontext = NULL;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(WorkerSharedGet, 2);
        sharecontext = (ShareContext *)(SYS_INT)PARAM(2);
    }

    if (!sharecontext) {
        ThreadMetaContainer * tmc = NULL;
        Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
        if (!tmc)
            return (void *)"Using a worker function on a non-worker";
        if (!tmc->sharecontext)
            return (void *)"No shared context set";

        sharecontext = tmc->sharecontext;
    }
    AnsiString *data = sharecontext->GetKey(PARAM(0), PARAM(1));
    if ((data) && (data->Length())) {
        RETURN_BUFFER(data->c_str(), data->Length());
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WorkerSharedGetSet, 3, 4)
    T_STRING(WorkerSharedSet, 0)
    T_STRING(WorkerSharedSet, 1)
    T_STRING(WorkerSharedSet, 2)

    ShareContext *sharecontext = NULL;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(WorkerSharedGet, 3);
        sharecontext = (ShareContext *)(SYS_INT)PARAM(3);
    }
    int size = 0;
    if (!sharecontext) {
        ThreadMetaContainer * tmc = NULL;
        Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
        if (!tmc)
            return (void *)"Using a worker function on a non-worker";
        if (!tmc->sharecontext)
            return (void *)"No shared context set";

        sharecontext = tmc->sharecontext;
    }
    AnsiString *data = sharecontext->GetSetKey(PARAM(0), PARAM(1), PARAM(2), PARAM_LEN(2));
    if ((data) && (data->Length())) {
        RETURN_BUFFER(data->c_str(), data->Length());
    } else
    if (PARAM_LEN(2)) {
        RETURN_BUFFER(PARAM(2), PARAM_LEN(2));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WorkerSharedRemove, 1, 3)
    T_STRING(WorkerSharedRemove, 0)

    ShareContext *sharecontext = NULL;
    void *worker = NULL;
    char *key = NULL;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(WorkerSharedRemove, 1);
        key = PARAM(1);
        if ((key) && (!key[0]))
            key = NULL;
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(WorkerSharedRemove, 2);
        sharecontext = (ShareContext *)(SYS_INT)PARAM(2);
    }
    int size = 0;
    if (sharecontext) {
        size = sharecontext->RemoveKey(PARAM(0), key);
    } else {
        ThreadMetaContainer * tmc = NULL;
        Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
        if (!tmc)
            return (void *)"Using a worker function on a non-worker";
        if (!tmc->sharecontext)
            return (void *)"No shared context set";

        size = tmc->sharecontext->RemoveKey(PARAM(0), key);
    }
    RETURN_NUMBER(size);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(WorkerSharedContext, 0)
    ShareContext *context = new ShareContext();
    context->Retain();
    RETURN_NUMBER((SYS_INT)context);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(WorkerSharedRelease, 1)
    T_HANDLE(WorkerSharedRelease, 0)
    ShareContext *sharecontext = (ShareContext *)(SYS_INT)PARAM(0);
    sharecontext->Release();
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(WorkerSharedRWLock, 2)
    T_STRING(WorkerSharedRWLock, 0);
    T_NUMBER(WorkerSharedRWLock, 1);

    ThreadMetaContainer * tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    int non_blocking = 0;
    int return_code = 0;
    switch (PARAM_INT(1)) {
        case 1:
            // read lock
            return_code = tmc->rwlock.ReadLock(PARAM(0), non_blocking);
            break;
        case 2:
            // write lock
            return_code = tmc->rwlock.WriteLock(PARAM(0), non_blocking);
            break;
        case 3:
            return_code = tmc->rwlock.ReadUnlock(PARAM(0));
            break;
        case 4:
            return_code = tmc->rwlock.WriteUnlock(PARAM(0));
            break;
        case 0:
            return_code = tmc->rwlock.DestroyLock(PARAM(0));
            break;
    }
    RETURN_NUMBER(return_code);
END_IMPL
//---------------------------------------------------------------------------
