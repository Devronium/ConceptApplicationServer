//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "library.h"
//------------ end of standard header ----------------------------//
#include <string.h>
#include <stdlib.h>

extern "C" {
    #include "linenoise.h"
}
#ifdef _WIN32
    static void *HINT_DELEGATE = NULL;
    static void *COMPLETION_DELEGATE = NULL;
    static INVOKE_CALL InvokePtr = NULL;
#else
    static __thread void *HINT_DELEGATE = NULL;
    static __thread void *COMPLETION_DELEGATE = NULL;
    static __thread INVOKE_CALL InvokePtr = NULL;
#endif
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifndef _WIN32
    linenoiseSetMultiLine(1);
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------------------------------------
void ttycompletion(const char *buf, linenoiseCompletions *lc) {
    if ((COMPLETION_DELEGATE) && (InvokePtr)) {
        INVOKE_CALL Invoke = InvokePtr;
        void *EXCEPTION = 0;
        void *RES       = 0;
        Invoke(INVOKE_CALL_DELEGATE, COMPLETION_DELEGATE, &RES, &EXCEPTION, (INTEGER)1, VARIABLE_STRING, buf, (NUMBER)0);
        if (EXCEPTION) {
            FREE_VARIABLE(EXCEPTION);
        }
        if (RES) {
            INTEGER    type     = 0;
            char       *szValue = 0;
            NUMBER     nValue   = 0;

            Invoke(INVOKE_GET_VARIABLE, RES, &type, &szValue, &nValue);

            if (type == VARIABLE_ARRAY) {
                char **values = GetCharList(RES, Invoke);
                if (values) {
                    int i = 0;
                    char *value = values[i++];
                    while (value) {
                        linenoiseAddCompletion(lc, value);
                        value = values[i++];
                    }
                    delete[] values;
                }
            } else
            if ((type == VARIABLE_STRING) && (szValue)) {
                linenoiseAddCompletion(lc, szValue);
            }
            FREE_VARIABLE(RES);
        }
    }
}

char *hints(const char *line, int *color, int *bold) {
    if ((HINT_DELEGATE) && (InvokePtr)) {
        static char buf[4096];
        INVOKE_CALL Invoke = InvokePtr;
        void *EXCEPTION = 0;
        void *RES       = 0;
        Invoke(INVOKE_CALL_DELEGATE, HINT_DELEGATE, &RES, &EXCEPTION, (INTEGER)1, VARIABLE_STRING, line, (NUMBER)0);
        if (EXCEPTION) {
            FREE_VARIABLE(EXCEPTION);
        }
        if (RES) {
            INTEGER    type     = 0;
            char       *szValue = 0;
            NUMBER     nValue   = 0;

            InvokePtr(INVOKE_GET_VARIABLE, RES, &type, &szValue, &nValue);

            if (type == VARIABLE_STRING) {
                if (nValue > 0) {
                    if (nValue > sizeof(buf) - 1)
                        nValue = sizeof(buf) - 1;
                    memcpy(buf, szValue, (int)nValue);
                    buf[(int)nValue] = 0;
                    *color = 31;
                    *bold = 1;
                    FREE_VARIABLE(RES);
                    return buf;
                }
            }
            FREE_VARIABLE(RES);
        }
    }
    return NULL;
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ttyreadline, 0, 3)
    const char *prompt = "";
    if (PARAMETERS_COUNT > 0) {
        T_STRING(ttyreadline, 0)
        prompt = PARAM(0);
    }
    InvokePtr = Invoke;
    if (PARAMETERS_COUNT > 1) {
        T_DELEGATE(ttyreadline, 1)
        COMPLETION_DELEGATE = PARAMETER(1);
        linenoiseSetCompletionCallback(ttycompletion);
    }
    if (PARAMETERS_COUNT > 2) {
        T_DELEGATE(ttyreadline, 2)
        HINT_DELEGATE = PARAMETER(2);
#ifndef _WIN32
        linenoiseSetHintsCallback(hints);
#endif
    }
    char *line = linenoise(prompt);
    COMPLETION_DELEGATE = NULL;
    HINT_DELEGATE = NULL;
    linenoiseSetCompletionCallback(NULL);
#ifndef _WIN32
    linenoiseSetHintsCallback(NULL);
#endif
    if (line) {
        RETURN_STRING(line);
        linenoiseHistoryAdd(line);
        free(line);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ttysavehistory, 0, 1)
    const char *file = NULL;
    if (PARAMETERS_COUNT > 0) {
        T_STRING(ttysavehistory, 0)
        file = PARAM(0);
        if ((file) && (!file[0]))
            file = NULL;
    }
    if (!file) {
#ifdef _WIN32
        char *path = getenv("USERPROFILE");
        char buf[8192];
        buf[0] = 0;
        if (path)
            strcpy(buf, path);
        strcat(buf, "\\conceptttyreadline");
        file = buf;
#else
        char *path = getenv("HOME");
        char buf[8192];
        buf[0] = 0;
        if (path)
            strcpy(buf, path);
        strcat(buf, "/.conceptttyreadline");
        file = buf;
#endif
    }
    int err = linenoiseHistorySave(file);
    RETURN_NUMBER(err);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ttyloadhistory, 0, 1)
    const char *file = NULL;
    if (PARAMETERS_COUNT > 0) {
        T_STRING(ttyloadhistory, 0)
        file = PARAM(0);
        if ((file) && (!file[0]))
            file = NULL;
    }
    if (!file) {
#ifdef _WIN32
        char *path = getenv("USERPROFILE");
        char buf[8192];
        buf[0] = 0;
        if (path)
            strcpy(buf, path);
        strcat(buf, "\\conceptttyreadline");
        file = buf;
#else
        char *path = getenv("HOME");
        char buf[8192];
        buf[0] = 0;
        if (path)
            strcpy(buf, path);
        strcat(buf, "/.conceptttyreadline");
        file = buf;
#endif
    }
    int err = linenoiseHistoryLoad(file);
    RETURN_NUMBER(err);
END_IMPL
//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ttymaxhistory, 1)
    T_NUMBER(ttymaxhistory, 0)
    int is_set = linenoiseHistorySetMaxLen(PARAM_INT(0));
    RETURN_NUMBER(is_set);
END_IMPL
//-----------------------------------------------------------------------------------
