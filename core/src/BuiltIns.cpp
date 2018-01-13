#include "BuiltIns.h"
#include "PIFAlizator.h"
#include "ModuleLink.h"
#include "BuiltInsHelpers.h"
#include <stdlib.h>
#include <string.h>

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

    return NULL;
}

void BUILTINDONE() {
    // nothing
}