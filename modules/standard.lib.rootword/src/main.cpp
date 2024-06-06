//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#include "roRootWord.c"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rootword, 2)
    T_STRING(rootword, 0);
    T_STRING(rootword, 1);

    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }
    if ((PARAM_LEN(1) <= 0) || (strcmp(PARAM(1), "ro"))) {
        RETURN_STRING(PARAM(0));
        return 0;
    }
    const char *str = rootWord(PARAM(0), PARAM_LEN(0));
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
