#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(radix_sort_key_value);
    CONCEPT_FUNCTION(radix_sort);
}
#endif // __LIBRARY_H
