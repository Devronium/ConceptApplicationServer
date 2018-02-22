//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <codec2.h>
#include <string.h>
#include <stdlib.h>
//-------------------------//
// Local variables         //
//-------------------------//
static INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    /* DEFINE_SCONSTANT("speex_nb_mode", "0")
    DEFINE_SCONSTANT("speex_wb_mode", "1")
    DEFINE_SCONSTANT("speex_uwb_mode", "2")*/
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_create, 1)
    T_NUMBER(codec2_create, 0)
    int mode;
    switch (PARAM_INT(0)) {
        case 3200:
            mode = CODEC2_MODE_3200;
            break;
        case 2400:
            mode = CODEC2_MODE_2400;
            break;
        case 1600:
            mode = CODEC2_MODE_1600;
            break;
        case 1400:
            mode = CODEC2_MODE_1400;
            break;
        case 1300:
            mode = CODEC2_MODE_1300;
            break;
        case 1200:
            mode = CODEC2_MODE_1200;
            break;
        case 700:
            mode = CODEC2_MODE_700;
            break;
        case 701:
            mode = CODEC2_MODE_700B;
            break;
        case 702:
            mode = CODEC2_MODE_700C;
            break;
        default:
            RETURN_NUMBER(0);
            return 0;
    }
    CODEC2 *codec2 = codec2_create(mode);
    codec2_set_natural_or_gray(codec2, 1);
    RETURN_NUMBER((SYS_INT)codec2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_destroy, 1)
    T_HANDLE(codec2_destroy, 0)
    CODEC2 *codec2 = (CODEC2 *)(SYS_INT)PARAM(0);
    codec2_destroy(codec2);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_samples_per_frame, 1)
    T_HANDLE(codec2_samples_per_frame, 0)
    CODEC2 *codec2 = (CODEC2 *)(SYS_INT)PARAM(0);

    int nsam = codec2_samples_per_frame(codec2);
    RETURN_NUMBER(nsam);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_bits_per_frame, 1)
    T_HANDLE(codec2_bits_per_frame, 0)
    CODEC2 *codec2 = (CODEC2 *)(SYS_INT)PARAM(0);

    int nbits = codec2_bits_per_frame(codec2);
    RETURN_NUMBER(nbits);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_encode, 2)
    T_HANDLE(codec2_encode, 0)
    T_STRING(codec2_encode, 1)
    CODEC2 *codec2 = (CODEC2 *)(SYS_INT)PARAM(0);

    int nsam = codec2_samples_per_frame(codec2);
    int nbit = codec2_bits_per_frame(codec2);
    int nbyte = (nbit + 7) / 8;
    int len = PARAM_LEN(1) / 2;
    short *buf = (short *)PARAM(1);

    unsigned char *bits_buffer = (unsigned char*)malloc(nbyte * sizeof(char) * (len/nsam + 1));
    unsigned char *bits = bits_buffer;
    int encoded = 0;
    while (len >= nsam) {
        codec2_encode(codec2, bits, buf);
        buf += nsam;
        len -= nsam;
        bits += nbyte;
        encoded += nbyte;
    }
    if (encoded) {
        RETURN_BUFFER((char *)bits_buffer, encoded);
    } else {
        RETURN_STRING("");
    }
    free(bits_buffer);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(codec2_decode, 2)
    T_HANDLE(codec2_decode, 0)
    T_STRING(codec2_decode, 1)
    CODEC2 *codec2 = (CODEC2 *)(SYS_INT)PARAM(0);

    int nsam = codec2_samples_per_frame(codec2);
    int nbit = codec2_bits_per_frame(codec2);
    int nbyte = (nbit + 7) / 8;
    int len = PARAM_LEN(1);
    char *buf2 = NULL;
    int buf_size = nsam * sizeof(short) * (len/nbyte + 1);
    CORE_NEW(buf_size, buf2);
    short *buf = (short *)buf2;
    if (buf) {
        unsigned char *bits = (unsigned char *)PARAM(1);
        int decoded = 0;

        while (len >= nbyte) {
            codec2_decode(codec2, buf, bits);
            bits += nbyte;
            len -= nbyte;
            buf += nsam;
            decoded += nsam;
        }
        if (decoded) {
            buf2[decoded * sizeof(short)] = 0;
            SetVariable(RESULT, -1, buf2, decoded * sizeof(short));
        } else {
            RETURN_STRING("");
            CORE_DELETE(buf);
        }
    }
END_IMPL
//------------------------------------------------------------------------
