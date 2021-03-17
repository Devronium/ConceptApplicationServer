//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <iconv.h>
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

#ifdef WITH_LIBICONV
    DEFINE_ECONSTANT(ICONV_TRIVIALP)
    DEFINE_ECONSTANT(ICONV_GET_TRANSLITERATE)
    DEFINE_ECONSTANT(ICONV_SET_TRANSLITERATE)
    DEFINE_ECONSTANT(ICONV_GET_DISCARD_ILSEQ)
    DEFINE_ECONSTANT(ICONV_SET_DISCARD_ILSEQ)
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(iconv_open, 2)
    T_STRING(iconv_open, 0)
    T_STRING(iconv_open, 1)

    RETURN_NUMBER((long)iconv_open((const char *)PARAM(0), (const char *)PARAM(1)));

END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(iconv2, 3, 4)
//CONCEPT_FUNCTION_IMPL(iconv2,3)
    T_STRING(iconv2, 0)
    T_STRING(iconv2, 1)
    T_STRING(iconv2, 2)

    iconv_t cd = iconv_open((const char *)PARAM(0), (const char *)PARAM(1));
    if (cd == (iconv_t)(-1)) {
        RETURN_STRING("");
        return 0;
    }
    size_t len          = PARAM_LEN(2) * 3 + 1;
    char   *out         = new char [len];
    char   *initial_out = out;
    size_t initial_len  = len;
    out[0] = 0;
    char   *in    = PARAM(2);
    size_t len_in = PARAM_LEN(2);
#ifdef WITH_LIBICONV
    int val = 1;
    iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, &val);
#endif
    if (PARAMETERS_COUNT == 4) {
        T_NUMBER(iconv2, 3)
#ifdef WITH_LIBICONV
        val = PARAM_INT(3);
        iconvctl(cd, ICONV_SET_TRANSLITERATE, &val);
#endif
    }
    int result = iconv(cd, (char **)&in, &len_in, &out, &len);
    len = initial_len - len;
    if ((result >= 0) && (len > 0)) {
        RETURN_BUFFER(initial_out, len);
    } else {
        RETURN_STRING("");
    }
    delete[] initial_out;
    iconv_close(cd);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(iconv, 3)
    T_NUMBER(iconv, 0)
    T_STRING(iconv, 1)

    iconv_t cd = (iconv_t)PARAM_INT(0);
    if (cd == (iconv_t)(-1)) {
        RETURN_NUMBER(-1);
        return 0;
    }

    size_t len          = PARAM_LEN(1) * 3 + 1;
    char   *out         = new char [len];
    char   *initial_out = out;
    size_t initial_len  = len;
    out[0] = 0;
    char   *in    = PARAM(1);
    size_t len_in = PARAM_LEN(1);

    int result = iconv(cd, (char **)&in, &len_in, &out, &len);
    len = initial_len - len;
    if ((result >= 0) && (len > 0)) {
        SET_BUFFER(2, initial_out, len);
    } else {
        SET_STRING(2, "");
    }

    delete[] out;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(iconv_close, 1)
    T_NUMBER(iconv_close, 0)

    RETURN_NUMBER((long)iconv_close((iconv_t)PARAM_INT(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(iconvctl, 3)
    T_NUMBER(iconvctl, 0)
    T_NUMBER(iconvctl, 1)
    int result = -1;

    iconv_t cd = (iconv_t)PARAM_INT(0);
    if ((cd == (iconv_t)(-1)) || (!cd)) {
        RETURN_NUMBER(-1);
        return 0;
    }
#ifdef WITH_LIBICONV
    switch (PARAM_INT(1)) {
        case ICONV_TRIVIALP:
        case ICONV_GET_TRANSLITERATE:
        case ICONV_GET_DISCARD_ILSEQ:
            {
                int arg = 0;
                result = iconvctl(cd, PARAM_INT(1), &arg);
                SET_NUMBER(2, arg);
            }
            break;

        case ICONV_SET_TRANSLITERATE:
        case ICONV_SET_DISCARD_ILSEQ:
            {
                T_NUMBER(iconvctl, 2)

                int arg = (const int)PARAM_INT(2);
                result  = iconvctl(cd, PARAM_INT(1), &arg);
            }
            break;
    }
#endif
    RETURN_NUMBER(result);
END_IMPL
//-----------------------------------------------------//

