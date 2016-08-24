//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>
#include <pocketsphinx.h>

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRConfInit, 1)
    T_ARRAY(VRConfInit, 0)

    cmd_ln_t * config = cmd_ln_init(NULL, ps_args(), TRUE, NULL);

    int len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    for (INTEGER i = 0; i < len; i++) {
        INTEGER type    = 0;
        char    *szData = 0;
        NUMBER  nData   = 0;
        char    *key    = 0;
        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(0), i, &type, &szData, &nData);
        if (type == VARIABLE_STRING) {
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(0), i, &key);
            if ((key) && (key[0]))
                cmd_ln_set_str_r(config, key, szData);
        } else
        if (type == VARIABLE_NUMBER) {
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(0), i, &key);
            if ((key) && (key[0]))
                cmd_ln_set_float_r(config, key, nData);
        }
    }

    RETURN_NUMBER((SYS_INT)config);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRConfDone, 1)
    T_NUMBER(VRConfDone, 0)

    cmd_ln_t * config = (cmd_ln_t *)(SYS_INT)PARAM(0);
    if (config) {
        cmd_ln_free_r(config);
        SET_NUMBER(0, 0)
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRInit, 1)
    T_HANDLE(VRInit, 0)
    cmd_ln_t * config = (cmd_ln_t *)(SYS_INT)PARAM(0);

    ps_decoder_t *ps = ps_init(config);
    RETURN_NUMBER((SYS_INT)ps);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRDone, 1)
    T_NUMBER(VRDone, 0)
    ps_decoder_t * ps = (ps_decoder_t *)(SYS_INT)PARAM(0);
    if (ps) {
        ps_free(ps);
        SET_NUMBER(0, 0)
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRAddBuffer, 2)
    T_HANDLE(VRAddBuffer, 0)
    T_STRING(VRAddBuffer, 1)

    ps_decoder_t * ps = (ps_decoder_t *)(SYS_INT)PARAM(0);
    int16 *buf    = (int16 *)PARAM(1);
    int   samples = PARAM_LEN(1) / 2;
    int   rv      = ps_process_raw(ps, buf, samples, FALSE, FALSE);
    RETURN_NUMBER(rv);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VRStart, 1)
    T_HANDLE(VRStart, 0)

    ps_decoder_t * ps = (ps_decoder_t *)(SYS_INT)PARAM(0);
    int rv = ps_start_utt(ps);
    RETURN_NUMBER(rv);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(VREnd, 1)
    T_HANDLE(VREnd, 0)

    ps_decoder_t * ps = (ps_decoder_t *)(SYS_INT)PARAM(0);
    int rv = ps_end_utt(ps);
    RETURN_NUMBER(rv);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(VRResult, 1, 2)
    T_HANDLE(VRResult, 0)

    int32 score = 0;
    ps_decoder_t *ps = (ps_decoder_t *)(SYS_INT)PARAM(0);

    char const *hyp = ps_get_hyp(ps, &score);
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, score);
    }
    if (hyp) {
        RETURN_STRING(hyp);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//

