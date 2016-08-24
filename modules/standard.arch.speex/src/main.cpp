//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <speex/speex.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>
#include <string.h>
#include <stdlib.h>
//-------------------------//
// Local variables         //
//-------------------------//
#define MAX_IN_SAMPLES    640
extern "C" {
unsigned char linear2alaw(int pcm_val);
int alaw2linear(unsigned char a_val);
unsigned char linear2ulaw(int pcm_val);
int ulaw2linear(unsigned char u_val);
unsigned char alaw2ulaw(unsigned char aval);
unsigned char ulaw2alaw(unsigned char uval);
}

static inline int val_seg(int val) {
    int r = 1;

    val >>= 8;
    if (val & 0xf0) {
        val >>= 4;
        r    += 4;
    }
    if (val & 0x0c) {
        val >>= 2;
        r    += 2;
    }
    if (val & 0x02)
        r += 1;
    return r;
}

static unsigned char s16_to_alaw(int pcm_val) {
    int           mask;
    int           seg;
    unsigned char aval;

    if (pcm_val >= 0) {
        mask = 0xD5;
    } else {
        mask    = 0x55;
        pcm_val = -pcm_val;
        if (pcm_val > 0x7fff)
            pcm_val = 0x7fff;
    }

    if (pcm_val < 256)
        aval = pcm_val >> 4;
    else {
        seg  = val_seg(pcm_val);
        aval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0x0f);
    }
    return aval ^ mask;
}

static int alaw_to_s16(unsigned char a_val) {
    int t;
    int seg;

    a_val ^= 0x55;
    t      = a_val & 0x7f;
    if (t < 16)
        t = (t << 4) + 8;
    else {
        seg = (t >> 4) & 0x07;
        t   = ((t & 0x0f) << 4) + 0x108;
        t <<= seg - 1;
    }
    return (a_val & 0x80) ? t : -t;
}

INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    DEFINE_ECONSTANT(SPEEX_SET_ENH)
    DEFINE_ECONSTANT(SPEEX_GET_ENH)
    DEFINE_ECONSTANT(SPEEX_GET_FRAME_SIZE)
    DEFINE_ECONSTANT(SPEEX_SET_QUALITY)
    DEFINE_ECONSTANT(SPEEX_SET_MODE)
    DEFINE_ECONSTANT(SPEEX_GET_MODE)
    DEFINE_ECONSTANT(SPEEX_SET_LOW_MODE)
    DEFINE_ECONSTANT(SPEEX_GET_LOW_MODE)
    DEFINE_ECONSTANT(SPEEX_SET_HIGH_MODE)
    DEFINE_ECONSTANT(SPEEX_GET_HIGH_MODE)
    DEFINE_ECONSTANT(SPEEX_SET_VBR)
    DEFINE_ECONSTANT(SPEEX_GET_VBR)
    DEFINE_ECONSTANT(SPEEX_SET_VBR_QUALITY)
    DEFINE_ECONSTANT(SPEEX_GET_VBR_QUALITY)
    DEFINE_ECONSTANT(SPEEX_SET_COMPLEXITY)
    DEFINE_ECONSTANT(SPEEX_GET_COMPLEXITY)
    DEFINE_ECONSTANT(SPEEX_SET_BITRATE)
    DEFINE_ECONSTANT(SPEEX_GET_BITRATE)
    DEFINE_ECONSTANT(SPEEX_SET_HANDLER)
    DEFINE_ECONSTANT(SPEEX_SET_USER_HANDLER)
    DEFINE_ECONSTANT(SPEEX_SET_SAMPLING_RATE)
    DEFINE_ECONSTANT(SPEEX_GET_SAMPLING_RATE)
    DEFINE_ECONSTANT(SPEEX_RESET_STATE)
    DEFINE_ECONSTANT(SPEEX_GET_RELATIVE_QUALITY)
    DEFINE_ECONSTANT(SPEEX_SET_VAD)
    DEFINE_ECONSTANT(SPEEX_GET_VAD)
    DEFINE_ECONSTANT(SPEEX_SET_ABR)
    DEFINE_ECONSTANT(SPEEX_GET_ABR)
    DEFINE_ECONSTANT(SPEEX_SET_DTX)
    DEFINE_ECONSTANT(SPEEX_GET_DTX)
    DEFINE_ECONSTANT(SPEEX_SET_SUBMODE_ENCODING)
    DEFINE_ECONSTANT(SPEEX_GET_SUBMODE_ENCODING)
    DEFINE_ECONSTANT(SPEEX_GET_LOOKAHEAD)
    DEFINE_ECONSTANT(SPEEX_SET_PLC_TUNING)
    DEFINE_ECONSTANT(SPEEX_GET_PLC_TUNING)
    DEFINE_ECONSTANT(SPEEX_SET_VBR_MAX_BITRATE)
    DEFINE_ECONSTANT(SPEEX_GET_VBR_MAX_BITRATE)
    DEFINE_ECONSTANT(SPEEX_SET_HIGHPASS)
    DEFINE_ECONSTANT(SPEEX_GET_HIGHPASS)
    DEFINE_ECONSTANT(SPEEX_GET_ACTIVITY)
    DEFINE_ECONSTANT(SPEEX_SET_PF)
    DEFINE_ECONSTANT(SPEEX_GET_PF)
    DEFINE_ECONSTANT(SPEEX_MODE_FRAME_SIZE)
    DEFINE_ECONSTANT(SPEEX_SUBMODE_BITS_PER_FRAME)
    DEFINE_ECONSTANT(SPEEX_LIB_GET_MAJOR_VERSION)
    DEFINE_ECONSTANT(SPEEX_LIB_GET_MINOR_VERSION)
    DEFINE_ECONSTANT(SPEEX_LIB_GET_MICRO_VERSION)
    DEFINE_ECONSTANT(SPEEX_LIB_GET_EXTRA_VERSION)
    DEFINE_ECONSTANT(SPEEX_LIB_GET_VERSION_STRING)
    DEFINE_ECONSTANT(SPEEX_NB_MODES)
    DEFINE_ECONSTANT(SPEEX_MODEID_NB)
    DEFINE_ECONSTANT(SPEEX_MODEID_WB)
    DEFINE_ECONSTANT(SPEEX_MODEID_UWB)
    DEFINE_SCONSTANT("speex_nb_mode", "0")
    DEFINE_SCONSTANT("speex_wb_mode", "1")
    DEFINE_SCONSTANT("speex_uwb_mode", "2")
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
//------------------------------------------------------------------------
// void speex_bits_init(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_init, 1)
    SpeexBits * bits = (SpeexBits *)malloc(sizeof(SpeexBits));
    if (!bits)
        return (void *)"speex_bits_init: not enough memory";

    speex_bits_init(bits);
    SET_NUMBER(0, (SYS_INT)bits)
    RETURN_NUMBER((SYS_INT)bits)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_init_buffer(SpeexBits* bits, void* buff, int buf_size)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_init_buffer, 2)
    SpeexBits * bits = (SpeexBits *)malloc(sizeof(SpeexBits));
    if (!bits)
        return (void *)"speex_bits_init_buffer: not enough memory";

    T_STRING(speex_bits_init_buffer, 1) // void* buff
    void *buff = (void *)PARAM(1);
    int buf_size = (int)PARAM_LEN(1);

    speex_bits_init_buffer(bits, buff, buf_size);
    SET_NUMBER(0, (SYS_INT)bits)
    RETURN_NUMBER((SYS_INT)bits)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_set_bit_buffer(SpeexBits* bits, void* buff, int buf_size)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_set_bit_buffer, 2)
    T_HANDLE(speex_bits_set_bit_buffer, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_STRING(speex_bits_set_bit_buffer, 1) // void* buff
    void *buff = (void *)PARAM(1);

    int buf_size = (int)PARAM_LEN(1);

    speex_bits_set_bit_buffer(bits, buff, buf_size);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_destroy(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_destroy, 1)
    T_HANDLE(speex_bits_destroy, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    speex_bits_destroy(bits);
    free(bits);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_reset(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_reset, 1)
    T_HANDLE(speex_bits_reset, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    speex_bits_reset(bits);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_rewind(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_rewind, 1)
    T_HANDLE(speex_bits_rewind, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    speex_bits_rewind(bits);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_read_from(SpeexBits* bits, char* bytes, int len)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_read_from, 2)
    T_HANDLE(speex_bits_read_from, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_STRING(speex_bits_read_from, 1) // char* bytes
    char *bytes = (char *)PARAM(1);
    int len = (int)PARAM_LEN(1);

    speex_bits_read_from(bits, bytes, len);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_read_whole_bytes(SpeexBits* bits, char* bytes, int len)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_read_whole_bytes, 2)
    T_HANDLE(speex_bits_read_whole_bytes, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_STRING(speex_bits_read_whole_bytes, 1) // char* bytes
    char *bytes = (char *)PARAM(1);
    int len = (int)PARAM_LEN(1);

    speex_bits_read_whole_bytes(bits, bytes, len);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_write(SpeexBits* bits, char* bytes, int max_len)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_write, 3)
    T_HANDLE(speex_bits_write, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_write, 2) // char* bytes
    int max_len = (int)PARAM_INT(2);

    char *bytes = new char[max_len + 1];
    bytes[max_len] = 0;
    int res = speex_bits_write(bits, bytes, max_len);
    if (res > 0) {
        SET_BUFFER(1, bytes, res);
    } else {
        SET_STRING(1, "");
    }
    delete[] bytes;

    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_write_whole_bytes(SpeexBits* bits, char* bytes, int max_len)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_write_whole_bytes, 3)
    T_HANDLE(speex_bits_write_whole_bytes, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_write_whole_bytes, 2) // char* bytes
    int max_len = (int)PARAM_INT(2);

    char *bytes = new char[max_len + 1];
    bytes[max_len] = 0;
    int res = speex_bits_write_whole_bytes(bits, bytes, max_len);
    if (res > 0) {
        SET_BUFFER(1, bytes, res);
    } else {
        SET_STRING(1, "");
    }
    delete[] bytes;

    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_pack(SpeexBits* bits, int data, int nbBits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_pack, 3)
    T_HANDLE(speex_bits_pack, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_pack, 1) // int data
    int data = (int)PARAM(1);

    T_NUMBER(speex_bits_pack, 2) // int nbBits
    int nbBits = (int)PARAM(2);

    speex_bits_pack(bits, data, nbBits);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_unpack_signed(SpeexBits* bits, int nbBits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_unpack_signed, 2)
    T_HANDLE(speex_bits_unpack_signed, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_unpack_signed, 1) // int nbBits
    int nbBits = (int)PARAM(1);

    RETURN_NUMBER(speex_bits_unpack_signed(bits, nbBits))
END_IMPL
//------------------------------------------------------------------------
// unsigned int speex_bits_unpack_unsigned(SpeexBits* bits, int nbBits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_unpack_unsigned, 2)
    T_HANDLE(speex_bits_unpack_unsigned, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_unpack_unsigned, 1) // int nbBits
    int nbBits = (int)PARAM(1);

    RETURN_NUMBER(speex_bits_unpack_unsigned(bits, nbBits))
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_nbytes(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_nbytes, 1)
    T_HANDLE(speex_bits_nbytes, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    RETURN_NUMBER(speex_bits_nbytes(bits))
END_IMPL
//------------------------------------------------------------------------
// unsigned int speex_bits_peek_unsigned(SpeexBits* bits, int nbBits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_peek_unsigned, 2)
    T_HANDLE(speex_bits_peek_unsigned, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_peek_unsigned, 1) // int nbBits
    int nbBits = (int)PARAM(1);

    RETURN_NUMBER(speex_bits_peek_unsigned(bits, nbBits))
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_peek(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_peek, 1)
    T_HANDLE(speex_bits_peek, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    RETURN_NUMBER(speex_bits_peek(bits))
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_advance(SpeexBits* bits, int n)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_advance, 2)
    T_HANDLE(speex_bits_advance, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    T_NUMBER(speex_bits_advance, 1) // int n
    int n = (int)PARAM(1);

    speex_bits_advance(bits, n);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int speex_bits_remaining(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_remaining, 1)
    T_HANDLE(speex_bits_remaining, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    RETURN_NUMBER(speex_bits_remaining(bits))
END_IMPL
//------------------------------------------------------------------------
// void speex_bits_insert_terminator(SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_bits_insert_terminator, 1)
    T_HANDLE(speex_bits_insert_terminator, 0) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(0);

    speex_bits_insert_terminator(bits);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// void* speex_encoder_init(SpeexMode* mode)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_encoder_init, 1)
    T_NUMBER(speex_encoder_init, 0) // SpeexMode* mode
    const SpeexMode * mode;
    switch (PARAM_INT(0)) {
        case 1:
            mode = &speex_wb_mode;
            break;

        case 2:
            mode = &speex_uwb_mode;
            break;

        default:
            mode = &speex_nb_mode;
            break;
    }

    RETURN_NUMBER((SYS_INT)speex_encoder_init(mode))
END_IMPL
//------------------------------------------------------------------------
// void speex_encoder_destroy(void* state)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_encoder_destroy, 1)
    T_HANDLE(speex_encoder_destroy, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    speex_encoder_destroy(state);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int speex_encode_int(void* state, spx_int16_t* in, SpeexBits* bits)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_encode_int, 3)
    T_HANDLE(speex_encode_int, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    T_STRING(speex_encode_int, 1) // spx_int16_t* in
    spx_int16_t * in = (spx_int16_t *)PARAM(1);
    spx_int32_t N = 0;
    spx_int16_t short_out[MAX_IN_SAMPLES];
    speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &N);
    if (PARAM_LEN(1) < N * sizeof(spx_int16_t))
        return (void *)"speex_encode_int: invalid buffer received (size is less than framesize)";

    T_HANDLE(speex_encode_int, 2) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(2);

    RETURN_NUMBER(speex_encode_int(state, in, bits))
END_IMPL
//------------------------------------------------------------------------
// int speex_encoder_ctl(void* state, int request, void* ptr)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_encoder_ctl, 3)
    T_HANDLE(speex_encoder_ctl, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    T_NUMBER(speex_encoder_ctl, 1) // int request
    int request = (int)PARAM(1);
    spx_int32_t ptr1;
    spx_int16_t ptr2;
    char        *ptr3 = 0;

    switch (request) {
        case SPEEX_SET_QUALITY:
        case SPEEX_SET_LOW_MODE:
        case SPEEX_SET_MODE:
        case SPEEX_RESET_STATE:
        case SPEEX_SET_ENH:
        case SPEEX_SET_SUBMODE_ENCODING:
        case SPEEX_SET_SAMPLING_RATE:
            //case SPEEX_SET_WIDEBAND:
            {
                T_NUMBER(speex_encoder_ctl, 2)
                ptr1 = PARAM_INT(2);
                RETURN_NUMBER(speex_encoder_ctl(state, request, &ptr1))
            }
            break;

        case SPEEX_GET_LOW_MODE:
        case SPEEX_GET_MODE:
        case SPEEX_GET_ENH:
        case SPEEX_GET_FRAME_SIZE:
        case SPEEX_GET_BITRATE:
        case SPEEX_GET_SAMPLING_RATE:
        case SPEEX_GET_SUBMODE_ENCODING:
        case SPEEX_GET_LOOKAHEAD:
        case SPEEX_SET_HIGHPASS:
        case SPEEX_GET_HIGHPASS:
            //case SPEEX_GET_DTX_STATUS:
            RETURN_NUMBER(speex_encoder_ctl(state, request, &ptr1))
            SET_NUMBER(2, ptr1)
            break;

        /*case SPEEX_SET_INNOVATION_SAVE:
            {
                T_NUMBER(speex_encoder_ctl, 2)
                ptr2=PARAM_INT(2);
                RETURN_NUMBER(speex_encoder_ctl(state, request, &ptr2))
            }
            break;
           case SPEEX_GET_STACK:
            RETURN_NUMBER(speex_encoder_ctl(state, request, &ptr3))
            SET_STRING(2, ptr3)
            break;
         */
        default:
            RETURN_NUMBER(-3)
    }
END_IMPL
//------------------------------------------------------------------------
// void* speex_decoder_init(SpeexMode* mode)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_decoder_init, 1)
    T_NUMBER(speex_decoder_init, 0) // SpeexMode* mode
    const SpeexMode * mode;
    switch (PARAM_INT(0)) {
        case 1:
            mode = &speex_wb_mode;
            break;

        case 2:
            mode = &speex_uwb_mode;
            break;

        default:
            mode = &speex_nb_mode;
            break;
    }

    RETURN_NUMBER((SYS_INT)speex_decoder_init(mode))
END_IMPL
//------------------------------------------------------------------------
// void speex_decoder_destroy(void* state)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_decoder_destroy, 1)
    T_HANDLE(speex_decoder_destroy, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    speex_decoder_destroy(state);
    SET_NUMBER(0, 0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
// int speex_decode_int(void* state, SpeexBits* bits, spx_int16_t* out)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_decode_int, 3)
    T_HANDLE(speex_decode_int, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    T_HANDLE(speex_decode_int, 1) // SpeexBits* bits
    SpeexBits * bits = (SpeexBits *)PARAM_INT(1);

    spx_int32_t N = 0;
    spx_int16_t short_out[MAX_IN_SAMPLES];
    speex_decoder_ctl(state, SPEEX_GET_FRAME_SIZE, &N);

    int res = speex_decode_int(state, bits, short_out);
    if (!res) {
        SET_BUFFER(2, (char *)short_out, N * sizeof(spx_int16_t))
    } else {
        SET_STRING(2, "")
    }

    RETURN_NUMBER(res)
END_IMPL
//------------------------------------------------------------------------
// int speex_decoder_ctl(void* state, int request, void* ptr)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_decoder_ctl, 3)
    T_HANDLE(speex_decoder_ctl, 0) // void* state
    void *state = (void *)PARAM_INT(0);

    T_NUMBER(speex_decoder_ctl, 1) // int request
    int request = (int)PARAM(1);
    spx_int32_t ptr1;
    spx_int16_t ptr2;
    char        *ptr3 = 0;

    switch (request) {
        case SPEEX_SET_LOW_MODE:
        case SPEEX_SET_ENH:
        case SPEEX_SET_MODE:
        case SPEEX_SET_QUALITY:
        case SPEEX_SET_SUBMODE_ENCODING:
        case SPEEX_RESET_STATE:
        case SPEEX_SET_HIGHPASS:
            //case SPEEX_SET_WIDEBAND:
            {
                T_NUMBER(speex_decoder_ctl, 2)
                ptr1 = PARAM_INT(2);
                RETURN_NUMBER(speex_decoder_ctl(state, request, &ptr1))
            }
            break;

        case SPEEX_GET_LOW_MODE:
        case SPEEX_GET_ENH:
        case SPEEX_GET_MODE:
        case SPEEX_GET_FRAME_SIZE:
        case SPEEX_GET_BITRATE:
        case SPEEX_SET_SAMPLING_RATE:
        case SPEEX_GET_SAMPLING_RATE:
        case SPEEX_GET_SUBMODE_ENCODING:
        case SPEEX_GET_HIGHPASS:
        case SPEEX_GET_LOOKAHEAD:
            RETURN_NUMBER(speex_decoder_ctl(state, request, &ptr1))
            SET_NUMBER(2, ptr1)
            break;

        /*case SPEEX_SET_INNOVATION_SAVE:
            {
                T_NUMBER(speex_decoder_ctl, 2)
                ptr1=PARAM_INT(2);
                RETURN_NUMBER(speex_decoder_ctl(state, request, &ptr2))
            }
            break;
           case SPEEX_GET_STACK:
            RETURN_NUMBER(speex_decoder_ctl(state, request, &ptr3))
            SET_STRING(2, ptr3)
            break;*/
        default:
            RETURN_NUMBER(-2)
    }
END_IMPL
//------------------------------------------------------------------------
// int speex_mode_query(SpeexMode* mode, int request, void* ptr)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_mode_query, 3)
    T_NUMBER(speex_mode_query, 0) // SpeexMode* mode
//SpeexMode* mode=(SpeexMode*)PARAM_INT(0);
    const SpeexMode * mode;
    switch (PARAM_INT(0)) {
        case 1:
            mode = &speex_wb_mode;
            break;

        case 2:
            mode = &speex_uwb_mode;
            break;

        default:
            mode = &speex_nb_mode;
            break;
    }


    T_NUMBER(speex_mode_query, 1) // int request
    int request = (int)PARAM(1);

    int ptr = 0;
    RETURN_NUMBER(speex_mode_query(mode, request, &ptr))
    SET_NUMBER(2, ptr)
END_IMPL
//------------------------------------------------------------------------
// int speex_lib_ctl(int request, void* ptr)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_lib_ctl, 2)
    T_NUMBER(speex_lib_ctl, 0) // int request
    int request = (int)PARAM(0);

//T_STRING(1) // void* ptr
    int  ptr;
    char *ptr2;
    switch (request) {
        case SPEEX_LIB_GET_MAJOR_VERSION:
        case SPEEX_LIB_GET_MICRO_VERSION:
        case SPEEX_LIB_GET_MINOR_VERSION:
            RETURN_NUMBER(speex_lib_ctl(request, &ptr))
            SET_NUMBER(1, ptr)
            break;

        case SPEEX_LIB_GET_EXTRA_VERSION:
        case SPEEX_LIB_GET_VERSION_STRING:
            RETURN_NUMBER(speex_lib_ctl(request, &ptr2))
            SET_STRING(1, ptr2);
            break;

        default:
            SET_NUMBER(1, 0)
    }
END_IMPL
//------------------------------------------------------------------------
// SpeexMode* speex_lib_get_mode(int mode)
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_lib_get_mode, 1)
    T_NUMBER(speex_lib_get_mode, 0) // int mode
    int mode = (int)PARAM_INT(0);

    int res = 0;
    const SpeexMode *smode = speex_lib_get_mode(mode);
    if (smode == &speex_wb_mode)
        res = 1;
    else
    if (smode == &speex_uwb_mode)
        res = 2;

    RETURN_NUMBER((SYS_INT)res)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(alaw2ulaw, 1)
    T_STRING(alaw2ulaw, 0)
    unsigned char *output = 0;
    int length = PARAM_LEN(0);
    CORE_NEW(length + 1, output);
    output[length] = 0;
    unsigned char *input = (unsigned char *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = alaw2ulaw(input[i]);

    SetVariable(RESULT, -1, (char *)output, length);

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ulaw2alaw, 1)
    T_STRING(ulaw2alaw, 0)
    unsigned char *output = 0;
    int length = PARAM_LEN(0);
    CORE_NEW(length + 1, output);
    output[length] = 0;
    unsigned char *input = (unsigned char *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = ulaw2alaw(input[i]);

    SetVariable(RESULT, -1, (char *)output, length);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(alaw2pcm, 1)
    T_STRING(alaw2pcm, 0)
/*unsigned */ short *output = 0;
    int length   = PARAM_LEN(0);
    int byte_len = length * 2;
    CORE_NEW(byte_len + 1, output);
    ((char *)output)[byte_len] = 0;
    unsigned char *input = (unsigned char *)PARAM(0);
    for (int i = 0; i < length; i++) {
        output[i] = alaw2linear(input[i]);
    }
    SetVariable(RESULT, -1, (char *)output, byte_len);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ulaw2pcm, 1)
    T_STRING(ulaw2pcm, 0)
    short *output = 0;
    int length   = PARAM_LEN(0);
    int byte_len = length * 2;
    CORE_NEW(byte_len + 1, output);
    ((char *)output)[byte_len] = 0;
    unsigned char *input = (unsigned char *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = ulaw2linear(input[i]);
    SetVariable(RESULT, -1, (char *)output, byte_len);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(pcm2ulaw, 1)
    T_STRING(pcm2ulaw, 0)
    unsigned char *output = 0;
    int byte_len = PARAM_LEN(0);
    int length   = byte_len / 2;
    CORE_NEW(length + 1, output);
    output[length] = 0;

    short *input = (short *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = linear2ulaw(input[i]);

    SetVariable(RESULT, -1, (char *)output, length);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(pcm2alaw, 1)
    T_STRING(pcm2alaw, 0)
    unsigned char *output = 0;
    int byte_len = PARAM_LEN(0);
    int length   = byte_len / 2;
    CORE_NEW(length + 1, output);
    output[length] = 0;

    short *input = (short *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = linear2alaw(input[i]);

    SetVariable(RESULT, -1, (char *)output, length);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(s162alaw, 1)
    T_STRING(s162alaw, 0)
    unsigned char *output = 0;
    int byte_len = PARAM_LEN(0);
    int length   = byte_len / 2;
    CORE_NEW(length + 1, output);
    output[length] = 0;

    short *input = (short *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = s16_to_alaw(input[i]);

    SetVariable(RESULT, -1, (char *)output, length);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(alaw2s16, 1)
    T_STRING(alaw2s16, 0)
    short *output = 0;
    int length   = PARAM_LEN(0);
    int byte_len = length * 2;
    CORE_NEW(byte_len + 1, output);
    ((char *)output)[byte_len] = 0;
    unsigned char *input = (unsigned char *)PARAM(0);
    for (int i = 0; i < length; i++)
        output[i] = alaw_to_s16(input[i]);
    SetVariable(RESULT, -1, (char *)output, byte_len);
END_IMPL
//------------------------------------------------------------------------
static const int divs_8000[] = { 5, 6, 6, 5 };
static const int divs_11025[] = { 4 };
static const int divs_22050[] = { 2 };
static const int divs_44100[] = { 1 };

int downsample(const short *in, short *out, int len, const int *divs, int divs_len) {
    int i, j, lsum, rsum;
    int di, div;
    int oi;

    i   = 0;
    oi  = 0;
    di  = 0;
    div = divs[0];
    while (i + div /** 2*/ <= len) {
        for (j = 0, lsum = 0, rsum = 0; j < div; j++) {
            lsum += in[i + j /** 2*/];
            //rsum += in[i + j * 2 + 1];
        }
        out[oi] = (lsum + rsum) / (div /** 2*/);

        oi++;
        i  += div;// * 2;
        div = divs[++di % divs_len];
    }
    return oi;
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(downsample44k, 2)
    T_STRING(downsample44k, 0)
    T_NUMBER(downsample44k, 1)

    int new_rate = PARAM_INT(1);
    short     *output = 0;
    short     *input  = (short *)PARAM(0);
    int       len     = PARAM_LEN(0);
    int       n       = len / 2;
    const int *divs;
    int       divs_len;

    if (new_rate == 44100) {
        RETURN_BUFFER((char *)input, n);
        return 0;
    }
    switch (new_rate) {
        case 8000:
            divs     = divs_8000;
            divs_len = 4;
            break;

        case 11025:
            divs     = divs_11025;
            divs_len = 1;
            break;

        case 22050:
            divs     = divs_22050;
            divs_len = 1;
            break;

        case 44100:
            divs     = divs_44100;
            divs_len = 1;
            break;

        default:
            return (void *)"downsample: invalid bitrate. Allowed values: 8000, 11025, 22050, 44100";
    }

    CORE_NEW(len + 1, output);
    ((char *)output)[len] = 0;
    int size = downsample(input, output, n, divs, divs_len);

    SetVariable(RESULT, -1, (char *)output, size * sizeof(short));
END_IMPL
//------------------------------------------------------------------------
struct EchoContainer {
    SpeexEchoState       *echo_state;
    SpeexPreprocessState *preprocess;
};

CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(speex_echo_state_init, 2, 3)
    T_NUMBER(speex_echo_state_init, 0)
    T_NUMBER(speex_echo_state_init, 1)

    int r = 8000;
    if (PARAMETERS_COUNT == 3) {
        T_NUMBER(speex_echo_state_init, 2)
        r = PARAM_INT(2);
    }
    EchoContainer *echo = (EchoContainer *)malloc(sizeof(EchoContainer));

    echo->echo_state = speex_echo_state_init(PARAM_INT(0), PARAM_INT(1));
    speex_echo_ctl(echo->echo_state, SPEEX_ECHO_SET_SAMPLING_RATE, &r);

    echo->preprocess = speex_preprocess_state_init(PARAM_INT(0), r);
    speex_preprocess_ctl(echo->preprocess, SPEEX_PREPROCESS_SET_ECHO_STATE, echo->echo_state);

    RETURN_NUMBER((long)echo)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_echo_state_destroy, 1)
    T_NUMBER(speex_echo_state_destroy, 0)
    EchoContainer * echo = (EchoContainer *)(PARAM_INT(0));
    if (echo) {
        speex_echo_state_destroy(echo->echo_state);
        speex_preprocess_state_destroy(echo->preprocess);
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_echo_state_reset, 1)
    T_NUMBER(speex_echo_state_reset, 0)
    EchoContainer * echo_state = (EchoContainer *)(PARAM_INT(0));
    if (echo_state)
        speex_echo_state_reset(echo_state->echo_state);

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_echo_cancellation, 4)
    T_HANDLE(speex_echo_cancellation, 0)
    T_STRING(speex_echo_cancellation, 1)
    T_STRING(speex_echo_cancellation, 2)

    EchoContainer * echo_state = (EchoContainer *)(PARAM_INT(0));

    spx_int16_t *input_frame  = (spx_int16_t *)PARAM(1);
    spx_int16_t *echo_frame   = (spx_int16_t *)PARAM(2);
    spx_int16_t *output_frame = 0;

    int length = PARAM_LEN(1);
    CORE_NEW(length + 1, output_frame);
    output_frame[length] = 0;
    output_frame[0]      = 0;

    speex_echo_cancellation(echo_state->echo_state, input_frame, echo_frame, output_frame);

    SetVariable(PARAMETER(3), -1, (char *)output_frame, length);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_echo_playback, 2)
    T_HANDLE(speex_echo_playback, 0)
    T_STRING(speex_echo_playback, 1)
    EchoContainer * echo_state = (EchoContainer *)(PARAM_INT(0));
    spx_int16_t *echo_frame = (spx_int16_t *)PARAM(1);

    speex_echo_playback(echo_state->echo_state, echo_frame);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(speex_echo_capture, 3)
    T_HANDLE(speex_echo_capture, 0)
    T_STRING(speex_echo_capture, 1)

    EchoContainer * echo_state = (EchoContainer *)(PARAM_INT(0));
    spx_int16_t *input_frame  = (spx_int16_t *)PARAM(1);
    spx_int16_t *output_frame = 0;

    int length = PARAM_LEN(1);
    CORE_NEW(length + 1, output_frame);
    output_frame[length] = 0;
    output_frame[0]      = 0;

    speex_echo_capture(echo_state->echo_state, input_frame, output_frame);
    speex_preprocess_run(echo_state->preprocess, output_frame);

    SetVariable(PARAMETER(2), -1, (char *)output_frame, length);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

