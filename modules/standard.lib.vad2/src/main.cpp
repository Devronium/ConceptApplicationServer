//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "pointer_list.h"

#include "vad.cpp"

DEFINE_LIST(vad_list);
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(vad_list);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        DEINIT_LIST(vad_list);
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(vad_new, 0, 2)
    const char *model = NULL;
    size_t model_len = 0;

    if (PARAMETERS_COUNT > 0) {
        T_STRING(vad_new, 0);
        model = PARAM(0);
        model_len = PARAM_LEN(0);
    }
    int Sample_rate = 16000;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(vad_new, 1);
        Sample_rate = PARAM_INT(1);
    }
    try {
        VadIterator *vad = new VadIterator(model, model_len, Sample_rate);
        RETURN_NUMBER(MAP_POINTER(vad_list, vad, PARAMETERS->HANDLER));
    } catch (...) {
        RETURN_NUMBER(0);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(vad_process, 2, 3)
    T_HANDLE(vad_process, 0);
    T_STRING(vad_process, 1);

    if (PARAMETERS_COUNT > 2) {
        CREATE_ARRAY(PARAMETER(2));
    }

    size_t len = PARAM_LEN(1) / 2;

    if (len > 0) {
        std::vector<float> input_wav(len);
        std::vector<float> output_wav;

        unsigned char *data = (unsigned char *)PARAM(1);
        size_t idx = 0;
        for (int i = 0; i < PARAM_LEN(1); i += sizeof(int16_t)) {
            int16_t sample = *((int16_t *)&data[i]);
            input_wav[idx ++] = static_cast<float>(sample) / 32768;
        }

        VadIterator *vad = (VadIterator *)GET_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);

        try {
            vad->process(input_wav, output_wav);
        } catch (...) {
            // nothing
        }

        if (PARAMETERS_COUNT > 2) {
            std::vector<timestamp_t> stamps = vad->get_speech_timestamps();
            for (INTEGER i = 0; i < stamps.size(); i++) {
                void *var = NULL;
    
                Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), i, &var);
                if (var) {
                    CREATE_ARRAY(var);

                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "start", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stamps[i].start * 2);
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "end", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)stamps[i].end * 2);
                }
            }
        }

        len = output_wav.size();
        if (len > 0) {
            unsigned char *ptr = NULL;
            CORE_NEW((len * 2) + 1, ptr);
            if (ptr) {
                for (size_t i = 0; i < len; i ++)
                    *((int16_t *)&ptr[i * 2]) = output_wav[i] * 32768;

                ptr[output_wav.size()] = 0;
            }
            SetVariable(RESULT, -1, (const char *)ptr, len * 2);
            return 0;
        }
    }
    RETURN_STRING("");
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(vad_free, 1)
    T_HANDLE(fad_free, 0);

    VadIterator *vad = (VadIterator *)FREE_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);

    if (vad)
        delete vad;

    SET_NUMBER(0, 0);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
