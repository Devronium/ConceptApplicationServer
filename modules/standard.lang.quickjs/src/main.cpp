//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "pointer_list.h"
//------------ end of standard header ----------------------------//
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "library.h"
#include "quickjs/quickjs.h"
//---------------------------------------------------------------------------
static INVOKE_CALL  InvokePtr        = 0;

DEFINE_LIST(quick_js);

struct quickjs_wrapper_container {
    JSRuntime *jsrt;
    JSContext *jsctx;

    void **HANDLERS;
    unsigned short HLEN;
    unsigned char binary_mode;
    clock_t timeout;
    clock_t expires;

    int lastErrorCode;
    void *HANDLER;
};
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS{
    InvokePtr       = Invoke;
    INIT_LIST(quick_js);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS{
    if (!HANDLER)
        DEINIT_LIST(quick_js);
    return 0;
}
//---------------------------------------------------------------------------
int eval_execution_time(JSRuntime *rt, void *opaque) {
    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container *)opaque;
    if (!ref)
        return 0;
    
    if ((ref->expires) && (clock() > ref->expires))
        return 1;

    return 0;
}

void RecursiveValue(JSContext *ctx, JSValue val, void *RESULT, SYS_INT index, INVOKE_CALL Invoke, const char *func_name = NULL, int max_level = 10) {
    if (max_level <= 0)
        return;
    if ((JS_IsNumber(val)) || (JS_IsBigInt(ctx, val))) {
        double v = 0;
        JS_ToFloat64(ctx, &v, val);
        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char*)"", (NUMBER)v);
    } else
    if (JS_IsString(val)) {
        const char *str;
        size_t len;

        str = JS_ToCStringLen(ctx, &len, val);
        if ((str) && (len > 0))
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)len);
        else
            Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        JS_FreeCString(ctx, str);
    } else
    if (JS_IsFunction(ctx, val)) {
        CREATE_ARRAY(RESULT);

        void* elem_data = NULL;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "__proto__", &elem_data);
        JSValue proto = JS_GetPrototype(ctx, val);

        Invoke(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_STRING, (char *)"[function]", (NUMBER)0);

        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "toString()", &elem_data);
        const char *str;
        size_t len;
        str = JS_ToCStringLen(ctx, &len, val);
        if ((str) && (len > 0)) {
            Invoke(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_STRING, (char *)str, (NUMBER)len);
        } else
            Invoke(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
        
        JS_FreeCString(ctx, str);
        JS_FreeValue(ctx, proto);
    } else
    if (JS_IsObject(val)) {
        CREATE_ARRAY(RESULT);

        uint32_t len, i;
        JSPropertyEnum *tab = NULL;
        if (JS_IsArray(ctx, val)) {
            if (JS_GetOwnPropertyNames(ctx, &tab, &len, val, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) >= 0) {
                for (i = 0; i < len; i++) {
                    JSValue member = JS_GetProperty(ctx, val, tab[i].atom);

                    void* elem_data = NULL;
                    Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &elem_data);

                    RecursiveValue(ctx, member, elem_data, i, Invoke, func_name, max_level - 1);
                    JS_FreeValue(ctx, member);
                    JS_FreeAtom(ctx, tab[i].atom);
                }
            }
        } else {
            void* elem_data = NULL;

            JSValue proto = JS_GetPrototype(ctx, val);

            const char *str;
            size_t len;
            str = JS_ToCStringLen(ctx, &len, proto);

            if ((str) && (len > 0) && (strcmp(str, "[object Object]"))) {
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "__proto__", &elem_data);
                Invoke(INVOKE_SET_VARIABLE, elem_data, (INTEGER)VARIABLE_STRING, (char *)str,  (NUMBER)len);
            }

            JS_FreeCString(ctx, str);
            JS_FreeValue(ctx, proto);
            
            uint32_t prop_len;
            if (JS_GetOwnPropertyNames(ctx, &tab, &prop_len, val, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) >= 0) {
                for (i = 0; i < prop_len; i++) {
                    const char *member_name = JS_AtomToCString(ctx, tab[i].atom);
                    JS_FreeCString(ctx, member_name);

                    if (member_name) {
                        JSValue member = JS_GetProperty(ctx, val, tab[i].atom);

                        void* elem_data = NULL;
                        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, member_name, &elem_data);

                        RecursiveValue(ctx, member, elem_data, -1, Invoke, func_name, max_level - 1);

                        JS_FreeValue(ctx, member);
                    }
                    JS_FreeAtom(ctx, tab[i].atom);
                }
            }
        }
        js_free(ctx, tab);
    } else
    if (JS_IsBool(val)) {
        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)JS_ToBool(ctx, val));
    } else {
        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)0);
    }
}

JSValue RecursiveSet(JSContext *ctx, void *var, INVOKE_CALL Invoke, unsigned char binary_mode, int max_level = 10) {
    if (max_level <= 0)
        return JS_UNDEFINED;

    INTEGER    type     = 0;
    char       *szValue = 0;
    NUMBER     nValue   = 0;

    Invoke(INVOKE_GET_VARIABLE, var, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_STRING:
            if (binary_mode)
                return JS_NewArrayBufferCopy(ctx, (const uint8_t *)szValue, (int)nValue);
            return JS_NewStringLen(ctx, szValue, (int)nValue);
        case VARIABLE_NUMBER:
            return JS_NewInt64(ctx, nValue);
        case VARIABLE_ARRAY:
            {
                INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, var);
                if (count) {
                    char *key = NULL;
                    void *elem_data = NULL;
                    Invoke(INVOKE_GET_ARRAY_KEY, var, 0, &key);
                    if (key) {
                        JSValue obj = JS_NewObject(ctx);
                        for (INTEGER i = 0; i < count; i++) {
                            InvokePtr(INVOKE_ARRAY_VARIABLE, var, i, &elem_data);
                            InvokePtr(INVOKE_GET_ARRAY_KEY, var, i, &key);
                            if ((key) && (elem_data))
                                JS_SetPropertyStr(ctx, obj, key, RecursiveSet(ctx, elem_data, Invoke, binary_mode, max_level - 1));
                        }
                        return obj;
                    } else {
                        JSValue array = JS_NewArray(ctx);

                        for (INTEGER i = 0; i < count; i++) {
                            Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)i, &elem_data);
                            if (elem_data)
                                JS_SetPropertyUint32(ctx, array, (uint32_t)i, RecursiveSet(ctx, elem_data, Invoke, binary_mode, max_level - 1));
                        }
                        return array;
                    }
                } else {
                    return JS_NewArray(ctx);
                }
            }
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
                JSValue obj = JS_NewObject(ctx);
                if (IS_OK(result)) {
                    for (int i = 0; i < members_count; i++) {
                            char *key = members[i];
                            void *elem_data =  variable_data[i];
                            if ((key) && (elem_data) && (!flags[i]))
                                JS_SetPropertyStr(ctx, obj, key, RecursiveSet(ctx, elem_data, Invoke, binary_mode, max_level - 1));
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

                return obj;
            }
    }
    return JS_UNDEFINED;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSNewRuntime, 0, 1)
    size_t memory_limit = 1024 * 1024;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(JSNewRuntime, 0);
        memory_limit = (size_t)PARAM_INT(0);
    }

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container *)malloc(sizeof(struct quickjs_wrapper_container));
    if (ref) {
        memset(ref, 0, sizeof(struct quickjs_wrapper_container));
        ref->jsrt = JS_NewRuntime();
        if (ref->jsrt) {
            ref->jsctx = JS_NewContext(ref->jsrt);
            JS_SetContextOpaque(ref->jsctx, ref);
            ref->HANDLER = PARAMETERS->HANDLER;
            if (memory_limit > 0)
                JS_SetMemoryLimit(ref->jsrt, memory_limit);
        } else {
            free(ref);
            ref = NULL;
        }
    }
    RETURN_NUMBER(MAP_POINTER(quick_js, ref, PARAMETERS->HANDLER));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSDestroyRuntime, 1)
    T_NUMBER(JSDestroyRuntime, 0)
    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container *)FREE_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (ref) {
        if (ref->HANDLERS) {
            for (int i = 0; i < ref->HLEN; i++) {
                void *deleg = ref->HANDLERS[i];
                if (deleg) {
                    FREE_VARIABLE(deleg);
                }
            }
            free(ref->HANDLERS);
            ref->HANDLERS = NULL;
        }
        if (ref->jsctx)
            JS_FreeContext(ref->jsctx);
        JS_FreeRuntime(ref->jsrt);
        free(ref);
    }
    SET_NUMBER(0, 0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSEvaluateScript, 2, 5)
    T_HANDLE(JSEvaluateScript, 0)
    T_STRING(JSEvaluateScript, 1)
    const char *script_name = "<input>";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(JSEvaluateScript, 2)
        script_name = PARAM(2);
    }
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }
    if (PARAMETERS_COUNT > 4) {
        SET_NUMBER(4, 0);
    }

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSEvaluateScript: invalid js handler";

    if (ref->timeout)
        ref->expires = clock() + ref->timeout * CLOCKS_PER_SEC / 1000;
    else
        ref->expires = 0;

    JSValue result = JS_Eval(ref->jsctx, PARAM(1), PARAM_LEN(1), script_name, 0);
    if (JS_IsException(result)) {
        if (PARAMETERS_COUNT > 3) {
            JSValue exception = JS_GetException(ref->jsctx);
            const char *str;
            size_t len;
            str = JS_ToCStringLen(ref->jsctx, &len, exception);

            if (PARAMETERS_COUNT > 4) {
                JSValue stack = JS_GetPropertyStr(ref->jsctx, exception, "stack");
                if (!JS_IsUndefined(stack))
                    RecursiveValue(ref->jsctx, stack, PARAMETER(4), -1, Invoke);
                JS_FreeValue(ref->jsctx, stack);
            }

            if ((str) && (len > 0)) {
                SET_BUFFER(3, str, len);
            } else {
                SET_STRING(3, "");
            }
            JS_FreeValue(ref->jsctx, exception);
        }
        RETURN_NUMBER(0);
    } else
        RecursiveValue(ref->jsctx, result, RESULT, -1, Invoke);
    JS_FreeValue(ref->jsctx, result);
END_IMPL
//---------------------------------------------------------------------------
JSValue concept_handler_func(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic) {
    JSValue return_value = JS_UNDEFINED;

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)JS_GetContextOpaque(ctx);
    if (!ref)
        return return_value;

    INVOKE_CALL Invoke = InvokePtr;
    if (!Invoke)
        return return_value;

    void *deleg = NULL;

    if ((!ref) || (ref->jsctx != ctx) || (!ref->HANDLERS) || (magic < 0) || (magic >= ref->HLEN))
        return return_value;

    deleg = ref->HANDLERS[magic];
    if (!deleg)
        return return_value;

    void **PARAMETERS = new void * [argc + 1];
    PARAMETERS[argc] = 0;

    for (int i = 0; i < argc; i++) {
        NEW_VARIABLE(ref->HANDLER, PARAMETERS[i]);
        RecursiveValue(ctx, argv[i], PARAMETERS[i], i, Invoke);
    }

    void *EXCEPTION = 0;
    void *RES       = 0;

    Invoke(INVOKE_CALL_DELEGATE, deleg, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);
    if (EXCEPTION) {
        JSValue exception = RecursiveSet(ctx, EXCEPTION, Invoke, ref->binary_mode);
        FREE_VARIABLE(EXCEPTION);

        return_value = JS_Throw(ctx, exception);
        JS_FreeValue(ctx, exception);
    }
    if (RES) {
        if (!EXCEPTION)
            return_value = RecursiveSet(ctx, RES, Invoke, ref->binary_mode);
        FREE_VARIABLE(RES);
    }

    for (int i = 0; i < argc; i++) {
        FREE_VARIABLE(PARAMETERS[i]);
    }
    if (PARAMETERS)
        delete[] PARAMETERS;

    return return_value;
}

CONCEPT_FUNCTION_IMPL(JSWrap, 3)
    T_HANDLE(JSWrap, 0)
    T_DELEGATE(JSWrap, 1)
    T_STRING(JSWrap, 2)

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSWrap: invalid js handler";

    ref->HANDLERS = (void **)realloc(ref->HANDLERS, sizeof(void *) * (ref->HLEN + 1));
    short delegate_id = 0;
    if (ref->HANDLERS) {
        void *var = NULL;
        DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), var);
        if (var) {
            ref->HANDLERS[ref->HLEN] = var;
            ref->HLEN++;
        }
    } else {
        ref->HLEN = 0;
        RETURN_NUMBER(-1);
        return 0;
    }

    JSValue global_obj = JS_GetGlobalObject(ref->jsctx);
    JS_SetPropertyStr(ref->jsctx, global_obj, PARAM(2), JS_NewCFunctionMagic(ref->jsctx, concept_handler_func, PARAM(2), 0, JS_CFUNC_generic_magic, ref->HLEN - 1));
    JS_FreeValue(ref->jsctx, global_obj);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSGC, 0)
    T_HANDLE(JSGC, 0)

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSGC: invalid js handler";

    JS_RunGC(ref->jsrt);

    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSModeBinary, 1, 2)
    T_HANDLE(JSModeBinary, 0)
    unsigned char binary_mode = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(JSModeBinary, 1)
        if (!PARAM_INT(1))
            binary_mode = 0;
    }
    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSModeBinary: invalid js handler";
    int old_binary_mode = ref->binary_mode;
    ref->binary_mode = binary_mode;
    RETURN_NUMBER(old_binary_mode);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSTimeout, 2)
    T_HANDLE(JSTimeout, 0)
    T_NUMBER(JSTimeout, 1)

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSTimeout: invalid js handler";
    int timeout = PARAM_INT(1);
    if (timeout > 0) {
        ref->timeout = timeout;
        ref->expires = clock() + ref->timeout * CLOCKS_PER_SEC / 1000;
        JS_SetInterruptHandler(ref->jsrt, eval_execution_time, ref);
    } else {
        ref->timeout = 0;
        JS_SetInterruptHandler(ref->jsrt, NULL, NULL);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSCall, 3, 4)
    T_HANDLE(JSCall, 0)
    T_STRING(JSCall, 1)
    T_ARRAY(JSCall, 2)
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSCall: invalid js handler";
    if (ref->timeout)
        ref->expires = clock() + ref->timeout * CLOCKS_PER_SEC / 1000;
    else
        ref->expires = 0;

    INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    JSValue *argv = NULL;
    if (count > 0) {
        argv = (JSValue *)malloc(count * sizeof(JSValue));
        for (INTEGER i = 0; i < count; i++) {
            void *elem_data = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), (INTEGER)i, &elem_data);
            if (elem_data)
                argv[i] = RecursiveSet(ref->jsctx, elem_data, Invoke, ref->binary_mode);
            else
                argv[i] = JS_UNDEFINED;
        }
    }
    
    JSValue global_obj = JS_GetGlobalObject(ref->jsctx);
    JSValue func_obj = JS_GetPropertyStr(ref->jsctx, global_obj, PARAM(1));

    JSValue result = JS_Call(ref->jsctx, func_obj, global_obj, count, argv);

    JS_FreeValue(ref->jsctx, func_obj);
    JS_FreeValue(ref->jsctx, global_obj);

    if (argv) {
        for (INTEGER i = 0; i < count; i ++)
            JS_FreeValue(ref->jsctx, argv[i]);

        free(argv);
    }
    if (JS_IsException(result)) {
        if (PARAMETERS_COUNT > 3) {
            JSValue exception = JS_GetException(ref->jsctx);
            const char *str;
            size_t len;
            str = JS_ToCStringLen(ref->jsctx, &len, exception);
            if ((str) && (len > 0)) {
                SET_BUFFER(3, str, len);
            } else {
                SET_STRING(3, "");
            }
            JS_FreeValue(ref->jsctx, exception);
        }
        RETURN_NUMBER(0);
    } else
        RecursiveValue(ref->jsctx, result, RESULT, -1, Invoke);

    JS_FreeValue(ref->jsctx, result);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSVariable, 3)
    T_HANDLE(JSVariable, 0)
    T_STRING(JSVariable, 1)

    struct quickjs_wrapper_container *ref = (struct quickjs_wrapper_container*)GET_POINTER(quick_js, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ref)
        return (void *)"JSVariable: invalid js handler";
    const char *key = PARAM(1);
    if ((key) && (key[0])) {
        JSValue global_obj = JS_GetGlobalObject(ref->jsctx);
        JS_SetPropertyStr(ref->jsctx, global_obj, PARAM(1), RecursiveSet(ref->jsctx, PARAMETER(2), Invoke, ref->binary_mode));
        JS_FreeValue(ref->jsctx, global_obj);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
