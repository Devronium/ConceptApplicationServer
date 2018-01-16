#include "BuiltIns.h"
#include "PIFAlizator.h"
#include "ModuleLink.h"
#include "BuiltInsHelpers.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
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
    Reprog *reg = JS_regcomp(PARAM(0), PARAM_INT(1), &errorp);
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
    int res = JS_regexec(reg, PARAM(1), &sub, 0);
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
    int res = JS_regexec(reg, PARAM(1), &sub, 0);
    RETURN_NUMBER(!res);
END_IMPL

CONCEPT_FUNCTION_IMPL(RE_done, 1)
    T_NUMBER(RE_done, 0)
    Reprog *reg = (Reprog *)(SYS_INT)PARAM(0);
    if (reg) {
        JS_regfree(reg);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL

WRAP_FUNCTION(Math, abs)
WRAP_FUNCTION(Math, acos)
WRAP_FUNCTION(Math, acosh)
WRAP_FUNCTION(Math, asin)
WRAP_FUNCTION(Math, asinh)
WRAP_FUNCTION(Math, atan)
WRAP_FUNCTION(Math, atanh)
WRAP_FUNCTION2(Math, atan2)
WRAP_FUNCTION(Math, cbrt)
WRAP_FUNCTION(Math, ceil)
WRAP_FUNCTION(Math, cos)
WRAP_FUNCTION(Math, cosh)
WRAP_FUNCTION(Math, exp)
WRAP_FUNCTION(Math, expm1)
WRAP_FUNCTION(Math, floor)
WRAP_FUNCTION2(Math, hypot)
WRAP_FUNCTION(Math, log)
WRAP_FUNCTION(Math, log1p)
WRAP_FUNCTION(Math, log10)
WRAP_FUNCTION(Math, log2)
WRAP_FUNCTION2(Math, pow)
WRAP_FUNCTION(Math, round)
WRAP_FUNCTION(Math, sin)
WRAP_FUNCTION(Math, sinh)
WRAP_FUNCTION(Math, sqrt)
WRAP_FUNCTION(Math, tan)
WRAP_FUNCTION(Math, tanh)
WRAP_FUNCTION(Math, trunc)
WRAP_FUNCTION0(Math, rand)
WRAP_VOID_FUNCTION(Math, srand)
// ==================================================== //
// BUILT-IN INITIALIZATION (CONSTANTS, etc.)            //
// ==================================================== //


void BUILTININIT(void *pif) {
    PIFAlizator *PIF = (PIFAlizator *)pif;
    if (!PIF)
        return;
    PIF->DefineConstant("_GB", "GLOBALS()", 0);
    PIF->DefineConstant("_argv", "CLArg()", 0);

    DEFINE2(M_PI, "3.14159265358979323846");
    DEFINE2(M_E, "2.71828182845904523536");
    DEFINE2(M_LOG2E, "1.44269504088896340736");
    DEFINE2(M_LOG10E, "0.434294481903251827651");
    DEFINE2(M_LN2, "0.693147180559945309417");
    DEFINE2(M_LN10, "2.30258509299404568402");
    DEFINE2(M_PI_2, "1.57079632679489661923");
    DEFINE2(M_PI_4, " 0.785398163397448309616");
    DEFINE2(M_1_PI, " 0.318309886183790671538");
    DEFINE2(M_2_PI, "0.636619772367581343076");
    DEFINE2(M_2_SQRTPI, "1.12837916709551257390");
    DEFINE2(M_SQRT2, "1.41421356237309504880");
    DEFINE2(M_SQRT1_2, "0.707106781186547524401");
    DEFINE(DBL_EPSILON);
    DEFINE(DBL_MIN);
    DEFINE(DBL_MIN_EXP);
    DEFINE(DBL_MIN_10_EXP);
    DEFINE(DBL_MAX);
    DEFINE(DBL_MAX_EXP);
    DEFINE(DBL_MAX_10_EXP);
    DEFINE(DBL_DIG);
    DEFINE(DBL_MANT_DIG);

    srand((unsigned)time(NULL));
}

int BUILTINOBJECTS(void *pif, const char *classname) {
    PIFAlizator *PIF = (PIFAlizator *)pif;
    // check if privates are enabled (recursive)
    if ((!PIF) || (!classname) || (PIF->enable_private))
        return 0;

    BUILTINCLASS("RegExp", "class RegExp{private var h;RegExp(str,f=0,var e=null){this.h=RE_create(str,f,e);}test(str){return RE_test(this.h,str);}exec(str){return RE_exec(this.h,str);}finalize(){RE_done(this.h);}}");
    BUILTINCLASS("Math", "class Math{"
        DECLARE_WRAPPER(Math, abs)
        DECLARE_WRAPPER(Math, acos)
        DECLARE_WRAPPER(Math, acosh)
        DECLARE_WRAPPER(Math, asin)
        DECLARE_WRAPPER(Math, asinh)
        DECLARE_WRAPPER(Math, atan)
        DECLARE_WRAPPER(Math, atanh)
        DECLARE_WRAPPER2(Math, atan2)
        DECLARE_WRAPPER(Math, cbrt)
        DECLARE_WRAPPER(Math, ceil)
        DECLARE_WRAPPER(Math, cos)
        DECLARE_WRAPPER(Math, cosh)
        DECLARE_WRAPPER(Math, exp)
        DECLARE_WRAPPER(Math, expm1)
        DECLARE_WRAPPER(Math, floor)
        DECLARE_WRAPPER2(Math, hypot)
        DECLARE_WRAPPER(Math, log)
        DECLARE_WRAPPER(Math, log1p)
        DECLARE_WRAPPER(Math, log10)
        DECLARE_WRAPPER(Math, log2)
        DECLARE_WRAPPER(Math, pow)
        DECLARE_WRAPPER(Math, round)
        DECLARE_WRAPPER(Math, sin)
        DECLARE_WRAPPER(Math, sqrt)
        DECLARE_WRAPPER(Math, tan)
        DECLARE_WRAPPER(Math, tanh)
        DECLARE_WRAPPER(Math, trunc)

        DECLARE_WRAPPER0(Math, rand)
        DECLARE_WRAPPER_VOID(Math, srand)
    "}");
    return 0;
}

void *BUILTINADDR(void *pif, const char *name, unsigned char *is_private) {
    if ((!name) || (!name[0]))
        return NULL;

    PIFAlizator *PIF = (PIFAlizator *)pif;
    if (is_private)
        *is_private = 0;

    BUILTIN(GLOBALS);
    BUILTIN(CLArg);
    BUILTIN(CheckReachability);
    BUILTIN(___CONCEPT_INTERFACE_HELPER_GENERATE_UNIQUE_ID);

    // math
    BUILTIN(abs)
    BUILTIN(acos)
    BUILTIN(acosh)
    BUILTIN(asin)
    BUILTIN(asinh)
    BUILTIN(atan)
    BUILTIN(atanh)
    BUILTIN(atan2)
    BUILTIN(cbrt)
    BUILTIN(ceil)
    BUILTIN(cos)
    BUILTIN(cosh)
    BUILTIN(exp)
    BUILTIN(expm1)
    BUILTIN(floor)
    BUILTIN(hypot)
    BUILTIN(log)
    BUILTIN(log1p)
    BUILTIN(log10)
    BUILTIN(log2)
    BUILTIN(pow)
    BUILTIN(round)
    BUILTIN(sin)
    BUILTIN(sinh)
    BUILTIN(sqrt)
    BUILTIN(tan)
    BUILTIN(tanh)
    BUILTIN(trunc)
    BUILTIN(rand)
    BUILTIN(srand)

    if ((!PIF) || (PIF->enable_private)) {
        if (is_private)
            *is_private = 1;

        // regex
        BUILTIN(RE_create);
        BUILTIN(RE_exec);
        BUILTIN(RE_test);
        BUILTIN(RE_done);

        if (is_private)
            *is_private = 0;
    }

    return NULL;
}

void BUILTINDONE() {
    // nothing
}