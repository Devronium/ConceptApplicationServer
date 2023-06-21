#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;


    // other functions declared here
    CONCEPT_DLL_API CONCEPT_RunThread      CONCEPT_API_PARAMETERS;
    CONCEPT_DLL_API CONCEPT_KillThread     CONCEPT_API_PARAMETERS;
    CONCEPT_DLL_API CONCEPT_WaitThread     CONCEPT_API_PARAMETERS;
    CONCEPT_DLL_API CONCEPT_ThreadIsActive CONCEPT_API_PARAMETERS;
    CONCEPT_DLL_API CONCEPT__Sleep         CONCEPT_API_PARAMETERS;

    CONCEPT_FUNCTION(Greenlets)
    CONCEPT_FUNCTION(GreenletAdd)
    CONCEPT_FUNCTION(GreenLoop)
    CONCEPT_FUNCTION(yield)

    // semaphores
    CONCEPT_FUNCTION(_semcreate)
    CONCEPT_FUNCTION(_semdone)
    CONCEPT_FUNCTION(_seminit)
    CONCEPT_FUNCTION(_semp)
    CONCEPT_FUNCTION(_semv)
    CONCEPT_FUNCTION(IsThreaded)

    CONCEPT_FUNCTION(CreateWorker)
    CONCEPT_FUNCTION(CurrentWorker)
    CONCEPT_FUNCTION(FreeWorker)
    CONCEPT_FUNCTION(AddWorkerData)
    CONCEPT_FUNCTION(AddWorkerDataAll)
    CONCEPT_FUNCTION(GetWorkerData)
    CONCEPT_FUNCTION(GetWorkerDataAll)
    CONCEPT_FUNCTION(AddWorkerResultData)
    CONCEPT_FUNCTION(AddWorkerResultDataAll)
    CONCEPT_FUNCTION(GetWorkerResultData)
    CONCEPT_FUNCTION(GetWorkerResultDataAll)
    CONCEPT_FUNCTION(RemoveWorkerData)
    CONCEPT_FUNCTION(SetWorkerName)
    CONCEPT_FUNCTION(GetWorkerInputSemaphore)
    CONCEPT_FUNCTION(GetWorkerOutputSemaphore)

    CONCEPT_FUNCTION(WorkerSharedGet)
    CONCEPT_FUNCTION(WorkerSharedSet)
    CONCEPT_FUNCTION(WorkerSharedRemove)
    CONCEPT_FUNCTION(WorkerSharedGetSet)
    CONCEPT_FUNCTION(WorkerSharedClear)

    CONCEPT_FUNCTION(WorkerSharedContext)
    CONCEPT_FUNCTION(WorkerSharedRelease)
    CONCEPT_FUNCTION(WorkerSharedRWLock)
    CONCEPT_FUNCTION(WorkerMemoryInfo)
    CONCEPT_FUNCTION(WorkerGC)
    CONCEPT_FUNCTION(WorkerErrors)

    CONCEPT_FUNCTION(WorkerSandbox)

    CONCEPT_FUNCTION(SemaphoreSocket)
    CONCEPT_FUNCTION(SemaphoreSocketRead)
    CONCEPT_FUNCTION(SemaphoreSocketWrite)
    CONCEPT_FUNCTION(SemaphoreSocketClose)
}
#endif // __LIBRARY_H
