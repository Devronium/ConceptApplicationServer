//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "radixsort.h"
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(radix_sort, 1, 3)
    T_ARRAY(radix_sort, 0)

    uint32_t *keys = 0;
    uint32_t *keys_out = 0;
    float *numbers = 0;
    float *numbers_out = 0;

    INTEGER reversed = 0;
    INTEGER max_elements = -1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(radix_sort, 1)

        reversed = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(radix_sort, 2)

        max_elements = PARAM_INT(2);
    }

    INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    if (count > 0) {
        keys = (uint32_t *)malloc(sizeof(uint32_t) * count);
        keys_out = (uint32_t *)malloc(sizeof(uint32_t) * count);
        numbers = (float *)malloc(sizeof(float) * count);
        numbers_out = (float *)malloc(sizeof(float) * count);
    }

    CREATE_ARRAY(RESULT);

    if ((keys) && (numbers) && (numbers_out) && (keys_out) && (count > 0)) {
        for (INTEGER i = 0; i < count; i++) {
            char *str = 0;
            NUMBER number;
            INTEGER type = -1;

            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(0), i, &type, &str, &number);

            keys[i] = (uint32_t)i;
            if (type == VARIABLE_NUMBER)
                numbers[i] = number;
            else
                numbers[i] = 0;
        }

        if (radix11sort_f32(numbers, numbers_out, keys, keys_out, count) > 0) {
            INTEGER start = 0;
            if (max_elements > count)
                max_elements = count;
            else
            if (max_elements > 0) {
                if (reversed)
                    start = count - max_elements;
                else
                    count = max_elements;
            }
            for (INTEGER i = start; i < count; i++) {
                if (reversed)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)(count - i - 1), VARIABLE_NUMBER, "", (NUMBER)numbers_out[i]);
                else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, VARIABLE_NUMBER, "", (NUMBER)numbers_out[i]);
            }
        }
    }
    if (keys)
        free(keys);
    if (keys_out)
        free(keys_out);
    if (numbers)
        free(numbers);
    if (numbers_out)
        free(numbers_out);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(radix_sort_key_value, 1, 3)
    T_ARRAY(radix_sort_key_value, 0)

    uint32_t *keys = 0;
    uint32_t *keys_out = 0;
    float *numbers = 0;
    float *numbers_out = 0;

    INTEGER reversed = 0;
    INTEGER max_elements = -1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(radix_sort_key_value, 1)

        reversed = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(radix_sort_key_value, 2)

        max_elements = PARAM_INT(2);
    }

    INTEGER count = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    if (count > 0) {
        keys = (uint32_t *)malloc(sizeof(uint32_t) * count);
        keys_out = (uint32_t *)malloc(sizeof(uint32_t) * count);
        numbers = (float *)malloc(sizeof(float) * count);
        numbers_out = (float *)malloc(sizeof(float) * count);
    }

    CREATE_ARRAY(RESULT);

    if ((keys) && (numbers) && (numbers_out) && (keys_out) && (count > 0)) {
        for (INTEGER i = 0; i < count; i++) {
            char *str = 0;
            NUMBER number;
            INTEGER type = -1;

            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(0), i, &type, &str, &number);

            keys[i] = (uint32_t)i;
            if (type == VARIABLE_NUMBER)
                numbers[i] = number;
            else
                numbers[i] = 0;
        }

        if (radix11sort_f32(numbers, numbers_out, keys, keys_out, count) > 0) {
            char **keys_str = (char **)malloc(sizeof(char *) * count);
            if (keys_str) {
                Invoke(INVOKE_ARRAY_KEYS, PARAMETER(0), keys_str, (INTEGER)count);
                INTEGER start = 0;
                if (max_elements > count)
                    max_elements = count;
                else
                if (max_elements > 0) {
                    if (reversed)
                        start = count - max_elements;
                    else
                        count = max_elements;
                }
                for (INTEGER i = start; i < count; i++) {
                    uint32_t index = keys_out[i];
                    const char *key = keys_str[index] ? keys_str[index] : "";

                    if (reversed)
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)(count - i - 1), VARIABLE_STRING, key, (NUMBER)0);
                    else
                        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, VARIABLE_STRING, key, (NUMBER)0);
                }
                free(keys_str);
            }
        }
    }
    if (keys)
        free(keys);
    if (keys_out)
        free(keys_out);
    if (numbers)
        free(numbers);
    if (numbers_out)
        free(numbers_out);
END_IMPL
//---------------------------------------------------------------------------
