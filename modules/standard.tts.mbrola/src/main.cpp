//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <windows.h>
#include "mbrola.h"
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    if (!load_MBR())
        return (void *)"Can't load mbrola.dll";
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    unload_MBR();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(InitMBR, 1)
    T_STRING(0)

    RETURN_NUMBER(init_MBR(PARAM(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CloseMBR, 0)
    close_MBR();
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ResetMBR, 0)
    reset_MBR();
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(WriteMBR, 1)
    T_STRING(0)

    RETURN_NUMBER(write_MBR(PARAM(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ReadMBR, 2)
    T_NUMBER(1)
    SET_STRING(0, "")

    int len = PARAM_INT(1);

    short *buf = new short[len + 1];
    buf[len] = 0;

    int res = read_MBR(buf, len);
    if (res > 0) {
        res *= sizeof(short);
        SET_BUFFER(0, (char *)buf, res);
    }
    delete[] buf;
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SetFreqMBR, 1)
    T_NUMBER(0)

    setFreq_MBR(PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GetFreqMBR, 0)
    RETURN_NUMBER(getFreq_MBR())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SetVolumeRatioMBR, 1)
    T_NUMBER(0)

    setVolumeRatio_MBR(PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(GetVolumeRatioMBR, 0)
    RETURN_NUMBER(getVolumeRatio_MBR())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(FlushMBR, 0)
    RETURN_NUMBER(flush_MBR())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(LastErrorStrMBR, 0)
    char bufferin[261];
    bufferin[260] = 0;

    lastErrorStr_MBR(bufferin, 260);
    RETURN_STRING(bufferin)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(LastErrorMBR, 0)
    RETURN_NUMBER(lastError_MBR())
END_IMPL
