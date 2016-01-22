//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <pcre.h>

#define OV_SIZE    6000
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    AnsiString value = PCRE_CASELESS;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_CASELESS", value.c_str());
    value = PCRE_MULTILINE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_MULTILINE", value.c_str());
    value = PCRE_DOTALL;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_DOTALL", value.c_str());
    value = PCRE_EXTENDED;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_EXTENDED", value.c_str());
    value = PCRE_ANCHORED;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_ANCHORED", value.c_str());
    value = PCRE_DOLLAR_ENDONLY;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_DOLLAR_ENDONLY", value.c_str());
    value = PCRE_EXTRA;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_EXTRA", value.c_str());
    value = PCRE_NOTBOL;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_NOTBOL", value.c_str());
    value = PCRE_NOTEOL;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_NOTEOL", value.c_str());
    value = PCRE_UNGREEDY;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_UNGREEDY", value.c_str());
    value = PCRE_NOTEMPTY;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_NOTEMPTY", value.c_str());
    value = PCRE_UTF8;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_UTF8", value.c_str());
    value = PCRE_NO_AUTO_CAPTURE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_NO_AUTO_CAPTURE", value.c_str());
    value = PCRE_NO_UTF8_CHECK;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_NO_UTF8_CHECK", value.c_str());
    value = PCRE_AUTO_CALLOUT;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_AUTO_CALLOUT", value.c_str());
    value = PCRE_PARTIAL;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_PARTIAL", value.c_str());
    value = PCRE_DFA_SHORTEST;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_DFA_SHORTEST", value.c_str());
    value = PCRE_DFA_RESTART;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_DFA_RESTART", value.c_str());
    value = PCRE_FIRSTLINE;
    Invoke(INVOKE_DEFINE_CONSTANT, HANDLER, "PCRE_FIRSTLINE", value.c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_preg_match CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "preg_match takes 2 or 3 parameters : string_to_parse, pattern[, options]");

    LOCAL_INIT;

    char   *str;
    char   *exp;
    NUMBER slen;
    NUMBER elen;
    NUMBER options = 0;

    GET_CHECK_BUFFER(0, str, slen, "preg_match : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_BUFFER(1, exp, elen, "preg_match : parameter 1 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_NUMBER(2, options, "preg_match : parameter 2 should be a number (STATIC_NUMBER)");
    }


    pcre       *re         = 0;
    const char *error      = 0;
    int        erroroffset = 0;

    re = pcre_compile(exp, (int)options, &error, &erroroffset, 0);
    if (!re) {
        RETURN_STRING((char *)error);
        return 0;
    }

    int ovector[OV_SIZE];

    int retval = pcre_exec(re, 0, str, (int)slen, 0, PCRE_NOTEMPTY, ovector, OV_SIZE);
    pcre_free(re);

    if (retval == -1)
        retval = 0;

    RETURN_NUMBER(retval);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_preg CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(2, 3, "preg takes 2 or 3 parameters : string_to_parse, pattern[, options]");

    LOCAL_INIT;

    char   *str;
    char   *exp;
    NUMBER slen;
    NUMBER elen;
    NUMBER options = 0;

    GET_CHECK_BUFFER(0, str, slen, "preg : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_BUFFER(1, exp, elen, "preg : parameter 1 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT == 3) {
        GET_CHECK_NUMBER(2, options, "preg : parameter 2 should be a number (STATIC_NUMBER)");
    }


    pcre       *re         = 0;
    const char *error      = 0;
    int        erroroffset = 0;

    re = pcre_compile(exp, (int)options, &error, &erroroffset, 0);
    if (!re) {
        RETURN_STRING((char *)error);
        return 0;
    }

    int ovector[OV_SIZE];

    int retval = pcre_exec(re, 0, str, (int)slen, 0, PCRE_NOTEMPTY, ovector, OV_SIZE);

    if (retval == -1)
        retval = 0;


    // pro stuff :)
    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"preg : Failed to INVOKE_CREATE_ARRAY";

    if (retval > 0) {
        const char **stringlist = 0;
        if (pcre_get_substring_list(str, ovector, retval, &stringlist) < 0)
            return 0;

        for (INTEGER i = 0; i < retval; i++) {
            if (stringlist[i])
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, stringlist[i], (double)0);
            else
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, "", (double)0);
        }
        pcre_free_substring_list(stringlist);
    }
    pcre_free(re);
    return 0;
}
//---------------------------------------------------------------------------
