//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"
extern "C" {
    #include "mustach.h"
}

static char *array_type = "array";
static char *delegate_type = "delegate";
static char *object_type = "object";
static char *null_type = "null";
static char *exception_type = "exception";

struct mustache_stack {
    void *DATA;
    void *arrdata;
    int pos;
    int count;
};

struct mustache_closure {
    INVOKE_CALL Invoke;
    void *OWNER;
    void *DATA;
    void *arrdata;
    int stack_level;
    int pos;
    int count;
    struct mustache_stack stack[0x100];
    char temp[0xFFF];
};
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_SCONSTANT("MUSTACH_OK", "0")
    DEFINE_SCONSTANT("MUSTACH_ERROR_SYSTEM", "-1")
    DEFINE_SCONSTANT("MUSTACH_ERROR_UNEXPECTED_END", "-2")
    DEFINE_SCONSTANT("MUSTACH_ERROR_EMPTY_TAG", "-3")
    DEFINE_SCONSTANT("MUSTACH_ERROR_TAG_TOO_LONG", "-4")
    DEFINE_SCONSTANT("MUSTACH_ERROR_BAD_SEPARATORS", "-5")
    DEFINE_SCONSTANT("MUSTACH_ERROR_TOO_DEPTH", "-6")
    DEFINE_SCONSTANT("MUSTACH_ERROR_CLOSING", "-7")
    DEFINE_SCONSTANT("MUSTACH_ERROR_BAD_UNESCAPE_TAG", "-8")
    DEFINE_SCONSTANT("MUSTACH_ERROR_UNDEFINED", "-9")
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
char *resolve(mustache_closure *mustacheclosure, const char *name, void *DATA) {
    INTEGER     type = 0;
    char        *str = 0;
    NUMBER      nr   = 0;
    int len;
    if (!DATA)
        return null_type;

    int invoke_err;
    if (name) {
        invoke_err = mustacheclosure->Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, DATA, name, &type, &str, &nr);
        if (invoke_err) {
            void *var = NULL;
            invoke_err = mustacheclosure->Invoke(INVOKE_GET_CLASS_VARIABLE, DATA, name, &var);
            if ((IS_OK(invoke_err)) && (var)) {
                invoke_err = mustacheclosure->Invoke(INVOKE_GET_VARIABLE, var, &type, &str, &nr);
            }
        }
    } else {
        invoke_err = mustacheclosure->Invoke(INVOKE_GET_VARIABLE, DATA, &type, &str, &nr);
    }
    if (IS_OK(invoke_err)) {
        switch (type) {
            case VARIABLE_STRING:
                return str;
            case VARIABLE_NUMBER:
                mustacheclosure->temp[0] = 0;
                snprintf(mustacheclosure->temp, 0xFF, "%.30g", nr);
                len = strlen(mustacheclosure->temp);
                if (((len > 1) && ((mustacheclosure->temp[len - 1] == '.') || (mustacheclosure->temp[len - 1] == ','))))
                    mustacheclosure->temp[len - 1] = 0;
                return mustacheclosure->temp;
            case VARIABLE_DELEGATE:
                {
                    void *DELEGATE = NULL;
                    mustacheclosure->Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, DATA, name, &DELEGATE);

                    void *RES       = 0;
                    void *EXCEPTION = 0;
                    mustacheclosure->Invoke(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)0);
                    if (EXCEPTION) {
                        mustacheclosure->Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
                        if (RES)
                            mustacheclosure->Invoke(INVOKE_FREE_VARIABLE, RES);
                        return exception_type;
                    }
                    if (RES) {
                        char *data = resolve(mustacheclosure, NULL, RES);
                        mustacheclosure->temp[0] = 0;
                        if (data) {
                            len = strlen(data);
                            if (len >= sizeof(mustacheclosure->temp))
                                len = sizeof(mustacheclosure->temp) - 1;
                            memcpy(mustacheclosure->temp, data, len);
                        }
                        mustacheclosure->Invoke(INVOKE_FREE_VARIABLE, RES);
                        return mustacheclosure->temp;
                    }
                }
                return null_type;
            case VARIABLE_ARRAY:
                return array_type;
            case VARIABLE_CLASS:
                return object_type;
        }
    }
    return null_type;
}

char *mustache_put(void *closure, const char *name, int escape) {
    mustache_closure *mustacheclosure = (mustache_closure *)closure;
    if (!name)
        return NULL;

    if ((name) && (name[0] == '.') && (!name[1]))
        name = NULL;

    void *DATA = mustacheclosure->arrdata;
    if (!DATA)
        DATA = mustacheclosure->DATA;

    return resolve(mustacheclosure, name, DATA);
}

int mustache_enter(void *closure, const char *name) {
    if (!name)
        return 0;
    mustache_closure *mustacheclosure = (mustache_closure *)closure;
    int count = 0;
    void *var = NULL;
    void *arr2;
    int is_ok; 
    if ((name) && (name[0] == '.') && (!name[1])) {
        var = mustacheclosure->DATA;
        is_ok = 1;
    } else
        is_ok = IS_OK(mustacheclosure->Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, mustacheclosure->DATA, name, &var));

    if (is_ok) {
        if (mustacheclosure->stack_level >= 0x100)
            return MUSTACH_ERROR_TOO_DEPTH;


        count = mustacheclosure->Invoke(INVOKE_GET_ARRAY_COUNT, var);
        if (count > 0) {
            mustacheclosure->Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)0, &arr2);
            if (arr2) {
                mustacheclosure->stack[mustacheclosure->stack_level].DATA = mustacheclosure->DATA;
                mustacheclosure->stack[mustacheclosure->stack_level].pos = mustacheclosure->pos;
                mustacheclosure->stack[mustacheclosure->stack_level].count = mustacheclosure->count;
                mustacheclosure->stack[mustacheclosure->stack_level].arrdata = mustacheclosure->arrdata;
                mustacheclosure->stack_level++;
                mustacheclosure->DATA = var;
                mustacheclosure->arrdata = arr2;
                mustacheclosure->pos = 0;
                mustacheclosure->count = count;
                if (count < 0)
                    return 0;
                return count;
            }
        }
        return 0;
    }
    return count;
}

int mustache_next(void *closure) {
    mustache_closure *mustacheclosure = (mustache_closure *)closure;
    mustacheclosure->pos++;
    if (mustacheclosure->pos >= mustacheclosure->count)
        return 0;
    if (IS_OK(mustacheclosure->Invoke(INVOKE_ARRAY_VARIABLE, mustacheclosure->DATA, (INTEGER)mustacheclosure->pos, &mustacheclosure->arrdata)))
        return 1;
    return 0;
}

int mustache_leave(void *closure) {
    mustache_closure *mustacheclosure = (mustache_closure *)closure;
    if (mustacheclosure->stack_level) {
        mustacheclosure->stack_level--;
        mustacheclosure->DATA = mustacheclosure->stack[mustacheclosure->stack_level].DATA;
        mustacheclosure->pos = mustacheclosure->stack[mustacheclosure->stack_level].pos;
        mustacheclosure->count = mustacheclosure->stack[mustacheclosure->stack_level].count;
        mustacheclosure->arrdata = mustacheclosure->stack[mustacheclosure->stack_level].arrdata;
    }
    return 0;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mustache, 2, 3)
    T_STRING(mustache, 0)

    struct mustach_itf itf = {NULL, mustache_put, mustache_enter, mustache_next, mustache_leave};
    struct mustache_closure closure = {Invoke, PARAMETERS->HANDLER, PARAMETER(1), NULL, 0, -1, -1};
    char *result = NULL;
    size_t size = 0;

    int err = mustach(PARAM(0), &itf, &closure, &result, &size);
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, err);
    }
    if (size > 0) {
        RETURN_BUFFER(result, size);
    } else
    if (result) {
        RETURN_STRING(result);
    } else {
        RETURN_STRING("");
    }
    if (result)
        free(result);
END_IMPL
//------------------------------------------------------------------------
