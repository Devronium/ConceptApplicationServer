#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(fvad_new)
    CONCEPT_FUNCTION(fvad_set_sample_rate)
    CONCEPT_FUNCTION(fvad_reset)
    CONCEPT_FUNCTION(fvad_set_mode)
    CONCEPT_FUNCTION(fvad_process)
    CONCEPT_FUNCTION(fvad_free)
}
#endif // __LIBRARY_H
