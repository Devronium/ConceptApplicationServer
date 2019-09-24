//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
extern "C" {
    #include "shine/layer3.h"
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
    shine_config_t config;

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
                void *pData = 0;
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
    T_HANDLE(MP3EncoderFlush, 0)
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
    
    int samples = shine_samples_per_pass(s);
    if (PARAMETERS_COUNT > 2) {
        T_STRING(MP3EncoderEncode, 2)
        if (PARAM_LEN(2) * 2 < samples) {
            RETURN_STRING("");
            return 0;
        }
        ptr[0] = (int16_t *)PARAM(1);
        ptr[1] = (int16_t *)PARAM(2);
    }
    if (PARAM_LEN(1) * 2 < samples) {
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
