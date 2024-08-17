#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(llama_new)
    CONCEPT_FUNCTION(llama_load)
    CONCEPT_FUNCTION(llama_query)
    CONCEPT_FUNCTION(llama_prompt)
    CONCEPT_FUNCTION(llama_free)
}
#endif // __LIBRARY_H
