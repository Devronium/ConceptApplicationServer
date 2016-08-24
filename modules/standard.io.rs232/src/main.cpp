//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include "rs232.h"

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(OpenComport, 2, 3)
    T_NUMBER(OpenComport, 0)
    T_NUMBER(OpenComport, 1)
    int bits = 8;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(OpenComport, 2)
        bits = PARAM_INT(2);
        if ((bits != 7) && (bits != 8)) {
            return (void *)"OpenComport: parameter 3 (bits) must be 7 or 8";
        }
    }

    RETURN_NUMBER(OpenComport(PARAM_INT(0), PARAM_INT(1), bits));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PollComport, 3)
    T_NUMBER(PollComport, 0)
    T_NUMBER(PollComport, 2)

    int len = PARAM_INT(2);
    unsigned char *buf = (unsigned char *)malloc(len + 1);
    buf[len] = 0;
    int res = PollComport(PARAM_INT(0), buf, len);
    SET_STRING(1, "");
    if (res > 0) {
        SET_BUFFER(1, (char *)buf, res);
    }
    free(buf);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SendComport, 2)
    T_NUMBER(SendComport, 0)
    T_STRING(SendComport, 1)

    RETURN_NUMBER(SendBuf(PARAM_INT(0), (unsigned char *)PARAM(1), PARAM_LEN(1)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CloseComport, 1)
    T_NUMBER(CloseComport, 0)
    CloseComport(PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

