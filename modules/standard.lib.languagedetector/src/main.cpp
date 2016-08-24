//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <compact_lang_det.h>
#include <ext_lang_enc.h>
#include <lang_enc.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DetectLanguage, 1, 3)
    T_STRING(DetectLanguage, 0)

    bool is_plain_text = true;
    bool       do_allow_extended_languages = true;
    bool       do_pick_summary_language    = false;
    bool       do_remove_weak_matches      = false;
    bool       is_reliable;
    Language   plus_one      = UNKNOWN_LANGUAGE;
    const char *tld_hint     = NULL;
    int        encoding_hint = UNKNOWN_ENCODING;
    Language   language_hint = UNKNOWN_LANGUAGE;

    double   normalized_score3[3];
    Language language3[3];
    int      percent3[3];
    int      text_bytes;

    Language lang;
    lang = CompactLangDet::DetectLanguage(0,
                                          PARAM(0), PARAM_LEN(0),
                                          is_plain_text,
                                          do_allow_extended_languages,
                                          do_pick_summary_language,
                                          do_remove_weak_matches,
                                          tld_hint,
                                          encoding_hint,
                                          language_hint,
                                          language3,
                                          percent3,
                                          normalized_score3,
                                          &text_bytes,
                                          &is_reliable);

    const char *s = LanguageName(lang);
    if (PARAMETERS_COUNT > 1) {
        CREATE_ARRAY(PARAMETER(1));
        if (language3[0] != UNKNOWN_LANGUAGE) {
            const char *s1 = LanguageName(language3[0]);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), (INTEGER)0, (INTEGER)VARIABLE_STRING, s1, (double)0);
        }
        if (language3[1] != UNKNOWN_LANGUAGE) {
            const char *s1 = LanguageName(language3[1]);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), (INTEGER)1, (INTEGER)VARIABLE_STRING, s1, (double)0);
        }
        if (language3[2] != UNKNOWN_LANGUAGE) {
            const char *s1 = LanguageName(language3[2]);
            Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(1), (INTEGER)2, (INTEGER)VARIABLE_STRING, s1, (double)0);
        }
    }
    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, (NUMBER)(int)is_reliable);
    }
    if (s) {
        RETURN_STRING(s);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//

