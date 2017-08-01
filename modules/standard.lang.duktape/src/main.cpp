//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "AnsiString.h"
#include "duktape.h"
#include "library.h"
//---------------------------------------------------------------------------
#define DUK_STRING_PUSH_SAFE (1 << 0)

static INVOKE_CALL  InvokePtr        = 0;

#define DUK_CTX(container)    ((container)->ctx)
#define DUK_BINARY(container) ((container)->binary_mode)

struct duk_wrapper_container {
    duk_context *ctx;
    void **HANDLERS;
    unsigned short HLEN;
    unsigned char binary_mode;
};

void on_error(void *udata, const char *msg) {
    fprintf(stderr, "CRITICAL ERROR: %s\n", msg);
    exit(0);
}

static duk_ret_t native_print(duk_context *ctx) {
	duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	printf("%s\n", duk_safe_to_string(ctx, -1));
	return 0;
}


static void debug_dump(duk_context *ctx) {
    duk_push_context_dump(ctx);
    fprintf(stderr, "%s\n", duk_safe_to_string(ctx, -1));
    duk_pop(ctx);
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr       = Invoke;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewRuntime, 0)
    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, on_error);
    if (ctx) {
	    duk_push_c_function(ctx, native_print, DUK_VARARGS);
        duk_put_global_string(ctx, "print");

        struct duk_wrapper_container *ref = (struct duk_wrapper_container *)malloc(sizeof(struct duk_wrapper_container));
        if (ref) {
            memset(ref, 0, sizeof(struct duk_wrapper_container));
            ref->ctx = ctx;

            duk_push_global_stash(ctx);
            duk_push_pointer(ctx, ref); 
            duk_put_prop_string(ctx, 0, "\xff_____concept_internal_object");
            //duk_put_global_string(ctx, "\xff_____concept_internal_object");

            RETURN_NUMBER((SYS_INT)ref);
        } else {
            duk_destroy_heap(ctx);
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSDestroyRuntime, 1)
    T_NUMBER(JSDestroyRuntime, 0)

    duk_wrapper_container *ctx = (duk_wrapper_container *)(intptr_t)PARAM(0);
    if (ctx) {
        SET_NUMBER(0, 0);
        if (ctx->HANDLERS) {
            for (int i = 0; i < ctx->HLEN; i++) {
                void *deleg = ctx->HANDLERS[i];
                if (deleg) {
                    FREE_VARIABLE(deleg);
                }
            }
            free(ctx->HANDLERS);
            ctx->HANDLERS = NULL;
        }
        duk_destroy_heap(ctx->ctx);
        free(ctx);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
void RecursiveValue(duk_context *ctx, void *RESULT, SYS_INT index, INVOKE_CALL Invoke, const char *func_name = NULL) {
        switch (duk_get_type(ctx, index)) {
            case DUK_TYPE_NUMBER:
                Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)duk_get_number(ctx, index));
                break;
            case DUK_TYPE_STRING:
                {
                    duk_size_t out_len = 0;
                    const char *str = duk_get_lstring(ctx, index, &out_len);
                    if (out_len) {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, str, (NUMBER)out_len);
                    } else {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, str, (NUMBER)0);
                    }
                }
                break;
            case DUK_TYPE_OBJECT:
                CREATE_ARRAY(RESULT);
                if (duk_is_buffer_data(ctx, index)) {
                    duk_size_t out_size = 0;
                    void *bufdata = duk_get_buffer_data(ctx, index, &out_size);
                    if (out_size > 0) {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, (const char *)bufdata, (NUMBER)out_size);
                    } else {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, (const char *)"", (NUMBER)0);
                    }
                } else
                if (duk_is_array(ctx, index)) {
                    size_t n = duk_get_length(ctx, index);
                    for (size_t i = 0; i < n; i++) {
                        void *elem_data = NULL;
                        Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &elem_data);
                        duk_get_prop_index(ctx, index, i);
                        if (elem_data)
                            RecursiveValue(ctx, elem_data, -1, Invoke);
                        duk_pop(ctx);
                    }
                } else
                if (duk_is_function(ctx, index)) {
                    Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, (const char *)func_name ? func_name : "anonymous()", (NUMBER)0);
                } else
                if (duk_is_object(ctx, index)) {
                    duk_enum(ctx, index, DUK_ENUM_INCLUDE_NONENUMERABLE);
                    while (duk_next(ctx, -1, 1)) {
                        const char *key = duk_to_string(ctx, -2);
                        if ((key) && (strcmp(key, "__proto__")) && (strcmp(key, "constructor")) && (strcmp(key, "toString")) &&
                            (strcmp(key, "toLocaleString")) && (strcmp(key, "valueOf")) && (strcmp(key, "hasOwnProperty")) &&
                            (strcmp(key, "isPrototypeOf")) && (strcmp(key, "propertyIsEnumerable"))) {
                            void *elem_data = NULL;
                            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, key, &elem_data);
                            if (elem_data)
                                RecursiveValue(ctx, elem_data, -1, Invoke, key);
                        }
                        duk_pop_2(ctx);
                    }
                    duk_pop(ctx);
                }
                break;
            case DUK_TYPE_BUFFER:
                {
                    void *ptr;
                    duk_size_t sz;

                    ptr = duk_get_buffer_data(ctx, index, &sz);
                    if ((ptr) && (sz > 0)) {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, ptr, (NUMBER)sz);
                    } else {
                        Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_STRING, "", (NUMBER)0);
                    }
                }
                break;
            case DUK_TYPE_BOOLEAN:
                if (duk_get_boolean(ctx, index)) {
                    Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (const char *)"", (NUMBER)1);
                } else {
                    Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (const char *)"", (NUMBER)0);
                }
                break;
            default:
                Invoke(INVOKE_SET_VARIABLE, RESULT, (INTEGER)VARIABLE_NUMBER, (const char *)"", (NUMBER)0);
    }
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSEvaluateScript, 2, 4)
    T_HANDLE(JSEvaluateScript, 0)
    T_STRING(JSEvaluateScript, 1)

    duk_context *ctx = DUK_CTX((duk_wrapper_container *)(intptr_t)PARAM(0));

    if (PARAMETERS_COUNT > 2) {
        T_STRING(JSEvaluateScript, 2)
        if (PARAMETERS_COUNT > 3) {
            SET_NUMBER(3, 0);
        }
        duk_push_string(ctx, PARAM(2));
        if (duk_pcompile_lstring_filename(ctx, 0, PARAM(1), PARAM_LEN(1))) {
            if (PARAMETERS_COUNT > 3) {
                RecursiveValue(ctx, PARAMETER(3), -1, Invoke);
            }
        } else {
            duk_call(ctx, 0);
            RecursiveValue(ctx, RESULT, -1, Invoke);
        }
        duk_pop(ctx);
    } else {
        duk_peval_lstring(ctx, PARAM(1), PARAM_LEN(1));
        RecursiveValue(ctx, RESULT, -1, Invoke);
        duk_pop(ctx);
    }
END_IMPL
//------------------------------------------------------------------------
const char *duk_push_string_file_raw(duk_context *ctx, const char *path, duk_uint_t flags) {
	FILE *f = NULL;
	char *buf;
	long sz;  /* ANSI C typing */

	if (!path) {
		goto fail;
	}
	f = fopen(path, "rb");
	if (!f) {
		goto fail;
	}
	if (fseek(f, 0, SEEK_END) < 0) {
		goto fail;
	}
	sz = ftell(f);
	if (sz < 0) {
		goto fail;
	}
	if (fseek(f, 0, SEEK_SET) < 0) {
		goto fail;
	}
	buf = (char *) duk_push_fixed_buffer(ctx, (duk_size_t) sz);
	if ((size_t) fread(buf, 1, (size_t) sz, f) != (size_t) sz) {
		duk_pop(ctx);
		goto fail;
	}
	(void) fclose(f);  /* ignore fclose() error */
	return duk_buffer_to_string(ctx, -1);

 fail:
	if (f) {
		(void) fclose(f);  /* ignore fclose() error */
	}

	if (flags & DUK_STRING_PUSH_SAFE) {
		duk_push_undefined(ctx);
	} else {
		duk_error(ctx, DUK_ERR_TYPE_ERROR, "read file error");
	}
	return NULL;
}

duk_int_t duk_peval_file(duk_context *ctx, const char *path) {
	duk_int_t rc;

	duk_push_string_file_raw(ctx, path, DUK_STRING_PUSH_SAFE);
	duk_push_string(ctx, path);
	rc = duk_pcompile(ctx, DUK_COMPILE_EVAL);
	if (rc != 0) {
		return rc;
	}
	duk_push_global_object(ctx); 
	rc = duk_pcall_method(ctx, 0);
	return rc;
}

CONCEPT_FUNCTION_IMPL(JSEvaluateFile, 2)
    T_HANDLE(JSEvaluateFile, 0)
    T_STRING(JSEvaluateFile, 1)

    duk_context *ctx = DUK_CTX((duk_wrapper_container *)(intptr_t)PARAM(0));
    if (duk_peval_file(ctx, PARAM(1)) != 0) {
        RETURN_NUMBER(0);
    } else {
        RecursiveValue(ctx, RESULT, -1, Invoke);
        duk_pop(ctx);
    }
END_IMPL
//------------------------------------------------------------------------
void RecursivePush(duk_context *ctx, void *var, INVOKE_CALL Invoke, unsigned char binary_mode) {
    INTEGER    type     = 0;
    char       *szValue = 0;
    NUMBER     nValue   = 0;

    Invoke(INVOKE_GET_VARIABLE, var, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_STRING:
            if (binary_mode) {
                duk_size_t buf_size = (duk_size_t)nValue;
                void *arraybuffer = duk_push_fixed_buffer(ctx, buf_size);
                // duk_push_buffer_object(ctx, -1, 0, buf_size, DUK_BUFOBJ_ARRAYBUFFER);
                if ((arraybuffer) && (buf_size > 0) && (szValue))
                    memcpy(arraybuffer, szValue, buf_size);
            } else
                duk_push_lstring(ctx, szValue, (int)nValue);
            break;
        case VARIABLE_NUMBER:
            duk_push_number(ctx, nValue);
            break;
        case VARIABLE_ARRAY:
            {
                INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, var);
                if (count) {
                    char *key = NULL;
                    void *elem_data = NULL;
                    Invoke(INVOKE_GET_ARRAY_KEY, var, 0, &key);
                    if (key) {
                        duk_idx_t objid = duk_push_object(ctx);
                        for (INTEGER i = 0; i < count; i++) {
                            InvokePtr(INVOKE_ARRAY_VARIABLE, var, i, &elem_data);
                            InvokePtr(INVOKE_GET_ARRAY_KEY, var, i, &key);
                            if ((key) && (elem_data)) {
                                RecursivePush(ctx, elem_data, Invoke, binary_mode);
                                duk_put_prop_string(ctx, objid, key);
                            }
                        }
                    } else {
                        duk_idx_t arr_idx = duk_push_array(ctx);
                        for (INTEGER i = 0; i < count; i++) {
                            Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)i, &elem_data);
                            if (elem_data) {
                                RecursivePush(ctx, elem_data, Invoke, binary_mode);
                                duk_put_prop_index(ctx, arr_idx, i);
                            }
                        }
                    }
                } else {
                    duk_push_array(ctx);
                }
            }
            break;
        case VARIABLE_CLASS:
            {
                duk_idx_t objid = duk_push_object(ctx);
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
                    for (int i = 0; i < members_count; i++) {
                            char *key = members[i];
                            void *elem_data =  variable_data[i];
                            if ((key) && (elem_data)) {
                                RecursivePush(ctx, elem_data, Invoke, binary_mode);
                                duk_put_prop_string(ctx, objid, key);
                            }
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
        default:
            duk_push_undefined(ctx);
    }

}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSCall, 3, 4)
    T_HANDLE(JSCall, 0)
    T_STRING(JSCall, 1)
    T_ARRAY(JSCall, 2)
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }

    duk_context *ctx = DUK_CTX((duk_wrapper_container *)(intptr_t)PARAM(0));
    unsigned char binary_mode = DUK_BINARY((duk_wrapper_container *)(intptr_t)PARAM(0));
    const char *func = PARAM(1);
    if ((func) && (func[0])) {
        duk_peval_lstring(ctx, func, PARAM_LEN(1));
        if (duk_is_callable (ctx, -1)) {
            INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
            duk_idx_t initial_top = duk_get_top(ctx);
            for (INTEGER i = 0; i < count; i++) {
                void *elem_data = NULL;
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), (INTEGER)i, &elem_data);
                if (elem_data) {
                    RecursivePush(ctx, elem_data, Invoke, binary_mode);
                }
            }
            duk_idx_t parameters_count = duk_get_top(ctx) - initial_top;
            if (duk_pcall(ctx, parameters_count)) {
                if (PARAMETERS_COUNT > 3) {
                    const char *err = duk_safe_to_string(ctx, -1);
                    SET_STRING(3, err);
                }
                RETURN_NUMBER(0);
            } else {
                RecursiveValue(ctx, RESULT, -1, Invoke);
            }
            duk_pop(ctx);
        } else {
            duk_pop(ctx);
            RETURN_NUMBER(0);
        }
    } else {
        RETURN_NUMBER(0);
    }    
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSVariable, 3)
    T_HANDLE(JSVariable, 0)
    T_STRING(JSVariable, 1)

    duk_wrapper_container *container = (duk_wrapper_container *)(intptr_t)PARAM(0);
    duk_context *ctx = DUK_CTX(container);
    const char *key = PARAM(1);
    if ((key) && (key[0])) {
        RecursivePush(ctx, PARAMETER(2), Invoke, DUK_BINARY(container));
        duk_put_global_string(ctx, key);
    }
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSThrow, 2)
    T_HANDLE(JSThrow, 0)

    duk_wrapper_container *container = (duk_wrapper_container *)(intptr_t)PARAM(0);
    duk_context *ctx = DUK_CTX(container);
    RecursivePush(ctx, PARAMETER(1), Invoke, DUK_BINARY(container));
    duk_throw(ctx);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSErrorCode, 1, 2)
    T_HANDLE(JSErrorCode, 0)

    duk_context *ctx = DUK_CTX((duk_wrapper_container *)(intptr_t)PARAM(0));
    int err_code = duk_get_error_code(ctx, 0); 
    if (PARAMETERS_COUNT > 1) {
        if (err_code) {
            SET_STRING(1, duk_safe_to_string(ctx, 0));
        } else {
            SET_STRING(1, "");
        }
    }
    RETURN_NUMBER(err_code);
END_IMPL
//------------------------------------------------------------------------
static duk_ret_t concept_handler_func(duk_context *ctx) {
    int magic = duk_get_current_magic(ctx);

    INVOKE_CALL Invoke = InvokePtr;
    if (!Invoke)
        return 0;

	duk_idx_t n = duk_get_top(ctx);
    void *deleg = NULL;

    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "\xff_____concept_internal_object");
    //duk_get_global_string(ctx, "\xff_____concept_internal_object");
    struct duk_wrapper_container *container = (struct duk_wrapper_container *)duk_to_pointer(ctx, -1);
    duk_pop_2(ctx);

    if ((!container) || (container->ctx != ctx) || (!container->HANDLERS) || (magic < 0) || (magic >= container->HLEN))
        return 0;

    deleg = container->HANDLERS[magic];
    if (!deleg)
        return 0;

    void **PARAMETERS = new void * [n + 1];
    PARAMETERS[n] = 0;

	for (duk_idx_t i = 0; i < n; i++) {
        CREATE_VARIABLE(PARAMETERS[i]);
        RecursiveValue(ctx, PARAMETERS[i], i, Invoke);
    }

    void *EXCEPTION = 0;
    void *RES       = 0;

    Invoke(INVOKE_CALL_DELEGATE, deleg, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);
    if (EXCEPTION) {
        RecursivePush(ctx, EXCEPTION, Invoke, DUK_BINARY(container));
        FREE_VARIABLE(EXCEPTION);
        duk_throw(ctx);
    }
    if (RES) {
        RecursivePush(ctx, RES, Invoke, DUK_BINARY(container));
        FREE_VARIABLE(RES);
    }
    for (duk_idx_t i = 0; i < n; i++) {
        FREE_VARIABLE(PARAMETERS[i]);
    }
    if (PARAMETERS)
        delete[] PARAMETERS;

    if (RES)
	    return 1;

    return 0;
}

CONCEPT_FUNCTION_IMPL(JSWrap, 3)
    T_HANDLE(JSWrap, 0)
    T_DELEGATE(JSWrap, 1)
    T_STRING(JSWrap, 2)
    
    struct duk_wrapper_container *container = (struct duk_wrapper_container *)(intptr_t)PARAM(0);
    duk_context *ctx = DUK_CTX(container);

    container->HANDLERS = (void **)realloc(container->HANDLERS, sizeof(void *) * (container->HLEN + 1));
    short delegate_id = 0;
    if (container->HANDLERS) {
        container->HANDLERS[container->HLEN] = PARAMETER(1);
        LOCK_VARIABLE(PARAMETER(1));
        container->HLEN++;
    } else {
        container->HLEN = 0;
        RETURN_NUMBER(-1);
        return 0;
    }
    duk_push_global_object(ctx);
    duk_push_c_function(ctx, concept_handler_func, DUK_VARARGS);
    duk_set_magic(ctx, -1, container->HLEN - 1);
    duk_put_global_lstring(ctx, PARAM(2), PARAM_LEN(2));
    RETURN_NUMBER(container->HLEN);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSGC, 1)
    T_HANDLE(JSGC, 0)
    duk_context *ctx = DUK_CTX((duk_wrapper_container *)(intptr_t)PARAM(0));
    duk_gc(ctx, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSModeBinary, 1, 2)
    T_HANDLE(JSModeBinary, 0)
    unsigned char binary_mode = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(JSModeBinary, 1)
        if (!PARAM_INT(1))
            binary_mode = 0;
    }
    duk_wrapper_container *container = (duk_wrapper_container *)(intptr_t)PARAM(0);
    int old_binary_mode = container->binary_mode;
    container->binary_mode = binary_mode;
    RETURN_NUMBER(old_binary_mode);
END_IMPL
//------------------------------------------------------------------------
