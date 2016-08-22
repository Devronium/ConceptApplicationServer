//#define GO_DEBUG

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#ifdef _WIN32
 #include <windows.h>
#else
 #include <pthread.h>
 #include <unistd.h>
 #include <sched.h>
 #define DWORD     long
 #define LPVOID    void *
 #define POSIX_SEMAPHORES
#endif

#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <queue>
#include "semhh.h"

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
    void        *variable;
    AnsiString  class_name;
    AnsiString  data;
    INVOKE_CALL Invoke;
};

class ThreadMetaContainer {
private:
    std::queue<AnsiString *> input_data;
    std::queue<AnsiString *> output_data;
    HHSEM input_lock;
    HHSEM output_lock;
public:
    void *worker;

    int AddInput(char *S, int len) {
        AnsiString *temp = new AnsiString();

        temp->LoadBuffer(S, len);
        semp(input_lock);
        input_data.push(temp);
        int size = input_data.size();
        semv(input_lock);
        return size;
    }

    int AddOutput(char *S, int len) {
        AnsiString *temp = new AnsiString();

        temp->LoadBuffer(S, len);
        semp(output_lock);
        output_data.push(temp);
        int size = output_data.size();
        semv(output_lock);
        return size;
    }

    int GetInput(AnsiString **S) {
        int size = input_data.size();

        if (size > 0) {
            semp(input_lock);
            *S = input_data.front();
            input_data.pop();
            semv(input_lock);
        }
        return size;
    }

    int GetOutput(AnsiString **S) {
        int size = output_data.size();

        if (size > 0) {
            semp(output_lock);
            *S = output_data.front();
            output_data.pop();
            semv(output_lock);
        }
        return size;
    }

    ThreadMetaContainer(void *worker) {
        seminit(input_lock, 1);
        seminit(output_lock, 1);
        this->worker = worker;
    }

    ~ThreadMetaContainer() {
        semp(input_lock);
        while (!output_data.empty()) {
            AnsiString *S = output_data.front();
            output_data.pop();
            if (S)
                delete S;
        }
        while (!input_data.empty()) {
            AnsiString *S = input_data.front();
            input_data.pop();
            if (S)
                delete S;
        }
        semv(input_lock);
        semdel(input_lock);
        semdel(output_lock);
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
    sem_t * sm = (sem_t *)(SYS_INT)PARAM(0);
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

    RETURN_NUMBER((SYS_INT)items);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GreenletAdd, 2)
    T_HANDLE(GreenletAdd, 0)
    T_DELEGATE(GreenletAdd, 1)

    std::vector<void *> *items = (std::vector<void *> *)(SYS_INT) PARAM(0);
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

    std::vector<void *> *items = (std::vector<void *> *)(SYS_INT) PARAM(0);
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
    void            *variable  = cdc->variable;
    AnsiString      class_name = cdc->class_name;
    AnsiString      data       = cdc->data;
    INVOKE_CALL     Invoke     = cdc->Invoke;
    delete cdc;

    void *CONTEXT[2];
    void *parameter = NULL;
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
CONCEPT_FUNCTION_IMPL(CreateWorker, 3)
    T_STRING(CreateWorker, 0)
    SET_NUMBER(1, 0);
    T_STRING(CreateWorker, 2)
    void *worker = NULL;
    Invoke(INVOKE_CREATE_WORKER, PARAMETERS->HANDLER, &worker);
    if (worker) {
        void *variable = NULL;
        Invoke(INVOKE_CREATE_VARIABLE_2, worker, &variable);
        if (variable) {
            ThreadMetaContainer *tmc = new ThreadMetaContainer(worker);
            Invoke(INVOKE_SETPROTODATA, worker, (int)2, tmc, thread_destroy_metadata);

            WorkerContainer *cdc = new WorkerContainer();
            cdc->worker     = worker;
            cdc->variable   = variable;
            cdc->class_name = PARAM(0);
            cdc->data.LoadBuffer(PARAM(2), PARAM_LEN(2));
            cdc->Invoke = Invoke;
#ifdef _WIN32
            DWORD  threadID = 0;
            HANDLE thandle  = CreateThread(NULL, 0, WorkerFunction, (LPVOID)cdc, 0, &threadID);
            threadID = (long)thandle;
#else
            pthread_t threadID = 0;
            if (pthread_create(&threadID, NULL, WorkerFunction, (LPVOID)cdc))
                threadID = 0;
#endif
            SET_NUMBER(1, (SYS_INT)threadID);
        }
        if (!variable) {
            Invoke(INVOKE_FREE_WORKER, worker);
            return (void *)"CreateWorker: Class not found";
        }
    }
    RETURN_NUMBER((SYS_INT)worker);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(AddWorkerData, 2)
    T_HANDLE(AddWorkerData, 0)
    T_STRING(AddWorkerData, 1)

    void *worker = (void *)(SYS_INT)PARAM(0);
    ThreadMetaContainer *tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, worker, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    int size = tmc->AddInput(PARAM(1), PARAM_LEN(1));
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
CONCEPT_FUNCTION_IMPL(GetWorkerResultData, 2)
    T_HANDLE(GetWorkerResultData, 0)

    void *worker = (void *)(SYS_INT)PARAM(0);
    ThreadMetaContainer *tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, worker, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    AnsiString *temp = NULL;
    int        size  = tmc->GetOutput(&temp);
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
CONCEPT_FUNCTION_IMPL(GetWorkerData, 1)
    ThreadMetaContainer * tmc = NULL;
    Invoke(INVOKE_GETPROTODATA, PARAMETERS->HANDLER, (int)2, &tmc);
    if (!tmc)
        return (void *)"Using a worker function on a non-worker";

    AnsiString *temp = NULL;
    int        size  = tmc->GetInput(&temp);
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

    void *worker = (void *)(SYS_INT)PARAM(0);
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
    RETURN_NUMBER((SYS_INT)tmc->worker);
END_IMPL
//---------------------------------------------------------------------------

