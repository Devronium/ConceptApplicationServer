//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "libfvad/include/fvad.h"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_new, 0)
    Fvad *vad = fvad_new();

    RETURN_NUMBER((SYS_INT)vad);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_set_sample_rate, 2)
    T_HANDLE(fvad_set_sample_rate, 0);
    T_NUMBER(fvad_set_sample_rate, 1);

    Fvad *vad = (Fvad *)(SYS_INT)PARAM(0);

    int err = fvad_set_sample_rate(vad, PARAM_INT(1));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_reset, 1)
    T_HANDLE(fvad_reset, 0);

    Fvad *vad = (Fvad *)(SYS_INT)PARAM(0);

    fvad_reset(vad);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_set_mode, 2)
    T_HANDLE(fvad_set_mode, 0);
    T_NUMBER(fvad_set_mode, 1);

    Fvad *vad = (Fvad *)(SYS_INT)PARAM(0);

    int err = fvad_set_mode(vad, PARAM_INT(1));

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_process, 2)
    T_HANDLE(fvad_process, 0);
    T_STRING(fvad_process, 1);

    Fvad *vad = (Fvad *)(SYS_INT)PARAM(0);

    int len = PARAM_LEN(1) / 2;
    int err = -1;

    if (len > 0)
        err = fvad_process(vad, (const int16_t *)PARAM(1), len);

    RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(fvad_free, 1)
    T_HANDLE(fvad_free, 0);

    Fvad *vad = (Fvad *)(SYS_INT)PARAM(0);
    SET_NUMBER(0, 0);

    fvad_free(vad);
END_IMPL
//=====================================================================================//
