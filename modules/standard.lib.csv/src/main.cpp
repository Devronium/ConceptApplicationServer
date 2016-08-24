//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <string.h>
#include <stdio.h>
extern "C" {
#include "csv.h"
}

struct CSVContainer {
    void *RESULT;
    void *ARR;
    int  index;
    int  masterindex;
};
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(CSV_TAB)
    DEFINE_ECONSTANT(CSV_SPACE)
    DEFINE_ECONSTANT(CSV_CR)
    DEFINE_ECONSTANT(CSV_LF)
    DEFINE_ECONSTANT(CSV_COMMA)
    DEFINE_ECONSTANT(CSV_QUOTE)

    DEFINE_ECONSTANT(CSV_SUCCESS)
    DEFINE_ECONSTANT(CSV_EPARSE)
    DEFINE_ECONSTANT(CSV_ENOMEM)
    DEFINE_ECONSTANT(CSV_ETOOBIG)
    DEFINE_ECONSTANT(CSV_EINVALID)

    DEFINE_ECONSTANT(CSV_STRICT)
    DEFINE_ECONSTANT(CSV_REPALL_NL)
    DEFINE_ECONSTANT(CSV_STRICT_FINI)
    DEFINE_ECONSTANT(CSV_APPEND_NULL)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(CSVCreate, 0, 1)
    int flags = 0;
    if (PARAMETERS_COUNT) {
        T_NUMBER(CSVCreate, 0)
        flags = PARAM_INT(0);
    }

    struct csv_parser *cp = new csv_parser;
    csv_init(cp, flags);
    RETURN_NUMBER((SYS_INT)cp)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CSVDone, 1)
    T_NUMBER(CSVDone, 0)
    struct csv_parser *cp = (struct csv_parser *)PARAM_INT(0);
    if (cp) {
        csv_free(cp);
        delete cp;
    }
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
void cb1(void *s, size_t i, void *p) {
    CSVContainer *cvc = (CSVContainer *)p;

    if (cvc) {
        if (!cvc->ARR) {
            InvokePtr(INVOKE_ARRAY_VARIABLE, cvc->RESULT, (INTEGER)cvc->masterindex++, &cvc->ARR);
            if (!cvc->ARR)
                return;
            InvokePtr(INVOKE_CREATE_ARRAY, cvc->ARR);
        }
        int index = cvc->index++;
        if ((i > 0) && (s))
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT, cvc->ARR, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)i);
        else
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT, cvc->ARR, (INTEGER)index, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
    }
}

void cb2(int c, void *p) {
    CSVContainer *cvc = (CSVContainer *)p;

    if (cvc) {
        cvc->ARR   = 0;
        cvc->index = 0;
    }
}

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(CSVParse, 2, 3)
    T_HANDLE(CSVParse, 0)
    T_STRING(CSVParse, 1)
    int last_chunk = 0;
    if (PARAMETERS_COUNT == 3) {
        T_NUMBER(CSVParse, 2);
        last_chunk = PARAM_INT(2);
    }
    struct csv_parser *cp = (struct csv_parser *)PARAM_INT(0);
    CSVContainer      cvc = { RESULT, 0, 0, 0 };
    CREATE_ARRAY(RESULT);
    csv_parse(cp, PARAM(1), PARAM_LEN(1), cb1, cb2, &cvc);
    if (last_chunk)
        csv_fini(cp, cb1, cb2, &cvc);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CSVError, 1)
    T_HANDLE(CSVError, 0)
    struct csv_parser *cp = (struct csv_parser *)PARAM_INT(0);

    RETURN_NUMBER(csv_error(cp))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CSVErrorExplain, 1)
    T_NUMBER(CSVErrorExplain, 0)
    RETURN_STRING(csv_strerror(PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CSVSetDelim, 2)
    T_HANDLE(CSVSetDelim, 0)
    T_STRING(CSVSetDelim, 1)
    struct csv_parser *cp = (struct csv_parser *)PARAM_INT(0);
    char              *s  = PARAM(1);
    char              c   = 0;
    if (s) {
        c = s[0];
        csv_set_delim(cp, c);
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CSVSetQuote, 2)
    T_HANDLE(CSVSetQuote, 0)
    T_STRING(CSVSetQuote, 1)
    struct csv_parser *cp = (struct csv_parser *)PARAM_INT(0);
    char              *s  = PARAM(1);
    char              c   = 0;
    if (s) {
        c = s[0];
        csv_set_quote(cp, c);
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//

