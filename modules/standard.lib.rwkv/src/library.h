#ifndef __LIBRARY_H
#define __LIBRARY_H

#include "stdlibrary.h"

extern "C" {
    CONCEPT_DLL_API ON_CREATE_CONTEXT  MANAGEMENT_PARAMETERS;
    CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS;

    CONCEPT_FUNCTION(rwkv_set_print_errors)
    CONCEPT_FUNCTION(rwkv_get_print_errors)
    CONCEPT_FUNCTION(rwkv_get_last_error)
    CONCEPT_FUNCTION(rwkv_init_from_file)
    CONCEPT_FUNCTION(rwkv_clone_context)
    CONCEPT_FUNCTION(rwkv_free)
    CONCEPT_FUNCTION(rwkv_quantize_model_file)
    CONCEPT_FUNCTION(rwkv_get_system_info_string)

    CONCEPT_FUNCTION(rwkv_duplicate_state)
    CONCEPT_FUNCTION(rwkv_free_state)

    CONCEPT_FUNCTION(rwkv_tokenizer_load)
    CONCEPT_FUNCTION(rwkv_tokenizer_initial_context)
    CONCEPT_FUNCTION(rwkv_run)
    CONCEPT_FUNCTION(rwkv_tokenizer_free)
}
#endif // __LIBRARY_H
