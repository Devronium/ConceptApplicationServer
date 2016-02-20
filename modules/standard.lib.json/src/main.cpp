//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

extern "C" {
#include <json.h>
#include <math.h>
}
//-------------------------//
// Local variables         //
//-------------------------//
struct json_object *do_array(void *arr, bool as_object = false);

#define JSON_NUMBER(my_elem, nData)                         \
    if (floor(nData) == nData) {                            \
        if ((nData >= 0x7FFFFFFF) || (nData < -0x7FFFFFFE)) \
            my_elem = json_object_new_double(nData);        \
        else                                                \
            my_elem = json_object_new_int((int)nData);      \
    } else                                                  \
        my_elem = json_object_new_double(nData);

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
struct json_object *do_object(void *pData, bool as_object = false) {
    struct json_object *my_obj = 0;
    char *class_name           = 0;

    my_obj = json_object_new_object();

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
                    struct json_object *my_elem = 0;
                    switch (types[i]) {
                        case VARIABLE_STRING:
                            my_elem = json_object_new_string_len(szValues[i], (int)nValues[i]);
                            break;

                        case VARIABLE_NUMBER:
                            //my_elem=json_object_new_double(nValues[i]);
                            JSON_NUMBER(my_elem, nValues[i]);
                            break;

                        case VARIABLE_ARRAY:
                            my_elem = do_array(variable_data[i], as_object);
                            break;

                        case VARIABLE_CLASS:
                            my_elem = do_object((void *)class_data[i], as_object);
                            break;

                        default:
                            break;
                    }
                    if (my_elem)
                        json_object_object_add(my_obj, (char *)(members[i] ? members[i] : ""), my_elem);
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
struct json_object *do_array(void *arr, bool as_object) {
    struct json_object *my_obj = 0;
    void *newpData;
    char *key;

    int count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);

    bool local_as_object = as_object;
    if ((count > 0) && (as_object)) {
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, 0, &key);
        if ((!key) || (!key[0]))
            local_as_object = false;
    }

    if (local_as_object) {
        my_obj = json_object_new_object();
    } else
        my_obj = json_object_new_array();

    for (int i = 0; i < count; i++) {
        newpData = 0;
        key      = 0;
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
        if (newpData) {
            char               *szData;
            INTEGER            type;
            NUMBER             nData;
            struct json_object *my_elem = 0;

            if ((local_as_object) && (!key))
                continue;

            InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

            switch (type) {
                case VARIABLE_STRING:
                    my_elem = json_object_new_string_len(szData, (int)nData);
                    break;

                case VARIABLE_NUMBER:
                    //my_elem=json_object_new_double(nData);
                    JSON_NUMBER(my_elem, nData);
                    break;

                case VARIABLE_ARRAY:
                    my_elem = do_array(newpData, as_object);
                    break;

                case VARIABLE_CLASS:
                    my_elem = do_object((void *)szData);
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
        }
    }

    return my_obj;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(_JSONSerialize, 1)
    char *dclass = 0;
    NUMBER dmember   = 0;
    bool   as_object = false;

    if (PARAMETERS_COUNT > 2) {
        return (void *)"JSONSerialize: invalid parameters count: object, array_as_objects=false";
    } else
    if (PARAMETERS_COUNT == 2) {
        T_NUMBER(1)

        as_object = (int)PARAM_INT(1);
    }

    struct json_object *my_obj = 0;

    GET_DELEGATE(0, dclass, dmember)

    switch (TYPE) {
        case VARIABLE_STRING:
            my_obj = json_object_new_string_len(dclass, (int)dmember);
            break;

        case VARIABLE_NUMBER:
            //my_obj=json_object_new_double(dmember);
            JSON_NUMBER(my_obj, dmember);
            break;

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
        RETURN_STRING(json_object_to_json_string(my_obj))
    } else {
        RETURN_STRING("")
    }
END_IMPL
//-----------------------------------------------------//
void deserialize_object(struct json_object *obj, void *RESULT, json_type mtype) {
    if (mtype == json_type_array) {
        InvokePtr(INVOKE_CREATE_ARRAY, RESULT, 0);
        int len = json_object_array_length(obj);
        for (int i = 0; i < len; i++) {
            struct json_object *val = json_object_array_get_idx(obj, i);
            if (val) {
                json_type type      = json_object_get_type(val);
                void      *var_data = 0;
                INTEGER   i_type;
                char      *str = 0;
                NUMBER    nvalue;
                void      *new_result = 0;

                switch (type) {
                    case json_type_boolean:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_boolean(val));
                        break;

                    case json_type_double:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_double(val));
                        break;

                    case json_type_int:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)json_object_get_int(val));
                        break;

                    case json_type_string:
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)json_object_get_string(val), (NUMBER)0);
                        break;

                    case json_type_object:
                    case json_type_array:
                        InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &new_result);
                        deserialize_object(val, new_result, type);
                        break;
                }

                //InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, VARIABLE_STRING, (char *)"", (NUMBER)0);
            } else
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)0);
        }
    } else
    if (mtype == json_type_object) {
        InvokePtr(INVOKE_CREATE_ARRAY, RESULT, 0);

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
    }
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_JSONDeserialize, 1)
    T_STRING(0)

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
                //return (void *)"JSONDeserialize: Invalid JSON string";
                RETURN_NUMBER(0)
        }
    } else {
        //return (void *)"JSONDeserialize: Invalid JSON string";
        RETURN_NUMBER(0)
    }
    if (obj)
        json_tokener_free(tok);
END_IMPL
//-----------------------------------------------------//
