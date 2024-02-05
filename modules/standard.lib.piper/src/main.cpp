//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "piper.hpp"

struct piper::PiperConfig piper_config;

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    try {
        piper::initialize(piper_config);
    } catch (...) {
    }
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        try {
            piper::terminate(piper_config);
        } catch (...) {
        }
    }
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperInit, 2)
    T_STRING(PiperInit, 0);
    T_STRING(PiperInit, 1);

    struct piper::Voice *handle = new piper::Voice;
    if (handle) {
        try {
            std::optional<piper::SpeakerId> speaker_id;
            piper::loadVoice(piper_config, PARAM(0), PARAM(1), *handle, speaker_id, false);
       } catch (...) {
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

    struct piper::Voice *handle = (struct piper::Voice *)(SYS_INT)PARAM(0);
    try {
        textToAudio(piper_config, *handle, PARAM(1), audioBuffer, result, NULL);
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

    struct piper::Voice *handle = (struct piper::Voice *)(SYS_INT)PARAM(0);
    if (handle) {
        delete handle;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
