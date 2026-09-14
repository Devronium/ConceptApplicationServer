#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(needle_init)
    CONCEPT_FUNCTION(needle_complete)
    CONCEPT_FUNCTION(needle_reset)
}
#endif // __LIBRARY_H
