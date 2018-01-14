#include "BuiltIns.h"
#include "PIFAlizator.h"
#include "ModuleLink.h"
#include "BuiltInsHelpers.h"
#include <stdlib.h>
#include <string.h>
extern "C" {
    #include "builtin/regexp.h"
}

// ==================================================== //
// BUILT-IN functions                                   //
// ==================================================== //
CONCEPT_FUNCTION_IMPL(___CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID, 0)
    unsigned int ref_id = Invoke(INVOKE_GETID, PARAMETERS->PIF);
    RETURN_NUMBER(ref_id);
END_IMPL

CONCEPT_FUNCTION_IMPL(CLArg, 0)
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
END_IMPL

CONCEPT_FUNCTION_IMPL(GLOBALS, 0)
    void *GOARRAY = NULL;
    Invoke(INVOKE_GETGLOBALS, PARAMETERS->PIF, &GOARRAY);
    RETURN_ARRAY(GOARRAY);
END_IMPL

CONCEPT_FUNCTION_IMPL(CheckReachability, 0) 
    int res = Invoke(INVOKE_CHECK_REACHABILITY, PARAMETERS->PIF);
    RETURN_NUMBER(res); 
END_IMPL

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RE_create, 2, 3)
    T_STRING(RE_create, 0)
    T_NUMBER(RE_create, 1)

    const char *errorp = NULL;
    Reprog *reg = regcomp(PARAM(0), PARAM_INT(1), &errorp);
    if (PARAMETERS->COUNT > 2) {
        if (errorp) {
            SET_STRING(2, errorp);
        } else {
            SET_STRING(2, "");
        }
    }
    RETURN_NUMBER((SYS_INT)reg);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_exec, 2) 
    T_HANDLE(RE_exec, 0)
    T_STRING(RE_exec, 1)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    Resub sub;
    memset(&sub, 0, sizeof(Resub));
    int res = regexec(reg, PARAM(1), &sub, 0);
    if (!res) {
        const char *sp = sub.sub[0].sp;
        const char *ep = sub.sub[0].ep;
        if ((sp) && (ep) && (sp != ep)) {
            uintptr_t delta = ep - sp;
            RETURN_BUFFER(sp, delta);
        } else {
            RETURN_STRING("")
        }
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_test, 2) 
    T_HANDLE(RE_exec, 0)
    T_STRING(RE_exec, 1)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    Resub sub;
    memset(&sub, 0, sizeof(Resub));
    int res = regexec(reg, PARAM(1), &sub, 0);
    RETURN_NUMBER(!res);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_done, 1)
    T_NUMBER(RE_done, 0)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    if (reg) {
        regfree(reg);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
// ==================================================== //
// BUILT-IN INITIALIZATION (CONSTANTS, etc.)            //
// ==================================================== //

void BUILTININIT(void *pif) {
    PIFAlizator *PIF = (PIFAlizator *)pif;
    if (!PIF)
        return;
    PIF->DefineConstant("_GB", "GLOBALS()", 0);
    PIF->DefineConstant("_argv", "CLArg()", 0);
}

void *BUILTINADDR(const char *name) {
    if ((!name) || (!name[0]))
        return NULL;

    BUILTIN(GLOBALS);
    BUILTIN(CLArg);
    BUILTIN(CheckReachability);
    BUILTIN(___CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID);
    BUILTIN(RE_create);
    BUILTIN(RE_exec);
    BUILTIN(RE_test);
    BUILTIN(RE_done);

    return NULL;
}

void BUILTINDONE() {
    // nothing
}