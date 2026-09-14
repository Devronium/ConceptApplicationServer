//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "needle.h"

//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
	return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
	return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(needle_init, 0, 2)
	const char *system_prompt = "";
	const char *tools_json = "";

	if (PARAMETERS_COUNT > 0) {
		T_STRING(needle_init, 0)
		if (PARAM(0))
			system_prompt = PARAM(0);
	}

	if (PARAMETERS_COUNT > 1) {
		T_STRING(needle_init, 1)
		if (PARAM(1))
			tools_json = PARAM(1);
	}


	int err = needle_init(system_prompt, tools_json, "");

	RETURN_NUMBER(err);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(needle_complete, 1, 2)
	T_STRING(needle_complete, 0)

	int max_tokens = 0x400;
	if (PARAMETERS_COUNT > 1) {
		T_NUMBER(needle_complete, 1);
		max_tokens = PARAM_INT(1);
		if (max_tokens <= 0)
			max_tokens = 0x400;
	}

	char out[0x4000];
	out[0] = 0;

	needle_complete(PARAM(0), max_tokens, out, sizeof(out) - 1);

	RETURN_STRING(out);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(needle_reset, 0)
	needle_reset();
	RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
