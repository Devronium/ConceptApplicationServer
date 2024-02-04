//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "piper.hpp"

struct PiperContext {
    struct piper::PiperConfig piper_config;
    struct piper::Voice voice;
    piper::SpeakerId speaker;
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
CONCEPT_FUNCTION_IMPL(PiperInit, 2)
    T_STRING(PiperInit, 0);
    T_STRING(PiperInit, 1);

    struct PiperContext *handle = new PiperContext;
    if (handle) {
        try {
            piper::initialize(handle->piper_config);
            std::optional<piper::SpeakerId> speaker_id;
            piper::loadVoice(handle->piper_config, PARAM(0), PARAM(1), handle->voice, speaker_id, false);
       } catch (...) {
            piper::terminate(handle->piper_config);
            delete handle;
            handle = NULL;
       }
    }
    RETURN_NUMBER((SYS_INT)handle);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperTTS, 2)
    T_HANDLE(PiperTTS, 0);
    T_STRING(PiperTTS, 1);

    std::vector<int16_t> audioBuffer;
    piper::SynthesisResult result;

    struct PiperContext *handle = (struct PiperContext *)(SYS_INT)PARAM(0);
    try {
        textToAudio(handle->piper_config, handle->voice, PARAM(1), audioBuffer, result, NULL);
    } catch (...) {
    }

    if (audioBuffer.size() > 0) {
        RETURN_BUFFER((const char *)audioBuffer.data(), audioBuffer.size() * 2);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperDone, 1)
    T_NUMBER(PiperDone, 0);

    struct PiperContext *handle = (struct PiperContext *)(SYS_INT)PARAM(0);
    if (handle) {
        try {
            piper::terminate(handle->piper_config);
        } catch (...) {
        }
        delete handle;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
