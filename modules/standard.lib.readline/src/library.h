#ifndef __LIBRARY_H
#define __LIBRARY_H

// user definition ...

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(ttyreadline)
    CONCEPT_FUNCTION(ttysavehistory)
    CONCEPT_FUNCTION(ttyloadhistory)
    CONCEPT_FUNCTION(ttymaxhistory)
}
#endif // __LIBRARY_H
