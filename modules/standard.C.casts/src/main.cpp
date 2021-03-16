//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//-----------------------------------------------------------------------------------
#ifdef _WIN64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef __x86_64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef _M_IA64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef _LP64
 #define __SIZEOF_POINTER__    8
#endif

#ifdef __LP64__
 #define __SIZEOF_POINTER__    8
#endif
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__int8 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_int8' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_int8: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (char)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__int16 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_int16' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_int16: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (short int)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__int32 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_int32' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_int32: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (int)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__float CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_float' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_float: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (float)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__int64 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_int64' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_int64: parameter 1 should be of STATIC NUMBER type";

    // function call
#if __SIZEOF_POINTER__ == 8
    _C_call_result = (long)nParam0;
#else
    _C_call_result = (int64_t)nParam0;
#endif

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__uint8 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_uint8' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_uint8: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned char)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__uint16 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_uint16' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_uint16: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned short int)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__uint32 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_uint32' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_uint32: parameter 1 should be of STATIC NUMBER type";

    // function call
    _C_call_result = (unsigned int)nParam0;

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__uint64 CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)": '_uint64' parameters error. This fuction takes one parameter.";

    // General variables
    char    *STRING_DUMMY;
    INTEGER TYPE;

    // Result
    NUMBER _C_call_result;
    // Specific variables
    NUMBER nParam0;

    // Variable type check
    // Parameter 1
    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &STRING_DUMMY, &nParam0);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"_uint64: parameter 1 should be of STATIC NUMBER type";

    // function call
#if __SIZEOF_POINTER__ == 8
    _C_call_result = (unsigned long)nParam0;
#else
    _C_call_result = (uint64_t)nParam0;
#endif

    SetVariable(RESULT, VARIABLE_NUMBER, "", _C_call_result);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_tobin, 2)
    T_NUMBER(_tobin, 0)
    T_NUMBER(_tobin, 1)

    char vint8 = (char)PARAM_INT(0);
    short int vint16 = (short int )PARAM_INT(0);
    int       vint32 = (int)PARAM_INT(0);
    int64_t   vint64 = (int64_t)PARAM_INT(0);
    switch (PARAM_INT(1)) {
        case 8:
            RETURN_BUFFER((char *)&vint8, sizeof(vint8));
            break;

        case 16:
            RETURN_BUFFER((char *)&vint16, sizeof(vint16));
            break;

        case 32:
            RETURN_BUFFER((char *)&vint32, sizeof(vint32));
            break;

        case 64:
            RETURN_BUFFER((char *)&vint64, sizeof(vint64));
            break;

        default:
            RETURN_STRING("");
            return (void *)"_tobin: bits must be 8, 16, 32, 64";
            break;
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_toubin, 2)
    T_NUMBER(_toubin, 0)
    T_NUMBER(_toubin, 1)

    unsigned char vint8 = (unsigned char)PARAM_INT(0);
    unsigned short int vint16 = (unsigned short int )PARAM_INT(0);
    unsigned int       vint32 = (unsigned int)PARAM_INT(0);
    uint64_t           vint64 = (uint64_t)PARAM_INT(0);
    switch (PARAM_INT(1)) {
        case 8:
            RETURN_BUFFER((char *)&vint8, sizeof(vint8));
            break;

        case 16:
            RETURN_BUFFER((char *)&vint16, sizeof(vint16));
            break;

        case 32:
            RETURN_BUFFER((char *)&vint32, sizeof(vint32));
            break;

        case 64:
            RETURN_BUFFER((char *)&vint64, sizeof(vint64));
            break;

        default:
            RETURN_STRING("");
            return (void *)"_toubin: bits must be 8, 16, 32, 64";
            break;
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_tounumber, 2)
    T_STRING(_tounumber, 0)
    T_NUMBER(_tounumber, 1)
    switch (PARAM_INT(1)) {
        case 8:
            {
                if (PARAM_LEN(0) < sizeof(char))
                    return (void *)"_tounumber: invalid buffer size (must be at least 1)";

                RETURN_NUMBER(*(unsigned char *)PARAM(0))
            }
            break;

        case 16:
            {
                if (PARAM_LEN(0) < sizeof(short int))
                    return (void *)"_tounumber: invalid buffer size (must be at least 2)";

                RETURN_NUMBER(*(unsigned short int *)PARAM(0))
            }
            break;

        case 32:
            {
                if (PARAM_LEN(0) < sizeof(int))
                    return (void *)"_tounumber: invalid buffer size (must be at least 4)";

                RETURN_NUMBER(*(unsigned int *)PARAM(0))
            }
            break;

        case 64:
            {
                if (PARAM_LEN(0) < sizeof(int64_t))
                    return (void *)"_tounumber: invalid buffer size (must be at least 8)";

                RETURN_NUMBER(*(uint64_t *)PARAM(0))
            }
            break;

        default:
            RETURN_NUMBER(0);
            return (void *)"_tounumber: bits must be 8, 16, 32, 64";
            break;
    }

END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_tonumber, 2)
    T_STRING(_tonumber, 0)
    T_NUMBER(_tonumber, 1)
    switch (PARAM_INT(1)) {
        case 8:
            {
                if (PARAM_LEN(0) < sizeof(char))
                    return (void *)"_tonumber: invalid buffer size (must be at least 1)";

                RETURN_NUMBER(*(char *)PARAM(0))
            }
            break;

        case 16:
            {
                if (PARAM_LEN(0) < sizeof(short int))
                    return (void *)"_tonumber: invalid buffer size (must be at least 2)";

                RETURN_NUMBER(*(short int *)PARAM(0))
            }
            break;

        case 32:
            {
                if (PARAM_LEN(0) < sizeof(int))
                    return (void *)"_tonumber: invalid buffer size (must be at least 4)";

                RETURN_NUMBER(*(int *)PARAM(0))
            }
            break;

        case 64:
            {
                if (PARAM_LEN(0) < sizeof(int64_t))
                    return (void *)"_tonumber: invalid buffer size (must be at least 8)";

                RETURN_NUMBER(*(int64_t *)PARAM(0))
            }
            break;

        default:
            RETURN_NUMBER(0);
            return (void *)"_tonumber: bits must be 8, 16, 32, 64";
            break;
    }

END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CreateStaticArray, 2)
    T_NUMBER(CreateStaticArray, 0)
    T_NUMBER(CreateStaticArray, 1)

    void *arr = 0;
    int e_size = PARAM_INT(0);
    int len    = PARAM_INT(1);
    if (len < 0)
        len *= -1;
    if ((len) && (e_size > 0)) {
        arr = malloc(len * e_size);
        if (arr)
            memset(arr, 0, len * e_size);
    }

    RETURN_NUMBER((SYS_INT)arr)
END_IMPL
//---------------------------------
CONCEPT_FUNCTION_IMPL(FreeStaticArray, 1)
    T_NUMBER(FreeStaticArray, 0)
    void *arr = (void *)(SYS_INT)PARAM(0);
    if (arr) {
        free(arr);
        SET_NUMBER(0, 0);
    }

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------
CONCEPT_FUNCTION_IMPL(GetStaticArray, 3)
    T_NUMBER(GetStaticArray, 0)
    T_NUMBER(GetStaticArray, 1)
    T_NUMBER(GetStaticArray, 2)

    NUMBER res = 0;
    void *arr    = (void *)(SYS_INT)PARAM(0);
    int  size    = PARAM_INT(1);
    int  element = PARAM_INT(2);
    if ((arr) && (size > 0) && (element >= 0)) {
        switch (size) {
            case -1:
                res = ((char *)arr)[element];
                break;

            case -2:
                res = ((short *)arr)[element];
                break;

            case -4:
                res = ((int *)arr)[element];
                break;

            case -8:
                res = ((int64_t *)arr)[element];
                break;

            case 1:
                res = ((unsigned char *)arr)[element];
                break;

            case 2:
                res = ((unsigned short *)arr)[element];
                break;

            case 4:
                res = ((unsigned int *)arr)[element];
                break;

            case 8:
                res = ((uint64_t *)arr)[element];
                break;
        }
    }

    RETURN_NUMBER(res)
END_IMPL
//---------------------------------
CONCEPT_FUNCTION_IMPL(SetStaticArray, 4)
    T_NUMBER(SetStaticArray, 0)
    T_NUMBER(SetStaticArray, 1)
    T_NUMBER(SetStaticArray, 2)
    T_NUMBER(SetStaticArray, 3)

    void *arr = (void *)(SYS_INT)PARAM(0);
    int size    = PARAM_INT(1);
    int element = PARAM_INT(2);
    if ((arr) && (size > 0) && (element >= 0)) {
        switch (size) {
            case -1:
                ((char *)arr)[element] = (char)PARAM_INT(3);
                break;

            case -2:
                ((short *)arr)[element] = (short)PARAM_INT(3);
                break;

            case -4:
                ((int *)arr)[element] = (int)PARAM_INT(3);
                break;

            case -8:
                ((int64_t *)arr)[element] = (int64_t)PARAM(3);
                break;

            case 1:
                ((unsigned char *)arr)[element] = (unsigned char)PARAM_INT(3);
                break;

            case 2:
                ((unsigned short *)arr)[element] = (unsigned short)PARAM_INT(3);
                break;

            case 4:
                ((unsigned int *)arr)[element] = (unsigned int)PARAM_INT(3);
                break;

            case 8:
                ((uint64_t *)arr)[element] = (uint64_t)PARAM(3);
                break;

            default:
                RETURN_NUMBER(-1);
                return 0;
        }
    }

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------
CONCEPT_FUNCTION_IMPL(FillStaticArray, 4)
    T_NUMBER(FillStaticArray, 0)
    T_NUMBER(FillStaticArray, 1)
    T_NUMBER(FillStaticArray, 2)
    T_NUMBER(FillStaticArray, 3)

    void *arr = (void *)(SYS_INT)PARAM(0);
    int size        = PARAM_INT(1);
    int vector_size = PARAM_INT(2);
    if ((arr) && (size > 0)) {
        for (int element = 0; element < vector_size; element++) {
            switch (size) {
                case -1:
                    ((char *)arr)[element] = (char)PARAM_INT(3);
                    break;

                case -2:
                    ((short *)arr)[element] = (short)PARAM_INT(3);
                    break;

                case -4:
                    ((int *)arr)[element] = (int)PARAM_INT(3);
                    break;

                case -8:
                    ((int64_t *)arr)[element] = (int64_t)PARAM(3);
                    break;

                case 1:
                    ((unsigned char *)arr)[element] = (unsigned char)PARAM_INT(3);
                    break;

                case 2:
                    ((unsigned short *)arr)[element] = (unsigned short)PARAM_INT(3);
                    break;

                case 4:
                    ((unsigned int *)arr)[element] = (unsigned int)PARAM_INT(3);
                    break;

                case 8:
                    ((uint64_t *)arr)[element] = (uint64_t)PARAM(3);
                    break;

                default:
                    RETURN_NUMBER(-1);
                    return 0;
            }
        }
    }

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------
