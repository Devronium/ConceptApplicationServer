//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#define WITH_PARSON

#define MAX_JSON_LEVEL   4096

extern "C" {
    #include "JSON_checker.h"
#ifdef WITH_PARSON
    // NOTE: Using a modified version of parson. Parson doesn't allow duplicate keys in json.
    // JSON specification does not make any mention of duplicate keys being invalid or valid.
    // REMOVED THE FOLLOWING LINES FROM json_object_add:
    // if (json_object_get_value(object, name) != NULL) {
    //    return JSONFailure;
    // }

    #include "parson.h"
    #define json_type_boolean           JSONBoolean
    #define json_type_double            JSONNumber
    #define json_type_int               JSONNumber
    #define json_type_string            JSONString
    #define json_type_object            JSONObject
    #define json_type_array             JSONArray

    #define json_type                   JSON_Value_Type

    #define json_object_array_length    json_array_get_count
    #define json_object_array_get_idx   json_array_get_value
#else
    #include <json.h>
#endif
    #include <math.h>
}
//-------------------------//
// Local variables         //
//-------------------------//
#ifdef WITH_PARSON
    JSON_Value *do_array(void *arr, bool as_object = false, int level = 0);
#else
    struct json_object *do_array(void *arr, bool as_object = false, int level = 0);

    #define JSON_NUMBER(my_elem, nData)                         \
        if (floor(nData) == nData) {                            \
            if ((nData >= 0x7FFFFFFF) || (nData < -0x7FFFFFFE)) \
                my_elem = json_object_new_double(nData);        \
            else                                                \
                my_elem = json_object_new_int((int)nData);      \
        } else                                                  \
            my_elem = json_object_new_double(nData);
#endif

INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
#ifdef WITH_PARSON
JSON_Value *do_object(void *pData, bool as_object = false, int level = 0) {
    JSON_Value *my_obj = json_value_init_object();
    JSON_Object *my_elem = json_value_get_object(my_obj);
#else
struct json_object *do_object(void *pData, bool as_object = false, int level = 0) {
    struct json_object *my_obj = json_object_new_object();
#endif
    if (level >= MAX_JSON_LEVEL)
        return my_obj;
    char *class_name           = 0;

    int members_count = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);

    if (members_count > 0) {
        char   **members       = new char * [members_count];
        char   *flags          = new char[members_count];
        char   *access         = new char[members_count];
        char   *types          = new char[members_count];
        char   **szValues      = new char * [members_count];
        NUMBER *nValues        = new NUMBER[members_count];
        void   **class_data    = new void * [members_count];
        void   **variable_data = new void * [members_count];

        int result = InvokePtr(INVOKE_GET_SERIAL_CLASS, pData, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);

        if (IS_OK(result)) {
            for (int i = 0; i < members_count; i++) {
                if (flags[i] == 0) {
#ifdef WITH_PARSON
                    switch (types[i]) {
                        case VARIABLE_STRING:
                            json_object_set_string(my_elem, (char *)(members[i] ? members[i] : ""), szValues[i]);
                            break;

                        case VARIABLE_NUMBER:
                            json_object_set_number(my_elem, (char *)(members[i] ? members[i] : ""), nValues[i]);
                            break;

                        case VARIABLE_ARRAY:
                            json_object_set_value(my_elem, (char *)(members[i] ? members[i] : ""), do_array(variable_data[i], as_object, level + 1));
                            break;

                        case VARIABLE_CLASS:
                            json_object_set_value(my_elem, (char *)(members[i] ? members[i] : ""), do_object((void *)class_data[i], as_object, level + 1));
                            break;

                        default:
                            break;
                    }
#else
                    struct json_object *my_elem = 0;
                    switch (types[i]) {
                        case VARIABLE_STRING:
                            my_elem = json_object_new_string_len(szValues[i], (int)nValues[i]);
                            break;

                        case VARIABLE_NUMBER:
                            JSON_NUMBER(my_elem, nValues[i]);
                            break;

                        case VARIABLE_ARRAY:
                            my_elem = do_array(variable_data[i], as_object, level + 1);
                            break;

                        case VARIABLE_CLASS:
                            my_elem = do_object((void *)class_data[i], as_object, level + 1);
                            break;

                        default:
                            break;
                    }
                    if (my_elem)
                        json_object_object_add(my_obj, (char *)(members[i] ? members[i] : ""), my_elem);
#endif
                }
            }
        }
        delete[] members;
        delete[] flags;
        delete[] access;
        delete[] types;
        delete[] szValues;
        delete[] nValues;
        delete[] class_data;
        delete[] variable_data;
    }
    return my_obj;
}

//-----------------------------------------------------//
#ifdef WITH_PARSON
JSON_Value *do_array(void *arr, bool as_object, int level) {
    JSON_Value *my_obj = 0;
#else
struct json_object *do_array(void *arr, bool as_object, int level) {
    struct json_object *my_obj = 0;
#endif
    void *newpData;
    char *key;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    bool local_as_object = as_object;
    if ((count > 0) && (as_object)) {
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, 0, &key);
        if (!key)
            local_as_object = false;
    }

#ifdef WITH_PARSON
    JSON_Array *my_array = NULL;
    JSON_Object *my_object = NULL;

    if (local_as_object) {
        my_obj = json_value_init_object();
        my_object = json_value_get_object(my_obj);
    } else {
        my_obj = json_value_init_array();
        my_array = json_value_get_array(my_obj);
    }
#else
    if (local_as_object)
        my_obj = json_object_new_object();
    else
        my_obj = json_object_new_array();
#endif
    if (level >= MAX_JSON_LEVEL)
        return my_obj;

    for (int i = 0; i < count; i++) {
        newpData = 0;
        key      = 0;
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
        if (newpData) {
            if ((local_as_object) && (!key))
                continue;

            char               *szData;
            INTEGER            type;
            NUMBER             nData;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
#ifdef WITH_PARSON
            JSON_Value *my_elem = 0;
            switch (type) {
                case VARIABLE_STRING:
                    my_elem = json_value_init_string(szData);
                    break;

                case VARIABLE_NUMBER:
                    my_elem = json_value_init_number(nData);
                    break;

                case VARIABLE_ARRAY:
                    my_elem = do_array(newpData, as_object, level + 1);
                    break;

                case VARIABLE_CLASS:
                    my_elem = do_object((void *)szData, as_object, level + 1);
                    break;

                default:
                    break;
            }
            if (my_elem) {
                if (local_as_object)
                    json_object_set_value(my_object, key, my_elem);
                else
                    json_array_append_value(my_array, my_elem);
            }

#else
            struct json_object *my_elem = 0;


            switch (type) {
                case VARIABLE_STRING:
                    my_elem = json_object_new_string_len(szData, (int)nData);
                    break;

                case VARIABLE_NUMBER:
                    JSON_NUMBER(my_elem, nData);
                    break;

                case VARIABLE_ARRAY:
                    my_elem = do_array(newpData, as_object, level + 1);
                    break;

                case VARIABLE_CLASS:
                    my_elem = do_object((void *)szData, as_object, level + 1);
                    break;

                default:
                    break;
            }

            if (my_elem) {
                if (local_as_object)
                    json_object_object_add(my_obj, key, my_elem);
                else
                    json_object_array_add(my_obj, my_elem);
            }
#endif
        }
    }

    return my_obj;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(_JSONSerialize, 1)
    char *dclass = 0;
    NUMBER dmember   = 0;
    bool   as_object = false;
    bool   pretty    = false;
    if (PARAMETERS_COUNT > 3) {
        return (void *)"JSONSerialize: invalid parameters count: object, array_as_objects=false, pretty=false";
    } else
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(_JSONSerialize, 1)

        as_object = (bool)PARAM_INT(1);
        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(_JSONSerialize, 2)
            pretty = (bool)PARAM_INT(2);
        }
    }

#ifdef WITH_PARSON
    JSON_Value *my_obj = 0;
#else
    struct json_object *my_obj = 0;
#endif

    GET_DELEGATE(0, dclass, dmember)

    switch (TYPE) {
#ifdef WITH_PARSON
        case VARIABLE_STRING:
            my_obj = json_value_init_string(dclass);
            break;

        case VARIABLE_NUMBER:
            my_obj = json_value_init_number(dmember);
            break;
#else
        case VARIABLE_STRING:
            my_obj = json_object_new_string_len(dclass, (int)dmember);
            break;

        case VARIABLE_NUMBER:
            JSON_NUMBER(my_obj, dmember);
            break;
#endif

        case VARIABLE_ARRAY:
            my_obj = do_array(PARAMETER(0), as_object);
            break;

        case VARIABLE_CLASS:
            my_obj = do_object((void *)dclass, as_object);
            break;
        default:
            return (void *)"Unsupported data type (delegate ?)";
    }

    if (my_obj) {
#ifdef WITH_PARSON
        char *json_str;
        if (pretty)
            json_str = json_serialize_to_string_pretty(my_obj);
        else
            json_str = json_serialize_to_string(my_obj);
#else
        const char *json_str;
        if (pretty)
            json_str = json_object_to_json_string_ext(my_obj, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED);
        else
            json_str = json_object_to_json_string(my_obj);
#endif
        if (json_str) {
            RETURN_STRING(json_str)
#ifdef WITH_PARSON
            json_free_serialized_string(json_str);
#endif
        } else {
            RETURN_STRING("");
        }
#ifdef WITH_PARSON
        json_value_free(my_obj);
#else
        json_object_put(my_obj);
#endif
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
#ifdef WITH_PARSON
void deserialize_object(JSON_Value *val, void *RESULT, json_type mtype) {
#else
void deserialize_object(struct json_object *obj, void *RESULT, json_type mtype) {
#endif
    if (mtype == json_type_array) {
#ifdef WITH_PARSON
        JSON_Array *obj = json_value_get_array(val);
#endif
        InvokePtr(INVOKE_CREATE_ARRAY, RESULT, 0);
        int len = json_object_array_length(obj);
        for (int i = 0; i < len; i++) {
#ifdef WITH_PARSON
            JSON_Value *val = json_object_array_get_idx(obj, i);
#else
            struct json_object *val = json_object_array_get_idx(obj, i);
#endif
            if (val) {
#ifdef WITH_PARSON
                json_type type      = json_value_get_type(val);
#else
                json_type type      = json_object_get_type(val);
#endif
                void      *var_data = 0;
                INTEGER   i_type;
                char      *str = 0;
                NUMBER    nvalue;
                void      *new_result = 0;

                switch (type) {
                    case json_type_boolean:
#ifdef WITH_PARSON
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_value_get_boolean(val));
#else
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_boolean(val));
#endif
                        break;

                    case json_type_double:
#ifdef WITH_PARSON
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_value_get_number(val));
#else
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_double(val));
#endif
                        break;

#ifdef WITH_PARSON
                    case JSONNull:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)0);
                        break;
#else
                    case json_type_int:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_int(val));
                        break;
#endif

                    case json_type_string:
#ifdef WITH_PARSON
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)json_value_get_string(val), (NUMBER)0);
#else
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)json_object_get_string(val), (NUMBER)0);
#endif
                        break;

                    case json_type_object:
                    case json_type_array:
                        InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &new_result);
                        deserialize_object(val, new_result, type);
                        break;
                }
            } else
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)0);
        }
    } else
    if (mtype == json_type_object) {
        InvokePtr(INVOKE_CREATE_ARRAY, RESULT, 0);
#ifdef WITH_PARSON
        const JSON_Object *obj = json_value_get_object(val);
        int len = json_object_get_count(obj);
        for (int i = 0; i < len; i++) {
            const char *key = json_object_get_name(obj, i);
            JSON_Value *val2 = json_object_get_value_at(obj, i);
            if ((key) && (val2)) {
                json_type type      = json_value_get_type(val2);
                void      *var_data = 0;
                INTEGER   i_type;
                char      *str = 0;
                NUMBER    nvalue;
                void      *new_result = 0;

                switch (type) {
                    case json_type_boolean:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_value_get_boolean(val2));
                        break;

                    case json_type_double:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_value_get_number(val2));
                        break;

                    case JSONNull:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)0);
                        break;

                    case json_type_string:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, (char *)json_value_get_string(val2), (NUMBER)0);
                        break;

                    case json_type_object:
                    case json_type_array:
                        InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)key, &new_result);
                        deserialize_object(val2, new_result, type);
                        break;
                }
            }
        }
#else
        json_object_object_foreach(obj, key, val) {
            if (val) {
                json_type type      = json_object_get_type(val);
                void      *var_data = 0;
                INTEGER   i_type;
                char      *str = 0;
                NUMBER    nvalue;
                void      *new_result = 0;

                switch (type) {
                    case json_type_boolean:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_boolean(val));
                        break;

                    case json_type_double:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_double(val));
                        break;

                    case json_type_int:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_int(val));
                        break;

                    case json_type_string:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, (char *)json_object_get_string(val), (NUMBER)0);
                        break;

                    case json_type_object:
                    case json_type_array:
                        InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)key, &new_result);
                        deserialize_object(val, new_result, type);
                        break;
                }
            } else
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)0);
        }
#endif
    }
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_JSONDeserialize, 1)
    T_STRING(_JSONDeserialize, 0)

#ifdef WITH_PARSON
    JSON_Value *val = json_parse_string_with_comments(PARAM(0));

    if (val) {
        json_type type = json_value_get_type(val);
        switch (type) {
            case json_type_boolean:
                RETURN_NUMBER(json_value_get_boolean(val));
                break;

            case json_type_double:
                RETURN_NUMBER(json_value_get_number(val));
                break;

            case json_type_string:
                RETURN_STRING(json_value_get_string(val));
                break;

            case json_type_object:
            case json_type_array:
                deserialize_object(val, RESULT, type);
                break;

            default:
                RETURN_NUMBER(0)
        }
        json_value_free(val);
    } else {
        RETURN_NUMBER(0);
    }
#else
    struct json_tokener *tok = json_tokener_new(); //json_tokener_parse(PARAM(0)
    struct json_object  *obj = json_tokener_parse_ex(tok, PARAM(0), PARAM_LEN(0));

    if (obj) {
        json_type type = json_object_get_type(obj);
        switch (type) {
            case json_type_boolean:
                RETURN_NUMBER(json_object_get_boolean(obj));
                break;

            case json_type_double:
                RETURN_NUMBER(json_object_get_double(obj));
                break;

            case json_type_int:
                RETURN_NUMBER(json_object_get_int(obj));
                break;

            case json_type_string:
                RETURN_STRING(json_object_get_string(obj));
                break;

            case json_type_object:
            case json_type_array:
                deserialize_object(obj, RESULT, type);
                break;

            default:
                RETURN_NUMBER(0)
        }
    } else {
        RETURN_NUMBER(0)
    }
    if (obj) {
        json_object_put(obj);
        json_tokener_free(tok);
    }
#endif
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(_JSONCheck, 1, 2)
    T_STRING(JSONCheck, 0)
    int max_depth = 0xFFF;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(JSONCheck, 1)
        max_depth = PARAM_INT(1);
        if (max_depth <= 0)
            max_depth = 0xFFF;
    }

    JSON_checker jc = new_JSON_checker(max_depth);
    int line = 1;
    int chr = 0;
    int err = 0;

    int len = PARAM_LEN(0);
    char *json = PARAM(0);
    for (int i = 0; i < len; i++) {
        chr++;
        int next_char = json[i];
        if (next_char == '\n') {
            line++;
            chr = 1;
        }
        if (!JSON_checker_char(jc, next_char)) {
            err = 1;
            break;
        }
    }
    if (!JSON_checker_done(jc))
        err = 1;

    if (err) {
        CREATE_ARRAY(RESULT);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "line", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)line);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "character", (INTEGER)VARIABLE_NUMBER, "", (NUMBER)chr);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//-----------------------------------------------------//
