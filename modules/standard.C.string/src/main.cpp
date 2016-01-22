//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "library.h"
//------------ end of standard header ----------------------------//
#include <string.h>

/*extern "C" {
   #include "snprintf.h"
   }*/
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__memchr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'memchr' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    void *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParamLen0;
    NUMBER nParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_BUFFER(0, szParam0, nParamLen0, "'memchr' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'memchr' parameter 1 should be a number (STATIC NUMBER).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'memchr' parameter 2 should be a number (STATIC NUMBER).");

    if (nParam2 > nParamLen0) {
        return (char *)"'memchr': Segmentation fault prevented by managed context";
    }

    // function call
    _C_call_result = (void *)memchr((void *)szParam0, (int)nParam1, (size_t)nParam2);

    RETURN_BUFFER(_C_call_result, _C_call_result ? nParam2 : 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__memcmp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'memcmp' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParamLen0;
    NUMBER nParamLen1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'memcmp' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'memcmp' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'memcmp' parameter 2 should be a number (STATIC NUMBER).");

    if ((nParam2 > nParamLen0) || (nParam2 > nParamLen1)) {
        return (char *)"'memcmp': Segmentation fault prevented by managed context";
    }

    // function call
    _C_call_result = (int)memcmp((void *)szParam0, (void *)szParam1, (size_t)nParam2);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__memcpy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'memcpy' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    void *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParamLen0;
    char   *szParam1;
    NUMBER nParamLen1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_BUFFER(0, szParam0, nParamLen0, "'memcpy' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_BUFFER(1, szParam1, nParamLen1, "'memcpy' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'memcpy' parameter 2 should be a number (STATIC NUMBER).");


    if (nParam2 > nParamLen1) {
        return (char *)"'memcpy': Segmentation fault prevented by managed context";
    }

    if (((size_t)nParam2) == 0) {
        SET_STRING(0, szParam0);
        RETURN_STRING("");
        return 0;
    }
    szParam0 = new char[(size_t)nParam2 + 1];
    if (!szParam0) {
        SET_STRING(0, szParam0);
        RETURN_STRING("");
        return 0;
    }
    szParam0[(size_t)nParam2] = 0;

    // function call
    _C_call_result = (void *)memcpy((void *)szParam0, (void *)szParam1, (size_t)nParam2);

    RETURN_BUFFER(_C_call_result, _C_call_result ? nParam2 : 0);
    //SET_STRING(0, szParam0);
    if (_C_call_result)
        SET_BUFFER(0, szParam0, (size_t)nParam2);

    delete[] szParam0;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__memmove CONCEPT_API_PARAMETERS {
    //return _memcpy FORWARD_CONCEPT_API_PARAMETERS;
    ALIAS_FOR(CONCEPT__memcpy);

    /*PARAMETERS_CHECK(3,"'memmove' takes 3 parameters. See help for details.");
       LOCAL_INIT;

       void*	_C_call_result;
       // Specific variables
       char *       szParam0;
       char *       szParam1;
       NUMBER       nParam2;

       // Variable type check
       // Parameter 1
       GET_CHECK_STRING(0, szParam0, "'memmove' parameter 0 should be a string (STATIC STRING).");
       // Parameter 2
       GET_CHECK_STRING(1, szParam1, "'memmove' parameter 1 should be a string (STATIC STRING).");
       // Parameter 3
       GET_CHECK_NUMBER(2, nParam2, "'memmove' parameter 2 should be a number (STATIC NUMBER).");

       // function call
       _C_call_result=(void*)memmove((void*)szParam0, (void*)szParam1, (size_t)nParam2);

       RETURN_BUFFER(_C_call_result,_C_call_result ? nParam2 : 0);
       SET_STRING(0, szParam0);
       return 0;*/
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__memset CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'memset' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    void *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParamLen0;
    NUMBER nParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_BUFFER(0, szParam0, nParamLen0, "'memset' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'memset' parameter 1 should be a number (STATIC NUMBER).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'memset' parameter 2 should be a number (STATIC NUMBER).");

    if (nParam2 > nParamLen0) {
        return (char *)"'memset': Segmentation fault prevented by managed context";
    }

    // function call
    _C_call_result = (void *)memset((void *)szParam0, (int)nParam1, (size_t)nParam2);

    RETURN_BUFFER(_C_call_result, _C_call_result ? nParamLen0 : 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strcat CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strcat' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strcat' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strcat' parameter 1 should be a string (STATIC STRING).");

    // function call
    int len1 = strlen(szParam0);
    int len2 = strlen(szParam1);

    if (len2) {
        int  len     = len1 + len2;
        char *buffer = new char[len + 1];
        strcpy(buffer, szParam0);

        _C_call_result = (char *)strcat((char *)buffer, (char *)szParam1);

        RETURN_STRING(_C_call_result);
        SET_STRING(0, buffer);
        delete[] buffer;
    } else {
        RETURN_STRING(szParam0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strchr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strchr' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strchr' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'strchr' parameter 1 should be a number (STATIC NUMBER).");

    // function call
    _C_call_result = (char *)strchr((char *)szParam0, (int)nParam1);

    RETURN_STRING(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strcmp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strcmp' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strcmp' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strcmp' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (int)strcmp((char *)szParam0, (char *)szParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strcoll CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strcoll' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strcoll' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strcoll' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (int)strcoll((char *)szParam0, (char *)szParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strcpy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strcpy' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strcpy' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strcpy' parameter 1 should be a string (STATIC STRING).");

    // function call
    //_C_call_result=(char*)strcpy((char*)szParam0, (char*)szParam1);
    _C_call_result = szParam1;

    RETURN_STRING(_C_call_result);
    SET_STRING(0, szParam1);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strcspn CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strcspn' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    size_t _C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strcspn' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strcspn' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (size_t)strcspn((char *)szParam0, (char *)szParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT___strerror CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'strerror' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_NUMBER(0, nParam0, "'strerror' parameter 0 should be a number (STATIC NUMBER).");

    // function call
    _C_call_result = (char *)strerror((int)nParam0);

    RETURN_STRING(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strlen CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "'strlen' takes 1 parameters. See help for details.");
    LOCAL_INIT;

    size_t _C_call_result;
    // Specific variables
    char *szParam0;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strlen' parameter 0 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (size_t)strlen((char *)szParam0);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strncat CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'strncat' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strncat' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strncat' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'strncat' parameter 2 should be a number (STATIC NUMBER).");

    // function call
    int len1 = strlen(szParam0);
    int len2 = strlen(szParam1);

    if (len2) {
        if ((int)nParam2 < len2)
            len2 = (int)nParam2;

        int  len     = len1 + len2;
        char *buffer = new char[len + 1];
        strcpy(buffer, szParam0);

        _C_call_result = (char *)strncat((char *)buffer, (char *)szParam1, len2);

        RETURN_STRING(_C_call_result);
        SET_STRING(0, buffer);
        delete[] buffer;
    } else {
        RETURN_STRING(szParam0);
        //SET_STRING(0, szParam0);
    }
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strncmp CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'strncmp' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    int _C_call_result;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strncmp' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strncmp' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'strncmp' parameter 2 should be a number (STATIC NUMBER).");

    // function call
    _C_call_result = (int)strncmp((char *)szParam0, (char *)szParam1, (size_t)nParam2);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strncpy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'strncpy' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strncpy' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strncpy' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'strncpy' parameter 2 should be a number (STATIC NUMBER).");

    // function call
    //_C_call_result=(char*)strncpy((char*)szParam0, (char*)szParam1, (size_t)nParam2);
    _C_call_result = szParam0;
    size_t len = strlen(szParam1);
    if ((size_t)nParam2 < len)
        len = (size_t)nParam2;
    if (!len) {
        SET_STRING(0, "");
        RETURN_STRING("");
        return 0;
    }

    char *buffer = new char[len + 1];
    if (!buffer) {
        SET_STRING(0, "");
        RETURN_STRING("");
        return 0;
    }
    buffer[len] = 0;
    memcpy(buffer, szParam0, len);

    RETURN_STRING(buffer);
    SET_STRING(0, buffer);

    delete[] buffer;
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strpbrk CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strpbrk' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strpbrk' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strpbrk' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (char *)strpbrk((char *)szParam0, (char *)szParam1);

    RETURN_STRING(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strrchr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strrchr' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char   *szParam0;
    NUMBER nParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strrchr' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_NUMBER(1, nParam1, "'strrchr' parameter 1 should be a number (STATIC NUMBER).");

    // function call
    _C_call_result = (char *)strrchr((char *)szParam0, (int)nParam1);

    RETURN_STRING(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strspn CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strspn' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    size_t _C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strspn' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strspn' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (size_t)strspn((char *)szParam0, (char *)szParam1);

    RETURN_NUMBER(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strstr CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strstr' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strstr' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strstr' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (char *)strstr((char *)szParam0, (char *)szParam1);

    RETURN_STRING(_C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strtok CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "'strtok' takes 2 parameters. See help for details.");
    LOCAL_INIT;

    char *_C_call_result;
    // Specific variables
    char *szParam0;
    char *szParam1;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strtok' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strtok' parameter 1 should be a string (STATIC STRING).");

    // function call
    _C_call_result = (char *)strtok((char *)szParam0, (char *)szParam1);

    RETURN_STRING(_C_call_result);
    SET_STRING(0, szParam0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__strxfrm CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "'strxfrm' takes 3 parameters. See help for details.");
    LOCAL_INIT;

    size_t _C_call_result = 0;
    // Specific variables
    char   *szParam0;
    char   *szParam1;
    NUMBER nParam2;

    // Variable type check
    // Parameter 1
    GET_CHECK_STRING(0, szParam0, "'strxfrm' parameter 0 should be a string (STATIC STRING).");
    // Parameter 2
    GET_CHECK_STRING(1, szParam1, "'strxfrm' parameter 1 should be a string (STATIC STRING).");
    // Parameter 3
    GET_CHECK_NUMBER(2, nParam2, "'strxfrm' parameter 2 should be a number (STATIC NUMBER).");

    char *buffer = 0;
    if ((int)nParam2) {
        buffer = new char[(int)nParam2 + 1];
        buffer[(int)nParam2] = 0;

        // function call
        _C_call_result = (size_t)strxfrm((char *)buffer, (char *)szParam1, (size_t)nParam2);
        SET_STRING(0, buffer);
        delete[] buffer;
        RETURN_NUMBER(_C_call_result);
    } else {
        RETURN_NUMBER(0);
        SET_STRING(0, "");
    }
    return 0;
}
//-----------------------------------------------------------------------------------
