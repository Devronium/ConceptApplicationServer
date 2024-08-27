//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <string.h>
extern "C" {
    #include "libstemmer/include/libstemmer.h"
}
#include "pointer_list.h"

DEFINE_LIST(stemmer_list);

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(stemmer_list);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        DEINIT_LIST(stemmer_list);
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(stemmer_new, 0, 2)
    const char *language = "english";
    const char *charenc = NULL;
    if  (PARAMETERS_COUNT > 0) {
        T_STRING("stemmer_new", 0);
        language = PARAM(0);
        if ((!language) || (!language[0]))
            language = "english";
    }
    if  (PARAMETERS_COUNT > 1) {
        T_STRING("stemmer_new", 1);
        charenc = PARAM(1);
        if ((!charenc) || (!charenc[0]))
            charenc = NULL;
    }

    struct sb_stemmer *stemmer = sb_stemmer_new(language, charenc);
    if (!stemmer) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(MAP_POINTER(stemmer_list, stemmer, PARAMETERS->HANDLER));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(stemmer_stem, 2)
    T_HANDLE("stemmer_stem", 0)
    T_STRING("stemmer_stem", 1)

    struct sb_stemmer *stemmer = (struct sb_stemmer *)GET_POINTER(stemmer_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (stemmer) {
        const sb_symbol *stemmed = sb_stemmer_stem(stemmer, (sb_symbol *)PARAM(1), PARAM_LEN(1));
        if ((stemmed) && (stemmed[0])) {
            RETURN_STRING((char *)stemmed);
        } else {
            RETURN_STRING("");
        }
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(stemmer_delete, 1)
    T_HANDLE("stemmer_delete", 0)

    struct sb_stemmer *stemmer = (struct sb_stemmer *)FREE_POINTER(stemmer_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (stemmer)
        sb_stemmer_delete(stemmer); 
    SET_NUMBER(0, 0); 
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(extractWords, 1, 2)
    T_STRING("extractWords", 0)

    const char *word_separators = " !?,.;:@#$\%&*()-+={}[]|\\/<>\"'@`^\t\r\n\b";
    if (PARAMETERS_COUNT > 1) {
        T_STRING("extractWords", 1)
        word_separators = PARAM(1);

        if ((!word_separators) || (!word_separators[0]))
            word_separators = " ";
    }

    CREATE_ARRAY(RESULT);
    INTEGER index = 0;

    char *str = PARAM(0);
    INTEGER len = PARAM_LEN(0);

    size_t word_len = strcspn(str, word_separators);
    while (len > 0) {
        if (word_len > 0)
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)index ++, (INTEGER)VARIABLE_STRING, str, (double)word_len);
        word_len ++;
        str += word_len;
        len -= word_len;

        if (len <= 0)
            break;

        word_len = strcspn(str, word_separators);
    }        
    // }
END_IMPL
//=====================================================================================//
