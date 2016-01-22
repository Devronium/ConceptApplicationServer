//#define GO_DEBUG

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "_GB", "GLOBALS()");
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_CLArg CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "CLArg: CLArg()");

    LOCAL_INIT;
    long argc   = 0;
    char **argv = 0;


    Invoke(INVOKE_CLI_ARGUMENTS, &argc, &argv);

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    if (CLIENT_SOCKET <= 0) {
        for (int i = 0; i < argc; i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, argv[i], (double)0);
        }
    } else {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)0, (INTEGER)VARIABLE_STRING, getenv("CONCEPT_ARGUMENTS"), (double)0);
    }

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GLOBALS CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "GLOBALS: GLOBALS() takes no parameters");
    LOCAL_INIT;
    void *GOARRAY = NULL;
    Invoke(INVOKE_GETGLOBALS, PARAMETERS->HANDLER, &GOARRAY);
    RETURN_ARRAY(GOARRAY);
    END_IMPL
//---------------------------------------------------------------------------
    CONCEPT_DLL_API CONCEPT_eval CONCEPT_API_PARAMETERS {
        PARAMETERS_CHECK_MIN_MAX(1, 3, "eval takes 1 to 3 parameters: eval(string buffer [[, append_main_body = true], var error_code])");

        LOCAL_INIT;

        int    res;
        char   append_main_body = 1;
        char   *szvariable      = 0;
        NUMBER len2             = 0;
        int    len;
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szvariable, &len2);
        if (TYPE != VARIABLE_STRING)
            return (void *)"eval: first parameter should be a string";

        if (PARAMETERS_COUNT > 1) {
            double nappend_body = 0;
            GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &szDUMMY_FILL, &nappend_body);
            if (TYPE != VARIABLE_NUMBER)
                return (void *)"eval: second parameter should be a number (boolean value)";
            append_main_body = (int)nappend_body;
        }

        len = (int)len2;
        if (append_main_body) {
            static char to_append[]     = "class Main{function Main(){";
            static char to_append_end[] = "}}";
            int         len_append      = sizeof(to_append) - 1;
            int         len_end         = sizeof(to_append_end) - 1;

            int  new_len = len_append + len_end + len + 1;
            char *append = (char *)malloc(new_len);
            if (append) {
                append[new_len - 1] = 0;
                char *ptr = append;
                memcpy(ptr, to_append, len_append);
                ptr += len_append;
                memcpy(ptr, szvariable, len);
                ptr += len;
                memcpy(ptr, to_append_end, len_end);

                // without the null character
                res = Invoke(INVOKE_EXECUTECODE, PARAMETERS->HANDLER, append, (int)(new_len - 1), RESULT);
                free(append);
            }
        } else
            res = Invoke(INVOKE_EXECUTECODE, PARAMETERS->HANDLER, szvariable, (int)len, RESULT);
        if (res < 0) {
            if (PARAMETERS_COUNT > 2) {
                SET_NUMBER(2, res);
            }
        }
        return 0;
    }
//---------------------------------------------------------------------------
    CONCEPT_DLL_API CONCEPT_IncludeCode CONCEPT_API_PARAMETERS {
        PARAMETERS_CHECK(1, "IncludeRequest takes 1 parameter: IncludeRequest(string buffer)");

        LOCAL_INIT;
        char   *szvariable = 0;
        NUMBER len2        = 0;

        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szvariable, &len2);
        if (TYPE != VARIABLE_STRING)
            return (void *)"IncludeRequest: first parameter should be a string";

        // 64k error buffer
        INTEGER err_buf_size = 0xFFFF;
        char    *err_buf     = (char *)malloc(err_buf_size);
        Invoke(INVOKE_DYNAMIC_INCLUDE, PARAMETERS->HANDLER, szvariable, err_buf, err_buf_size);
        if (err_buf[0]) {
            RETURN_STRING(err_buf);
        } else {
            RETURN_STRING("");
        }
        free(err_buf);
        return 0;
    }
//---------------------------------------------------------------------------
    CONCEPT_DLL_API CONCEPT_RemoveClass CONCEPT_API_PARAMETERS {
        PARAMETERS_CHECK(1, "RemoveClass takes 1 parameter: RemoveClass(string classname)");

        LOCAL_INIT;
        char   *szvariable = 0;
        NUMBER len2        = 0;

        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &szvariable, &len2);
        if (TYPE != VARIABLE_STRING)
            return (void *)"RemoveClass: first parameter should be a string";

        int res = Invoke(INVOKE_DYNAMIC_REMOVE_CLASS, PARAMETERS->HANDLER, szvariable);
        if (res) {
            RETURN_NUMBER(0);
        } else {
            RETURN_NUMBER(1);
        }
        return 0;
    }
    CONCEPT_DLL_API CONCEPT_CheckReachability CONCEPT_API_PARAMETERS {
        PARAMETERS_CHECK(0, "CheckReachability takes no parameters");
        int res = Invoke(INVOKE_CHECK_REACHABILITY, PARAMETERS->HANDLER);
        RETURN_NUMBER(res);
        return 0;
    }
