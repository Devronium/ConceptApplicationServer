//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
extern "C" {
    #include "shine/layer3.h"

    #define MINIMP3_IMPLEMENTATION
    #include "minimp3_ex.h"
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MP3EncoderCreate, 0, 1)
    static shine_config_t config;

    memset(&config, 0, sizeof(shine_config_t));
    config.wave.channels = PCM_MONO;
    config.wave.samplerate = 8000;
    shine_set_config_mpeg_defaults(&config.mpeg);
    config.mpeg.bitr = 16;
    if (PARAMETERS_COUNT > 0) {
        T_ARRAY(MP3EncoderCreate, 0);
        char *key;
        int  count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));

        for (INTEGER i = 0; i < count; i++) {
            key = 0;
            Invoke(INVOKE_GET_ARRAY_KEY, PARAMETER(0), i, &key);
            if (key) {
                void *newpData = 0;
                INTEGER type = 0;
                char *szData = 0;
                NUMBER nData = 0;

                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(0), i, &newpData);
                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

                if (type != VARIABLE_NUMBER)
                    continue;

                if (!strcmp(key, "samplerate"))
                    config.wave.samplerate = (int)nData;
                else
                if (!strcmp(key, "bitrate"))
                    config.mpeg.bitr = (int)nData;
                else
                if (!strcmp(key, "channels")) {
                    if (((int)nData > 1)) {
                        config.wave.channels = PCM_STEREO;
                        config.mpeg.mode = STEREO;
                    } else {
                        config.mpeg.mode = MONO;
                        config.wave.channels = PCM_MONO;
                    }
                }
            }
        }
    }

    shine_t s = shine_initialise(&config);
    RETURN_NUMBER((NUMBER)(intptr_t)s);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MP3EncoderSamples, 1)
    T_HANDLE(MP3EncoderSamples, 0)
    shine_t s = (shine_t)(intptr_t)PARAM(0);
    RETURN_NUMBER(shine_samples_per_pass(s));
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MP3EncoderEncode, 2, 3)
    T_HANDLE(MP3EncoderEncode, 0)
    T_STRING(MP3EncoderEncode, 1)
    int16_t *ptr[2];
    shine_t s = (shine_t)(intptr_t)PARAM(0);
    int written = 0;
    
    ptr[0] = (int16_t *)PARAM(1);
    int samples = shine_samples_per_pass(s) * 2;
    if (PARAMETERS_COUNT > 2) {
        T_STRING(MP3EncoderEncode, 2)
        if (PARAM_LEN(2) < samples) {
            RETURN_STRING("");
            return 0;
        }
        ptr[1] = (int16_t *)PARAM(2);
    } else
        ptr[1] = NULL;

    if (PARAM_LEN(1) < samples) {
        RETURN_STRING("");
        return 0;
    }
    unsigned char *data;
    if (PARAMETERS_COUNT > 2)
        data = shine_encode_buffer(s, ptr, &written);
    else
        data = shine_encode_buffer_interleaved(s, (int16_t *)PARAM(1), &written);

    if ((data) && (written > 0)) {
        RETURN_BUFFER((const char *)data, written);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MP3EncoderFlush, 1)
    T_HANDLE(MP3EncoderFlush, 0)
    shine_t s = (shine_t)(intptr_t)PARAM(0);
    int written = 0;
    unsigned char *data = shine_flush(s, &written);
    if ((data) && (written > 0)) {
        RETURN_BUFFER((const char *)data, written);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MP3EncoderDone, 1)
    T_HANDLE(MP3EncoderDone, 0)
    shine_t s = (shine_t)(intptr_t)PARAM(0);
    shine_close(s);
    SET_NUMBER(0, 0);
    RETURN_NUMBER(0);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MP3Decode, 1, 2)
    T_STRING(MP3Decode, 0)

    mp3dec_file_info_t info;
    mp3dec_t mp3d;

    memset(&info, 0, sizeof(info));

    int err = mp3dec_load_buf(&mp3d, (const uint8_t *)PARAM(0), (size_t)PARAM_LEN(0), &info, NULL, NULL);

    if (PARAMETERS_COUNT > 1) {
        CREATE_ARRAY(PARAMETER(1));

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), "samples", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(size_t)info.samples);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), "channels", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(int)info.channels);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), "hz", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(int)info.hz);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), "avg_bitrate_kbps", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(int)info.layer);
    }

    if ((err) || (info.samples <= 0)) {
        RETURN_STRING("");
    } else {
        RETURN_BUFFER((const char *)info.buffer, info.samples * sizeof(mp3d_sample_t));
    }
    free(info.buffer);
END_IMPL
//---------------------------------------------------------------------------
