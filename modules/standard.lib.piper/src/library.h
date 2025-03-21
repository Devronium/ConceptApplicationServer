#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(PiperInit)
    CONCEPT_FUNCTION(PiperTTS)
    CONCEPT_FUNCTION(PiperDone)
}
#endif // __LIBRARY_H
