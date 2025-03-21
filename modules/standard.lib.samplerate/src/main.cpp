//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

extern "C" {
    #include "libsamplerate/samplerate.h"

    unsigned char linear2alaw(int pcm_val);
    int alaw2linear(unsigned char a_val);
    unsigned char linear2ulaw(int pcm_val);
    int ulaw2linear(unsigned char u_val);
    unsigned char alaw2ulaw(unsigned char aval);
    unsigned char ulaw2alaw(unsigned char uval);
}

#define USE_ALTERNATIVE_G711
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

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_SCONSTANT("SRC_SINC_BEST_QUALITY", "0")
    DEFINE_SCONSTANT("SRC_SINC_MEDIUM_QUALITY", "1")
    DEFINE_SCONSTANT("SRC_SINC_FASTEST", "2")
    DEFINE_SCONSTANT("SRC_ZERO_ORDER_HOLD", "3")
    DEFINE_SCONSTANT("SRC_LINEAR", "4")
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(resample, 3, 5)
    T_STRING(resample, 0)   // PCM IN DATA
    T_NUMBER(resample, 1)   // PCM SAMPLE RATE
    T_NUMBER(resample, 2)   // OUTPUT SAMPLE RATE

    int conversion_type = SRC_SINC_BEST_QUALITY;
    int channels = 1;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(resample, 3)   // CONVERSION TYPE
        conversion_type = PARAM_INT(3);
        if (PARAMETERS_COUNT > 4) {
            T_NUMBER(resample, 4)   // CHANNELS
            channels = PARAM_INT(4);
        }
    }
    if ((PARAM_INT(1) != 0) && (PARAM_INT(2) != 0) && (PARAM_LEN(0) >= 2) && (channels > 0)) {
        float src_ratio = (PARAM(2) / PARAM(1));
        int len = PARAM_LEN(0) / 2;
        int len_output = (int)floor((float)len * (float)src_ratio);

        float *input = (float *)malloc((len + 1) * sizeof(float));
        float *output = (float *)malloc((len_output + 1) * sizeof(float));

        src_short_to_float_array((short *)PARAM(0), input, len);

        SRC_DATA data;
        memset(&data, 0, sizeof(SRC_DATA));

        data.data_in = input;
        data.input_frames = len / channels;

        data.data_out = output;
        data.output_frames = len_output / channels;

        data.src_ratio = src_ratio;

        int err = src_simple(&data, conversion_type, channels);

        char *buffer = NULL;
        CORE_NEW((data.output_frames_gen * 2) + 1, buffer);
        if (buffer) {
            src_float_to_short_array(output, (short *)buffer, data.output_frames_gen);
            SetVariable(RESULT, -1, buffer, data.output_frames_gen * 2);
        } else {
            RETURN_STRING("");
        }
        free(input);
        free(output);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(level, 1)
    T_STRING(level, 0)

    int len = PARAM_LEN(0) / 2;
    if (!len) {
        RETURN_NUMBER(0);
        return 0;
    }

    unsigned short *buf = (unsigned short *)PARAM(0);
    float *input = (float *)malloc((len + 1) * sizeof(float));
    src_short_to_float_array((short *)PARAM(0), input, len);

    double sum = 0.0;
    for (int i = 0; i < len; i ++)
        sum += input[i];

    double average = sum / len;

    double sumMeanSquare = 0.0;

    for (int i = 0; i < len; i ++)
        sumMeanSquare += pow(input[i] - average, 2);

    double averageMeanSquare = sumMeanSquare/len;
    double rootMeanSquare = sqrt(averageMeanSquare);

    free(input);

    RETURN_NUMBER(rootMeanSquare);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(to16bits, 1)
    T_STRING(to16bits, 0)

    if (PARAM_LEN(0) < 1) {
        RETURN_STRING("");
        return 0;
    }

    short *buffer = NULL;
    int len = PARAM_LEN(0) * 2;
    char *in_buffer = PARAM(0);
    CORE_NEW(len + 1, buffer);
    if (!buffer) {
        RETURN_STRING("");
        return 0;
    }

    for (int i = 0; i < PARAM_LEN(0); i ++)
        buffer[i] = in_buffer[i] << 8;

    SetVariable(RESULT, -1, (char *)buffer, len);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(to8bits, 1)
    T_STRING(to8bits, 0)

    if (PARAM_LEN(0) < 2) {
        RETURN_STRING("");
        return 0;
    }

    char *buffer = NULL;
    int len = PARAM_LEN(0) / 2;
    short *in_buffer = (short *)PARAM(0);
    CORE_NEW(len + 1, buffer);
    if (!buffer) {
        RETURN_STRING("");
        return 0;
    }

    for (int i = 0; i < len; i ++)
        buffer[i] = in_buffer[i] >> 8;

    SetVariable(RESULT, -1, buffer, len);
END_IMPL
//=====================================================================================//
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
    T_STRING(pcm2alaw, 0)
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
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(mix, 2, 128)
    int i = 1;
    int max_len = 0;
    int j = 0;
    int l = 0;
    T_NUMBER(mix, 0)

    char *params[128];
    int params_len[128];

    int sample_size = PARAM_INT(0) / 8;
    if (sample_size <= 0)
        sample_size = 1;
    if (sample_size > 2)
        sample_size = 2;

    for (i = 1; i < PARAMETERS_COUNT; i ++) {
        T_STRING(mix, i);

        params[i] = PARAM(i);
        params_len[i] = PARAM_LEN(i);

        if (PARAM_LEN(i) > max_len)
            max_len = PARAM_LEN(i);
    }

    char *buffer = NULL;
    CORE_NEW((PARAMETERS_COUNT * max_len * sample_size) + 1, buffer);
    if (buffer) {
        memset(buffer, 0, (PARAMETERS_COUNT * max_len) + 1);
        int buf_offset = 0;
        for (j = 0; j < max_len; j += sample_size) {
            for (i = 1; i < PARAMETERS_COUNT; i ++) {
                for (l = 0; l < sample_size; l ++) {
                    int index = j + l;
                    if (index < params_len[i])
                        buffer[buf_offset ++] = params[i][index];
                    else
                        buffer[buf_offset ++] = 0;
                }
            }
        }
        SetVariable(RESULT, -1, buffer, buf_offset);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------ 
