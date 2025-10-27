//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "pointer_list.h"

#include <atomic>

std::atomic_flag atomic_flag = ATOMIC_FLAG_INIT;

#define USE_SILERO

#ifdef USE_SILERO
    #include "vad.cpp"
#else
    #include "ten-vad/ten_vad.cc"
#endif

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

#ifdef USE_SILERO
    VadIterator *owner = NULL;
#endif
    if (PARAMETERS_COUNT > 0) {
#ifdef USE_SILERO
        NUMBER vad_id = 0;
        GET_NUMBER(0, vad_id);
        if (TYPE == VARIABLE_NUMBER) {
#ifdef USE_POINTERS
            owner = (VadIterator *)(intptr_t)vad_id;
#else
            owner = (VadIterator *)GET_POINTER(vad_list, (SYS_INT)vad_id, PARAMETERS->HANDLER);
#endif
        } else {
            T_STRING(vad_new, 0);
            model = PARAM(0);
            model_len = PARAM_LEN(0);
        }
#else
        T_STRING(vad_new, 0);
        model = PARAM(0);
        model_len = PARAM_LEN(0);
        if (model_len > 0)
            return (void *)"vad_new: custom model not supported. Try recompiling library with USE_SILERO flag enabled.";
#endif
    }

    int sample_rate = 16000;
#ifdef USE_SILERO
    int windows_frame_size = 32;
#else
    int windows_frame_size = 0x100;
#endif
    float threshold = 0.5;
    int min_silence_duration_ms = 0;
    int speech_pad_ms = 32;
    int min_speech_duration_ms = 32;
    float max_speech_duration_s = std::numeric_limits<float>::infinity();

    if (PARAMETERS_COUNT > 1) {
        T_ARRAY(vad_new, 1);

        INTEGER type = 0;
        char    *str = 0;
        NUMBER  nr   = 0;

#define SET_VAD_PARAMETER(name) if (Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, PARAMETER(1), (INTEGER)-1, #name) == 1) { type = 0; str = NULL, nr = 0; Invoke(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, PARAMETER(1), #name, &type, &str, &nr); if (type == VARIABLE_NUMBER) name = nr; }
        SET_VAD_PARAMETER(sample_rate);
        SET_VAD_PARAMETER(windows_frame_size);
        SET_VAD_PARAMETER(threshold);
        SET_VAD_PARAMETER(min_silence_duration_ms);
        SET_VAD_PARAMETER(speech_pad_ms);
        SET_VAD_PARAMETER(min_speech_duration_ms);
        SET_VAD_PARAMETER(max_speech_duration_s);
    }

#ifdef USE_SILERO
    while (atomic_flag.test_and_set(std::memory_order_acquire)) { };
    try {
        VadIterator *vad = new VadIterator(owner, model, model_len, sample_rate, windows_frame_size, threshold, min_silence_duration_ms, speech_pad_ms, min_speech_duration_ms, max_speech_duration_s);
#ifdef USE_POINTERS
        RETURN_NUMBER((NUMBER)(intptr_t)vad);
#else
        RETURN_NUMBER(MAP_POINTER(vad_list, vad, PARAMETERS->HANDLER));
#endif
    } catch (...) {
        RETURN_NUMBER(0);
    }
    atomic_flag.clear(std::memory_order_release);
#else
    ten_vad_handle_t vad = NULL;
    if (ten_vad_create(&vad, windows_frame_size, threshold)) {
        RETURN_NUMBER(0);
    } else {
        RETURN_NUMBER(MAP_POINTER(vad_list, vad, PARAMETERS->HANDLER));
    }
#endif
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
#ifdef USE_SILERO
        std::vector<float> input_wav(len);
        std::vector<float> output_wav;

        unsigned char *data = (unsigned char *)PARAM(1);
        size_t idx = 0;
        for (int i = 0; i < PARAM_LEN(1); i += sizeof(int16_t)) {
            int16_t sample = *((int16_t *)&data[i]);
            input_wav[idx ++] = static_cast<float>(sample) / 32768;
        }

#ifdef USE_POINTERS
        VadIterator *vad = (VadIterator *)(intptr_t)PARAM(0);
#else
        VadIterator *vad = (VadIterator *)GET_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
#endif
        if (!vad)
            return (void *)"vad handle is not valid";

        while (atomic_flag.test_and_set(std::memory_order_acquire)) { };
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

                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "start", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(stamps[i].start * 2));
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "end", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(stamps[i].end * 2));
                }
            }
        }
        atomic_flag.clear(std::memory_order_release);

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
#else
        ten_vad_handle_t vad = (ten_vad_handle_t)GET_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);

        int pcm_size = PARAM_LEN(1) / 2;

        int buffer_len = 0;
        char *frame = PARAM(1);

        if (vad) {
            Aed_St *ptr = (Aed_St *)vad;

            int frame_num = ptr->stCfg.hopSz;

            char *buffer = (char *)malloc(PARAM_LEN(1) + 10);
            if (!buffer)
                return (void *)"vad_process: error allocationg memory";

            int start = 0;

            int index = 0;
            INTEGER array_index = 0;
            void *var = NULL;
            while (pcm_size >= frame_num) {
                float out_probs = 0;
                int32_t out_flags = 0;

                if (!ten_vad_process(vad, (const int16_t *)frame, frame_num, &out_probs, &out_flags)) {
                    if (out_flags) {
                        if (out_flags) {
                            memcpy(buffer + buffer_len, frame, frame_num * 2);
                            buffer_len += frame_num * 2;

                            if (!start)
                                start = index;
                        }
                    } else
                    if (start) {
                        if (PARAMETERS_COUNT > 2) {
                            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), array_index, &var);
                            array_index ++;
                            if (var) {
                                CREATE_ARRAY(var);

                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "start", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(start * 2));
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "end", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(index * 2));
                            }
                        }
                        start = 0;
                    }
                }

                index += frame_num;
                frame += frame_num * 2;
                pcm_size -= frame_num;
            }
            if (start) {
                if (PARAMETERS_COUNT > 2) {
                    Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(2), array_index, &var);
                    if (var) {
                        CREATE_ARRAY(var);

                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "start", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(start * 2));
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "end", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(index * 2));
                    }
                }
            }
            buffer[buffer_len] = 0;
            RETURN_BUFFER(buffer, buffer_len);

            free(buffer);

            return 0;
        }
#endif
    }
    RETURN_STRING("");
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(vad_free, 1)
    T_HANDLE(fad_free, 0);

#ifdef USE_SILERO
#ifdef USE_POINTERS
    VadIterator *vad = (VadIterator *)(intptr_t)PARAM(0);
#else
    VadIterator *vad = (VadIterator*)(VadIterator *)FREE_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
#endif

    if (vad) {
        while (atomic_flag.test_and_set(std::memory_order_acquire)) { }
        delete vad;
        atomic_flag.clear(std::memory_order_release);
    }
#else
    ten_vad_handle_t vad = (ten_vad_handle_t)FREE_POINTER(vad_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (vad)
        ten_vad_destroy(&vad);
#endif

    SET_NUMBER(0, 0);

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
