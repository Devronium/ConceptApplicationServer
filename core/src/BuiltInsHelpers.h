#ifndef __BUILTINSHELPER_H
#define __BUILTINSHELPER_H

#define CONCEPT_API_PARAMETERS            (ParamListExtra * PARAMETERS, VariableDATA * *LOCAL_CONTEXT, VariableDATA * RESULT, CALL_BACK_VARIABLE_SET SetVariable, CALL_BACK_VARIABLE_GET GetVariable, SYSTEM_SOCKET CLIENT_SOCKET, char *LOCAL_PUBLIC_KEY, char *LOCAL_PRIVATE_KEY, char *REMOTE_PUBLIC_KEY, CALL_BACK_CLASS_MEMBER_SET CallBACKClassSet, CALL_BACK_CLASS_MEMBER_GET CallBACKClassGet, INVOKE_CALL Invoke)
#define FORWARD_CONCEPT_API_PARAMETERS    (PARAMETERS, LOCAL_CONTEXT, RESULT, SetVariable, GetVariable, CLIENT_SOCKET, LOCAL_PUBLIC_KEY, LOCAL_PRIVATE_KEY, REMOTE_PUBLIC_KEY, CallBACKClassSet, CallBACKClassGet, Invoke) 

#define PARAMETERS_CHECK(parameter_count, error_return)       if (PARAMETERS->COUNT != parameter_count) return (void *)error_return;
#define PARAMETERS_CHECK_MIN_MAX(pmin, pmax, error_return)    if ((PARAMETERS->COUNT < pmin) || (PARAMETERS->COUNT > pmax)) return (void *)error_return;
#define PARAMETERS_CHECK_MIN(pmin, error_return)              if (PARAMETERS->COUNT < pmin) return (void *)error_return;
#define LOCAL_INIT          NUMBER nDUMMY_FILL = 0; char *szDUMMY_FILL = 0; INTEGER TYPE = 0

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
#define RETURN_NUMBER(nvariable)                                                              SetVariable(RESULT, VARIABLE_NUMBER, (char *)"", nvariable);
#define RETURN_BUFFER(szvariable, len)                                                        SetVariable(RESULT, VARIABLE_STRING, (char *)((szvariable && len) ? szvariable : ""), len);
#define RETURN_ARRAY(ptrvariable)                                                             SetVariable(RESULT, VARIABLE_ARRAY, (char *)ptrvariable, 0);

#define SET_STRING(param_index, szvariable)                                                   SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], VARIABLE_STRING, (char *)(szvariable ? szvariable : ""), 0);
#define SET_NUMBER(param_index, nvariable)                                                    SetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[param_index] - 1], VARIABLE_NUMBER, (char *)"", nvariable);
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

#define STR_HELPER(x) #x
#define DEFINE_SCONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (char *)constant_value);
#define DEFINE_ICONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (const char *)TinyString((long)constant_value).c_str());
#define DEFINE_FCONSTANT(constant_name, constant_value)                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (char *)constant_name, (const char *)TinyString((NUMBER)constant_value).c_str());
#define DEFINE_ECONSTANT(constant_name)                                                       Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, (const char*)#constant_name, (const char *)TinyString((long)constant_name).c_str());
#define DEFINE_ESCONSTANT(constant_name)                                                      Invoke(INVOKE_DEFINE_CONSTANT, (void *)HANDLER, #constant_name, constant_name);

#define CONCEPT_FUNCTION(function_name)                                                       void * CONCEPT_ ## function_name CONCEPT_API_PARAMETERS;
#define CONCEPT_FUNCTION_IMPL(function_name, parameters_count)                                void * CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if (PARAMETERS->COUNT != parameters_count) { return (void *)(#function_name " takes " #parameters_count " parameters");  } LOCAL_INIT;
#define CONCEPT_FUNCTION_IMPL2(lib, function_name, parameters_count)                          void * CONCEPT_ ## lib ## _ ## function_name CONCEPT_API_PARAMETERS { if (PARAMETERS->COUNT != parameters_count) { return (void *)(#function_name " takes " #parameters_count " parameters");  } LOCAL_INIT;

#define CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(function_name, parameters_count)                void * CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if (PARAMETERS->COUNT < parameters_count) { return (void *)(#function_name " takes at least " #parameters_count " parameters."); } LOCAL_INIT;
#define CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(function_name, parameters_min, parameters_max)    void * CONCEPT_ ## function_name CONCEPT_API_PARAMETERS { if ((PARAMETERS->COUNT < parameters_min) || (PARAMETERS->COUNT > parameters_max)) { return (void *)(#function_name " takes at least " #parameters_min ", at most " #parameters_max " parameters."); } LOCAL_INIT;

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
    GET_CHECK_ARRAY(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"array"));

#define T_DELEGATE(func_name, parameter_index)                                                                                          \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                           \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                       \
    GET_CHECK_DELEGATE(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __INTERNAL_PARAMETER(bind_len, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"delegate"));

#define T_OBJECT(func_name, parameter_index)                                                                                           \
    __INTERNAL_PARAMETER_DECL(char *, bind, parameter_index);                                                                          \
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, parameter_index);                                                                      \
    GET_CHECK_OBJECT(parameter_index, __INTERNAL_PARAMETER(bind, parameter_index), __INTERNAL_PARAMETER(bind_len, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"object"));

#define PARAM(index)        __INTERNAL_PARAMETER(bind, index)

#define PARAM_INT(index)    (long)__INTERNAL_PARAMETER(bind, index)

#define PARAM_LEN(index)    (long)__INTERNAL_PARAMETER(bind_len, index)

#define FAIL_ERROR(text)    return (void *)text;

#define END_IMPL    return 0; }

#define DEFINE_CLASS(class_name, ...)                     Invoke(INVOKE_DEFINE_CLASS, HANDLER, class_name, __VA_ARGS__, 0)
#define CREATE_VARIABLE(VARIABLE)                         Invoke(INVOKE_CREATE_VARIABLE, & VARIABLE)
#define CREATE_ARRAY(VARIABLE)                            Invoke(INVOKE_CREATE_ARRAY, VARIABLE)
#define FREE_VARIABLE(VARIABLE)                           Invoke(INVOKE_FREE_VARIABLE, VARIABLE)
#define FREE_VARIABLE_REFERENCE(VARIABLE)                 Invoke(INVOKE_FREE_VARIABLE_REFERENCE, VARIABLE)
#define CREATE_OBJECT(VARIABLE, class_name)               IS_OK(Invoke(INVOKE_CREATE_OBJECT, HANDLER, VARIABLE, class_name))
#define GET_MEMBER_VAR(VARIABLE, member_name, MEM_PTR)    Invoke(INVOKE_GET_CLASS_VARIABLE, VARIABLE, member_name, & MEM_PTR)

#define SET_STRING_VARIABLE(VARIABLE, szvariable)         SetVariable(VARIABLE, VARIABLE_STRING, (char *)(szvariable ? szvariable : ""), 0);
#define SET_NUMBER_VARIABLE(VARIABLE, nvariable)          SetVariable(VARIABLE, VARIABLE_NUMBER, (char *)"", nvariable);
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
    GET_CHECK_NUMBER(parameter_index, __INTERNAL_PARAMETER(bind_internal, parameter_index), __PARAM_ERR_MSG(func_name,parameter_index,"should be a handle"));\
    if (!(__INTERNAL_PARAMETER(bind_internal, parameter_index))) {                                                                                         \
        return (void *)__PARAM_ERR_MSG(func_name,parameter_index,"valid handle (not null)");                                                               \
    } else {                                                                                                                                               \
        __INTERNAL_PARAMETER(bind, parameter_index) = (HANDLE_TYPE)(long)__INTERNAL_PARAMETER(bind_internal, parameter_index);                             \
    }

#define CORE_NEW(SIZE, VARIABLE)    Invoke(INVOKE_NEW_BUFFER, (INTEGER)SIZE, &VARIABLE)
#define CORE_DELETE(VARIABLE)       Invoke(INVOKE_DELETE_BUFFER, VARIABLE) 

#define BUILTIN(builtinfunction)                        IS_BUILTIN(name, builtinfunction)
#define IS_BUILTIN(name, builtinfunction)               IS_BUILTIN2(name, #builtinfunction, builtinfunction)
#define IS_BUILTIN2(name, funcname, builtinfunction)    if (!strcmp(name, funcname)) return (void *)&CONCEPT_ ## builtinfunction;

#define BUILTINCLASS(cls, code)                         BUILTINCLASS2(classname, PIF, cls, code)
#define BUILTINCLASS2(name, PIF, cls, code)             if (!strcmp(name, cls)) { PIF->enable_private = 1; PIF->RuntimeIncludeCode(code); PIF->enable_private = 0; return 1; }

#define WRAP_FUNCTION(lib, name)     CONCEPT_FUNCTION_IMPL(name, 1) T_NUMBER(name, 0) RETURN_NUMBER(name(PARAM(0))); END_IMPL
#define WRAP_FUNCTION2(lib, name)    CONCEPT_FUNCTION_IMPL(name, 2) T_NUMBER(name, 0) T_NUMBER(name, 1) RETURN_NUMBER(name(PARAM(0), PARAM(1))); END_IMPL
#define WRAP_FUNCTION0(lib, name)    CONCEPT_FUNCTION_IMPL(name, 0) RETURN_NUMBER(name()); END_IMPL
#define WRAP_VOID_INT_FUNCTION(lib, name)     CONCEPT_FUNCTION_IMPL(name, 1) T_NUMBER(name, 0) name(PARAM_INT(0)); RETURN_NUMBER(0); END_IMPL
#define WRAP_INT_FUNCTION(lib, name)     CONCEPT_FUNCTION_IMPL(name, 1) T_NUMBER(name, 0) RETURN_NUMBER(name(PARAM_INT(0))); END_IMPL

#define MAKE_STRING2(s)     #s
#define MAKE_STRING(s)      MAKE_STRING2(s)
#define DEFINE(a)           PIF->DefineConstant(#a, MAKE_STRING(a), 0);
#define DEFINE2(a, b)       PIF->DefineConstant(#a, b, 0);

#define DECLARE_WRAPPER(lib, a)      "static " #a "(a){return " #a "(a);}"
#define DECLARE_WRAPPER0(lib, a)     "static " #a "(){return " #a "();}"
#define DECLARE_WRAPPER2(lib, a)     "static " #a "(a,b){return " #a "(a,b);}"
#define DECLARE_WRAPPER_VOID(lib, a) "static " #a "(a){" #a "(a);}"

#endif
