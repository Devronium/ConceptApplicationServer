//------------ standard header -----------------------------------//
#include "stdlibrary.h"
#include "pointer_list.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "llama.h"

#include <vector>
#include <string>
#include <math.h>
#include <algorithm>

DEFINE_LIST(llama_list);
DEFINE_LIST(llama_model_list);

struct chunk {
    INTEGER key;
    // original text data
    std::string textdata = "";
    // tokenized text data
    std::vector<llama_token> tokens;
    // embedding
    std::vector<float> embedding;
};

struct llama_container {
    llama_context *ctx;
    llama_model *model;
    std::vector<chunk> chunks;
    struct llama_context_params params;
};

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    INIT_LIST(llama_list);
    INIT_LIST(llama_model_list);
    llama_backend_init();
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        DEINIT_LIST(llama_list);
        DEINIT_LIST(llama_model_list);
        llama_backend_free();
    }
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(llama_load_model_from_file, 1)
    T_STRING(llama_load_model_from_file, 0)

    llama_model *model = llama_load_model_from_file(PARAM(0), llama_model_default_params());
    if (model) {
        RETURN_NUMBER(MAP_POINTER(llama_model_list, model, PARAMETERS->HANDLER));
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(llama_free_model, 1)
    T_HANDLE(llama_free_model, 0)

    llama_model *model = (llama_model *)FREE_POINTER(llama_model_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (model)
        llama_free_model(model);

    SET_NUMBER(0, 0); 

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(llama_new, 1, 2)
    // model file
    T_HANDLE(llama_new, 0)

    struct llama_context_params params = llama_context_default_params();
    params.n_ubatch = params.n_batch;
    params.embeddings = false;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(llama_new, 1)
        params.pooling_type = (enum llama_pooling_type)PARAM_INT(1);
        if (params.pooling_type)
            params.embeddings = true;
    }

    llama_model *model = (llama_model *)GET_POINTER(llama_model_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (!model) {
        RETURN_NUMBER(0)
        return 0;
    }

    llama_context *ctx = llama_new_context_with_model(model, params);
    if (!ctx) {
        RETURN_NUMBER(0)
        return 0;
    }

    struct llama_container *context = new llama_container;
    if (!context) {
        llama_free(ctx);
        RETURN_NUMBER(0)
        return 0;
    }
    context->model = model;
    context->ctx = ctx;
    context->params = params;

    RETURN_NUMBER(MAP_POINTER(llama_list, context, PARAMETERS->HANDLER));
END_IMPL
//=====================================================================================//
std::vector<llama_token> llama_tokenize_helper(
    const struct llama_model * model,
           const std::string & text,
                        bool   add_special,
                        bool   parse_special) {
    // upper limit for the number of tokens
    int n_tokens = text.length() + 2 * add_special;
    std::vector<llama_token> result(n_tokens);
    n_tokens = llama_tokenize(model, text.data(), text.length(), result.data(), result.size(), add_special, parse_special);
    if (n_tokens < 0) {
        result.resize(-n_tokens);
        int check = llama_tokenize(model, text.data(), text.length(), result.data(), result.size(), add_special, parse_special);
        GGML_ASSERT(check == -n_tokens);
    } else {
        result.resize(n_tokens);
    }
    return result;
}

void llama_embd_normalize(const float * inp, float * out, int n, int embd_norm = 2) {
    double sum = 0.0;

    switch (embd_norm) {
        case -1: // no normalisation
            sum = 1.0;
            break;
        case 0: // max absolute
            for (int i = 0; i < n; i++) {
                if (sum < std::abs(inp[i])) sum = std::abs(inp[i]);
            }
            sum /= 32760.0; // make an int16 range
            break;
        case 2: // euclidean
            for (int i = 0; i < n; i++) {
                sum += inp[i] * inp[i];
            }
            sum = std::sqrt(sum);
            break;
        default: // p-norm (euclidean is p-norm p=2)
            for (int i = 0; i < n; i++) {
                sum += std::pow(std::abs(inp[i]), embd_norm);
            }
            sum = std::pow(sum, 1.0 / embd_norm);
            break;
    }

    const float norm = sum > 0.0 ? 1.0 / sum : 0.0f;

    for (int i = 0; i < n; i++) {
        out[i] = inp[i] * norm;
    }
}

float llama_embd_similarity_cos(const float * embd1, const float * embd2, int n) {
    double sum  = 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0;

    for (int i = 0; i < n; i++) {
        sum  += embd1[i] * embd2[i];
        sum1 += embd1[i] * embd1[i];
        sum2 += embd2[i] * embd2[i];
    }

    // Handle the case where one or both vectors are zero vectors
    if (sum1 == 0.0 || sum2 == 0.0) {
        if (sum1 == 0.0 && sum2 == 0.0) {
            return 1.0f; // two zero vectors are similar
        }
        return 0.0f;
    }

    return sum / (sqrt(sum1) * sqrt(sum2));
}

void llama_batch_clear(struct llama_batch & batch) {
    batch.n_tokens = 0;
}

void llama_batch_add(
                 struct llama_batch & batch,
                        llama_token   id,
                          llama_pos   pos,
    const std::vector<llama_seq_id> & seq_ids,
                               bool   logits) {
    batch.token   [batch.n_tokens] = id;
    batch.pos     [batch.n_tokens] = pos;
    batch.n_seq_id[batch.n_tokens] = seq_ids.size();
    for (size_t i = 0; i < seq_ids.size(); ++i) {
        batch.seq_id[batch.n_tokens][i] = seq_ids[i];
    }
    batch.logits  [batch.n_tokens] = logits;

    batch.n_tokens++;
}

static void batch_add_seq(llama_batch & batch, const std::vector<int32_t> & tokens, llama_seq_id seq_id) {
    size_t n_tokens = tokens.size();
    for (size_t i = 0; i < n_tokens; i++) {
        llama_batch_add(batch, tokens[i], i, { seq_id }, true);
    }
}

static void batch_decode(llama_context * ctx, llama_batch & batch, float * output, int n_seq, int n_embd) {
    // clear previous kv_cache values (irrelevant for embeddings)
    llama_kv_cache_clear(ctx);

    // run model
    fprintf(stderr, "%s: n_tokens = %d, n_seq = %d\n", __func__, batch.n_tokens, n_seq);
    if (llama_decode(ctx, batch) < 0) {
        fprintf(stderr, "%s : failed to decode\n", __func__);
    }

    for (int i = 0; i < batch.n_tokens; i++) {
        if (!batch.logits[i]) {
            continue;
        }

        // try to get sequence embeddings - supported only when pooling_type is not NONE
        const float * embd = llama_get_embeddings_seq(ctx, batch.seq_id[i][0]);
        if (embd == NULL) {
            embd = llama_get_embeddings_ith(ctx, i);
            if (embd == NULL) {
                fprintf(stderr, "%s: failed to get embeddings for token %d\n", __func__, i);
                continue;
            }
        }

        float * out = output + batch.seq_id[i][0] * n_embd;
        llama_embd_normalize(embd, out, n_embd);
    }
}

std::string llama_token_to_piece(const struct llama_context * ctx, llama_token token, bool special = true) {
    std::string piece;
    piece.resize(piece.capacity());  // using string internal cache, 15 bytes + '\n'
    const int n_chars = llama_token_to_piece(llama_get_model(ctx), token, &piece[0], piece.size(), 0, special);
    if (n_chars < 0) {
        piece.resize(-n_chars);
        int check = llama_token_to_piece(llama_get_model(ctx), token, &piece[0], piece.size(), 0, special);
        GGML_ASSERT(check == -n_chars);
    }
    else {
        piece.resize(n_chars);
    }

    return piece;
}

CONCEPT_FUNCTION_IMPL(llama_load, 2)
    T_HANDLE(llama_query, 0)
    T_ARRAY(llama_query, 1)

    struct llama_container *ctx = (struct llama_container *)GET_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if ((!ctx) || (!ctx->ctx)) {
        RETURN_NUMBER(-1);
        return 0;
    }

    const enum llama_pooling_type pooling_type = llama_pooling_type(ctx->ctx);
    if (pooling_type == LLAMA_POOLING_TYPE_NONE) {
        fprintf(stderr, "%s: error: pooling type NONE not supported\n", __func__);
        RETURN_NUMBER(-1);
        return 0;
    }

    const int n_ctx_train = llama_n_ctx_train(ctx->model);
    const int n_ctx = llama_n_ctx(ctx->ctx);

    if (n_ctx > n_ctx_train) {
        fprintf(stderr, "%s: warning: model was trained on only %d context tokens (%d specified)\n", __func__, n_ctx_train, n_ctx);
        RETURN_NUMBER(-1);
        return 0;
    }

    INTEGER len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));

    if (len > 0) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;
        for (INTEGER i = 0; i < len; i++) {
            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(1), i, &type, &str, &nDummy);
            if (type == VARIABLE_STRING) {
                int len = (int)nDummy;
                if (len > 0) {
                    struct chunk current_chunk;
                    current_chunk.key = i;
                    current_chunk.textdata = str;
                    ctx->chunks.push_back(current_chunk);
                }
            }
        }
    }
    // max batch size
    const uint64_t n_batch = ctx->params.n_batch;

    // tokenize the prompts and trim
    for (auto & chunk : ctx->chunks) {
        auto inp = ::llama_tokenize_helper(ctx->model, chunk.textdata, true, false);
        if (inp.size() > n_batch) {
            fprintf(stderr, "%s: error: chunk size (%lld) exceeds batch size (%lld), increase batch size and re-run\n", __func__, (long long int) inp.size(), (long long int) n_batch);
            RETURN_NUMBER(-1);
            return 0;
        }
        // add eos if not present
        if (llama_token_eos(ctx->model) >= 0 && (inp.empty() || inp.back() != llama_token_eos(ctx->model))) {
            inp.push_back(llama_token_eos(ctx->model));
        }
        chunk.tokens = inp;
    }

    // initialize batch
    const int n_chunks = ctx->chunks.size();
    struct llama_batch batch = llama_batch_init(n_batch, 0, 1);

    // allocate output
    const int n_embd = llama_n_embd(ctx->model);
    std::vector<float> embeddings(n_chunks * n_embd, 0);
    float * emb = embeddings.data();

    // break into batches
    int p = 0; // number of prompts processed already
    int s = 0; // number of prompts in current batch
    for (int k = 0; k < n_chunks; k++) {
        // clamp to n_batch tokens
        auto & inp = ctx->chunks[k].tokens;

        const uint64_t n_toks = inp.size();

        // encode if at capacity
        if (batch.n_tokens + n_toks > n_batch) {
            float * out = emb + p * n_embd;
            batch_decode(ctx->ctx, batch, out, s, n_embd);
            llama_batch_clear(batch);
            p += s;
            s = 0;
        }

        // add to batch
        batch_add_seq(batch, inp, s);
        s += 1;
    }

    // final batch
    float * out = emb + p * n_embd;
    batch_decode(ctx->ctx, batch, out, s, n_embd);

    // save embeddings to chunks
    for (int i = 0; i < n_chunks; i++) {
        ctx->chunks[i].embedding = std::vector<float>(emb + i * n_embd, emb + (i + 1) * n_embd);
        // clear tokens as they are no longer needed
        ctx->chunks[i].tokens.clear();
    }

    RETURN_NUMBER(ctx->chunks.size());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(llama_query, 2, 5)
    T_HANDLE(llama_query, 0)
    T_STRING(llama_query, 1)

    CREATE_ARRAY(RESULT);

    int top_k = 3;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(llama_query, 2)
        top_k = PARAM_INT(2);
        if (top_k <= 0)
            top_k = 3;
    }
    if (PARAMETERS_COUNT > 3) {
        CREATE_ARRAY(PARAMETER(3));
    }
    if (PARAMETERS_COUNT > 4) {
        CREATE_ARRAY(PARAMETER(4));
    }

    struct llama_container *ctx = (struct llama_container *)GET_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if ((!ctx) || (!ctx->ctx) || (PARAM_LEN(1) <= 0))
        return 0;

    std::vector<int32_t> query_tokens = llama_tokenize_helper(llama_get_model(ctx->ctx), PARAM(1), true, false);

    const int n_chunks = ctx->chunks.size();
    const int n_embd = llama_n_embd(ctx->model);
    const uint64_t n_batch = ctx->params.n_batch;

    struct llama_batch query_batch = llama_batch_init(n_batch, 0, 1);

    batch_add_seq(query_batch, query_tokens, 0);

    std::vector<float> query_emb(n_embd, 0);
    batch_decode(ctx->ctx, query_batch, query_emb.data(), 1, n_embd);

    llama_batch_clear(query_batch);

    // compute cosine similarities
    {
        std::vector<std::pair<int, float>> similarities;
        for (int i = 0; i < n_chunks; i++) {
            float sim = llama_embd_similarity_cos(ctx->chunks[i].embedding.data(), query_emb.data(), n_embd);
            similarities.push_back(std::make_pair(i, sim));
        }

        // sort similarities
        std::sort(similarities.begin(), similarities.end(), [](const std::pair<int, float> & a, const std::pair<int, float> & b) {
            return a.second > b.second;
        });

        for (int i = 0; i < std::min(top_k, (int) ctx->chunks.size()); i++) {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ctx->chunks[similarities[i].first].textdata.c_str(), (NUMBER)ctx->chunks[similarities[i].first].textdata.length());
            if (PARAMETERS_COUNT > 3) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(3), (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char*)"", (NUMBER)similarities[i].second);
            }
            if (PARAMETERS_COUNT > 4) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, PARAMETER(4), (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char*)"", (NUMBER)ctx->chunks[similarities[i].first].key);
            }
        }
    }
    llama_batch_free(query_batch);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(llama_prompt, 2, 3)
    T_HANDLE(llama_query, 0)
    T_STRING(llama_query, 1)

    struct llama_container *ctx = (struct llama_container *)GET_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if ((!ctx) || (!ctx->ctx) || (PARAM_LEN(1) <= 0)) {
        RETURN_STRING("");
        return 0;
    }

    int n_predict = 32;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(llama_query, 2);
        n_predict = PARAM_INT(2);
    }

    std::vector<int32_t> tokens_list = llama_tokenize_helper(llama_get_model(ctx->ctx), PARAM(1), true, false);

    const int n_ctx    = llama_n_ctx(ctx->ctx);
    const int n_kv_req = tokens_list.size() + (n_predict - tokens_list.size());

    // make sure the KV cache is big enough to hold all the prompt and generated tokens
    if (n_kv_req > n_ctx) {
        RETURN_STRING("");
        return 0;
    }

    llama_batch batch = llama_batch_init(512, 0, 1);

    for (size_t i = 0; i < tokens_list.size(); i++) {
        llama_batch_add(batch, tokens_list[i], i, { 0 }, false);
    }

    batch.logits[batch.n_tokens - 1] = true;

    if (llama_decode(ctx->ctx, batch) != 0) {
        llama_batch_free(batch);
        RETURN_STRING("");
        return 0;
    }

    int n_cur    = batch.n_tokens;
    int n_decode = 0;

    const auto t_main_start = ggml_time_us();

    std::string data;
    while (n_cur <= n_predict) {
        // sample the next token
        {
            auto   n_vocab = llama_n_vocab(ctx->model);
            auto * logits  = llama_get_logits_ith(ctx->ctx, batch.n_tokens - 1);

            std::vector<llama_token_data> candidates;
            candidates.reserve(n_vocab);

            for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
                candidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
            }

            llama_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

            // sample the most likely token
            const llama_token new_token_id = llama_sample_token_greedy(ctx->ctx, &candidates_p);

            // is it an end of generation?
            if (llama_token_is_eog(ctx->model, new_token_id) || n_cur == n_predict)
                break;

            data += llama_token_to_piece(ctx->ctx, new_token_id).c_str();

            // prepare the next batch
            llama_batch_clear(batch);

            // push this new token for next evaluation
            llama_batch_add(batch, new_token_id, n_cur, { 0 }, true);

            n_decode += 1;
        }

        n_cur += 1;

        // evaluate the current batch with the transformer model
        if (llama_decode(ctx->ctx, batch)) {
            llama_batch_free(batch);
            RETURN_STRING("");
            return 0;
        }
    }
    RETURN_STRING(data.c_str());

    llama_batch_free(batch);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(llama_save_state, 1)
    T_HANDLE(llama_save_state, 0)

    struct llama_container *ctx = (struct llama_container *)GET_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if ((!ctx) || (!ctx->ctx)) {
        RETURN_STRING("");
        return 0;
    }

    char *ptr = NULL;
    size_t size = llama_state_get_size(ctx->ctx);
    CORE_NEW(size + 1, ptr);

    if (!ptr) {
        RETURN_STRING("");
        return 0;
    }

    const size_t written = llama_state_get_data(ctx->ctx, (uint8_t *)ptr, size);

    if (!written) {
        CORE_DELETE(ptr);

        RETURN_STRING("");
        return 0;
    }

    SetVariable(RESULT, -1, ptr, written);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(llama_load_state, 2)
    T_HANDLE(llama_load_state, 0)
    T_STRING(llama_load_state, 1)

    struct llama_container *ctx = (struct llama_container *)GET_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if ((!ctx) || (!ctx->ctx) || (PARAM_LEN(1) <= 0)) {
        RETURN_NUMBER(0);
        return 0;
    }

    if (llama_state_set_data(ctx->ctx, (const uint8_t *)PARAM(1), PARAM_LEN(1)) != PARAM_LEN(1)) {
        RETURN_NUMBER(0);
        return 0;
    }

    RETURN_NUMBER(1);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(llama_free, 1)
    T_HANDLE(llama_free, 0)

    struct llama_container *ctx = (struct llama_container *)FREE_POINTER(llama_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
    if (ctx) {
        llama_free(ctx->ctx);
        delete ctx;
    }
    SET_NUMBER(0, 0); 

    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
