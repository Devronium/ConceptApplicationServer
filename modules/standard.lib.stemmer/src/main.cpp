//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
extern "C" {
    #include "libstemmer/include/libstemmer.h"
}

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(stem, 1, 3)
    T_STRING("stem", 0)
    const char *language = "english";
    const char *charenc = NULL;
    if  (PARAMETERS_COUNT > 1) {
        T_STRING("stem", 1);
        language = PARAM(1);
        if ((!language) || (!language[0]))
            language = "english";
    }
    if  (PARAMETERS_COUNT > 2) {
        T_STRING("stem", 2);
        charenc = PARAM(2);
        if ((!charenc) || (!charenc[0]))
            charenc = NULL;
    }
    struct sb_stemmer *stemmer = sb_stemmer_new(language, charenc);
    if (!stemmer) {
        RETURN_NUMBER(0);
        return 0;
    }
    const sb_symbol *stemmed = sb_stemmer_stem(stemmer, (sb_symbol *)PARAM(0), PARAM_LEN(0));
    if ((stemmed) && (stemmed[0])) {
        RETURN_STRING((char *)stemmed);
    } else {
        RETURN_STRING("");
    }
    sb_stemmer_delete(stemmer); 
END_IMPL
//=====================================================================================//
