//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#ifdef _WIN32
 #include <opus/opus.h>
#else
 #include <opus.h>
#endif
#include <string.h>
#include <stdlib.h>
//-------------------------//
// Local variables         //
//-------------------------//
#define MAX_IN_SAMPLES    640
#define USE_ALTERNATIVE_G711
extern "C" {
unsigned char linear2alaw(int pcm_val);
int alaw2linear(unsigned char a_val);
unsigned char linear2ulaw(int pcm_val);
int ulaw2linear(unsigned char u_val);
unsigned char alaw2ulaw(unsigned char aval);
unsigned char ulaw2alaw(unsigned char uval);
}

#ifdef USE_ALTERNATIVE_G711
#define         SIGN_BIT        (0x80)
#define         QUANT_MASK      (0xf)
#define         NSEGS           (8)
#define         SEG_SHIFT       (4)
#define         SEG_MASK        (0x70)
#define         BIAS            (0x84)

static unsigned char *linear_to_alaw = NULL;
static unsigned char *linear_to_ulaw = NULL;

static int ffmpeg_alaw2linear(unsigned char a_val) {
    int t;
    int seg;
    a_val ^= 0x55;
    t = a_val & QUANT_MASK;
    seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
    if (seg)
        t= (t + t + 1 + 32) << (seg + 2);
    else
        t= (t + t + 1     ) << 3;
    return (a_val & SIGN_BIT) ? t : -t;
}

static int ffmpeg_ulaw2linear(unsigned char u_val) {
    int t;
    u_val = ~u_val;
    t = ((u_val & QUANT_MASK) << 3) + BIAS;
    t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;
    return (u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS);
}

static void build_xlaw_table(unsigned char *linear_to_xlaw, int (*xlaw2linear)(unsigned char), int mask) {
    int i, j, v, v1, v2;
    j = 0;
    for(i=0;i<128;i++) {
        if (i != 127) {
            v1 = xlaw2linear(i ^ mask);
            v2 = xlaw2linear((i + 1) ^ mask);
            v = (v1 + v2 + 4) >> 3;
        } else {
            v = 8192;
        }
        for(;j<v;j++) {
            linear_to_xlaw[8192 + j] = (i ^ mask);
            if (j > 0)
                linear_to_xlaw[8192 - j] = (i ^ (mask ^ 0x80));
        }
    }
    linear_to_xlaw[0] = linear_to_xlaw[1];
}
#endif

INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(OPUS_SET_APPLICATION_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_APPLICATION_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_BITRATE_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_BITRATE_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_MAX_BANDWIDTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_MAX_BANDWIDTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_VBR_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_VBR_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_BANDWIDTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_BANDWIDTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_COMPLEXITY_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_COMPLEXITY_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_INBAND_FEC_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_INBAND_FEC_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_PACKET_LOSS_PERC_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_PACKET_LOSS_PERC_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_DTX_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_DTX_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_VBR_CONSTRAINT_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_VBR_CONSTRAINT_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_FORCE_CHANNELS_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_FORCE_CHANNELS_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_SIGNAL_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_SIGNAL_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_LOOKAHEAD_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_SAMPLE_RATE_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_FINAL_RANGE_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_PITCH_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_GAIN_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_GAIN_REQUEST)
    DEFINE_ECONSTANT(OPUS_SET_LSB_DEPTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_LSB_DEPTH_REQUEST)
    DEFINE_ECONSTANT(OPUS_GET_LAST_PACKET_DURATION_REQUEST)
    DEFINE_ECONSTANT(OPUS_AUTO)
    DEFINE_ECONSTANT(OPUS_BITRATE_MAX)
    DEFINE_ECONSTANT(OPUS_APPLICATION_VOIP)
    DEFINE_ECONSTANT(OPUS_APPLICATION_AUDIO)
    DEFINE_ECONSTANT(OPUS_APPLICATION_RESTRICTED_LOWDELAY)

    DEFINE_ECONSTANT(OPUS_SIGNAL_VOICE)
    DEFINE_ECONSTANT(OPUS_SIGNAL_MUSIC)
    DEFINE_ECONSTANT(OPUS_BANDWIDTH_NARROWBAND)
    DEFINE_ECONSTANT(OPUS_BANDWIDTH_MEDIUMBAND)
    DEFINE_ECONSTANT(OPUS_BANDWIDTH_WIDEBAND)
    DEFINE_ECONSTANT(OPUS_BANDWIDTH_SUPERWIDEBAND)
    DEFINE_ECONSTANT(OPUS_BANDWIDTH_FULLBAND)

    DEFINE_ECONSTANT(OPUS_OK)
    DEFINE_ECONSTANT(OPUS_BAD_ARG)
    DEFINE_ECONSTANT(OPUS_BUFFER_TOO_SMALL)
    DEFINE_ECONSTANT(OPUS_INTERNAL_ERROR)
    DEFINE_ECONSTANT(OPUS_INVALID_PACKET)
    DEFINE_ECONSTANT(OPUS_UNIMPLEMENTED)
    DEFINE_ECONSTANT(OPUS_INVALID_STATE)
    DEFINE_ECONSTANT(OPUS_ALLOC_FAIL)

    DEFINE_ECONSTANT(OPUS_RESET_STATE);
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
static inline int align2(int i) {
    return (i + sizeof(void *) - 1) & - ((int)sizeof(void *));
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(opus_decoder_create, 2, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    char *ptr = (char *)malloc(opus_decoder_get_size(PARAM_INT(1)) + align2(2));
    char *orig = ptr;
    ptr[0] = PARAM_INT(1);
    ptr   += align2(2);
    OpusDecoder *dec = (OpusDecoder *)ptr;
    int         err  = opus_decoder_init(dec, PARAM_INT(0), PARAM_INT(1));
    if (PARAMETERS_COUNT >= 3) {
        SET_NUMBER(2, (SYS_INT)err)
    }
    if (err) {
        free(orig);
        dec = 0;
    }
    RETURN_NUMBER((SYS_INT)dec)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(opus_encoder_create, 3, 4)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    char *ptr = (char *)malloc(opus_encoder_get_size(PARAM_INT(1)) + align2(2));
    char *orig = ptr;
    ptr[0] = PARAM_INT(1);
    ptr   += align2(2);

    OpusEncoder *dec = (OpusEncoder *)ptr;
    int         err  = opus_encoder_init(dec, PARAM_INT(0), PARAM_INT(1), PARAM_INT(2));
    if (PARAMETERS_COUNT >= 4) {
        SET_NUMBER(3, (SYS_INT)err)
    }
    if (err) {
        free(orig);
        dec = 0;
    }
    RETURN_NUMBER((SYS_INT)dec)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_encoder_destroy, 1)
    T_NUMBER(0)
    OpusEncoder * dec = (OpusEncoder *)(SYS_INT)PARAM_INT(0);
    if (dec) {
        //opus_encoder_destroy(dec);
        free(((char *)dec) - align2(2));
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_decoder_destroy, 1)
    T_NUMBER(0)
    OpusDecoder * dec = (OpusDecoder *)(SYS_INT)PARAM_INT(0);
    if (dec) {
        //opus_decoder_destroy(dec);
        free(((char *)dec) - align2(2));
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_encode, 3)
    T_HANDLE(0)
    T_STRING(1)
    char *ptr = (char *)(SYS_INT)PARAM_INT(0);
    ptr      -= align2(2);
    int channels = ptr[0];

    OpusEncoder *enc       = (OpusEncoder *)(SYS_INT)PARAM_INT(0);
    int         frame_size = PARAM_LEN(1) / sizeof(opus_int16);
    if (channels)
        frame_size /= channels;

    char *out = 0;
    int  len  = 1275;//PARAM_LEN(1);
    CORE_NEW(len, out);
    int res = opus_encode(enc, (opus_int16 *)PARAM(1), frame_size, (unsigned char *)out, len);
    if (res <= 0) {
        CORE_DELETE(out);
        SET_STRING(2, "");
    } else
        SetVariable(PARAMETER(2), -1, out, res);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(opus_decode, 4, 5)
    T_HANDLE(0)
    T_STRING(1)
    T_NUMBER(3)
    int fec = 0;
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4)
        fec = PARAM_INT(4);
        if (fec)
            fec = 1;
    }

    char *ptr = (char *)PARAM_INT(0);
    ptr -= align2(2);
    int channels = ptr[0];
    int len      = channels;// channels;
    if (!len)
        len = 1;

    OpusDecoder *dec = (OpusDecoder *)PARAM_INT(0);
    char        *out = 0;
    len *= sizeof(opus_int16);
// max frames
    len *= PARAM_INT(3);
    CORE_NEW(len, out);
    int res = opus_decode(dec, (unsigned char *)PARAM(1), PARAM_LEN(1), (opus_int16 *)out, PARAM_INT(3), fec);
    if (res <= 0) {
        CORE_DELETE(out);
        SET_STRING(2, "");
    } else
        SetVariable(PARAMETER(2), -1, out, res * sizeof(opus_int16) * channels);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_packet_get_bandwidth, 1)
    T_STRING(0)
    int res = opus_packet_get_bandwidth((unsigned char *)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_packet_get_nb_channels, 1)
    T_STRING(0)
    int res = opus_packet_get_nb_channels((unsigned char *)PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_packet_get_nb_frames, 1)
    T_STRING(0)
    int res = opus_packet_get_nb_frames((unsigned char *)PARAM(0), PARAM_LEN(0));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_packet_get_nb_samples, 2)
    T_STRING(0)
    T_NUMBER(1)
    int res = opus_packet_get_nb_samples((unsigned char *)PARAM(0), PARAM_LEN(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_decoder_get_nb_samples, 2)
    T_HANDLE(0)
    T_STRING(1)
    OpusDecoder * dec = (OpusDecoder *)PARAM_INT(0);
    int res = opus_decoder_get_nb_samples(dec, (unsigned char *)PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_encoder_ctl, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    OpusEncoder * enc = (OpusEncoder *)PARAM_INT(0);
    opus_int32 i   = PARAM_INT(2);
    int        res = -1;
    switch (PARAM_INT(1)) {
        case OPUS_GET_APPLICATION_REQUEST:
        case OPUS_GET_BITRATE_REQUEST:
        case OPUS_GET_MAX_BANDWIDTH_REQUEST:
        case OPUS_GET_VBR_REQUEST:
        case OPUS_GET_BANDWIDTH_REQUEST:
        case OPUS_GET_COMPLEXITY_REQUEST:
        case OPUS_GET_INBAND_FEC_REQUEST:
        case OPUS_GET_PACKET_LOSS_PERC_REQUEST:
        case OPUS_GET_DTX_REQUEST:
        case OPUS_GET_VBR_CONSTRAINT_REQUEST:
        case OPUS_GET_FORCE_CHANNELS_REQUEST:
        case OPUS_GET_SIGNAL_REQUEST:
        case OPUS_GET_LOOKAHEAD_REQUEST:
        case OPUS_GET_SAMPLE_RATE_REQUEST:
        case OPUS_GET_FINAL_RANGE_REQUEST:
        case OPUS_GET_PITCH_REQUEST:
        case OPUS_GET_GAIN_REQUEST:
        case OPUS_GET_LSB_DEPTH_REQUEST:
        case OPUS_GET_LAST_PACKET_DURATION_REQUEST:
            res = opus_encoder_ctl(enc, PARAM_INT(1), &i);
            SET_NUMBER(2, i);
            break;

        default:
            res = opus_encoder_ctl(enc, PARAM_INT(1), i);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_decoder_ctl, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    OpusDecoder * dec = (OpusDecoder *)PARAM_INT(0);
    opus_int32 i   = PARAM_INT(2);
    int        res = -1;
    switch (PARAM_INT(1)) {
        case OPUS_GET_APPLICATION_REQUEST:
        case OPUS_GET_BITRATE_REQUEST:
        case OPUS_GET_MAX_BANDWIDTH_REQUEST:
        case OPUS_GET_VBR_REQUEST:
        case OPUS_GET_BANDWIDTH_REQUEST:
        case OPUS_GET_COMPLEXITY_REQUEST:
        case OPUS_GET_INBAND_FEC_REQUEST:
        case OPUS_GET_PACKET_LOSS_PERC_REQUEST:
        case OPUS_GET_DTX_REQUEST:
        case OPUS_GET_VBR_CONSTRAINT_REQUEST:
        case OPUS_GET_FORCE_CHANNELS_REQUEST:
        case OPUS_GET_SIGNAL_REQUEST:
        case OPUS_GET_LOOKAHEAD_REQUEST:
        case OPUS_GET_SAMPLE_RATE_REQUEST:
        case OPUS_GET_FINAL_RANGE_REQUEST:
        case OPUS_GET_PITCH_REQUEST:
        case OPUS_GET_GAIN_REQUEST:
        case OPUS_GET_LSB_DEPTH_REQUEST:
        case OPUS_GET_LAST_PACKET_DURATION_REQUEST:
            res = opus_decoder_ctl(dec, PARAM_INT(1), &i);
            SET_NUMBER(2, i);
            break;

        default:
            res = opus_decoder_ctl(dec, PARAM_INT(1), i);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_create, 0)
    OpusRepacketizer * rp = opus_repacketizer_create();
    RETURN_NUMBER((SYS_INT)rp);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_destroy, 1)
    T_NUMBER(0)
    OpusRepacketizer * rp = (OpusRepacketizer *)PARAM_INT(0);
    if (rp) {
        opus_repacketizer_destroy(rp);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_cat, 2)
    T_HANDLE(0)
    T_STRING(1)
    OpusRepacketizer * rp = (OpusRepacketizer *)PARAM_INT(0);
    int res = opus_repacketizer_cat(rp, (unsigned char *)PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_out_range, 5)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(4)
    OpusRepacketizer * rp = (OpusRepacketizer *)PARAM_INT(0);
    char *output = 0;
    int  length  = PARAM_INT(4);
    CORE_NEW(length + 1, output);
    int res = opus_repacketizer_out_range(rp, PARAM_INT(1), PARAM_INT(2), (unsigned char *)output, length);
    if (res <= 0) {
        CORE_DELETE(output);
        SET_STRING(3, "");
    } else {
        SetVariable(PARAMETER(3), -1, (char *)output, res);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_get_nb_frames, 1)
    T_HANDLE(0)
    OpusRepacketizer * rp = (OpusRepacketizer *)PARAM_INT(0);
    int res = opus_repacketizer_get_nb_frames(rp);
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(opus_repacketizer_out, 3)
    T_HANDLE(0)
    T_NUMBER(2)
    OpusRepacketizer * rp = (OpusRepacketizer *)PARAM_INT(0);
    char *output = 0;
    int  length  = PARAM_INT(2);
    CORE_NEW(length + 1, output);
    int res = opus_repacketizer_out(rp, (unsigned char *)output, length);
    if (res <= 0) {
        CORE_DELETE(output);
        SET_STRING(1, "");
    } else {
        SetVariable(PARAMETER(1), -1, (char *)output, res);
    }
    RETURN_NUMBER(res);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(alaw2ulaw, 1)
    T_STRING(0)
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
    T_STRING(0)
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
    T_STRING(0)
    short *output = 0;
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
    T_STRING(0)
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
    T_STRING(0)
    unsigned char *output = 0;
#ifdef USE_ALTERNATIVE_G711
    int v;
    if (!linear_to_ulaw) {
        linear_to_ulaw = (unsigned char *)malloc(16384);
        build_xlaw_table(linear_to_ulaw, ffmpeg_ulaw2linear, 0xff);
    }
#endif
    int byte_len = PARAM_LEN(0);
    int length   = byte_len / 2;
    CORE_NEW(length + 1, output);
    output[length] = 0;

    short *input = (short *)PARAM(0);
    for (int i = 0; i < length; i++) {
#ifdef USE_ALTERNATIVE_G711
        v = input[i];
        output[i] = linear_to_ulaw[(v + 32768) >> 2];
#else
        output[i] = linear2ulaw(input[i]);
#endif
    }
    SetVariable(RESULT, -1, (char *)output, length);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(pcm2alaw, 1)
    T_STRING(0)
    unsigned char *output = 0;
#ifdef USE_ALTERNATIVE_G711
    int v;
    if (!linear_to_alaw) {
        linear_to_alaw = (unsigned char *)malloc(16384);
        build_xlaw_table(linear_to_alaw, ffmpeg_alaw2linear, 0xd5);
    }
#endif
    int byte_len = PARAM_LEN(0);
    int length   = byte_len / 2;
    CORE_NEW(length + 1, output);
    output[length] = 0;

    short *input = (short *)PARAM(0);
    for (int i = 0; i < length; i++) {
#ifdef USE_ALTERNATIVE_G711
        v = input[i];
        output[i] = linear_to_alaw[(v + 32768) >> 2];
#else
        output[i] = linear2alaw(input[i]);
#endif
    }

    SetVariable(RESULT, -1, (char *)output, length);
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
    while (i + div <= len) {
        for (j = 0, lsum = 0, rsum = 0; j < div; j++) {
            lsum += in[i + j];
            //rsum += in[i + j * 2 + 1];
        }
        out[oi] = (lsum + rsum) / (div);

        oi++;
        i  += div;// * 2;
        div = divs[++di % divs_len];
    }
    return oi;
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(downsample44k, 2)
    T_STRING(0)
    T_NUMBER(1)

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
