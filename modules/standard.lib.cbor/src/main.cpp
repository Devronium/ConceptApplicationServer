//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include "cbor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
cbor_item_t *serialize_recursive(void *var, INVOKE_CALL Invoke, int level) {
    if ((!var) || (level > 100))
        return NULL;

    cbor_item_t *item = NULL;
    INTEGER type = 0;
    char *str = NULL;
    NUMBER nvalue = 0;

    Invoke(INVOKE_GET_VARIABLE, var, &type, &str, &nvalue);

    switch (type) {
        case VARIABLE_NUMBER:
            if (round(nvalue) == nvalue) {
                if (nvalue >= 0) {
                    if (nvalue < 0x100)
                        item = cbor_build_uint8((uint8_t)nvalue);
                    else
                    if (nvalue < 0x10000)
                        item = cbor_build_uint16((uint16_t)nvalue);
                    else
                    if (nvalue < 0x100000000ULL)
                        item = cbor_build_uint32((uint32_t)nvalue);
                    else
                        item = cbor_build_uint64((uint64_t)nvalue);
                } else
                    item = cbor_build_negint64((uint64_t)-nvalue);
            } else
                item = cbor_build_float8(nvalue);
            break;
        case VARIABLE_STRING:
            item = cbor_build_bytestring((cbor_data)str, (size_t)nvalue);
            break;
        case VARIABLE_ARRAY:
            {
                int count = Invoke(INVOKE_GET_ARRAY_COUNT, var);
                int as_object = 1;
                int i;
                char *key;
                if (count > 0) {
                    Invoke(INVOKE_GET_ARRAY_KEY, var, 0, &key);
                    if ((!key) || (!key[0]))
                        as_object = 0;
                }
                if (as_object) {
                    if (count > 0)
                        item = cbor_new_definite_map(count);
                    else
                        item = cbor_new_indefinite_map();
                } else {
                    if (count > 0)
                        item = cbor_new_definite_array(count);
                    else
                        item = cbor_new_indefinite_array();
                }
                for (i = 0; i < count; i++) {
                    void *newdata = NULL;
                    cbor_item_t *child_item = NULL;
                    Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)i, &newdata);
                    if (newdata) {
                        if (as_object) {
                            Invoke(INVOKE_GET_ARRAY_KEY, var, (INTEGER)i, &key);
                            if ((key) && (key[0])) {
                                child_item = serialize_recursive(newdata, Invoke, level + 1);
                                if (child_item) {
                                    struct cbor_pair pair;
                                    pair.key = cbor_move(cbor_build_string(key));
                                    pair.value = cbor_move(child_item);
                                    cbor_map_add(item, pair);
                                }
                            }                                
                        } else {
                            child_item = serialize_recursive(newdata, Invoke, level + 1);
                            if (child_item)
                                cbor_array_set(item, (size_t)i, cbor_move(child_item));
                        }
                    }
                }
            }
            break;
        case VARIABLE_CLASS:
            {
                char *class_name = NULL;
                int members_count = Invoke(INVOKE_GET_SERIAL_CLASS, str, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                if (members_count > 0) {
                    char   **members       = (char **)malloc(sizeof(char *) * members_count);
                    char   *flags          = (char *)malloc(sizeof(char) * members_count);
                    char   *access         = (char *)malloc(sizeof(char) * members_count);
                    char   *types          = (char *)malloc(sizeof(char) * members_count);
                    char   **szValues      = (char **)malloc(sizeof(char *) * members_count);
                    NUMBER *nValues        = (NUMBER *)malloc(sizeof(NUMBER) * members_count);
                    void   **class_data    = (void **)malloc(sizeof(void *) * members_count);
                    void   **variable_data = (void **)malloc(sizeof(void *) * members_count);

                    item = cbor_new_definite_map(members_count);
                    int result = Invoke(INVOKE_GET_SERIAL_CLASS, str, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);
                    if (IS_OK(result)) {
                        for (int i = 0; i < members_count; i++) {
                            if ((flags[i] == 0) && (members[i])) {
                                cbor_item_t *child_item = serialize_recursive(variable_data[i], Invoke, level + 1);
                                if (child_item) {
                                    struct cbor_pair pair;
                                    pair.key = cbor_move(cbor_build_string(members[i]));
                                    pair.value = cbor_move(child_item);
                                    cbor_map_add(item, pair);
                                }
                            }
                        }
                    }
                    free(members);
                    free(flags);
                    free(access);
                    free(types);
                    free(szValues);
                    free(nValues);
                    free(class_data);
                    free(variable_data);
                } else {
                    item = cbor_new_indefinite_map();
                }
            }
            break;
    }

    return item;
}

CONCEPT_FUNCTION_IMPL(CBORSerialize, 1)
    cbor_item_t *root = serialize_recursive(PARAMETER(0), Invoke, 0);
    if (root) {
        unsigned char* buffer = NULL;
        size_t buffer_size;
        size_t length = cbor_serialize_alloc(root, &buffer, &buffer_size);
        if ((length > 0) && (buffer)) {
            RETURN_BUFFER((const char *)buffer, length);
        } else {
            RETURN_STRING("");
        }
        free(buffer);
        cbor_decref(&root);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
void deserialize_recursive(cbor_item_t *item, void *var, INVOKE_CALL Invoke) {
    size_t i;
    size_t array_size;
    switch (cbor_typeof(item)) {
        case CBOR_TYPE_UINT:
            Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)cbor_get_int(item));
            break;
        case CBOR_TYPE_NEGINT:
            Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)cbor_get_int(item) * -1);
            break;
        case CBOR_TYPE_BYTESTRING:
            array_size = cbor_bytestring_length(item);
            if (array_size > 0) {
                cbor_mutable_data data = cbor_bytestring_handle(item);
                Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_STRING, (char *)data, (NUMBER)array_size);
            } else {
                Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            }
            break;
        case CBOR_TYPE_STRING:
            array_size = cbor_string_codepoint_count(item);
            if (array_size > 0) {
                cbor_mutable_data data = cbor_string_handle(item);
                Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_STRING, (char *)data, (NUMBER)array_size);
            } else {
                Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            }
            break;
        case CBOR_TYPE_ARRAY:
            CREATE_ARRAY(var);
            if (cbor_array_is_definite(item)) {
                array_size = cbor_array_size(item);
                cbor_item_t **handle = cbor_array_handle(item);
                for (i = 0; i < array_size; i++) {
                    void *var2 = NULL;
                    Invoke(INVOKE_ARRAY_VARIABLE, var, (INTEGER)i, &var2);
                    if (var2)
                        deserialize_recursive(handle[i], var2, Invoke);
                }
            }
            break;
        case CBOR_TYPE_MAP:
            CREATE_ARRAY(var);
            if (cbor_map_is_definite(item)) {
                array_size = cbor_map_size(item);
                struct cbor_pair *handle = cbor_map_handle(item);
                for (i = 0; i < array_size; i++) {
                    if (cbor_typeof(handle[i].key) == CBOR_TYPE_STRING) {
                        void *var2 = NULL;
                        int len = cbor_string_codepoint_count(handle[i].key);
                        char *key = (char *)malloc(len + 1);
                        if (key) {
                            memcpy(key, cbor_string_handle(handle[i].key), len);
                            key[len] = 0;
                            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var, key, &var2);
                            free(key);
                            if (var2)
                                deserialize_recursive(handle[i].value, var2, Invoke);
                        }
                    }
                }
            }
            break;
        case CBOR_TYPE_TAG:
            // not supported
            break;
        case CBOR_TYPE_FLOAT_CTRL:
            Invoke(INVOKE_SET_VARIABLE, var, (INTEGER)VARIABLE_NUMBER, (char *)NULL, (NUMBER)cbor_float_get_float(item));
            break;
    }
}

CONCEPT_FUNCTION_IMPL(CBORDeserialize, 1)
    T_STRING(CBORSerialize, 0)

    struct cbor_load_result result;
    cbor_item_t *item = cbor_load((const unsigned char *)PARAM(0), PARAM_LEN(0), &result);
    if (!item) {
        RETURN_NUMBER(0);
        return 0;
    }
    deserialize_recursive(item, RESULT, Invoke);
    cbor_decref(&item);
END_IMPL
//-----------------------------------------------------//
