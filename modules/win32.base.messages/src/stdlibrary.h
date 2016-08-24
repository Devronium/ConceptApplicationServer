#ifndef __STDLIBRARY_H
#define __STDLIBRARY_H

#ifdef _WIN32
 #include <windows.h>
#else
#endif

#include "AnsiTypes.h"

#define INVOKE_SET_VARIABLE                   0x01
#define INVOKE_GET_VARIABLE                   0x02
#define INVOKE_SET_CLASS_MEMBER               0x03
#define INVOKE_GET_CLASS_MEMBER               0x04
#define INVOKE_FREE_VARIABLE                  0x05
#define INVOKE_CREATE_ARRAY                   0x06
#define INVOKE_GET_ARRAY_ELEMENT              0x07
#define INVOKE_SET_ARRAY_ELEMENT              0x08
#define INVOKE_GET_ARRAY_COUNT                0x09
#define INVOKE_GET_ARRAY_ELEMENT_BY_KEY       0x0A
#define INVOKE_SET_ARRAY_ELEMENT_BY_KEY       0x0B
#define INVOKE_CALL_DELEGATE                  0x0C
#define INVOKE_COUNT_DELEGATE_PARAMS          0x0D
#define INVOKE_LOCK_VARIABLE                  0x0E
#define INVOKE_GET_ARRAY_KEY                  0x0F
#define INVOKE_GET_ARRAY_INDEX                0x10
#define INVOKE_CREATE_VARIABLE                0x11
#define INVOKE_DEFINE_CONSTANT                0x12
#define INVOKE_ARRAY_VARIABLE                 0x13
#define INVOKE_ARRAY_VARIABLE_BY_KEY          0x14
#define INVOKE_GET_CLASS_VARIABLE             0x15
#define INVOKE_DEFINE_CLASS                   0x16
#define INVOKE_CREATE_OBJECT                  0x17
#define INVOKE_GET_APPLICATION_INFO           0x18
#define INVOKE_GET_SERIAL_CLASS               0x19
#define INVOKE_GET_SERIAL_CLASS_NO_DEFAULTS   0x1A
#define INVOKE_SET_SERIAL_CLASS               0x20
#define INVOKE_GET_MEMBER_FROM_ID             0x21
#define INVOKE_DYNAMIC_LOCK                   0x22
#define INVOKE_HAS_MEMBER                     0x23
#define INVOKE_OBJECT_LINKS                   0x24
#define INVOKE_VAR_LINKS                      0x25
#define INVOKE_CLI_ARGUMENTS                  0x26
#define INVOKE_SORT_ARRAY_BY_KEYS             0x27
#define INVOKE_CHECK_POINT                    0x28
#define INVOKE_ARRAY_ELEMENT_IS_SET           0x29
#define INVOKE_GET_DELEGATE_NAMES             0x2A
#define INVOKE_GET_USERDATA                   0x2B
#define INVOKE_GET_THREAD_DATA                0x2C
#define INVOKE_SET_THREAD_DATA                0x2D
#define INVOKE_HIBERNATE                      0x2E
#define INVOKE_NEW_BUFFER                     0x2F
#define INVOKE_DELETE_BUFFER                  0x30
#define INVOKE_PRINT                          0x31
#define INVOKE_CALL_DELEGATE_THREAD           0x32
#define INVOKE_CREATE_DELEGATE                0x33
#define INVOKE_FREE_VARIABLE_REFERENCE        0x34
#define INVOKE_CALL_DELEGATE_THREAD_SAFE      0x35
#define INVOKE_THREAD_LOCK                    0x36
#define INVOKE_EXTERNAL_THREADING             0x37
#define INVOKE_GET_PROTO_SOCKET               0x38
#define INVOKE_DELEGATE_BYTECODE              0x39
#define INVOKE_GET_RELOC_MEMBER_FROM_ID       0x3A
#define INVOKE_CREATE_OBJECT_NOCONSTRUCTOR    0x3B
#define INVOKE_DYNAMIC_UNLOCK                 0x3C
#define INVOKE_CALL_DELEGATE_THREAD_SPINLOCK  0x3D
#define INVOKE_GET_DIRECT_PIPE                0x3E
#define INVOKE_BYTECODE_FREE                  0x40
#define INVOKE_PROFILER                       0x41
#define INVOKE_MULTITHREADED                  0x42
#define INVOKE_GREENTHREAD                    0x43
#define INVOKE_FREEGREENTHREAD                0x44
#define INVOKE_GREENLOOP                      0x45
#define INVOKE_GREENLINK                      0x46
#define INVOKE_CURRENTGREENTHREAD             0x47
#define INVOKE_GREENINSERT                    0x48
#define INVOKE_SETSENDMESSAGEFUNCTION         0x49
#define INVOKE_CREATE_VARIABLE_2              0x4A
#define INVOKE_DYNAMIC_INCLUDE                0x4B
#define INVOKE_DYNAMIC_REMOVE_CLASS           0x4C
#define INVOKE_CREATE_OBJECT_3                0x4D
#define INVOKE_CHECK_REACHABILITY             0x4E
#define INVOKE_GET_CALLSTACK                  0x4F
#define INVOKE_GETGLOBALS                     0x50
#define INVOKE_GETID                          0x51
#define INVOKE_GETPROTODATA                   0x52
#define INVOKE_SETPROTODATA                   0x53
#define INVOKE_EXECUTECODE                    0x54
#define INVOKE_MEMBER_NAME                    0x55
#define INVOKE_CLASS_NAME                     0x56
#define INVOKE_COUNT_DELEGATE_PARAMS2         0x57
#define INVOKE_MEMBER_INFO                    0x58
#define INVOKE_GET_KEYS                       0x59
#define INVOKE_PROFILE_MEMORY                 0x60
#define INVOKE_CREATE_WORKER                  0x61
#define INVOKE_FREE_WORKER                    0x62
#define INVOKE_CREATE_OBJECT_2                0x63
#define INVOKE_ARRAY_KEYS                     0x64

#define     VARIABLE_CONSTANT                 0x00
#define     VARIABLE_UNDEFINED                0x01
#define     VARIABLE_NUMBER                   0x02
#define     VARIABLE_STRING                   0x03
#define     VARIABLE_CLASS                    0x04
#define     VARIABLE_ARRAY                    0x05
#define     VARIABLE_DELEGATE                 0x06

#define     GET_LOCAL_CERTIFICATE             -1
#define     GET_LOCAL_KEY                     -2
#define     GET_REMOTE_CERTIFICATE            -3


#ifdef _WIN32
 #define CONCEPT_DLL_API      __declspec(dllexport) void *_cdecl
#else
 #define CONCEPT_DLL_API      void *
#endif

#define ON_CREATE_CONTEXT     __CONCEPT_MEMORY_MANAGEMENT_CreateContext
#define ON_DESTROY_CONTEXT    __CONCEPT_MEMORY_MANAGEMENT_DestroyContext

typedef INTEGER (*INVOKE_CALL)(INTEGER INVOKE_TYPE, ...);

#define MANAGEMENT_PARAMETERS    (SYS_INT _MANAGEMENT_UNIQUE_ID, void *HANDLER, INVOKE_CALL Invoke)

typedef POINTER (*CONCEPT_MANAGEMENT_CALL) MANAGEMENT_PARAMETERS;

#define CONCEPT_API_PARAMETERS            (ParamList * PARAMETERS, VariableDATA * *LOCAL_CONTEXT, VariableDATA * RESULT, CALL_BACK_VARIABLE_SET SetVariable, CALL_BACK_VARIABLE_GET GetVariable, SYSTEM_SOCKET CLIENT_SOCKET, char *LOCAL_PUBLIC_KEY, char *LOCAL_PRIVATE_KEY, char *REMOTE_PUBLIC_KEY, CALL_BACK_CLASS_MEMBER_SET CallBACKClassSet, CALL_BACK_CLASS_MEMBER_GET CallBACKClassGet, INVOKE_CALL Invoke)
#define FORWARD_CONCEPT_API_PARAMETERS    (PARAMETERS, LOCAL_CONTEXT, RESULT, SetVariable, GetVariable, CLIENT_SOCKET, LOCAL_PUBLIC_KEY, LOCAL_PRIVATE_KEY, REMOTE_PUBLIC_KEY, CallBACKClassSet, CallBACKClassGet, Invoke)

#define ARGUMENT_LIST                     ParamList * PARAMETERS, VariableDATA * *LOCAL_CONTEXT, VariableDATA * RESULT, CALL_BACK_VARIABLE_SET SetVariable, CALL_BACK_VARIABLE_GET GetVariable, SYSTEM_SOCKET CLIENT_SOCKET, char *LOCAL_PUBLIC_KEY, char *LOCAL_PRIVATE_KEY, char *REMOTE_PUBLIC_KEY, CALL_BACK_CLASS_MEMBER_SET CallBACKClassSet, CALL_BACK_CLASS_MEMBER_GET CallBACKClassGet, INVOKE_CALL Invoke
#define FORWARD_ARGUMENT_LIST             PARAMETERS, LOCAL_CONTEXT, RESULT, SetVariable, GetVariable, CLIENT_SOCKET, LOCAL_PUBLIC_KEY, LOCAL_PRIVATE_KEY, REMOTE_PUBLIC_KEY, CallBACKClassSet, CallBACKClassGet, Invoke

#define CONCEPT_VA_LIST(LIST_NAME) \
    int LIST_NAME_position = 0;    \
    double LIST_NAME_nValue;       \
    char   *LIST_NAME_szValue;     \
    int    LIST_NAME_TYPE;

#define CONCEPT_VA_COPY(LIST_NAME1, LIST_NAME2)    LIST_NAME1_position = LIST_NAME2_position;
#define CONCEPT_VA_START(LIST_NAME, index)         LIST_NAME_position  = index
#define CONCEPT_VA_END

#define CONCEPT_VA_ARG(LIST_NAME, type)            (LIST_NAME_position < PARAMETERS->COUNT) ? ((GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[LIST_NAME_position++] - 1], &LIST_NAME_TYPE, &LIST_NAME_szValue, &LIST_NAME_nValue) | 1) ? ((LIST_NAME_TYPE == VARIABLE_STRING) ? *(type *)LIST_NAME_szValue : ((LIST_NAME_TYPE == VARIABLE_NUMBER) ? (type)(LIST_NAME_nValue) : (type)0)) : (type)0) : (type)0
#define CONCEPT_VA_ARG_PTR(LIST_NAME, type)        (LIST_NAME_position < PARAMETERS->COUNT) ? ((GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[LIST_NAME_position++] - 1], &LIST_NAME_TYPE, &LIST_NAME_szValue, &LIST_NAME_nValue) | 1) ? ((LIST_NAME_TYPE == VARIABLE_STRING) ? (type)LIST_NAME_szValue : ((LIST_NAME_TYPE == VARIABLE_NUMBER) ? *(type *)&LIST_NAME_nValue : (type)0)) : (type)0) : (type)0


#define ALIAS_FOR(FUNCTION)                        return FUNCTION FORWARD_CONCEPT_API_PARAMETERS;

#define TO_STRING(STRING_DATA)                     #STRING_DATA


typedef INTEGER (*CALL_BACK_VARIABLE_SET)(VariableDATA *VD, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);
typedef INTEGER (*CALL_BACK_VARIABLE_GET)(VariableDATA *VD, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);
typedef INTEGER (*CALL_BACK_CLASS_MEMBER_GET)(void *CLASS_PTR, char *class_member_name, INTEGER *TYPE, char **STRING_VALUE, NUMBER *NUMBER_VALUE);
typedef INTEGER (*CALL_BACK_CLASS_MEMBER_SET)(void *CLASS_PTR, char *class_member_name, INTEGER TYPE, char *STRING_VALUE, NUMBER NUMBER_VALUE);

//-------------------------------------------------------------------------------------------------------------
#define PARAMETERS_CHECK(parameter_count, error_return)       if (PARAMETERS->COUNT != parameter_count) return (void *)error_return;
#define PARAMETERS_CHECK_MIN_MAX(pmin, pmax, error_return)    if ((PARAMETERS->COUNT < pmin) || (PARAMETERS->COUNT > pmax)) return (void *)error_return;
#define PARAMETERS_CHECK_MIN(pmin, error_return)              if (PARAMETERS->COUNT < pmin) return (void *)error_return;
#define LOCAL_INIT          NUMBER nDUMMY_FILL = 0; char *szDUMMY_FILL = 0; INTEGER TYPE = 0
#define PARAMETERS_COUNT    PARAMETERS->COUNT

#define IS_OK(err)                                                                            ((err < 0) ? 0 : 1)

#define GET_STRING(param_index, szvariable)                                                   GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & szvariable, & nDUMMY_FILL);
#define GET_NUMBER(param_index, nvariable)                                                    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & szDUMMY_FILL, & nvariable)
#define GET_BUFFER(param_index, szvariable, nlength)                                          GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & szvariable, & nlength);
#define GET_ARRAY(param_index, dclass)                                                        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & dclass, & nDUMMY_FILL);
#define GET_CLASS(param_index, dclass)                                                        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & dclass, & nDUMMY_FILL);
#define GET_DELEGATE(param_index, dclass, dmember)                                            GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & dclass, & dmember);

#define GET_ARRAY_FROM_VARIABLE(__variable, __array)                                          GetVariable(__variable, & TYPE, (char **)& __array, & nDUMMY_FILL);

#define PARAMETER(param_index)                                                                LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1]

#define RETURN_STRING(szvariable)                                                             SetVariable(RESULT, VARIABLE_STRING, (char *)(szvariable ? szvariable : ""), 0);
#define RETURN_NUMBER(nvariable)                                                              SetVariable(RESULT, VARIABLE_NUMBER, "", nvariable);
#define RETURN_BUFFER(szvariable, len)                                                        SetVariable(RESULT, VARIABLE_STRING, (char *)((szvariable && len) ? szvariable : ""), len);
#define RETURN_ARRAY(ptrvariable)                                                             SetVariable(RESULT, VARIABLE_ARRAY, (char *)ptrvariable, 0);

#define SET_STRING(param_index, szvariable)                                                   SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], VARIABLE_STRING, (char *)(szvariable ? szvariable : ""), 0);
#define SET_NUMBER(param_index, nvariable)                                                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], VARIABLE_NUMBER, "", nvariable);
#define SET_BUFFER(param_index, szvariable, len)                                              SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], VARIABLE_STRING, (char *)((szvariable && len) ? szvariable : ""), len);

#define CHECK_STRING(error_return)                                                            if (TYPE != VARIABLE_STRING) return (void *)error_return;
#define CHECK_NUMBER(error_return)                                                            if (TYPE != VARIABLE_NUMBER) return (void *)error_return;

#define CHECK_CLASS(error_return)                                                             if (TYPE != VARIABLE_CLASS) return (void *)error_return;
#define CHECK_ARRAY(error_return)                                                             if (TYPE != VARIABLE_ARRAY) return (void *)error_return;
#define CHECK_DELEGATE(error_return)                                                          if (TYPE != VARIABLE_DELEGATE) return (void *)error_return;
#define CHECK_STATIC(error_return)                                                            if ((TYPE != VARIABLE_STRING) && (TYPE != VARIABLE_NUMBER)) return (void *)error_return;

#define CHECK_STATIC_PARAM(param_index, error_return)                                         GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], & TYPE, & szDUMMY_FILL, & nDUMMY_FILL); CHECK_STATIC(error_return);

#define GET_CHECK_STRING(param_index, szvariable, error_return)                               GET_STRING(param_index, szvariable); CHECK_STRING(error_return);
#define GET_CHECK_NUMBER(param_index, nvariable, error_return)                                GET_NUMBER(param_index, nvariable);  CHECK_NUMBER(error_return);
#define GET_CHECK_ARRAY(param_index, darray, error_return)                                    GET_ARRAY(param_index, darray); CHECK_ARRAY(error_return);
#define GET_CHECK_DELEGATE(param_index, dclass, dmember, error_return)                        GET_DELEGATE(param_index, dclass, dmember); CHECK_DELEGATE(error_return);
#define GET_CHECK_OBJECT(param_index, dclass, dmember, error_return)                          GET_CLASS(param_index, dclass); CHECK_CLASS(error_return);

#define GET_CHECK_BUFFER(param_index, szvariable, nlength, error_return)                      GET_BUFFER(param_index, szvariable, nlength); CHECK_STRING(error_return);

#define DEFINE_SCONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (char *)constant_value);
#define DEFINE_ICONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (const char *)#constant_value);
#define DEFINE_FCONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (const char *)#constant_value);

#define STR_HELPER(x) #x
#define NTOSTR(x) STR_HELPER(x)
#define DEFINE_ECONSTANT(constant_name)                                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (const char*)#constant_name, (const char *)NTOSTR(constant_name));
#define DEFINE_ESCONSTANT(constant_name)                                                      Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, #constant_name, constant_name);


#define CONCEPT_FUNCTION(function_name)                                                       CONCEPT_DLL_API CONCEPT_ ## function_name CONCEPT_API_PARAMETERS;
#define CONCEPT_FUNCTION_IMPL(function_name, parameters_count)                                CONCEPT_DLL_API CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if (PARAMETERS->COUNT != parameters_count) { return (void *)(#function_name " takes " #parameters_count " parameters");  } LOCAL_INIT;

#define CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(function_name, parameters_count)                CONCEPT_DLL_API CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if (PARAMETERS->COUNT < parameters_count) { return (void *)(#function_name " takes at least " #parameters_count " parameters."); } LOCAL_INIT;
#define CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(function_name, parameters_min, parameters_max)    CONCEPT_DLL_API CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if ((PARAMETERS->COUNT < parameters_min) || (PARAMETERS->COUNT > parameters_max)) { return (void *)(#function_name " takes at least " #parameters_min ", at most " #parameters_max " parameters."); } LOCAL_INIT;

#define __INTERNAL_PARAMETER_DECL(type, name, index)                                          type name ## index = 0
#define __INTERNAL_PARAMETER(name, index)                                                     name ## index

#define __PARAM_ERR_MSG(fname, param_index, type)                                             (#fname ": parameter " #param_index " should be a " type)

#define T_STRING(func_name, parameter_index)                                                                                          \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                         \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                     \
    GET_CHECK_BUFFER(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __INTERNAL_PARAMETER(bind_len, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"string"));

#define T_NUMBER(func_name, parameter_index)                                                                                          \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind, parameter_index);                                                                         \
    __INTERNAL_PARAMETER_DECL(INTEGER, bind_len, parameter_index);                                                                    \
    GET_CHECK_NUMBER(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"number"));

#define T_ARRAY(func_name, parameter_index)                                                                                           \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                         \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                     \
    GET_CHECK_ARRAY(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"string"));

#define T_DELEGATE(func_name, parameter_index)                                                                                          \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                           \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                       \
    GET_CHECK_DELEGATE(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __INTERNAL_PARAMETER(bind_len, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"string"));

#define T_OBJECT(func_name, parameter_index)                                                                                           \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                          \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                      \
    GET_CHECK_OBJECT(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __INTERNAL_PARAMETER(bind_len, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"string"));

#define PARAM(index)        __INTERNAL_PARAMETER(bind, index)

#define PARAM_INT(index)    (long)__INTERNAL_PARAMETER(bind, index)

#define PARAM_LEN(index)    (long)__INTERNAL_PARAMETER(bind_len, index)

#define FAIL_ERROR(text)    return (void *)text;

#define END_IMPL    return 0; }

#define DEFINE_CLASS(class_name, ...)                     Invoke(INVOKE_DEFINE_CLASS, HANDLER, class_name, __VA_ARGS__, 0)
#define CREATE_VARIABLE(VARIABLE)                         Invoke(INVOKE_CREATE_VARIABLE, & VARIABLE)
#define CREATE_ARRAY(VARIABLE)                            Invoke(INVOKE_CREATE_ARRAY, VARIABLE)
#define FREE_VARIABLE(VARIABLE)                           Invoke(INVOKE_FREE_VARIABLE, VARIABLE)
#define CREATE_OBJECT(VARIABLE, class_name)               IS_OK(Invoke(INVOKE_CREATE_OBJECT, HANDLER, VARIABLE, class_name))
#define GET_MEMBER_VAR(VARIABLE, member_name, MEM_PTR)    Invoke(INVOKE_GET_CLASS_VARIABLE, VARIABLE, member_name, & MEM_PTR)

#define SET_STRING_VARIABLE(VARIABLE, szvariable)         SetVariable(VARIABLE, VARIABLE_STRING, (char *)(szvariable ? szvariable : ""), 0);
#define SET_NUMBER_VARIABLE(VARIABLE, nvariable)          SetVariable(VARIABLE, VARIABLE_NUMBER, "", nvariable);
#define SET_BUFFER_VARIABLE(VARIABLE, szvariable, len)    SetVariable(VARIABLE, VARIABLE_STRING, (char *)((szvariable && len) ? szvariable : ""), len);
#define LOCK_VARIABLE(VARIABLE)                           Invoke(INVOKE_LOCK_VARIABLE, VARIABLE)

#define CALL_DELEGATE(DELEG_VAR, ...)                                             \
    {                                                                             \
        void *EXCEPTION = 0;                                                      \
        void *RES       = 0;                                                      \
        Invoke(INVOKE_CALL_DELEGATE, DELEG_VAR, & RES, & EXCEPTION, __VA_ARGS__); \
        if (RES)                                                                  \
            Invoke(INVOKE_FREE_VARIABLE, RES);                                    \
        if (EXCEPTION)                                                            \
            Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);                              \
    }

#define CONCEPT_INIT       CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS
#define CONCEPT_DESTROY    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS

#define T_STRING_NULL(func_name, parameter_index)                                                                                      \
    T_STRING(func_name,parameter_index)                                                                                                \
    if (!(PARAM_LEN(parameter_index)))                                                                                                 \
        __INTERNAL_PARAMETER(bind, parameter_index) = 0;

#define T_HANDLE(func_name, parameter_index)                                                                                                               \
    T_NUMBER(func_name, parameter_index)                                                                                                                              \
    if (!(PARAM_INT(parameter_index)))                                                                                                                     \
        return (void *)__PARAM_ERR_MSG(func_name,parameter_index,"valid handle (not null)");

#define T_HANDLE2(func_name, parameter_index, HANDLE_TYPE)                                                                                                 \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_internal, parameter_index);                                                                                     \
    __INTERNAL_PARAMETER_DECL(HANDLE_TYPE, bind, parameter_index);                                                                                         \
    GET_CHECK_NUMBER(parameter_index, __INTERNAL_PARAMETER(bind_internal, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"should be a handle"); \
    if (!(__INTERNAL_PARAMETER(bind_internal, parameter_index))) {                                                                                         \
        return (void *)__PARAM_ERR_MSG(func_name,parameter_index," should be a valid handle (not null)");                                                  \
    } else {                                                                                                                                               \
        __INTERNAL_PARAMETER(bind, parameter_index) = (HANDLE_TYPE)(long)__INTERNAL_PARAMETER(bind_internal, parameter_index);                             \
    }

float *GetFloatList(void *arr, INVOKE_CALL _Invoke);
char **GetCharList(void *arr, INVOKE_CALL _Invoke);
int *GetIntList(void *arr, INVOKE_CALL _Invoke);
bool *GetBoolList(void *arr, INVOKE_CALL _Invoke);
double *GetDoubleList(void *arr, INVOKE_CALL _Invoke);

#define CORE_NEW(SIZE, VARIABLE)    Invoke(INVOKE_NEW_BUFFER, (INTEGER)SIZE, &VARIABLE)
#define CORE_DELETE(VARIABLE)       Invoke(INVOKE_DELETE_BUFFER, VARIABLE)

//-------------------------------------------------------------------------------------------------------------
#endif //__STDLIBRARY_H

