#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(WhisperCreate)
    CONCEPT_FUNCTION(WhisperSampleRate)
    CONCEPT_FUNCTION(WhisperDecode)
    CONCEPT_FUNCTION(WhisperStateCreate)
    CONCEPT_FUNCTION(WhisperStateFeed)
    CONCEPT_FUNCTION(WhisperStateGetText)
    CONCEPT_FUNCTION(WhisperStateFree)
    CONCEPT_FUNCTION(WhisperFree)
}
#endif // __LIBRARY_H
