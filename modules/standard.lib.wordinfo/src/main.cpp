//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#include "roWordInfo.c"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(wordInfo, 2)
    T_STRING(wordInfo, 0);
    T_STRING(wordInfo, 1);

    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }
    if ((PARAM_LEN(1) <= 0) || (strcmp(PARAM(1), "ro"))) {
        RETURN_STRING("");
        return 0;
    }
    const char *str = wordInfo(PARAM(0), PARAM_LEN(0));
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(wordMap, 2)
    T_STRING(wordMap, 0);
    T_STRING(wordMap, 1);

    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }
    if ((PARAM_LEN(1) <= 0) || (strcmp(PARAM(1), "ro"))) {
        RETURN_STRING("");
        return 0;
    }
    const char *str = wordMap(PARAM(0), PARAM_LEN(0));
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(wordDefinition, 2)
    T_STRING(wordDefinition, 0);
    T_STRING(wordDefinition, 1);

    if (PARAM_LEN(0) <= 0) {
        RETURN_STRING("");
        return 0;
    }
    if ((PARAM_LEN(1) <= 0) || (strcmp(PARAM(1), "ro"))) {
        RETURN_STRING("");
        return 0;
    }
    const char *str = wordDefinition(PARAM(0), PARAM_LEN(0));
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
