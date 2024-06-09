//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#include "synonymsHash.c"
#include "synonyms.c"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(synonyms, 2)
    T_STRING(synonyms, 0);
    T_STRING(synonyms, 1);

    if (PARAM_LEN(0) <= 0) {
        RETURN_NUMBER(0);
        return 0;
    }
    if ((PARAM_LEN(1) <= 0) || (strcmp(PARAM(1), "ro"))) {
        RETURN_NUMBER(0);
        return 0;
    }
    const char **str = synonym(PARAM(0), PARAM_LEN(0));
    if (str) {
        int index = 0;
        CREATE_ARRAY(RESULT);
        while ((str) && (str[index])) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), str[index], (INTEGER)VARIABLE_STRING, (char *)str[index], (NUMBER)0);
            index ++;
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(wordIsSynonym, 3)
    T_STRING(synonyms, 0);
    T_STRING(synonyms, 1);
    T_STRING(synonyms, 2);

    if ((PARAM(0)) && (PARAM(1)) && (!strcmp(PARAM(0), PARAM(1)))) {
        RETURN_NUMBER(1);
        return 0;
    }

    if ((PARAM_LEN(0) <= 0) || (PARAM_LEN(1) <= 0)) {
        RETURN_NUMBER(0);
        return 0;
    }
    if ((PARAM_LEN(2) <= 0) || (strcmp(PARAM(2), "ro"))) {
        RETURN_NUMBER(0);
        return 0;
    }
    const char **str = synonym(PARAM(1), PARAM_LEN(1));
    if (str) {
        int index = 0;
        while ((str) && (str[index])) {
            if (!strcmp(str[index], PARAM(0))) {
                RETURN_NUMBER(1);
                return 0;
            }
            index ++;
        }
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
