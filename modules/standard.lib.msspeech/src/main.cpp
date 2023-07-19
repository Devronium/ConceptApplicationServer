//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <speechapi_c.h>

struct STT_Container {
        SPXSPEECHCONFIGHANDLE hconfig;
        SPXPROPERTYBAGHANDLE hpropbag;

        SPXAUDIOCONFIGHANDLE haudioConfig;
        SPXAUDIOSTREAMHANDLE haudioStream;
        SPXAUDIOSTREAMFORMATHANDLE hformat;
        SPXRECOHANDLE hreco;

        SPXASYNCHANDLE hasync;
};
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(MsSpeech_Create, 4)
    T_STRING(MsSpeech_Create, 0)
    T_STRING(MsSpeech_Create, 1)
    T_STRING(MsSpeech_Create, 2)
    T_NUMBER(MsSpeech_Create, 3)

    SPXSPEECHCONFIGHANDLE hconfig = NULL;
    struct STT_Container *container = NULL;
    speech_config_from_subscription(&hconfig, PARAM(1), PARAM(2));

    if (hconfig) {
        SPXPROPERTYBAGHANDLE hpropbag = NULL;
        speech_config_get_property_bag(hconfig, &hpropbag);
        if (hpropbag)
            property_bag_set_string(hpropbag, 3001, NULL, PARAM(0));

        container = (struct STT_Container *)malloc(sizeof(struct STT_Container));
        if (container) {
            memset(container, 0, sizeof(struct STT_Container));
            container->hconfig = hconfig;
            container->hpropbag = hpropbag;

            switch (PARAM_INT(3)) {
                case 4:
                    audio_stream_format_create_from_waveformat(&container->hformat, 8000, 8, 1, StreamWaveFormat_ALAW);
                    break;
                case 8:
                    audio_stream_format_create_from_waveformat(&container->hformat, 8000, 8, 1, StreamWaveFormat_MULAW);
                    break;
                default:
                    audio_stream_format_create_from_waveformat(&container->hformat, 16000, 16, 1, StreamWaveFormat_PCM);
                    break;
            }
            audio_stream_create_push_audio_input_stream(&container->haudioStream, container->hformat);
            audio_config_create_audio_input_from_stream(&container->haudioConfig, container->haudioStream);

            if (recognizer_create_speech_recognizer_from_config(&container->hreco, hconfig, container->haudioConfig)) {
                if (speech_config_is_handle_valid(container->hconfig))
                    speech_config_release(container->hconfig);
                if (container->hpropbag)
                    property_bag_release(container->hpropbag);
                if (audio_stream_is_handle_valid(container->haudioStream))
                    audio_stream_release(container->haudioStream);
                if (audio_stream_format_is_handle_valid(container->hformat))
                    audio_stream_format_release(container->hformat);
                if (audio_config_is_handle_valid(container->haudioConfig))
                    audio_config_release(container->haudioConfig);
                if (recognizer_handle_is_valid(container->hreco))
                    recognizer_handle_release(container->hreco);
                free(container);

                container = NULL;
            }
        }
    }

    RETURN_NUMBER((SYS_INT)container);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(MsSpeech_Free, 1)
    T_HANDLE(MsSpeech_Free, 0)

    struct STT_Container *container = (struct STT_Container *)(SYS_INT)PARAM(0);

    if (container) {
        if (speech_config_is_handle_valid(container->hconfig))
            speech_config_release(container->hconfig);
        if (container->hpropbag)
            property_bag_release(container->hpropbag);
        if (audio_stream_is_handle_valid(container->haudioStream))
            audio_stream_release(container->haudioStream);
        if (audio_stream_format_is_handle_valid(container->hformat))
            audio_stream_format_release(container->hformat);
        if (audio_config_is_handle_valid(container->haudioConfig))
            audio_config_release(container->haudioConfig);
        if (recognizer_async_handle_is_valid(container->hasync))
            recognizer_async_handle_release(container->hasync);
        if (recognizer_handle_is_valid(container->hreco))
            recognizer_handle_release(container->hreco);

        free(container);
    }

    SET_NUMBER(0, 0);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(MsSpeech_Reset, 1)
    T_HANDLE(MsSpeech_Stop, 0)

    struct STT_Container *container = (struct STT_Container *)(SYS_INT)PARAM(0);
    if ((container) && (container->haudioStream) && (container->hreco)) {
        if ((container->hasync) && (container->hasync != SPXHANDLE_INVALID)) {
            recognizer_async_handle_release(container->hasync);
            container->hasync = NULL;
        }
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MsSpeech_Decode, 1, 2)
    T_HANDLE(MsSpeech_Decode, 0)

    RETURN_STRING("");
    int ms = 0;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(MsSpeech_Decode, 1);
        ms = PARAM_INT(1);
    }
    struct STT_Container *container = (struct STT_Container *)(SYS_INT)PARAM(0);
    if ((container) && (container->haudioStream) && (container->hreco)) {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        if ((!container->hasync) || (container->hasync == SPXHANDLE_INVALID))
            recognizer_recognize_once_async(container->hreco, &container->hasync);

        recognizer_recognize_once_async_wait_for(container->hasync, ms, &hresult);
        if ((hresult) && (hresult != SPXHANDLE_INVALID) && (recognizer_result_handle_is_valid(hresult))) {
            Result_Reason reason = ResultReason_NoMatch;
            if ((!result_get_reason(hresult, &reason)) && (reason == ResultReason_RecognizedSpeech)) {
                char pszText[4096];
                pszText[0] = 0;
                uint32_t cchText = sizeof(pszText) - 1;
                if (!result_get_text(hresult, pszText, cchText))
                    RETURN_STRING(pszText);
            }
            recognizer_result_handle_release(hresult);
        }
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(MsSpeech_FeedAudioContent, 2)
    T_HANDLE(MsSpeech_SpeechToText, 0)
    T_STRING(MsSpeech_SpeechToText, 1)

    struct STT_Container *container = (struct STT_Container *)(SYS_INT)PARAM(0);
    if ((container) && (container->haudioStream) && (container->hreco)) {
        int err = push_audio_input_stream_write(container->haudioStream, (uint8_t *)PARAM(1), PARAM_LEN(1));
        RETURN_NUMBER(err);
    } else {
        RETURN_NUMBER(-1);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MsSpeech_TTS, 5, 6)
    T_STRING(MsSpeech_TTS, 0);
    T_STRING(MsSpeech_TTS, 1);
    T_STRING(MsSpeech_TTS, 2);
    T_STRING(MsSpeech_TTS, 3);
    T_STRING(MsSpeech_TTS, 4);

    SPXSPEECHCONFIGHANDLE hconfig = NULL;
    speech_config_from_subscription(&hconfig, PARAM(2), PARAM(3));

    int ssml = 0;
    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(MsSpeech_TTS, 5);
        ssml = PARAM_INT(5);
    }

    RETURN_STRING("");
    if (hconfig) {
        SPXPROPERTYBAGHANDLE hpropbag = NULL;
        speech_config_get_property_bag(hconfig, &hpropbag);
        if (hpropbag) {
            property_bag_set_string(hpropbag, 3001, NULL, PARAM(0));
            property_bag_set_string(hpropbag, 3101, NULL, PARAM(1));
        }

        SPXSYNTHHANDLE hsynth = NULL;
        if (!synthesizer_create_speech_synthesizer_from_config(&hsynth, hconfig, SPXHANDLE_INVALID)) {
            SPXRESULTHANDLE hresult = NULL;
            if (ssml)
                synthesizer_speak_ssml(hsynth, PARAM(4), PARAM_LEN(4), &hresult);
            else
                synthesizer_speak_text(hsynth, PARAM(4), PARAM_LEN(4), &hresult);
            if (synthesizer_result_handle_is_valid(hresult)) {
                uint8_t *buffer = NULL;
                uint32_t bufferSize = 0;
                uint32_t filledSize = 0;
                uint32_t audioLength = 0;
                uint64_t audioDuration = 0;

                synth_result_get_audio_length_duration(hresult, &audioLength, &audioDuration);
                if (audioLength) {
                    CORE_NEW(audioLength + 1, buffer);
                    if (buffer) {
                        bufferSize = audioLength;
                        synth_result_get_audio_data(hresult, buffer, bufferSize, &filledSize);
                        if (filledSize > 0)
                            bufferSize = filledSize;

                        buffer[bufferSize] = 0;

                        SetVariable(RESULT, -1, (char *)buffer, bufferSize);
                    }
                }

                synthesizer_result_handle_release(hresult);
            }

            synthesizer_handle_release(hsynth);
        }
        
        if (speech_config_is_handle_valid(hconfig))
            speech_config_release(hconfig);
        if (hpropbag)
            property_bag_release(hpropbag);
    }
END_IMPL
//=====================================================================================//
