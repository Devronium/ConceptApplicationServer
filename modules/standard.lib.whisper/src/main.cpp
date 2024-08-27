//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "pointer_list.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "whisper.h"

#include <string>
#include <vector>

DEFINE_LIST(whisper);

struct whisper_params {
    int32_t n_threads    = 1;
    int32_t offset_t_ms  =  0;
    int32_t offset_n     =  0;
    int32_t duration_ms  =  0;
    int32_t max_context  = -1;
    int32_t max_len      =  0;
    int32_t best_of      =  5;
    int32_t beam_size    =  5;

    float word_thold    =  0.01f;
    float entropy_thold =  2.40f;
    float logprob_thold = -1.00f;

    bool speed_up       = false;
    bool translate      = false;
    bool diarize        = false;
    bool split_on_word  = false;
    bool no_fallback    = false;
    bool output_txt     = false;
    bool output_vtt     = false;
    bool output_srt     = false;
    bool output_wts     = false;
    bool output_csv     = false;
    bool print_special  = false;
    bool print_colors   = false;
    bool print_progress = false;
    bool no_timestamps  = false;
};


struct stt_context {
    struct whisper_context *ctx;
    whisper_full_params wparams;
    int running;
    int main_context;
};

static void src_short_to_float_array (const short *in, float *out, int len) {
	while (len) {
        len -- ;
		out [len] = (float) (in [len] / (1.0 * 0x8000)) ;
    }
}
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(whisper);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER)
        DEINIT_LIST(whisper);
    return 0;
}
//=====================================================================================//
void whisper_print_segment_callback(struct whisper_context * ctx, struct whisper_state *, int n_new, void *user_data) {
    struct stt_context *whisper_ctx = (struct stt_context *)user_data;
    if ((whisper_ctx) && (whisper_ctx->running == 1))
        whisper_ctx->running = 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WhisperCreate, 2, 6)
    T_STRING(WhisperCreate, 0)

    int lang_id = -1;
    int threads = 0;
    int translate = 0;
    int running_flag = 1;

    const char *lang = "auto";

    struct stt_context *owner_ctx = NULL;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(WhisperCreate, 1)
        if (PARAM_LEN(1) > 0) {
            lang = PARAM(1);
            lang_id = whisper_lang_id(lang);
            if (lang_id == -1) {
                RETURN_NUMBER(0);
                return 0;
            }
        }
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(WhisperCreate, 2)
        translate = PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(WhisperCreate, 3)
        threads = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(WhisperCreate, 4)
        if (PARAM_INT(4))
            running_flag = 2;
    }

    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(WhisperCreate, 5)
        owner_ctx = (struct stt_context *)GET_POINTER(whisper, (SYS_INT)PARAM(5), PARAMETERS->HANDLER);
    }


    struct whisper_context *ctx = NULL;
    int ctx_created = 0;

    if (owner_ctx)
        ctx = owner_ctx->ctx;
    
    if (!ctx) {
        struct whisper_context_params whisper_params = whisper_context_default_params();
#ifdef GGML_USE_CUDA
        params.use_gpu = true;
#endif
        ctx = whisper_init_from_file_with_params(PARAM(0), whisper_params);
        if (!ctx) {
            RETURN_NUMBER(0);
            return 0;
        }
        ctx_created = 1;
    }

    struct stt_context *whisper_ctx = (struct stt_context *)malloc(sizeof(struct stt_context));
    if (!whisper_ctx) {
        whisper_free(ctx);
        RETURN_NUMBER(0);
        return 0;
    }

    fprintf(stderr, "system_info: n_threads = %d | %s\n", threads, whisper_print_system_info());

    whisper_ctx->ctx = ctx;
    whisper_ctx->main_context = ctx_created;

    whisper_ctx->wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    whisper_ctx->wparams.strategy = WHISPER_SAMPLING_GREEDY;


    whisper_params params;

    whisper_ctx->wparams.print_realtime   = false;
    whisper_ctx->wparams.print_progress   = 0;
    whisper_ctx->wparams.print_timestamps = 0;
    whisper_ctx->wparams.print_special    = 0;
    whisper_ctx->wparams.translate        = translate;
    whisper_ctx->wparams.language         = lang ? strdup(lang) : NULL;
    whisper_ctx->wparams.n_threads        = threads;
    whisper_ctx->wparams.n_max_text_ctx   = params.max_context >= 0 ? params.max_context : whisper_ctx->wparams.n_max_text_ctx;
    whisper_ctx->wparams.offset_ms        = params.offset_t_ms;
    whisper_ctx->wparams.duration_ms      = params.duration_ms;

    whisper_ctx->wparams.token_timestamps = params.output_wts || params.max_len > 0;
    whisper_ctx->wparams.thold_pt         = params.word_thold;
    whisper_ctx->wparams.max_len          = params.output_wts && params.max_len == 0 ? 60 : params.max_len;
    whisper_ctx->wparams.split_on_word    = params.split_on_word;

    whisper_ctx->wparams.speed_up         = params.speed_up;

    whisper_ctx->wparams.prompt_tokens     = NULL;
    whisper_ctx->wparams.prompt_n_tokens   = 0;

    whisper_ctx->wparams.greedy.best_of        = params.best_of;
    whisper_ctx->wparams.beam_search.beam_size = -1;

    whisper_ctx->wparams.temperature_inc  = 0.0f;
    whisper_ctx->wparams.entropy_thold    = params.entropy_thold;
    whisper_ctx->wparams.logprob_thold    = params.logprob_thold;

    whisper_ctx->wparams.new_segment_callback           = whisper_print_segment_callback;
    whisper_ctx->wparams.new_segment_callback_user_data = whisper_ctx;

    whisper_ctx->running = running_flag;

    whisper_ctx->wparams.encoder_begin_callback = [](struct whisper_context * /*ctx*/, struct whisper_state * /*state*/, void * user_data) {
        struct stt_context *ctx = (struct stt_context *)user_data;
        if ((ctx) && (!ctx->running))
            return false;

        return true;
    };
    whisper_ctx->wparams.encoder_begin_callback_user_data = whisper_ctx;

    RETURN_NUMBER(MAP_POINTER(whisper, whisper_ctx, PARAMETERS->HANDLER));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(WhisperSampleRate, 0)
    RETURN_NUMBER(WHISPER_SAMPLE_RATE);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(WhisperDecode, 2, 3)
    T_HANDLE(WhisperDecode, 0)
    T_STRING(WhisperDecode, 1)

    struct stt_context *ctx = (struct stt_context *)GET_POINTER(whisper, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!ctx) {
        RETURN_STRING("");
        return 0;
    }

    const char *lang = NULL;

    short *buf = (short *)PARAM(1);

    int len = PARAM_LEN(1)/ 2;
    if (!len) {
        RETURN_STRING("");
        return 0;
    }

    if (PARAMETERS_COUNT > 2) {
        T_STRING(WhisperDecode, 2)
        lang = PARAM(2);
        if ((!lang) || (!lang[0]) || (whisper_lang_id(lang) == -1))
            lang = NULL;
    }

    float *input = (float *)malloc((len + 1) * sizeof(float));
    src_short_to_float_array((short *)buf, input, len);

    whisper_full_params wparams = ctx->wparams;

    if (lang)
        wparams.language = lang;

    wparams.offset_ms = 0;
    wparams.duration_ms = len / (WHISPER_SAMPLE_RATE / 1000);

    struct stt_context ref_ctx = *ctx;

    wparams.new_segment_callback_user_data = &ref_ctx;
    wparams.encoder_begin_callback_user_data = &ref_ctx;

    struct whisper_state *state = whisper_init_state(ctx->ctx);
    if (whisper_full_with_state(ctx->ctx, state, wparams, input, len) != 0) {
        whisper_free_state(state);
        free(input);
        RETURN_STRING("");
        return 0;
    }
    free(input);

    std::string str;
    const int n_segments = whisper_full_n_segments_from_state(state);
    for (int i = 0; i < n_segments; ++i) {
        const char * text = whisper_full_get_segment_text_from_state(state, i);
        if (str.length())
            str += "\n";
        str += text;
    }

    whisper_free_state(state);
    RETURN_STRING(str.c_str());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(WhisperFree, 1)
    T_HANDLE(WhisperFree, 0)

    struct stt_context *ctx = (struct stt_context *)FREE_POINTER(whisper, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (ctx) {
        free((void *)ctx->wparams.language);
        if (ctx->main_context)
            whisper_free(ctx->ctx);
        free(ctx);
    }
    SET_NUMBER(0, 0); 

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
