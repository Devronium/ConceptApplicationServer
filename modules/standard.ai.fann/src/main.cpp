//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include <fann.h>


INVOKE_CALL InvokePtr = 0;

unsigned int *GetuintList(void *arr) {
    INTEGER      type      = 0;
    char         *str      = 0;
    NUMBER       nr        = 0;
    void         *newpData = 0;
    unsigned int *ret      = 0;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    ret = new unsigned int [count];

    for (int i = 0; i < count; i++) {
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_NUMBER) {
                ret[i] = (unsigned int)nData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------
fann_type *GetList(void *arr) {
    INTEGER   type      = 0;
    char      *str      = 0;
    NUMBER    nr        = 0;
    void      *newpData = 0;
    fann_type *ret      = 0;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    ret = new fann_type [count];

    for (int i = 0; i < count; i++) {
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (newpData) {
            char    *szData;
            INTEGER type;
            NUMBER  nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
            if (type == VARIABLE_NUMBER) {
                ret[i] = (fann_type)nData;
            } else
                ret[i] = 0;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------
void SetList(void *array_var, int count, fann_type *arr) {
    InvokePtr(INVOKE_CREATE_ARRAY, array_var);
    for (int i = 0; i < count; i++)
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, array_var, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)arr[i]);
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(FANN_TRAIN_INCREMENTAL)
    DEFINE_ECONSTANT(FANN_TRAIN_BATCH)
    DEFINE_ECONSTANT(FANN_TRAIN_RPROP)
    DEFINE_ECONSTANT(FANN_TRAIN_QUICKPROP)

    DEFINE_ECONSTANT(FANN_LINEAR)
    DEFINE_ECONSTANT(FANN_THRESHOLD)
    DEFINE_ECONSTANT(FANN_THRESHOLD_SYMMETRIC)
    DEFINE_ECONSTANT(FANN_SIGMOID)
    DEFINE_ECONSTANT(FANN_SIGMOID_STEPWISE)
    DEFINE_ECONSTANT(FANN_SIGMOID_SYMMETRIC)
    DEFINE_ECONSTANT(FANN_SIGMOID_SYMMETRIC_STEPWISE)
    DEFINE_ECONSTANT(FANN_GAUSSIAN)
    DEFINE_ECONSTANT(FANN_GAUSSIAN_SYMMETRIC)
    DEFINE_ECONSTANT(FANN_GAUSSIAN_STEPWISE)
    DEFINE_ECONSTANT(FANN_ELLIOT)
    DEFINE_ECONSTANT(FANN_ELLIOT_SYMMETRIC)
    DEFINE_ECONSTANT(FANN_LINEAR_PIECE)
    DEFINE_ECONSTANT(FANN_LINEAR_PIECE_SYMMETRIC)

    DEFINE_ECONSTANT(FANN_ERRORFUNC_LINEAR)
    DEFINE_ECONSTANT(FANN_ERRORFUNC_TANH)

    DEFINE_ECONSTANT(FANN_STOPFUNC_MSE)
    DEFINE_ECONSTANT(FANN_STOPFUNC_BIT)

    DEFINE_ECONSTANT(FANN_E_NO_ERROR)
    DEFINE_ECONSTANT(FANN_E_CANT_OPEN_CONFIG_R)
    DEFINE_ECONSTANT(FANN_E_CANT_OPEN_CONFIG_W)
    DEFINE_ECONSTANT(FANN_E_WRONG_CONFIG_VERSION)
    DEFINE_ECONSTANT(FANN_E_CANT_READ_CONFIG)
    DEFINE_ECONSTANT(FANN_E_CANT_READ_NEURON)
    DEFINE_ECONSTANT(FANN_E_CANT_READ_CONNECTIONS)
    DEFINE_ECONSTANT(FANN_E_WRONG_NUM_CONNECTIONS)
    DEFINE_ECONSTANT(FANN_E_CANT_OPEN_TD_W)
    DEFINE_ECONSTANT(FANN_E_CANT_OPEN_TD_R)
    DEFINE_ECONSTANT(FANN_E_CANT_READ_TD)
    DEFINE_ECONSTANT(FANN_E_CANT_ALLOCATE_MEM)
    DEFINE_ECONSTANT(FANN_E_CANT_TRAIN_ACTIVATION)
    DEFINE_ECONSTANT(FANN_E_CANT_USE_ACTIVATION)
    DEFINE_ECONSTANT(FANN_E_TRAIN_DATA_MISMATCH)
    DEFINE_ECONSTANT(FANN_E_CANT_USE_TRAIN_ALG)
    DEFINE_ECONSTANT(FANN_E_TRAIN_DATA_SUBSET)
    DEFINE_ECONSTANT(FANN_E_INDEX_OUT_OF_BOUND)

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_fann_create_standard,1)
        T_NUMBER(strptime2, 0) // int

        RETURN_NUMBER((long)fann_create_standard((int)PARAM(0)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_create_standard_array, 1)
//T_NUMBER(0) // int

    char *data1 = 0;
    GET_CHECK_ARRAY(0, data1, "Parameter 1 should be an array of integers");

    unsigned int *arr = GetuintList(PARAMETER(0));

    RETURN_NUMBER((long)fann_create_standard_array((int)Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0)), arr))

    if (arr)
        delete[] arr;
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_fann_create_sparse,2)
        T_NUMBER(_fann_create_standard_array, 0) // float
        T_NUMBER(_fann_create_standard_array, 1) // int

        RETURN_NUMBER((long)fann_create_sparse((float)PARAM(0), (int)PARAM(1)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_create_sparse_array, 2)
    T_NUMBER(_fann_create_sparse_array, 0)     // float
//T_NUMBER(1) // int

    char *data2 = 0;
    GET_CHECK_ARRAY(1, data2, "Parameter 2 should be an array of integers");

    unsigned int *arr = GetuintList(PARAMETER(1));

    RETURN_NUMBER((long)fann_create_sparse_array((float)PARAM(0), (int)InvokePtr(INVOKE_GET_ARRAY_COUNT, PARAMETER(1)), arr))

    if (arr)
        delete[] arr;

END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_fann_create_shortcut,1)
        T_NUMBER(_fann_create_sparse_array, 0) // int

        RETURN_NUMBER((long)fann_create_shortcut((int)PARAM(0)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_create_shortcut_array, 1)
//T_NUMBER(0) // int

    char *data1 = 0;
    GET_CHECK_ARRAY(0, data1, "Parameter 1 should be an array of integers");

    unsigned int *arr = GetuintList(PARAMETER(0));


    RETURN_NUMBER((long)fann_create_shortcut_array((int)InvokePtr(INVOKE_GET_ARRAY_COUNT, PARAMETER(0)), arr))

    if (arr)
        delete[] arr;

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_destroy, 1)
    T_NUMBER(_fann_destroy, 0)     // fann*

    fann_destroy((fann *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_run, 2)
    T_NUMBER(_fann_run, 0)     // fann*
//T_NUMBER(1) // fann_type*

    char *data1 = 0;
    GET_CHECK_ARRAY(1, data1, "Parameter 2 should be an array of integers");

    fann_type *arr = GetList(PARAMETER(1));

    fann_type *res = fann_run((fann *)(long)PARAM(0), arr);
    SetList(RESULT, fann_get_num_output((fann *)(long)PARAM(0)), res);

    if (arr)
        delete[] arr;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_randomize_weights, 3)
    T_NUMBER(_fann_randomize_weights, 0)     // fann*
    T_NUMBER(_fann_randomize_weights, 1)     // fann_type
    T_NUMBER(_fann_randomize_weights, 2)     // fann_type

    fann_randomize_weights((fann *)(long)PARAM(0), (fann_type)PARAM(1), (fann_type)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_init_weights, 2)
    T_NUMBER(_fann_init_weights, 0)     // fann*
    T_NUMBER(_fann_init_weights, 1)     // fann_train_data*

    fann_init_weights((fann *)(long)PARAM(0), (fann_train_data *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_print_connections, 1)
    T_NUMBER(_fann_print_connections, 0)     // fann*

    fann_print_connections((fann *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_print_parameters, 1)
    T_NUMBER(_fann_print_parameters, 0)     // fann*

    fann_print_parameters((fann *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_num_input, 1)
    T_NUMBER(_fann_get_num_input, 0)     // fann*

    RETURN_NUMBER(fann_get_num_input((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_num_output, 1)
    T_NUMBER(_fann_get_num_output, 0)     // fann*

    RETURN_NUMBER(fann_get_num_output((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_total_neurons, 1)
    T_NUMBER(_fann_get_total_neurons, 0)     // fann*

    RETURN_NUMBER(fann_get_total_neurons((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_total_connections, 1)
    T_NUMBER(_fann_get_total_connections, 0)     // fann*

    RETURN_NUMBER(fann_get_total_connections((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_error_log, 2)
    T_NUMBER(_fann_set_error_log, 0)     // fann_error*
    T_NUMBER(_fann_set_error_log, 1)

    fann_set_error_log((struct fann_error *)(long)PARAM(0), (FILE *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_reset_errno, 1)
    T_NUMBER(_fann_reset_errno, 0)     // fann_error*

    fann_reset_errno((struct fann_error *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_reset_errstr, 1)
    T_NUMBER(_fann_reset_errstr, 0)     // fann_error*

    fann_reset_errstr((struct fann_error *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_errstr, 1)
    T_NUMBER(_fann_get_errstr, 0)     // fann_error*

    RETURN_STRING((char *)fann_get_errstr((struct fann_error *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_print_error, 1)
    T_NUMBER(_fann_print_error, 0)     // fann_error*

    fann_print_error((struct fann_error *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_train, 3)
    T_NUMBER(_fann_train, 0)     // fann*
//T_NUMBER(1) // fann_type*
//T_NUMBER(2) // fann_type*

    char *data1 = 0;
    GET_CHECK_ARRAY(1, data1, "Parameter 2 should be an array");

    fann_type *arr = GetList(PARAMETER(1));

    char *data2 = 0;
    GET_CHECK_ARRAY(2, data2, "Parameter 3 should be an array");

    fann_type *arr2 = GetList(PARAMETER(2));


    fann_train((fann *)(long)PARAM(0), arr, arr2);
    RETURN_NUMBER(0)

    if (arr)
        delete[] arr;
    if (arr2)
        delete[] arr2;

END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_test, 3)
    T_NUMBER(_fann_test, 0)     // fann*
//T_NUMBER(1) // fann_type*
//T_NUMBER(2) // fann_type*

    char *data1 = 0;
    GET_CHECK_ARRAY(1, data1, "Parameter 2 should be an array");

    fann_type *arr = GetList(PARAMETER(1));

    char *data2 = 0;
    GET_CHECK_ARRAY(2, data2, "Parameter 3 should be an array");

    fann_type *arr2 = GetList(PARAMETER(2));

    fann_type *res = fann_test((fann *)(long)PARAM(0), arr, arr2);

    SetList(RESULT, fann_get_num_output((fann *)(long)PARAM(0)), res);

    if (arr)
        delete[] arr;

    if (arr2)
        delete[] arr2;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_MSE, 1)
    T_NUMBER(_fann_get_MSE, 0)     // fann*

    RETURN_NUMBER(fann_get_MSE((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_bit_fail, 1)
    T_NUMBER(_fann_get_bit_fail, 0)     // fann*

    RETURN_NUMBER(fann_get_bit_fail((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_reset_MSE, 1)
    T_NUMBER(_fann_reset_MSE, 0)     // fann*

    fann_reset_MSE((fann *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_train_on_data, 5)
    T_NUMBER(_fann_train_on_data, 0)     // fann*
    T_NUMBER(_fann_train_on_data, 1)     // fann_train_data*
    T_NUMBER(_fann_train_on_data, 2)     // int
    T_NUMBER(_fann_train_on_data, 3)     // int
    T_NUMBER(_fann_train_on_data, 4)     // float

    fann_train_on_data((fann *)(long)PARAM(0), (fann_train_data *)(long)PARAM(1), (int)PARAM(2), (int)PARAM(3), (float)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_train_on_file, 5)
    T_NUMBER(_fann_train_on_file, 0)     // fann*
    T_STRING(_fann_train_on_file, 1)     // char*
    T_NUMBER(_fann_train_on_file, 2)     // int
    T_NUMBER(_fann_train_on_file, 3)     // int
    T_NUMBER(_fann_train_on_file, 4)     // float

    fann_train_on_file((fann *)(long)PARAM(0), PARAM(1), (int)PARAM(2), (int)PARAM(3), (float)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_train_epoch, 2)
    T_NUMBER(_fann_train_epoch, 0)     // fann*
    T_NUMBER(_fann_train_epoch, 1)     // fann_train_data*

    RETURN_NUMBER(fann_train_epoch((fann *)(long)PARAM(0), (fann_train_data *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_test_data, 2)
    T_NUMBER(_fann_test_data, 0)     // fann*
    T_NUMBER(_fann_test_data, 1)     // fann_train_data*

    RETURN_NUMBER(fann_test_data((fann *)(long)PARAM(0), (fann_train_data *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_read_train_from_file, 1)
    T_STRING(_fann_read_train_from_file, 0)     // char*

    RETURN_NUMBER((long)fann_read_train_from_file(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_destroy_train, 1)
    T_NUMBER(_fann_destroy_train, 0)     // fann_train_data*

    fann_destroy_train((fann_train_data *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_shuffle_train_data, 1)
    T_NUMBER(_fann_shuffle_train_data, 0)     // fann_train_data*

    fann_shuffle_train_data((fann_train_data *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_scale_input_train_data, 3)
    T_NUMBER(_fann_scale_input_train_data, 0)     // fann_train_data*
    T_NUMBER(_fann_scale_input_train_data, 1)     // fann_type
    T_NUMBER(_fann_scale_input_train_data, 2)     // fann_type

    fann_scale_input_train_data((fann_train_data *)(long)PARAM(0), (fann_type)PARAM(1), (fann_type)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_scale_output_train_data, 3)
    T_NUMBER(_fann_scale_output_train_data, 0)     // fann_train_data*
    T_NUMBER(_fann_scale_output_train_data, 1)     // fann_type
    T_NUMBER(_fann_scale_output_train_data, 2)     // fann_type

    fann_scale_output_train_data((fann_train_data *)(long)PARAM(0), (fann_type)PARAM(1), (fann_type)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_scale_train_data, 3)
    T_NUMBER(_fann_scale_train_data, 0)     // fann_train_data*
    T_NUMBER(_fann_scale_train_data, 1)     // fann_type
    T_NUMBER(_fann_scale_train_data, 2)     // fann_type

    fann_scale_train_data((fann_train_data *)(long)PARAM(0), (fann_type)PARAM(1), (fann_type)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_merge_train_data, 2)
    T_NUMBER(_fann_merge_train_data, 0)     // fann_train_data*
    T_NUMBER(_fann_merge_train_data, 1)     // fann_train_data*

    RETURN_NUMBER((long)fann_merge_train_data((fann_train_data *)(long)PARAM(0), (fann_train_data *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_duplicate_train_data, 1)
    T_NUMBER(_fann_duplicate_train_data, 0)     // fann_train_data*

    RETURN_NUMBER((long)fann_duplicate_train_data((fann_train_data *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_subset_train_data, 3)
    T_NUMBER(_fann_subset_train_data, 0)     // fann_train_data*
    T_NUMBER(_fann_subset_train_data, 1)     // int
    T_NUMBER(_fann_subset_train_data, 2)     // int

    RETURN_NUMBER((long)fann_subset_train_data((fann_train_data *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_length_train_data, 1)
    T_NUMBER(_fann_length_train_data, 0)     // fann_train_data*

    RETURN_NUMBER(fann_length_train_data((fann_train_data *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_num_input_train_data, 1)
    T_NUMBER(_fann_num_input_train_data, 0)     // fann_train_data*

    RETURN_NUMBER(fann_num_input_train_data((fann_train_data *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_num_output_train_data, 1)
    T_NUMBER(_fann_num_output_train_data, 0)     // fann_train_data*

    RETURN_NUMBER(fann_num_output_train_data((fann_train_data *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_save_train, 2)
    T_NUMBER(_fann_save_train, 0)     // fann_train_data*
    T_STRING(_fann_save_train, 1)     // char*

    RETURN_NUMBER(fann_save_train((fann_train_data *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_save_train_to_fixed, 3)
    T_NUMBER(_fann_save_train_to_fixed, 0)     // fann_train_data*
    T_STRING(_fann_save_train_to_fixed, 1)     // char*
    T_NUMBER(_fann_save_train_to_fixed, 2)     // int

    RETURN_NUMBER(fann_save_train_to_fixed((fann_train_data *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_training_algorithm, 2)
    T_NUMBER(_fann_set_training_algorithm, 0)     // fann*
    T_NUMBER(_fann_set_training_algorithm, 1)     // fann_train_enum

    fann_set_training_algorithm((fann *)(long)PARAM(0), (fann_train_enum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_learning_rate, 1)
    T_NUMBER(_fann_get_learning_rate, 0)     // fann*

    RETURN_NUMBER(fann_get_learning_rate((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_learning_rate, 2)
    T_NUMBER(_fann_set_learning_rate, 0)     // fann*
    T_NUMBER(_fann_set_learning_rate, 1)     // float

    fann_set_learning_rate((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_learning_momentum, 1)
    T_NUMBER(_fann_get_learning_momentum, 0)     // fann*

    RETURN_NUMBER(fann_get_learning_momentum((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_learning_momentum, 2)
    T_NUMBER(_fann_set_learning_momentum, 0)     // fann*
    T_NUMBER(_fann_set_learning_momentum, 1)     // float

    fann_set_learning_momentum((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_function, 4)
    T_NUMBER(_fann_set_activation_function, 0)     // fann*
    T_NUMBER(_fann_set_activation_function, 1)     // fann_activationfunc_enum
    T_NUMBER(_fann_set_activation_function, 2)     // int
    T_NUMBER(_fann_set_activation_function, 3)     // int

    fann_set_activation_function((fann *)(long)PARAM(0), (fann_activationfunc_enum)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_function_layer, 3)
    T_NUMBER(_fann_set_activation_function_layer, 0)     // fann*
    T_NUMBER(_fann_set_activation_function_layer, 1)     // fann_activationfunc_enum
    T_NUMBER(_fann_set_activation_function_layer, 2)     // int

    fann_set_activation_function_layer((fann *)(long)PARAM(0), (fann_activationfunc_enum)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_function_hidden, 2)
    T_NUMBER(_fann_set_activation_function_hidden, 0)     // fann*
    T_NUMBER(_fann_set_activation_function_hidden, 1)     // fann_activationfunc_enum

    fann_set_activation_function_hidden((fann *)(long)PARAM(0), (fann_activationfunc_enum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_function_output, 2)
    T_NUMBER(_fann_set_activation_function_output, 0)     // fann*
    T_NUMBER(_fann_set_activation_function_output, 1)     // fann_activationfunc_enum

    fann_set_activation_function_output((fann *)(long)PARAM(0), (fann_activationfunc_enum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_steepness, 4)
    T_NUMBER(_fann_set_activation_steepness, 0)     // fann*
    T_NUMBER(_fann_set_activation_steepness, 1)     // fann_type
    T_NUMBER(_fann_set_activation_steepness, 2)     // int
    T_NUMBER(_fann_set_activation_steepness, 3)     // int

    fann_set_activation_steepness((fann *)(long)PARAM(0), (fann_type)PARAM(1), (int)PARAM(2), (int)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_steepness_layer, 3)
    T_NUMBER(_fann_set_activation_steepness_layer, 0)     // fann*
    T_NUMBER(_fann_set_activation_steepness_layer, 1)     // fann_type
    T_NUMBER(_fann_set_activation_steepness_layer, 2)     // int

    fann_set_activation_steepness_layer((fann *)(long)PARAM(0), (fann_type)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_steepness_hidden, 2)
    T_NUMBER(_fann_set_activation_steepness_hidden, 0)     // fann*
    T_NUMBER(_fann_set_activation_steepness_hidden, 1)     // fann_type

    fann_set_activation_steepness_hidden((fann *)(long)PARAM(0), (fann_type)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_activation_steepness_output, 2)
    T_NUMBER(_fann_set_activation_steepness_output, 0)     // fann*
    T_NUMBER(_fann_set_activation_steepness_output, 1)     // fann_type

    fann_set_activation_steepness_output((fann *)(long)PARAM(0), (fann_type)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_train_error_function, 1)
    T_NUMBER(_fann_get_train_error_function, 0)     // fann*

    RETURN_NUMBER(fann_get_train_error_function((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_train_error_function, 2)
    T_NUMBER(_fann_set_train_error_function, 0)     // fann*
    T_NUMBER(_fann_set_train_error_function, 1)     // fann_errorfunc_enum

    fann_set_train_error_function((fann *)(long)PARAM(0), (fann_errorfunc_enum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_train_stop_function, 2)
    T_NUMBER(_fann_set_train_stop_function, 0)     // fann*
    T_NUMBER(_fann_set_train_stop_function, 1)     // fann_stopfunc_enum

    fann_set_train_stop_function((fann *)(long)PARAM(0), (fann_stopfunc_enum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_bit_fail_limit, 1)
    T_NUMBER(_fann_get_bit_fail_limit, 0)     // fann*

    RETURN_NUMBER(fann_get_bit_fail_limit((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_bit_fail_limit, 2)
    T_NUMBER(_fann_set_bit_fail_limit, 0)     // fann*
    T_NUMBER(_fann_set_bit_fail_limit, 1)     // fann_type

    fann_set_bit_fail_limit((fann *)(long)PARAM(0), (fann_type)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(_fann_set_callback,2)
        T_NUMBER(_fann_set_bit_fail_limit, 0) // fann*
        T_NUMBER(_fann_set_bit_fail_limit, 1) // fann_callback_type

        fann_set_callback((fann*)(long)PARAM(0), (fann_callback_type)PARAM(1));
        RETURN_NUMBER(0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_quickprop_decay, 1)
    T_NUMBER(_fann_get_quickprop_decay, 0)     // fann*

    RETURN_NUMBER(fann_get_quickprop_decay((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_quickprop_decay, 2)
    T_NUMBER(_fann_set_quickprop_decay, 0)     // fann*
    T_NUMBER(_fann_set_quickprop_decay, 1)     // float

    fann_set_quickprop_decay((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_quickprop_mu, 1)
    T_NUMBER(_fann_get_quickprop_mu, 0)     // fann*

    RETURN_NUMBER(fann_get_quickprop_mu((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_quickprop_mu, 2)
    T_NUMBER(_fann_set_quickprop_mu, 0)     // fann*
    T_NUMBER(_fann_set_quickprop_mu, 1)     // float

    fann_set_quickprop_mu((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_rprop_increase_factor, 1)
    T_NUMBER(_fann_get_rprop_increase_factor, 0)     // fann*

    RETURN_NUMBER(fann_get_rprop_increase_factor((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_rprop_increase_factor, 2)
    T_NUMBER(_fann_set_rprop_increase_factor, 0)     // fann*
    T_NUMBER(_fann_set_rprop_increase_factor, 1)     // float

    fann_set_rprop_increase_factor((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_rprop_decrease_factor, 1)
    T_NUMBER(_fann_get_rprop_decrease_factor, 0)     // fann*

    RETURN_NUMBER(fann_get_rprop_decrease_factor((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_rprop_decrease_factor, 2)
    T_NUMBER(_fann_set_rprop_decrease_factor, 0)     // fann*
    T_NUMBER(_fann_set_rprop_decrease_factor, 1)     // float

    fann_set_rprop_decrease_factor((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_rprop_delta_min, 1)
    T_NUMBER(_fann_get_rprop_delta_min, 0)     // fann*

    RETURN_NUMBER(fann_get_rprop_delta_min((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_rprop_delta_min, 2)
    T_NUMBER(_fann_set_rprop_delta_min, 0)     // fann*
    T_NUMBER(_fann_set_rprop_delta_min, 1)     // float

    fann_set_rprop_delta_min((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_get_rprop_delta_max, 1)
    T_NUMBER(_fann_get_rprop_delta_max, 0)     // fann*

    RETURN_NUMBER(fann_get_rprop_delta_max((fann *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_set_rprop_delta_max, 2)
    T_NUMBER(_fann_set_rprop_delta_max, 0)     // fann*
    T_NUMBER(_fann_set_rprop_delta_max, 1)     // float

    fann_set_rprop_delta_max((fann *)(long)PARAM(0), (float)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_create_from_file, 1)
    T_STRING(_fann_create_from_file, 0)     // char*

    RETURN_NUMBER((long)fann_create_from_file(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_save, 2)
    T_NUMBER(_fann_save, 0)     // fann*
    T_STRING(_fann_save, 1)     // char*

    RETURN_NUMBER(fann_save((fann *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_fann_save_to_fixed, 2)
    T_NUMBER(_fann_save_to_fixed, 0)     // fann*
    T_STRING(_fann_save_to_fixed, 1)     // char*

    RETURN_NUMBER(fann_save_to_fixed((fann *)(long)PARAM(0), PARAM(1)))
END_IMPL

