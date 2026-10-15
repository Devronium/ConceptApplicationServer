//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "pointer_list.h"

#include <omnivoice.h>

DEFINE_LIST(omnivoice_list);
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(omnivoice_list);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        DEINIT_LIST(omnivoice_list);
    }
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(OmnivoiceInit, 1, 4)
    T_STRING(OmnivoiceInit, 0);

    omnivoice::RuntimeOptions options;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(OmnivoiceInit, 1);
        options.backend = PARAM(1);
    }

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(OmnivoiceInit, 2);
        options.device = PARAM_INT(2);
    }

    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(OmnivoiceInit, 3);
        options.threads = PARAM_INT(3);
    }

    omnivoice::OmniVoiceRuntime *tts = NULL; 
    try {
        tts = new omnivoice::OmniVoiceRuntime(PARAM(0), options);
    } catch (const std::exception & e) {
        RETURN_NUMBER(0);
        return (void *)e.what();
    }

    RETURN_NUMBER(MAP_POINTER(omnivoice_list, tts, PARAMETERS->HANDLER));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(OmnivoiceTTS, 2)
    T_HANDLE(OmnivoiceTTS, 0);
    T_ARRAY(OmnivoiceTTS, 1);

    omnivoice::OmniVoiceRuntime *tts = (omnivoice::OmniVoiceRuntime *)GET_POINTER(omnivoice_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);

    omnivoice::SynthesisParams params;

    INTEGER type = 0;
    char    *str = 0;
    NUMBER  nr   = 0;

#define SET_STRING_PARAMETER(name) if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(1), (INTEGER)-1, #name) == 1) { type = 0; str = NULL, nr = 0; Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), #name, &type, &str, &nr); if (type == VARIABLE_STRING) params.name = str; }
#define SET_NUMBER_PARAMETER(name) if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(1), (INTEGER)-1, #name) == 1) { type = 0; str = NULL, nr = 0; Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), #name, &type, &str, &nr); if (type == VARIABLE_NUMBER) params.name = (float)nr; }
#define SET_NUMBER_PARAMETER2(name) if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(1), (INTEGER)-1, #name) == 1) { type = 0; str = NULL, nr = 0; Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), #name, &type, &str, &nr); if (type == VARIABLE_NUMBER) params.generation.name = (float)nr; }
#define SET_BOOLEAN_PARAMETER(name) if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(1), (INTEGER)-1, #name) == 1) { type = 0; str = NULL, nr = 0; Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), #name, &type, &str, &nr); if (type == VARIABLE_NUMBER) params.name = (bool)(int)nr; }

    SET_STRING_PARAMETER(text);
    SET_STRING_PARAMETER(language);
    SET_STRING_PARAMETER(instruct);
    SET_BOOLEAN_PARAMETER(auto_voice);
    SET_NUMBER_PARAMETER(speed);
    SET_STRING_PARAMETER(ref_audio_path);
    SET_STRING_PARAMETER(ref_text);

    SET_NUMBER_PARAMETER2(num_step);
    SET_NUMBER_PARAMETER2(guidance_scale);
    SET_NUMBER_PARAMETER2(t_shift);
    SET_NUMBER_PARAMETER2(layer_penalty_factor);
    SET_NUMBER_PARAMETER2(position_temperature);
    SET_NUMBER_PARAMETER2(class_temperature);
    SET_NUMBER_PARAMETER2(audio_chunk_duration);
    SET_NUMBER_PARAMETER2(audio_chunk_threshold);
    SET_NUMBER_PARAMETER2(seed);
    SET_NUMBER_PARAMETER2(denoise);
    SET_NUMBER_PARAMETER2(preprocess_prompt);
    SET_NUMBER_PARAMETER2(postprocess_output);

    CREATE_ARRAY(RESULT);

    try {
        omnivoice::Audio audio = tts->generate(params);

        if (audio.samples.size() <= 0) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "data", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sample_rate", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)audio.sample_rate);
            return 0;
        }

        int16_t *pcm_data = NULL;
        CORE_NEW(audio.samples.size() * 2 + 1, pcm_data);
        pcm_data[audio.samples.size() * 2] = 0;

        int len = 0;
        for (float f : audio.samples) {
            f = std::max(-1.0f, std::min(1.0f, f));
            int16_t s = static_cast<int16_t>(std::lround(f * 32767.0f));
            pcm_data[len ++] = static_cast<uint16_t>(s);
        }

        void *data_var = NULL;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "data", &data_var);
        SetVariable(data_var, -1, (char *)pcm_data, len * 2);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sample_rate", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)audio.sample_rate);
    } catch (const std::exception & e) {
        return (void *)e.what();
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(OmnivoiceDone, 1)
    T_NUMBER(PiperDone, 0);

    omnivoice::OmniVoiceRuntime *handle = (omnivoice::OmniVoiceRuntime *)FREE_POINTER(omnivoice_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (handle) {
        delete handle;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
