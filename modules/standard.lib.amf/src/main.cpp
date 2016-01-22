//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string>
#include "amf.h"

typedef amf::amf_strict_array * am_ptr;

amf::amf_data_ptr amf_rec(void *arr);
amf::amf_object *do_object(void *pData);

void process_array(void *RESULT, amf::amf_strict_array *obj);
void process_object(void *RESULT, amf::amf_object *obj);
void process_ecma(void *RESULT, amf::amf_ecma_array *obj);
void ecma(void *RESULT, amf::ecma_array_t *e_arr);

//-------------------------//
// Local variables         //
//-------------------------//
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
void set_object(amf::amf_object *obj, char *key, void *newpData) {
    char    *szData;
    INTEGER type;
    NUMBER  nData;

    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);

    switch (type) {
        case VARIABLE_STRING:
            {
                std::string str;
                str.assign(szData, (int)nData);
                if (/*nData>0xFF*/ 0) {
                    amf::amf_long_string *s = new amf::amf_long_string(str);
                    obj->add_properity(amf::amf_string(key), amf::amf_data_ptr(s));
                } else {
                    amf::amf_string *s = new amf::amf_string(str);
                    obj->add_properity(amf::amf_string(key), amf::amf_data_ptr(s));
                }
            }
            break;

        case VARIABLE_NUMBER:
            {
                amf::amf_numeric *s = new amf::amf_numeric(nData);
                obj->add_properity(amf::amf_string(key), amf::amf_data_ptr(s));
            }
            break;

        case VARIABLE_ARRAY:
            {
                amf::amf_data_ptr o = amf_rec(newpData);
                obj->add_properity(amf::amf_string(key), o);
            }
            break;

        case VARIABLE_CLASS:
            obj->add_properity(amf::amf_string(key), amf::amf_data_ptr(do_object(szData)));
            break;

        default:
            break;
    }
}

//-----------------------------------------------------//
void set_list(amf::amf_strict_array *obj, void *newpData) {
    char    *szData;
    INTEGER type;
    NUMBER  nData;

    InvokePtr(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
    std::list<amf::amf_data_ptr> *lst = (std::list<amf::amf_data_ptr> *) & (obj->get_value());
    switch (type) {
        case VARIABLE_STRING:
            {
                std::string str;
                str.assign(szData, (int)nData);
                if (/*nData>0xFF*/ 0) {
                    amf::amf_long_string *s = new amf::amf_long_string(str);
                    lst->push_back(amf::amf_data_ptr(s));
                } else {
                    amf::amf_string *s = new amf::amf_string(str);
                    lst->push_back(amf::amf_data_ptr(s));
                }
            }
            break;

        case VARIABLE_NUMBER:
            {
                amf::amf_numeric *s = new amf::amf_numeric(nData);
                lst->push_back(amf::amf_data_ptr(s));
            }
            break;

        case VARIABLE_ARRAY:
            {
                amf::amf_data_ptr o = amf_rec(newpData);
                lst->push_back(o);
            }
            //obj->add(amf::amf_data_ptr(amf_rec(newpData)));
            break;

        case VARIABLE_CLASS:
            lst->push_back(amf::amf_data_ptr(do_object(szData)));
            break;

        default:
            break;
    }
}

//-----------------------------------------------------//
amf::amf_object *do_object(void *pData) {
    amf::amf_object *obj        = new amf::amf_object();
    char            *class_name = 0;

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
                    char *key = members[i];
                    if (key) {
                        set_object(obj, key, variable_data[i]);
                    }
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
    return obj;
}

//-----------------------------------------
amf::amf_data_ptr amf_rec(void *arr) {
    int     count = InvokePtr(INVOKE_GET_ARRAY_COUNT, arr);
    char    *res  = "";
    INTEGER len   = 0;

    void *newpData = 0;
    char *key      = 0;
    char is_array  = 0;

    if (count > 0) {
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, (INTEGER)0, &newpData);
        InvokePtr(INVOKE_GET_ARRAY_KEY, arr, (INTEGER)0, &key);
    }

    amf::amf_object       *obj  = 0;
    amf::amf_strict_array *obj3 = 0;

    if (key)
        obj = new amf::amf_object();
    else {
        is_array = 1;
        obj3     = new amf::amf_strict_array();
    }

    for (int i = 0; i < count; i++) {
        void *newpData = 0;
        char *key      = 0;
        InvokePtr(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
        if (is_array) {
            set_list(obj3, newpData);
        } else {
            InvokePtr(INVOKE_GET_ARRAY_KEY, arr, i, &key);
            if ((newpData) && (key))
                set_object(obj, key, newpData);
        }
    }
    if (obj)
        return amf::amf_data_ptr(obj);
    return amf::amf_data_ptr(obj3);
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(AMF, 1)
//T_ARRAY(0)
    __INTERNAL_PARAMETER_DECL(char *, bind, 0);
    __INTERNAL_PARAMETER_DECL(NUMBER, bind_len, 0);
    error = AnsiString(func_name) + ": parameter 1 should be an object or array";

    char *dclass = 0;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &dclass, &nDUMMY_FILL);
    if ((TYPE != VARIABLE_CLASS) && (TYPE != VARIABLE_ARRAY))
        return (void *)error.c_str();

    void *arr = PARAMETER(0);

    amf::amf_data_ptr obj;

    if (TYPE == VARIABLE_ARRAY)
        obj = amf_rec(arr);
    else
        obj = amf::amf_data_ptr(do_object(dclass));

    int size = 0;
    if (obj)
        size = obj->get_size();

    if (size > 0) {
        char *buf = new char[size + 1];
        buf[size] = 0;
        if (obj)
            obj->encode(buf);
        RETURN_BUFFER(buf, size);
        delete[] buf;
    } else
        RETURN_STRING("");
END_IMPL
//-----------------------------------------------------//
void process_array(void *RESULT, amf::amf_strict_array *obj) {
    InvokePtr(INVOKE_CREATE_ARRAY, RESULT);
    NUMBER      nVal;
    std::string sVal;
    void        *new_var;

    std::list<amf::amf_data_ptr>           *lst = (std::list<amf::amf_data_ptr> *) & (obj->get_value());
    std::list<amf::amf_data_ptr>::iterator it;

    INTEGER index = 0;
    for (it = lst->begin(); it != lst->end(); it++) {
        amf::amf_data_ptr val = *it;
        if (val) {
            void *val_ptr = val.get();
            switch (val->get_type()) {
                case amf::AMF_TYPE_NUMERIC:
                    nVal = ((amf::amf_numeric *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nVal);
                    break;

                case amf::AMF_TYPE_NULL_VALUE:
                case amf::AMF_TYPE_UNDEFINED:
                case amf::AMF_TYPE_OBJECT_END:
                    nVal = 0;
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nVal);
                    break;

                case amf::AMF_TYPE_STRING:
                    sVal = ((amf::amf_string *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_LONG_STRING:
                    sVal = ((amf::amf_long_string *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_OBJECT:
                    InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, index, &new_var);
                    process_object(new_var, (amf::amf_object *)val_ptr);
                    break;

                case amf::AMF_TYPE_STRICT_ARRAY:
                    InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, index, &new_var);
                    process_array(new_var, (amf::amf_strict_array *)val_ptr);
                    break;

                case amf::AMF_TYPE_DATE:
                    sVal = ((amf::amf_date *)val_ptr)->to_string();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_ECMA_ARRAY:
                    InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, index, &new_var);
                    process_ecma(new_var, (amf::amf_ecma_array *)val_ptr);
                    break;

                case amf::AMF_TYPE_REFERENCE:
                case amf::AMF_TYPE_UNSUPPORTED:
                case amf::AMF_TYPE_RECORD_SET:
                case amf::AMF_TYPE_XML_OBJECT:
                case amf::AMF_TYPE_MOVIECLIP:
                case amf::AMF_TYPE_TYPED_OBJECT:
                    sVal = "Unsupported";
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT, RESULT, index, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;
            }
        }
        index++;
    }
}

//-----------------------------------------------------//
void process_ecma(void *RESULT, amf::amf_ecma_array *obj) {
    InvokePtr(INVOKE_CREATE_ARRAY, RESULT);
    amf::ecma_array_t arr = obj->get_value();
    ecma(RESULT, &arr);
}

//-----------------------------------------------------//
void ecma(void *RESULT, amf::ecma_array_t *e_arr) {
    amf::ecma_array_t::iterator it;

    InvokePtr(INVOKE_CREATE_ARRAY, RESULT);
    NUMBER      nVal;
    std::string sVal;
    void        *new_var;
    for (it = e_arr->begin(); it != e_arr->end(); it++) {
        const char        *key = (*it).first.to_string().c_str();
        amf::amf_data_ptr val  = (*it).second;
        if ((key) && (val)) {
            void *val_ptr = val.get();
            switch (val->get_type()) {
                case amf::AMF_TYPE_NUMERIC:
                    nVal = ((amf::amf_numeric *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nVal);
                    break;

                case amf::AMF_TYPE_NULL_VALUE:
                case amf::AMF_TYPE_UNDEFINED:
                case amf::AMF_TYPE_OBJECT_END:
                    nVal = 0;
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_NUMBER, "", (NUMBER)nVal);
                    break;

                case amf::AMF_TYPE_STRING:
                    sVal = ((amf::amf_string *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_LONG_STRING:
                    sVal = ((amf::amf_long_string *)val_ptr)->get_value();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_OBJECT:
                    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)key, &new_var);
                    process_object(new_var, (amf::amf_object *)val_ptr);
                    break;

                case amf::AMF_TYPE_STRICT_ARRAY:
                    InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)key, &new_var);
                    process_array(new_var, (amf::amf_strict_array *)val_ptr);
                    break;

                case amf::AMF_TYPE_DATE:
                    sVal = ((amf::amf_date *)val_ptr)->to_string();
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;

                case amf::AMF_TYPE_ECMA_ARRAY:
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, &new_var);
                    process_ecma(new_var, (amf::amf_ecma_array *)val_ptr);
                    break;

                case amf::AMF_TYPE_REFERENCE:
                case amf::AMF_TYPE_UNSUPPORTED:
                case amf::AMF_TYPE_RECORD_SET:
                case amf::AMF_TYPE_XML_OBJECT:
                case amf::AMF_TYPE_MOVIECLIP:
                case amf::AMF_TYPE_TYPED_OBJECT:
                    sVal = "Unsupported";
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, key, (INTEGER)VARIABLE_STRING, sVal.length() ? sVal.c_str() : "", (NUMBER)sVal.length());
                    break;
            }
        }
    }
}

//-----------------------------------------------------//
void process_object(void *RESULT, amf::amf_object *obj) {
    amf::ecma_array_t e_arr = obj->get_value();

    ecma(RESULT, &e_arr);
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(UnAMF, 1)
    T_STRING(0)

    amf::amf_object * obj = new amf::amf_object();
    int ret = obj->decode(PARAM(0), PARAM_LEN(0));
    if (ret < 0) {
        amf::amf_strict_array *arr = new amf::amf_strict_array();
        ret = arr->decode(PARAM(0), PARAM_LEN(0));
        if (ret < 0) {
            RETURN_NUMBER(0);
        } else
            process_array(RESULT, arr);
    } else
        process_object(RESULT, obj);
END_IMPL
