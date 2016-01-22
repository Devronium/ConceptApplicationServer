//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>

#include "DtmfGenerator.hpp"
#include "DtmfDetector.hpp"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DTMFGeneratorInit, 0, 3)
    DtmfGenerator * gen = NULL;
    int frame_size     = 160;
    int duration_push  = 70;
    int duration_pause = 50;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(0)
        frame_size = PARAM_INT(0);
    }
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        duration_push = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(2)
        duration_pause = PARAM_INT(2);
    }
    gen = new DtmfGenerator(frame_size, duration_push, duration_pause);
    RETURN_NUMBER((SYS_INT)gen);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFGeneratorDone, 1)
    T_NUMBER(0)
    DtmfGenerator * gen = (DtmfGenerator *)(SYS_INT)PARAM(0);
    if (gen)
        delete gen;
    SET_NUMBER(0, 0)
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFGeneratorGenerate, 1)
    T_HANDLE(0)

    DtmfGenerator * gen = (DtmfGenerator *)(SYS_INT)PARAM(0);
    INT16 *buf = (INT16 *)malloc(gen->sizeOfFrame * sizeof(INT16));
    if (buf) {
        gen->dtmfGenerating(buf);
        RETURN_BUFFER((char *)buf, gen->sizeOfFrame * sizeof(INT16));
        free(buf);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFGeneratorTransmit, 2)
    T_HANDLE(0)
    T_STRING(1)
    DtmfGenerator * gen = (DtmfGenerator *)(SYS_INT)PARAM(0);
    gen->transmitNewDialButtonsArray(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(1)
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFGeneratorReady, 1)
    T_HANDLE(0)

    DtmfGenerator * gen = (DtmfGenerator *)(SYS_INT)PARAM(0);
    int res = gen->getReadyFlag();
    RETURN_NUMBER(res);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFGeneratorReset, 1)
    T_HANDLE(0)

    DtmfGenerator * gen = (DtmfGenerator *)(SYS_INT)PARAM(0);
    gen->dtmfGeneratorReset();
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(DTMFDetectorInit, 0, 1)
    int frame_size = 160;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(0)
        frame_size = PARAM_INT(0);
    }
    DtmfDetector *det = new DtmfDetector(frame_size);
    RETURN_NUMBER((SYS_INT)det);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFDetectorDone, 1)
    T_NUMBER(0)
    DtmfDetector * det = (DtmfDetector *)(SYS_INT)PARAM(0);
    if (det)
        delete det;
    SET_NUMBER(0, 0)
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFDetectorReset, 1)
    T_HANDLE(0)

    DtmfDetector * det = (DtmfDetector *)(SYS_INT)PARAM(0);
    det->zerosIndexDialButton();
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFDetectorDetect, 2)
    T_HANDLE(0)
    T_STRING(1)

    DtmfDetector * det = (DtmfDetector *)(SYS_INT)PARAM(0);
    if ((PARAM_LEN(1)) / 2 >= det->frameSize) {
        det->dtmfDetecting((INT16 *)PARAM(1));
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFDetectorButtons, 1)
    T_HANDLE(0)

    DtmfDetector * det = (DtmfDetector *)(SYS_INT)PARAM(0);
    char *s  = det->getDialButtonsArray();
    int  len = det->getIndexDialButtons();
    if (len > 0) {
        RETURN_BUFFER(s, len);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(DTMFDetectorThreshold, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    DtmfDetector * det  = (DtmfDetector *)(SYS_INT)PARAM(0);
    det->powerThreshold = PARAM_INT(1);
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
