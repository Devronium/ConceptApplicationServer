#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

CONCEPT_FUNCTION(NuoSetAutoCommit)
CONCEPT_FUNCTION(NuoGetAutoCommit)
CONCEPT_FUNCTION(NuoSetIsolationLevel)
CONCEPT_FUNCTION(NuoGetIsolationLevel)
CONCEPT_FUNCTION(NuoInfo)
CONCEPT_FUNCTION(NuoPrepareTransaction)
CONCEPT_FUNCTION(NuoRollback)
CONCEPT_FUNCTION(NuoCommit)

CONCEPT_FUNCTION(NuoConnect)
CONCEPT_FUNCTION(NuoConnectCreate)
CONCEPT_FUNCTION(NuoDisconnect)
CONCEPT_FUNCTION(NuoQuery)
CONCEPT_FUNCTION(NuoNonQuery)
CONCEPT_FUNCTION(NuoError)
CONCEPT_FUNCTION(NuoFreeResult)
CONCEPT_FUNCTION(NuoSetCursorName)
CONCEPT_FUNCTION(NuoSetNull)
CONCEPT_FUNCTION(NuoSetBlob)
CONCEPT_FUNCTION(NuoSetClob)
CONCEPT_FUNCTION(NuoBlobCreate)
CONCEPT_FUNCTION(NuoClobCreate)
CONCEPT_FUNCTION(NuoBlobWrite)
CONCEPT_FUNCTION(NuoClobWrite)

CONCEPT_FUNCTION(NuoPrepared)
CONCEPT_FUNCTION(NuoSetParameter)
CONCEPT_FUNCTION(NuoSetNull)
CONCEPT_FUNCTION(NuoExecute)
CONCEPT_FUNCTION(NuoFinish)
CONCEPT_FUNCTION(NuoParameterCount)
CONCEPT_FUNCTION(NuoParameterMeta)
CONCEPT_FUNCTION(NuoAnalyze)
CONCEPT_FUNCTION(NuoCountRows)

CONCEPT_FUNCTION(NuoCountColumns)
CONCEPT_FUNCTION(NuoColumnInfo)
CONCEPT_FUNCTION(NuoValues)
CONCEPT_FUNCTION(NuoValue)
CONCEPT_FUNCTION(NuoValueByName)
CONCEPT_FUNCTION(NuoBlob)
CONCEPT_FUNCTION(NuoBlobByName)
CONCEPT_FUNCTION(NuoClob)
CONCEPT_FUNCTION(NuoClobByName)

CONCEPT_FUNCTION(NuoFirst)
CONCEPT_FUNCTION(NuoNext)
CONCEPT_FUNCTION(NuoPrevious)
CONCEPT_FUNCTION(NuoLast)
CONCEPT_FUNCTION(NuoRefresh)
CONCEPT_FUNCTION(NuoAbsolute)
}
#endif // __LIBRARY_H