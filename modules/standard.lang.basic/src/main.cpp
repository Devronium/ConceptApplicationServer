//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "AnsiString.h"
extern "C" {
    #include "my_basic.h"
}
#include "library.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <algorithm>

typedef std::map<std::string, void *> StringMap;
//---------------------------------------------------------------------------
static INVOKE_CALL  InvokePtr        = 0;

#define BASIC_CTX(container)    ((container)->ctx)

class basic_wrapper_container {
public:
    struct mb_interpreter_t *ctx;
    StringMap HANDLERS;
    void *HANDLER;
};

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr       = Invoke;
    DEFINE_ECONSTANT(SE_NO_ERR)
    DEFINE_ECONSTANT(SE_CM_FUNC_EXISTS)
    DEFINE_ECONSTANT(SE_CM_FUNC_NOT_EXISTS)
    DEFINE_ECONSTANT(SE_CM_NOT_SUPPORTED)
    DEFINE_ECONSTANT(SE_PS_OPEN_FILE_FAILED)
    DEFINE_ECONSTANT(SE_PS_SYMBOL_TOO_LONG)
    DEFINE_ECONSTANT(SE_PS_INVALID_CHAR)
    DEFINE_ECONSTANT(SE_PS_INVALID_MODULE)
    DEFINE_ECONSTANT(SE_RN_EMPTY_PROGRAM)
    DEFINE_ECONSTANT(SE_RN_PROGRAM_TOO_LONG)
    DEFINE_ECONSTANT(SE_RN_SYNTAX_ERROR)
    DEFINE_ECONSTANT(SE_RN_OUT_OF_MEMORY)
    DEFINE_ECONSTANT(SE_RN_OVERFLOW)
    DEFINE_ECONSTANT(SE_RN_UNEXPECTED_TYPE)
    DEFINE_ECONSTANT(SE_RN_INVALID_STRING)
    DEFINE_ECONSTANT(SE_RN_INTEGER_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_NUMBER_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_STRING_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_VAR_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_INDEX_OUT_OF_BOUND)
    DEFINE_ECONSTANT(SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX)
    DEFINE_ECONSTANT(SE_RN_TOO_MANY_DIMENSIONS)
    DEFINE_ECONSTANT(SE_RN_RANK_OUT_OF_BOUND)
    DEFINE_ECONSTANT(SE_RN_INVALID_ID_USAGE)
    DEFINE_ECONSTANT(SE_RN_DUPLICATE_ID)
    DEFINE_ECONSTANT(SE_RN_INCOMPLETE_STRUCTURE)
    DEFINE_ECONSTANT(SE_RN_LABEL_NOT_EXISTS)
    DEFINE_ECONSTANT(SE_RN_NO_RETURN_POINT)
    DEFINE_ECONSTANT(SE_RN_COLON_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_COMMA_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_COMMA_OR_SEMICOLON_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_OPEN_BRACKET_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_CLOSE_BRACKET_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_NESTED_TOO_MUCH)
    DEFINE_ECONSTANT(SE_RN_OPERATION_FAILED)
    DEFINE_ECONSTANT(SE_RN_OPERATOR_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_ASSIGN_OPERATOR_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_ELSE_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_ENDIF_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_TO_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_NEXT_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_UNTIL_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_LOOP_VAR_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_JUMP_LABEL_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_CALCULATION_ERROR)
    DEFINE_ECONSTANT(SE_RN_INVALID_EXPRESSION)
    DEFINE_ECONSTANT(SE_RN_DIVIDE_BY_ZERO)
    DEFINE_ECONSTANT(SE_RN_WRONG_FUNCTION_REACHED)
    DEFINE_ECONSTANT(SE_RN_CANNOT_SUSPEND_HERE)
    DEFINE_ECONSTANT(SE_RN_CANNOT_MIX_INSTRUCTIONAL_AND_STRUCTURED)
    DEFINE_ECONSTANT(SE_RN_INVALID_ROUTINE)
    DEFINE_ECONSTANT(SE_RN_ROUTINE_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_DUPLICATE_ROUTINE)
    DEFINE_ECONSTANT(SE_RN_INVALID_CLASS)
    DEFINE_ECONSTANT(SE_RN_CLASS_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_DUPLICATE_CLASS)
    DEFINE_ECONSTANT(SE_RN_HASH_AND_COMPARE_MUST_BE_PROVIDED_TOGETHER)
    DEFINE_ECONSTANT(SE_RN_INVALID_LAMBDA)
    DEFINE_ECONSTANT(SE_RN_EMPTY_COLLECTION)
    DEFINE_ECONSTANT(SE_RN_LIST_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_INVALID_ITERATOR)
    DEFINE_ECONSTANT(SE_RN_ITERABLE_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_COLLECTION_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_COLLECTION_OR_ITERATOR_EXPECTED)
    DEFINE_ECONSTANT(SE_RN_REFERENCED_TYPE_EXPECTED)
    DEFINE_ECONSTANT(SE_EA_EXTENDED_ABORT)
    DEFINE_ECONSTANT(SE_COUNT)

    mb_init();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        mb_dispose();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(BasicNewRuntime, 0)
    basic_wrapper_container *ref = new basic_wrapper_container();
    if (ref) {
        ref->HANDLER = PARAMETERS->HANDLER;
        mb_open(&ref->ctx);
        mb_set_userdata(ref->ctx, ref);
        mb_set_gc_enabled(ref->ctx, true);
        if (ref->ctx) {
            RETURN_NUMBER((SYS_INT)ref);
        } else {
            delete ref;
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(BasicDestroyRuntime, 1)
    T_NUMBER(BasicDestroyRuntime, 0)

    basic_wrapper_container *ctx = (basic_wrapper_container *)(intptr_t)PARAM(0);
    if (ctx) {
        SET_NUMBER(0, 0);
        for (StringMap::iterator it = ctx->HANDLERS.begin(); it != ctx->HANDLERS.end();++it) {
            void *deleg = it->second;
            if (deleg) {
                FREE_VARIABLE(deleg);
            }
        }
        ctx->HANDLERS.clear();
        mb_close(&ctx->ctx);
        delete ctx;
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(BasicEvaluateScript, 2)
    T_HANDLE(BasicEvaluateScript, 0)
    T_STRING(BasicEvaluateScript, 1)

    struct mb_interpreter_t *ctx = BASIC_CTX((basic_wrapper_container *)(intptr_t)PARAM(0));

    int err = mb_load_string(ctx, PARAM(1), false);
    if (err) {
        RETURN_NUMBER(err);
    } else {
        err = mb_run(ctx, true);
        RETURN_NUMBER(err);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(BasicEvaluateFile, 2)
    T_HANDLE(BasicEvaluateFile, 0)
    T_STRING(BasicEvaluateFile, 1)

    struct mb_interpreter_t *ctx = BASIC_CTX((basic_wrapper_container *)(intptr_t)PARAM(0));

    int err = mb_load_file(ctx, PARAM(1));
    if (err) {
        RETURN_NUMBER(err);
    } else {
        err = mb_run(ctx, false);
        RETURN_NUMBER(err);
    }
END_IMPL
//------------------------------------------------------------------------
void RecursiveValue(struct mb_interpreter_t *ctx, void **l, mb_value_t *val, void *RESULT, INVOKE_CALL Invoke) {
    switch (val->type) {
        case MB_DT_NIL:
        case MB_DT_UNKNOWN:
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)0);
            break;
        case MB_DT_INT:
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)val->value.integer);
            break;
        case MB_DT_REAL:
        case MB_DT_NUM:
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)val->value.float_point);
            break;
        case MB_DT_STRING:
            {
                if (val->value.string) {
                    Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, val->value.string, (NUMBER)0);
                } else {
                    Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, "", (NUMBER)0);
                }
            }
            break;
        case MB_DT_ARRAY:
            {
                CREATE_ARRAY(RESULT);
                int arr_len = 0;
                mb_get_array_len(ctx, l, val->value.array, 0, &arr_len);
                void *elem_data = NULL;
                for (int i = 0; i < arr_len; i++) {
                    mb_value_t val2;
                    mb_get_array_elem(ctx, l, val->value.array, &i, 1, &val2);
                    Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &elem_data);
                    RecursiveValue(ctx, l, &val2, elem_data, Invoke);
                }
            }
            break;
        default:
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (const char *)"", (NUMBER)0);
    }
}

mb_value_t RecursivePush(struct mb_interpreter_t *ctx, void *var, void **l, INVOKE_CALL Invoke) {
    INTEGER    type     = 0;
    char       *szValue = 0;
    NUMBER     nValue   = 0;

	mb_value_t arg;
	mb_make_nil(arg);
    Invoke(INVOKE_GET_VARIABLE, var, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_STRING:
            mb_make_string(arg, (char *)(szValue ? strdup(szValue) : ""));
            // mb_push_value(ctx, l, arg);
            break;
        case VARIABLE_NUMBER:
            if (ceilf(nValue) == floorf(nValue))
                mb_make_int(arg, (int)nValue);
            else
                mb_make_real(arg, nValue);
            break;
        case VARIABLE_ARRAY:
            {
                INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, var);
                if (count) {
                    void *arr = NULL;
                    int d[1];
                    d[0] = count;
                    if (count) {
                        mb_value_t arg2;
                        Invoke(INVOKE_GET_ARRAY_ELEMENT, var, (INTEGER)0, &type, &szValue, &nValue);
                        if (type == VARIABLE_STRING) {
                            mb_init_array(ctx, l, MB_DT_STRING, d, 1, &arr);

                            int index = 0;
                            mb_make_string(arg2, (char *)(szValue ? strdup(szValue) : ""));
                            mb_set_array_elem(ctx, l, arr, &index, 1, arg2);

                            for (INTEGER i = 1; i < count; i++) {
                                Invoke(INVOKE_GET_ARRAY_ELEMENT, var, (INTEGER)i, &type, &szValue, &nValue);
                                if (type == VARIABLE_STRING) {
                                    mb_make_string(arg2, (char *)(szValue ? strdup(szValue) : ""));
                                } else {
	                                mb_make_nil(arg2);
                                }
                                mb_set_array_elem(ctx, l, arr, &i, 1, arg2);
                            }
                        } else {
                            mb_init_array(ctx, l, MB_DT_REAL, d, 1, &arr);
                            int index = 0;
                            mb_make_real(arg2, nValue);
                            mb_set_array_elem(ctx, l, arr, &index, 1, arg2);
                            for (int i = 1; i < count; i++) {
                                Invoke(INVOKE_GET_ARRAY_ELEMENT, var, (INTEGER)i, &type, &szValue, &nValue);
                                if (type == VARIABLE_NUMBER) {
                                    mb_make_real(arg2, nValue);
                                } else {
	                                mb_make_nil(arg2);
                                }
                                mb_set_array_elem(ctx, l, arr, &i, 1, arg2);
                            }
                        }
                    } else
                        mb_init_array(ctx, l, MB_DT_REAL, d, 1, &arr);

	                mb_make_nil(arg);
                    if (arr) {
                        arg.type = MB_DT_ARRAY;
                        arg.value.array = arr;
                    }
                    // mb_push_value(ctx, l, arg);
                } else {
	                mb_make_nil(arg);
	                // mb_push_value(ctx, l, arg);
                }
            }
            break;
        case VARIABLE_CLASS:
            {
                char *class_name = NULL;
                int members_count = Invoke(INVOKE_GET_SERIAL_CLASS, szValue, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                char   **members       = new char * [members_count];
                char   *flags          = new char[members_count];
                char   *access         = new char[members_count];
                char   *types          = new char[members_count];
                char   **szValues      = new char * [members_count];
                NUMBER *nValues        = new NUMBER[members_count];
                void   **class_data    = new void * [members_count];
                void   **variable_data = new void * [members_count];
                int result = Invoke(INVOKE_GET_SERIAL_CLASS, szValue, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);
                if (IS_OK(result)) {
                    if (!mb_begin_class(ctx, l, class_name, NULL, 0, &arg)) {
                        for (int i = 0; i < members_count; i++) {
                                char *key = members[i];
                                void *elem_data =  variable_data[i];
                                if ((key) && (elem_data))
                                    mb_add_var(ctx, l, key, RecursivePush(ctx, elem_data, l, Invoke), true);
                        }
                        mb_end_class(ctx, l);
                    }
                }

                delete[] members;
                delete[] flags;
                delete[] access;
                delete[] types;
                delete[] szValues;
                delete[] nValues;
                delete[] class_data;
                delete[] variable_data;
            }
            break;
    }
    return arg;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BasicErrorCode, 1, 2)
    T_HANDLE(BasicErrorCode, 0)

    struct mb_interpreter_t *ctx = BASIC_CTX((basic_wrapper_container *)(intptr_t)PARAM(0));

    const char *file = NULL;
    int pos = 0;
    unsigned short row = 0;
    unsigned short col = 0;
    mb_error_e err_code = mb_get_last_error(ctx, &file, &pos, &row, &col);

    if (PARAMETERS_COUNT > 1) {
        if (err_code) {
            AnsiString err((char *)mb_get_error_desc(err_code));
            if (file) {
                err += (char *)" in ";
                err += (char *)file;
                err += (char *)":";
            } else {
                err += (char *)" on line ";
            }
            err += AnsiString((long)row);
            err += (char *)":";
            err += AnsiString((long)col);
            SET_STRING(1, err.c_str());
        } else {
            SET_STRING(1, "");
        }
    }
    RETURN_NUMBER(err_code);
END_IMPL
//------------------------------------------------------------------------
static int concept_handler_func(struct mb_interpreter_t *ctx, void **arg) {
    void *ref = NULL;
    mb_value_t var;
    int magic = 0;

    mb_get_userdata(ctx, &ref);
    INVOKE_CALL Invoke = InvokePtr;
    if ((!Invoke) || (!ref))
        return MB_FUNC_ERR;
    basic_wrapper_container *container = (basic_wrapper_container *)ref;
    if ((!container) || (container->ctx != ctx))
        return MB_FUNC_ERR;

    void *deleg = container->HANDLERS[std::string((const char *)mb_get_calldata(ctx))];
    if (!deleg)
        return MB_FUNC_ERR;

	mb_check(mb_attempt_open_bracket(ctx, arg));

    int params = 0;
    void **PARAMETERS = (void **)malloc(sizeof(void *) * (params + 1));
    PARAMETERS[params] = 0;
    while ((mb_has_arg(ctx, arg)) && (mb_pop_value(ctx, arg, &var) == MB_FUNC_OK)) {
        NEW_VARIABLE(container->HANDLER, PARAMETERS[params]);
        RecursiveValue(ctx, arg, &var, PARAMETERS[params], Invoke);
        params++;
        PARAMETERS = (void **)realloc(PARAMETERS, sizeof(void *) * (params + 1));
        PARAMETERS[params] = 0;
    }

    void *EXCEPTION = 0;
    void *RES       = 0;

    Invoke(INVOKE_CALL_DELEGATE, deleg, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);

    mb_attempt_close_bracket(ctx, arg);

    if (EXCEPTION) {
        FREE_VARIABLE(EXCEPTION);
    }
    if (RES) {
        mb_value_t val = RecursivePush(ctx, RES, arg, Invoke);
        mb_push_value(ctx, arg, val);
        FREE_VARIABLE(RES);
    } else {
	    mb_value_t arg2;
	    mb_make_nil(arg2);
	    mb_push_value(ctx, arg, arg2);
    }
    for (int i = 0; i < params; i++) {
        FREE_VARIABLE(PARAMETERS[i]);
    }
    free(PARAMETERS);

    if (EXCEPTION)
        return MB_FUNC_WARNING;
    return MB_FUNC_OK;
}

CONCEPT_FUNCTION_IMPL(BasicWrap, 3)
    T_HANDLE(BasicWrap, 0)
    T_DELEGATE(BasicWrap, 1)
    T_STRING(BasicWrap, 2)
    
    basic_wrapper_container *container = (basic_wrapper_container *)(intptr_t)PARAM(0);
    struct mb_interpreter_t *ctx = BASIC_CTX(container);

    void *var = NULL;
    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), var);
    if (var) {
        std::string key(PARAM(2));
        // needs to be uppsercase
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        void *old_var = container->HANDLERS[key];
        if (old_var) {
            FREE_VARIABLE(old_var);
        }
        container->HANDLERS[key] = var;
    }

    int err = mb_register_func(ctx, PARAM(2), concept_handler_func);
    if (!err) {
        mb_remove_func(ctx, PARAM(2));
        mb_remove_reserved_func(ctx, PARAM(2));
        mb_register_func(ctx, PARAM(2), concept_handler_func);
    }
    RETURN_NUMBER(container->HANDLERS.size());
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(BasicGC, 1)
    T_HANDLE(BasicGC, 0)
    struct mb_interpreter_t *ctx = BASIC_CTX((basic_wrapper_container *)(intptr_t)PARAM(0));
    int_t collected = 0;
    mb_gc(ctx, &collected);
    RETURN_NUMBER(collected);
END_IMPL
//------------------------------------------------------------------------
