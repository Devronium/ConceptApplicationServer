//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include "regexp.h"
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_regex CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "regex takes 2 parameters : string_to_parse, pattern");

    LOCAL_INIT;

    char *str;
    char *exp;

    GET_CHECK_STRING(0, str, "regex : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, exp, "regex : parameter 1 should be a string (STATIC STRING)");

    regexp *compiled;

    int retval = re_comp(&compiled, exp);

    if (retval < 0) {
        RETURN_NUMBER(0);
        //return (void *)"regex : error compiling regular expression";
    }
    int n_matches = re_nsubexp(compiled);

    regmatch *matches = new regmatch[n_matches];

    retval = re_exec(compiled, str, n_matches, &matches[0]);

    re_free(compiled);

    if (retval < 0) {
        delete[] matches;
        RETURN_NUMBER(0);
        return 0;
    }

    // pro stuff :)
    if (!IS_OK(Invoke(INVOKE_CREATE_ARRAY, RESULT)))
        return (void *)"regex : Failed to INVOKE_CREATE_ARRAY";

    for (INTEGER i = 0; i < n_matches; i++) {
        char *ptr    = str + matches[i].begin;
        int  ptr_len = (str + matches[i].end) - ptr;
        if (matches[i].begin == matches[i].end)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, "", (double)0);
        else
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, i, (INTEGER)VARIABLE_STRING, ptr, (double)ptr_len);
    }

    delete[] matches;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_regex_match CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "regex_match takes 2 parameters : string_to_parse, pattern");

    LOCAL_INIT;

    char *str;
    char *exp;

    GET_CHECK_STRING(0, str, "regex_match : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, exp, "regex_match : parameter 1 should be a string (STATIC STRING)");

    regexp *compiled;

    int retval = re_comp(&compiled, exp);

    if (retval < 0) {
        RETURN_NUMBER(0);
        //return (void *)"regex_match : error compiling regular expression";
    }

    int n_matches = re_nsubexp(compiled);

    regmatch *matches = new regmatch[n_matches];

    retval = re_exec(compiled, str, n_matches, &matches[0]);

    re_free(compiled);

    if (retval < 0) {
        delete[] matches;
        RETURN_NUMBER(0);
        return 0;
    }

    INTEGER n_mat = 0;
    for (INTEGER i = 0; i < n_matches; i++) {
        char *ptr    = str + matches[i].begin;
        int  ptr_len = (str + matches[i].end) - ptr;
        if (ptr_len)
            n_mat++;
    }
    delete[] matches;
    RETURN_NUMBER(n_mat);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_regex_replace CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "regex_replace takes 3 parameters : string_to_parse, replace_string, pattern");

    LOCAL_INIT;

    char *str;
    char *rep;
    char *exp;

    GET_CHECK_STRING(0, str, "regex_replace : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, rep, "regex_replace : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, exp, "regex_replace : parameter 2 should be a string (STATIC STRING)");

    regexp *compiled;

    int retval = re_comp(&compiled, exp);

    if (retval < 0)
        return (void *)"regex : error compiling regular expression";

    int n_matches = re_nsubexp(compiled);

    regmatch *matches = new regmatch[n_matches];

    retval = re_exec(compiled, str, n_matches, &matches[0]);

    re_free(compiled);

    if (retval < 0) {
        delete[] matches;
        RETURN_NUMBER(0);
        return 0;
    }


    AnsiString replace;
    int        last_match = 0;

    for (INTEGER i = 0; i < n_matches; i++) {
        char *ptr    = str + last_match;
        int  ptr_len = (str + matches[i].begin) - ptr;

        if ((matches[i].begin != matches[i].end) && (ptr_len > 0)) {
            last_match = matches[i].end;
            AnsiString temp;
            temp.LoadBuffer(ptr, ptr_len);
            replace += temp;
            replace += rep;
        }
    }

    int len = strlen(str) - last_match;
    if (len) {
        char *ptr    = str + last_match;
        int  ptr_len = strlen(ptr);
        replace += ptr;
    }

    delete[] matches;
    RETURN_STRING(replace);
    return 0;
}
//---------------------------------------------------------------------------
