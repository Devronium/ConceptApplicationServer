//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <rwkv.h>
#include "tokenizer.h"
#include "typical.h"
#include "simple_tokenizer.h"
#include "AnsiString.h"

class rwkv_user_token {
public:
	std::optional<GPT2Tokenizer> tokenizer;
	int use_simple_tokenizer;
};

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
	DEFINE_ECONSTANT(RWKV_ERROR_NONE)

	DEFINE_ECONSTANT(RWKV_ERROR_ARGS)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE)
	DEFINE_ECONSTANT(RWKV_ERROR_MODEL)
	DEFINE_ECONSTANT(RWKV_ERROR_MODEL_PARAMS)
	DEFINE_ECONSTANT(RWKV_ERROR_GRAPH)
	DEFINE_ECONSTANT(RWKV_ERROR_CTX)

	DEFINE_ECONSTANT(RWKV_ERROR_ALLOC)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_OPEN)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_STAT)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_READ)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_WRITE)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_MAGIC)
	DEFINE_ECONSTANT(RWKV_ERROR_FILE_VERSION)
	DEFINE_ECONSTANT(RWKV_ERROR_DATA_TYPE)
	DEFINE_ECONSTANT(RWKV_ERROR_UNSUPPORTED)
	DEFINE_ECONSTANT(RWKV_ERROR_SHAPE)
	DEFINE_ECONSTANT(RWKV_ERROR_DIMENSION)
	DEFINE_ECONSTANT(RWKV_ERROR_KEY)
	DEFINE_ECONSTANT(RWKV_ERROR_DATA)
	DEFINE_ECONSTANT(RWKV_ERROR_PARAM_MISSING)

	tokenizer_init();
	return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
	return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_set_print_errors, 2)
	T_NUMBER(rwkv_set_print_errors, 0)
	T_NUMBER(rwkv_set_print_errors, 1)

	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(0);
	int print_errors = PARAM_INT(1);

	rwkv_set_print_errors(ctx, (bool)print_errors);

	RETURN_NUMBER((SYS_INT)0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_get_print_errors, 1)
	T_NUMBER(rwkv_get_print_errors, 0)

	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(0);
	int print_errors = (int)rwkv_get_print_errors(ctx);

	RETURN_NUMBER(print_errors);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_get_last_error, 1)
	T_NUMBER(rwkv_get_last_error, 0)

	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(0);
	int error = (int)rwkv_get_last_error(ctx);

	RETURN_NUMBER(error);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(rwkv_init_from_file, 2, 3)
	T_STRING(rwkv_init_from_file, 0)
	T_NUMBER(rwkv_init_from_file, 1)
	int gpu_layers = 0;
	if (PARAMETERS_COUNT > 2) {
		T_NUMBER(rwkv_init_from_file, 2)
		gpu_layers = PARAM_INT(2);
		if (gpu_layers < 0)
			gpu_layers = 0;
	}

	struct rwkv_context *ctx = rwkv_init_from_file(PARAM(0), (uint32_t)PARAM_INT(1), (uint32_t)gpu_layers);

	RETURN_NUMBER((SYS_INT)ctx);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_clone_context, 2)
	T_HANDLE(rwkv_clone_context, 0)
	T_NUMBER(rwkv_clone_context, 1)

	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(0);
	struct rwkv_context *ctx_cloned = rwkv_clone_context(ctx, (uint32_t)PARAM_INT(1));

	RETURN_NUMBER((SYS_INT)ctx_cloned);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_free, 1)
	T_HANDLE(rwkv_free, 0)

	rwkv_free((struct rwkv_context *)(SYS_INT)PARAM(0));
	SET_NUMBER(0, 0);

	RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_quantize_model_file, 3)
	T_STRING(rwkv_quantize_model_file, 0)
	T_STRING(rwkv_quantize_model_file, 1)
	T_STRING(rwkv_quantize_model_file, 2)

	int bool_error = rwkv_quantize_model_file(PARAM(0), PARAM(1), PARAM(2));

	RETURN_NUMBER(bool_error);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_get_system_info_string, 0)
	RETURN_STRING(rwkv_get_system_info_string());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(rwkv_tokenizer_load, 0, 2)
	char *fname = NULL;
	char *merges = NULL;
	if (PARAMETERS_COUNT > 0) {
		T_STRING(rwkv_tokenizer_load, 0)
		fname = PARAM(0);
	}
	if (PARAMETERS_COUNT > 1) {
		T_STRING(rwkv_tokenizer_load, 1)
		merges = PARAM(1);
	}

	rwkv_user_token *tokenizer = NULL;
	if ((fname) && (fname[0]) && (merges) && (merges[0])) {
		std::optional<GPT2Tokenizer> tokenizerop = GPT2Tokenizer::load(fname, merges);
		if (!tokenizerop.has_value()) {
			RETURN_NUMBER(0);
			return 0;
		};
		tokenizer = new rwkv_user_token();
		tokenizer->tokenizer.emplace(tokenizerop.value());
		tokenizer->use_simple_tokenizer = 0;
	} else {
		tokenizer = new rwkv_user_token();
		tokenizer->use_simple_tokenizer = 1;
	}

	RETURN_NUMBER((SYS_INT)tokenizer);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(rwkv_tokenizer_initial_context, 3, 4)
	T_NUMBER(rwkv_tokenizer_initial_context, 0)
	T_HANDLE(rwkv_tokenizer_initial_context, 1)
	T_STRING(rwkv_tokenizer_initial_context, 2)

	rwkv_user_token *tokenizer = (rwkv_user_token *)(SYS_INT)PARAM(0);
	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(1);
	
        float *state = NULL;

	if (PARAMETERS_COUNT > 3) {
		T_NUMBER(rwkv_tokenizer_initial_context, 3)
		state = (float *)(SYS_INT)PARAM(3);
	}

	if (!state) {
		state = (float *)calloc(rwkv_get_state_len(ctx), sizeof(float));
		rwkv_init_state(ctx, state);
	}

	if ((!tokenizer) || (tokenizer->use_simple_tokenizer)) {
		int size = 0;
		uint32_t *sequence = encode_sequence(PARAM(2), &size);
		rwkv_eval_sequence(ctx, sequence, size, state, state, NULL);
		free(sequence);
	} else {
		GPT2Tokenizer gpt_tokenizer = tokenizer->tokenizer.value();
		std::vector<long long> initial = gpt_tokenizer.encode(PARAM(2));

		uint32_t *sequence = new uint32_t [initial.size() + 1];

		for (int i = 0; i < initial.size(); i ++)
			sequence[i] = (uint32_t)initial[i];

		rwkv_eval_sequence(ctx, sequence, initial.size(), state, state, NULL);
		delete[] sequence;
	}

	RETURN_NUMBER((SYS_INT)state);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_duplicate_state, 2)
	T_HANDLE(rwkv_duplicate_state, 0)
	T_NUMBER(rwkv_duplicate_state, 1)

	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(0);
	int len = rwkv_get_state_len(ctx);
	float *state = (float *)calloc(len, sizeof(float));

	float *source = (float *)(SYS_INT)PARAM(1);
	if (source) {
		for (int i = 0; i < len; i ++)
			state[i] = source[i];
	} else {
		rwkv_init_state(ctx, state);
	}
	RETURN_NUMBER((SYS_INT)state);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_free_state, 1)
	T_HANDLE(rwkv_free_state, 0)

	float *state = (float *)(SYS_INT)PARAM(0);
	if (state) {
		free(state);
		SET_NUMBER(0, 0);
	}
	RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(rwkv_run, 3, 11)
	T_NUMBER(rwkv_tokenizer_run, 0)
	T_HANDLE(rwkv_tokenizer_run, 1)
	T_STRING(rwkv_tokenizer_run, 2)

	int MAX_GENERATION_LENGTH = 250;
	float TEMPERATURE = 0.8;
	float TOP_P = 0.5;
	float PRESENCE_PENALTY = 0.2;
	float FREQUENCY_PENALTY = 0.2;
	const char *stop_at = NULL;
	void *delegate = NULL;

	float *state = NULL;
	if (PARAMETERS_COUNT > 3) {
		T_NUMBER(rwkv_tokenizer_run, 3)
		state = (float *)(SYS_INT)PARAM(3);
	}

	if (PARAMETERS_COUNT > 4) {
		T_STRING(rwkv_tokenizer_run, 4)
		stop_at = PARAM(4);
		if ((stop_at) && (!stop_at[0]))
			stop_at = NULL;
	}

	if (PARAMETERS_COUNT > 5) {
		T_NUMBER(rwkv_tokenizer_run, 5)

		MAX_GENERATION_LENGTH = PARAM_INT(5);
		if (MAX_GENERATION_LENGTH <= 0)
			MAX_GENERATION_LENGTH = 250;
	}

	if (PARAMETERS_COUNT > 6) {
		T_NUMBER(rwkv_tokenizer_run, 6)
		TEMPERATURE = PARAM(6);
	}

	if (PARAMETERS_COUNT > 7) {
		T_NUMBER(rwkv_tokenizer_run, 7)
		TOP_P = PARAM(7);
	}

	if (PARAMETERS_COUNT > 8) {
		T_NUMBER(rwkv_tokenizer_run, 8)
		PRESENCE_PENALTY = PARAM(8);
	}

	if (PARAMETERS_COUNT > 9) {
		T_NUMBER(rwkv_tokenizer_run, 9)
		FREQUENCY_PENALTY = PARAM(9);
	}

	if (PARAMETERS_COUNT > 10) {
		T_DELEGATE(rwkv_tokenizer_run, 10)
		delegate = PARAMETER(10);
	}

	int END_OF_LINE_TOKEN = 187;
	int DOUBLE_END_OF_LINE_TOKEN = 535;
	int END_OF_TEXT_TOKEN = 0;

	rwkv_user_token *tokenizer = (rwkv_user_token *)(SYS_INT)PARAM(0);
	struct rwkv_context *ctx = (struct rwkv_context *)(SYS_INT)PARAM(1);

	int allocated_state = 0;
       	if (!state) {
		state = (float *)calloc(rwkv_get_state_len(ctx), sizeof(float));
		rwkv_init_state(ctx, state);
		allocated_state = 1;
	}

	float *logits = (float *)calloc(rwkv_get_logits_len(ctx), sizeof(float));

	RETURN_STRING("");

	int lasttoken = 0;
	int error = 0;

	if ((!tokenizer) || (tokenizer->use_simple_tokenizer)) {
		int size = 0;
		uint32_t *sequence = encode_sequence(PARAM(2), &size);
		error = (int) rwkv_eval_sequence(ctx, sequence, size, state, state, logits);
		free(sequence);
	} else {
		std::vector<long long> initial = tokenizer->tokenizer.value().encode(PARAM(2));

		uint32_t *sequence = new uint32_t [initial.size() + 1];

		for (int i = 0; i < initial.size(); i ++)
			sequence[i] = (uint32_t)initial[i];

		error = (int) rwkv_eval_sequence(ctx, sequence, initial.size(), state, state, logits);
		delete[] sequence;
	}

	std::string val;
	std::string str;
	std::vector<uint32_t> accumulated_tokens;
	std::map<int, int> token_counts;

	logits[END_OF_LINE_TOKEN] += -999999999;

	for (int i = 0; i < MAX_GENERATION_LENGTH; i ++) {
		for (std::map<int, int>::iterator iter = token_counts.begin(); iter != token_counts.end(); ++iter)
			logits[iter->first] -= PRESENCE_PENALTY + iter->second * FREQUENCY_PENALTY;

		lasttoken = typical(logits, TEMPERATURE, TOP_P);
		if (lasttoken == END_OF_TEXT_TOKEN)
			break;
		accumulated_tokens.push_back(lasttoken);

		if (token_counts.count(lasttoken))
			token_counts[lasttoken] ++;
		else
			token_counts[lasttoken] = 1;

		rwkv_eval(ctx, (uint32_t)lasttoken, state, state, logits);

		if ((!tokenizer) || (tokenizer->use_simple_tokenizer))
			val = decode_sequence(&accumulated_tokens[0], accumulated_tokens.size());
		else
			val = tokenizer->tokenizer.value().decode(accumulated_tokens);

		str += val;

		if (val.find("\uFFFD"))
			accumulated_tokens.clear();

		if (val.find("\n\n") != std::string::npos)
			break;

		if ((stop_at) && (val.find(stop_at) != std::string::npos))
			break;

		if (delegate) {
			void *RES       = 0;
			void *EXCEPTION = 0;

			Invoke(INVOKE_CALL_DELEGATE, delegate, &RES, &EXCEPTION, (INTEGER)2, (INTEGER)VARIABLE_STRING, (char *)str.c_str(), (double)str.size(), (INTEGER)VARIABLE_STRING, (char *)val.c_str(), (double)val.size());
			if (EXCEPTION) {
				FREE_VARIABLE(EXCEPTION);
				if (RES)
					FREE_VARIABLE(RES);
				break;
			}
			if (RES) {
				INTEGER    type     = 0;
				char       *szValue = 0;
				NUMBER     nValue   = 0;

				Invoke(INVOKE_GET_VARIABLE, RES, &type, &szValue, &nValue);

				FREE_VARIABLE(RES);

				if ((type != VARIABLE_NUMBER) || ((int)nValue != 0))
					break;
			}
		}
	}

	free(logits);

	if (allocated_state)
		free(state);

	RETURN_STRING(str.c_str());
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(rwkv_tokenizer_free, 1)
	T_HANDLE(rwkv_tokenizer_free, 0)

	rwkv_user_token *tokenizer = (rwkv_user_token *)(SYS_INT)PARAM(0);
	if (tokenizer) {
		delete tokenizer;
		SET_NUMBER(0, 0);
	}

	RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
