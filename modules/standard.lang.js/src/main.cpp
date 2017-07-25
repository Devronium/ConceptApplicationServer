//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#ifdef _WIN32
 #define XP_WIN
#else
 #define XP_UNIX
#endif
#include "library.h"
#include <string.h>
#include "AnsiString.h"

#include <jsapi.h>
#ifdef JS_OLDAPI
    #include <jsstr.h>
    #define DECLARE_JSVAL(val)    jsval val = JSVAL_VOID;
#else
    #include "js/Initialization.h"
    #include "jsfriendapi.h"

    #define jsval JS::Value
    #define JSVAL_IS_BOOLEAN(val)   val.isBoolean()
    #define JSVAL_IS_NULL(val)      val.isNull()
    #define JSVAL_IS_STRING(val)    val.isString()
    #define JSVAL_IS_PRIMITIVE(val) !val.isObject()
    #define JSVAL_IS_DOUBLE(val)    val.isDouble()
    #define JSVAL_IS_INT(val)       val.isInt32()
    #define JSVAL_IS_VOID(val)      val.isUndefined()

    #define JSVAL_TO_DOUBLE(val)    val.toDouble()
    #define JSVAL_TO_BOOLEAN(val)   val.toBoolean()
    #define JSVAL_TO_OBJECT(val)    val.toObject()
    #define JSVAL_TO_STRING(val)    val.toString()
    #define JSVAL_TO_INT(val)       val.toInt32()

    #define DOUBLE_TO_JSVAL(n)      JS::DoubleValue(n)
    #define STRING_TO_JSVAL(s)      JS::StringValue(s)
    #define OBJECT_TO_JSVAL(o)      JS::ObjectValue(o)
    #define DECLARE_JSVAL(val)      JS::Value val;

    #define uintN                   uint64_t
    #define jsuint                  unsigned int
    #define uint32                  uint32_t
    #define jsint                   int
    #define JSBool                  bool
    #define JS_TRUE                 true
    #define JS_FALSE                false
    #define jschar                  char
    #define nullptr                 NULL
#endif
#include <map>
#include <string>
//---------------------------------------------------------------------------
INVOKE_CALL            InvokePtr        = 0;
CALL_BACK_VARIABLE_SET _SetVariable     = 0;

#ifndef JS_OLDAPI
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    JS_GlobalObjectTraceHook
};
#endif

class LocalContainer {
public:
    std::map<std::string, void *> functions;
    void *ERR_DELEGATE;
    bool err_delegate_lock;
    bool recursive;
    bool binary_mode;
    void *handler;
#ifndef JS_OLDAPI
    void *global;
#endif

    LocalContainer() {
        ERR_DELEGATE = NULL;
        handler = NULL;
#ifndef JS_OLDAPI
        global = NULL;
#endif
        recursive = true;
        err_delegate_lock = true;
        binary_mode = false;
    }

    void ReleaseDelegates(INVOKE_CALL Invoke) {
        for (std::map<std::string, void *>::const_iterator it = functions.begin(); it != functions.end(); ++it) {
            void *ft = it->second;
            if (ft) {
                FREE_VARIABLE(ft);
            }
        }
        if (!functions.empty())
            functions.clear();

        if ((ERR_DELEGATE) && (err_delegate_lock)) {
            FREE_VARIABLE(ERR_DELEGATE);
        }
        ERR_DELEGATE = NULL;
    }
};

void *GetHandler(JSContext *cx) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (data)
            return data->handler;
    }
    return NULL;
}

void SetHandler(JSContext *cx, void *handler) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        data->handler = handler;
    }
}

#ifndef JS_OLDAPI
void *GetGlobal(JSContext *cx) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (data)
            return data->global;
    }
    return NULL;
}

void SetGlobal(JSContext *cx, void *global) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        data->global = global;
    }
}
#endif

bool GetRecursive(JSContext *cx) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (data)
            return data->recursive;
    }
    return true;
}

void SetRecursive(JSContext *cx, bool recursive) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        data->recursive = recursive;
    }
}

void *GetErrorDelegate(JSContext *cx) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (data)
            return data->ERR_DELEGATE;
    }
    return NULL;
}

void *SetErrorDelegate(JSContext *cx, void *ERR_DELEGATE, bool lock) {
    void *OLD_DELEGATE = NULL;
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        if (data->err_delegate_lock)
            OLD_DELEGATE = data->ERR_DELEGATE;
        data->ERR_DELEGATE = ERR_DELEGATE;
        data->err_delegate_lock = lock;
    }
    return OLD_DELEGATE;
}

bool SetBinary(JSContext *cx, bool use_arraybuffer) {
    bool old_binary_mode = false;
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data)
            data = new LocalContainer();
        old_binary_mode = data->binary_mode;
        data->binary_mode = use_arraybuffer;
    }
    return old_binary_mode;
}

bool GetBinary(JSContext *cx) {
    bool binary_mode = false;
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (data)
            return data->binary_mode;
    }
    return binary_mode;
}

std::map<std::string, void *> *GetFunctions(JSContext *cx) {
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        return &data->functions;
    }
    return NULL;
}
//---------------------------------------------------------------------------
inline bool ejs_throw_error(JSContext *cx, JSObject *obj, const char *msg) {
#ifdef JS_OLDAPI
    JSString *jsstr;

    // if we get errors during error reporting we report those
    if (((jsstr = JS_NewStringCopyZ(cx, msg))) &&
        (JS_AddNamedRoot(cx, &jsstr, "jsstr"))) {
        DECLARE_JSVAL(dummy);
        // We can't use JS_EvaluateScript since the stack would be wrong
        JSFunction *func;
        JSObject   *fobj;
        const char *fbody      = "throw new Error(msg);";
        const char *argnames[] = { "msg" };
        if ((func = JS_CompileFunction(cx, obj, NULL,
                                       1, argnames,
                                       fbody, strlen(fbody),
                                       NULL, 0))) {
            // root function
            if (((fobj = JS_GetFunctionObject(func))) &&
                (JS_AddNamedRoot(cx, &fobj, "fobj"))) {
                jsval args[] = { STRING_TO_JSVAL(jsstr) };
                JS_CallFunction(cx, obj, func, 1, args, &dummy);
                JS_RemoveRoot(cx, &fobj);
            }
        }
        JS_RemoveRoot(cx, &jsstr);
    }
    return true;
#else
    return false;
#endif
}
//---------------------------------------------------------------------------
void ClearFunctions(JSContext *cx) {
    std::map<std::string, void *> *functions = GetFunctions(cx);
    if (functions) {
        INVOKE_CALL Invoke   = InvokePtr;
        std::map<std::string, void *>::const_iterator it = functions->begin();

        for (it = functions->begin(); it != functions->end(); ++it) {
            void *ft = it->second;
            if (ft)
                FREE_VARIABLE(ft);
        }
        if (!functions->empty())
            functions->clear();
    }
}

//---------------------------------------------------------------------------
void *SetFunction(AnsiString func_name, void *DELEGATE, JSContext *ctx, JSObject *obj = 0) {
    std::map<std::string, void *> *functions = GetFunctions(ctx);
    void *OLD_DELEGATE = NULL;
    if (functions) {
        OLD_DELEGATE = (*functions)[(const char *)func_name.c_str()];
        (*functions)[(const char *)func_name.c_str()] = DELEGATE;
    }
    return OLD_DELEGATE;
}

//---------------------------------------------------------------------------
void JS_TO_CONCEPT(void *HANDLER, JSContext *cx, void *member, jsval rval, std::map<void *, void *> *circular_map = NULL, int level = 0) {
    INVOKE_CALL Invoke   = InvokePtr;
    CALL_BACK_VARIABLE_SET SetVariable = _SetVariable;
    std::map<void *, void *> *use_map = circular_map;
    if (level > 100)
        return;
#ifdef JS_OLDAPI
    if (!use_map)
        use_map = new std::map<void *, void *>();
#endif
    if (JSVAL_IS_DOUBLE(rval)) {
#ifdef JS_OLDAPI
        SET_NUMBER_VARIABLE(member, *JSVAL_TO_DOUBLE(rval));
#else
        SET_NUMBER_VARIABLE(member, JSVAL_TO_DOUBLE(rval));
#endif
    } else
    if (JSVAL_IS_INT(rval)) {
        SET_NUMBER_VARIABLE(member, JSVAL_TO_INT(rval));
    } else
    if (JSVAL_IS_STRING(rval)) {
#ifdef JS_OLDAPI
        struct JSString *str = JSVAL_TO_STRING(rval);
        SET_BUFFER_VARIABLE(member, JS_GetStringBytes(str), str->length);
#else
        char *buf = JS_EncodeString(cx, rval.toString());
        SET_STRING_VARIABLE(member, buf);
        if (buf)
            JS_free(cx, buf);
#endif
    } else
    if (JSVAL_IS_BOOLEAN(rval)) {
        SET_NUMBER_VARIABLE(member, (NUMBER)JSVAL_TO_BOOLEAN(rval));
    } else
    if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
        SET_NUMBER_VARIABLE(member, 0);
    } else
    if (!JSVAL_IS_PRIMITIVE(rval)) {
#ifdef JS_OLDAPI
        JSObject *object = JSVAL_TO_OBJECT(rval);
        if (object) {
            void *prev_object = (*use_map)[object];
#else
            JS::RootedObject object(cx, &(rval.toObject()));
            void *prev_object = NULL;//(*use_map)[object.address()];
#endif
            if (prev_object) {
                if (GetRecursive(cx)) {
                    INTEGER    type     = 0;
                    char       *szValue = 0;
                    NUMBER     nValue   = 0;

                    Invoke(INVOKE_GET_VARIABLE, prev_object, &type, &szValue, &nValue);
                    Invoke(INVOKE_SET_VARIABLE, member, type, szValue, nValue);
                }
            } else
#ifndef JS_OLDAPI
            if (JS_IsArrayBufferObject(object)) {
                size_t nbytes = JS_GetArrayBufferByteLength(object);
                void *bytes = NULL;
                JS::AutoCheckCannotGC nogc;
                if (nbytes > 0) {
                    bool sharedDummy;
                    bytes = JS_GetArrayBufferData(object, &sharedDummy, nogc);
                }
                if ((nbytes > 0) && (bytes)) {
                    SET_BUFFER_VARIABLE(member, bytes, nbytes);
                } else {
                    SET_STRING_VARIABLE(member, "");
                }
            } else
#endif
            {
                CREATE_ARRAY(member);
#ifdef JS_OLDAPI
                (*use_map)[object] = member;
                if (JS_IsArrayObject(cx, object)) {
#else
                // (*use_map)[object.address()] = member;
                bool isArray = false;
                if ((JS_IsArrayObject(cx, object, &isArray)) && (isArray)) {
#endif
                    // is array
                    jsuint lengthp = 0;
                    if (JS_GetArrayLength(cx, object, &lengthp)) {
                        for (jsuint i = 0; i < lengthp; i++) {
#ifdef JS_OLDAPI
                            DECLARE_JSVAL(rval2);
                            if (JS_GetElement(cx, object, i, &rval2)) {
#else
                            JS::RootedValue rval2(cx);
                            if (JS_GetElement(cx, object, i, &rval2)) {
#endif
                                void *elem_data = NULL;
                                Invoke(INVOKE_ARRAY_VARIABLE, member, (INTEGER)i, &elem_data);
                                if (elem_data) {
                                    JS_TO_CONCEPT(HANDLER, cx, elem_data, rval2, use_map, level + 1);
                                }
                            }
                        }
                    }
                } else {
                    // is object
                    if (JS_ObjectIsFunction(cx, object)) {
#ifdef JS_OLDAPI
                        JSFunction *func = JS_ValueToFunction(cx, rval);
#else
                        JS::RootedValue rval_handle(cx, rval);
                        JSFunction *func = JS_ValueToFunction(cx, rval_handle);
#endif
                        if (func) {
#ifdef JS_OLDAPI
                            JSString *str = JS_GetFunctionId(func);
#else
                            JSString *str = JS_GetFunctionDisplayId(func);
                            if (!str)
                                str = JS_GetFunctionId(func);
#endif
                            if (str) {
#ifdef JS_OLDAPI
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, member, ".function", (INTEGER)VARIABLE_STRING, (char *)JS_GetStringBytes(str), (NUMBER)0);
#else
                                char *func_buf = JS_EncodeString(cx, str);
                                if (func_buf) {
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, member, ".function", (INTEGER)VARIABLE_STRING, (char *)func_buf, (NUMBER)0);
                                    JS_free(cx, func_buf);
                                }
#endif
                            }
                        }
                    }
#ifdef JS_OLDAPI
                    JSIdArray *arr = JS_Enumerate(cx, object);
                    if (arr) {
                        for (jsint i = 0; i < arr->length; i++) {
                            DECLARE_JSVAL(prop);
                            if (JS_IdToValue(cx, arr->vector[i], &prop)) {
#else
                    JS::Rooted<JS::IdVector> arr(cx, JS::IdVector(cx));
                    if (JS_Enumerate(cx, object, &arr)) {
                        jsint len = arr.length();
                        for (jsint i = 0; i < len; i++) {
                            JS::RootedValue prop(cx);
                            if (JS_IdToValue(cx, arr[i], &prop)) {
#endif
                                JSString *str = NULL;
                                AnsiString temp;
                                if (JSVAL_IS_STRING(prop))
                                    str = JSVAL_TO_STRING(prop);
                                else
                                if (JSVAL_IS_INT(prop)) {
                                    temp = (long)JSVAL_TO_INT(prop);
                                } else
                                if (JSVAL_IS_DOUBLE(prop)) {
#ifdef JS_OLDAPI
                                    temp = *((double *)JSVAL_TO_DOUBLE(prop));
#else
                                    temp = (double)JSVAL_TO_DOUBLE(prop);
#endif
                                }
                                if ((str) || (temp.Length())) {
                                    void *elem_data = NULL;
#ifdef JS_OLDAPI
                                    DECLARE_JSVAL(rval2);
                                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, member, str ? JS_GetStringBytes(str) : temp.c_str(), &elem_data);
                                    if ((elem_data) && (JS_GetProperty(cx, object, str ? JS_GetStringBytes(str) : temp.c_str(), &rval2))) {
                                        JS_TO_CONCEPT(HANDLER, cx, elem_data, rval2, use_map, level + 1);
                                    }
#else
                                    JS::RootedValue rval2(cx);
                                    char *buf = str ? JS_EncodeString(cx, str) : temp.c_str();
                                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, member, buf, &elem_data);
                                    if ((elem_data) && (JS_GetPropertyById(cx, object, arr[i], &rval2))) {
                                        JS_TO_CONCEPT(HANDLER, cx, elem_data, rval2, use_map, level + 1);
                                    }
                                    if ((buf) && (str))
                                        JS_free(cx, buf);
#endif
                                }
                            }
                        }
#ifdef JS_OLDAPI
                        JS_DestroyIdArray(cx, arr);
#endif
                    }
                }
            }
#ifdef JS_OLDAPI
        }
#endif
    } else {
        SET_NUMBER_VARIABLE(member, (NUMBER)1);
    }
    if ((use_map) && (!circular_map))
        delete use_map;
}
//---------------------------------------------------------------------------
AnsiString CONCEPT_TO_STRING(JSContext *cx, void *member) {
    INTEGER    type     = 0;
    char       *szValue = 0;
    NUMBER     nValue   = 0;
    AnsiString ret;

    InvokePtr(INVOKE_GET_VARIABLE, member, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_NUMBER:
            ret = AnsiString(nValue);
            break;

        case VARIABLE_STRING:
            ret.LoadBuffer(szValue, (int)nValue);
            break;

        default:
            ret = (char *)"1";
    }
    return ret;
}

//---------------------------------------------------------------------------
jsval CONCEPT_TO_JS(void *HANDLER, JSContext *cx, void *member, std::map<void *, int> *circular_map = NULL) {
    INTEGER type     = 0;
    char    *szValue = 0;
    NUMBER  nValue   = 0;
    DECLARE_JSVAL(ret);
    INVOKE_CALL Invoke   = InvokePtr;
    std::map<void *, int> *use_map = circular_map;
    if (!use_map)
        use_map = new std::map<void *, int>();
#ifndef JS_OLDAPI
    bool binary_mode = GetBinary(cx);
#endif
    InvokePtr(INVOKE_GET_VARIABLE, member, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_NUMBER:
#ifdef JS_OLDAPI
            ret = DOUBLE_TO_JSVAL(JS_NewDouble(cx, nValue));
#else
            ret = DOUBLE_TO_JSVAL(nValue);
#endif
            break;

        case VARIABLE_STRING:
            {
#ifdef JS_OLDAPI
                ret = STRING_TO_JSVAL(JS_NewStringCopyN(cx, szValue, (int)nValue));
#else
                if (binary_mode) {
                    size_t buf_size = (size_t)nValue;
                    void *buf2 = NULL;
                    if (buf_size) {
                        buf2 = malloc(buf_size);
                        if (buf2)
                            memcpy(buf2, szValue, buf_size);
                        else
                            buf_size = 0;
                    }
                    // Ownership is tranferred to the new array, do not call free!
                    JS::RootedObject object(cx, JS_NewArrayBufferWithContents(cx, buf_size, buf2));
                    ret = OBJECT_TO_JSVAL(*object);
                } else {
                    JSString *str = JS_NewStringCopyN(cx, szValue, (int)nValue);
                    ret = STRING_TO_JSVAL(str);
                }
#endif
            }
            break;

        case VARIABLE_ARRAY:
            {
                // circular reference
                if ((*use_map)[(void *)szValue])
                    break;
                (*use_map)[(void *)szValue] = 1;
                int count = Invoke(INVOKE_GET_ARRAY_COUNT, member);
#ifdef JS_OLDAPI
                jsval *values = NULL;
                JSObject *object = NULL;
#endif
                if (count) {
                    char *key = NULL;
                    void *elem_data = NULL;
                    Invoke(INVOKE_GET_ARRAY_KEY, member, 0, &key);
                    if (key) {
#ifdef JS_OLDAPI
                        object = JS_NewObject(cx, NULL, NULL, NULL);
#else
                        JS::RootedObject object(cx, JS_NewObject(cx, NULL));
#endif
                        for (INTEGER i = 0; i < count; i++) {
                            InvokePtr(INVOKE_ARRAY_VARIABLE, member, i, &elem_data);
                            InvokePtr(INVOKE_GET_ARRAY_KEY, member, i, &key);
                            if (key) {
#ifdef JS_OLDAPI
                                jsval temp = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, &temp);
#else
                                JS::RootedValue temp(cx);
                                temp = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, temp);
#endif
                            }
                        }
#ifndef JS_OLDAPI
                        ret = OBJECT_TO_JSVAL(*object);
#endif
                    } else {
#ifdef JS_OLDAPI
                        values = (jsval *)malloc(sizeof(jsval) * count);
                        if (values) {
                            for (INTEGER i = 0; i < count; i++) {
                                values[i] = JSVAL_VOID;
                                Invoke(INVOKE_ARRAY_VARIABLE, member, (INTEGER)i, &elem_data);
                                if (elem_data)
                                    values[i] = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                            }
                        }
                        object = JS_NewArrayObject(cx, count, values);
                        if (values)
                            free(values);
#else
                        JS::RootedObject object(cx, JS_NewArrayObject(cx, count));
                        for (INTEGER i = 0; i < count; i++) {
                             Invoke(INVOKE_ARRAY_VARIABLE, member, (INTEGER)i, &elem_data);
                             JS::RootedValue temp(cx);
                             if (elem_data)
                                 temp = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                             JS_DefineElement(cx, object, i, temp, JSPROP_ENUMERATE);
                        }
                        ret = OBJECT_TO_JSVAL(*object);
#endif
                    }
                } else {
#ifdef JS_OLDAPI
                    object = JS_NewArrayObject(cx, 0, NULL);
#else
                    JS::RootedObject object(cx, JS_NewArrayObject(cx, count));
                    ret = OBJECT_TO_JSVAL(*object);
#endif
                }
#ifdef JS_OLDAPI
                if (object)
                    ret = OBJECT_TO_JSVAL(object);
#endif
            }
            break;

        case VARIABLE_CLASS:
            {
                // circular reference
                if ((*use_map)[(void *)szValue])
                    break;
                (*use_map)[(void *)szValue] = 1;
#ifdef JS_OLDAPI
                JSObject * object = JS_NewObject(cx, NULL, NULL, NULL);
                if (object) {
#else
                    JS::RootedObject object(cx, JS_NewObject(cx, NULL));
#endif
                    char *class_name = NULL;
                    int members_count = Invoke(INVOKE_GET_SERIAL_CLASS, szValue, (int)0, &class_name, (char **)0, (char *)0, (char *)0, (char *)0, (char **)0, (NUMBER *)0, (char *)0, (char *)0);
                    char   **members       = new char * [members_count];
                    char   *flags          = new char[members_count];
                    char   *access         = new char[members_count];
                    char   *types          = new char[members_count];
                    char   **szValues      = new char * [members_count];
                    NUMBER *nValues        = new NUMBER[members_count];
                    void   **class_data    = new void * [members_count];
                    void   **variable_data = new void * [members_count];

                    int result = Invoke(INVOKE_GET_SERIAL_CLASS, szValue, members_count, &class_name, members, flags, access, types, (const char **)szValues, nValues, class_data, variable_data);
                    if (IS_OK(result)) {
                        for (int i = 0; i < members_count; i++) {
                             char *key = members[i];
                             void *elem_data =  variable_data[i];
                             if ((key) && (elem_data)) {
#ifdef JS_OLDAPI
                                jsval temp = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, &temp);
#else
                                JS::RootedValue temp(cx);
                                if (elem_data)
                                    temp = CONCEPT_TO_JS(HANDLER, cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, temp);
#endif
                             }
                        }
                    }
#ifdef JS_OLDAPI
                    ret = OBJECT_TO_JSVAL(object);
#else
                    ret = OBJECT_TO_JSVAL(*object);
#endif

                    delete[] members;
                    delete[] flags;
                    delete[] access;
                    delete[] types;
                    delete[] szValues;
                    delete[] nValues;
                    delete[] class_data;
                    delete[] variable_data;
#ifdef JS_OLDAPI
                }
#endif
            }
            break;

        default:
#ifdef JS_OLDAPI
            ret = DOUBLE_TO_JSVAL(JS_NewDouble(cx, 1.0));
#else
            ret = DOUBLE_TO_JSVAL(1.0);
#endif
    }
    if ((use_map) && (!circular_map))
        delete use_map;
    return ret;
}

//---------------------------------------------------------------------------
#ifdef JS_OLDAPI
static JSBool function_handler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
#else
static bool function_handler(JSContext *cx, unsigned argc, JS::Value *vp) {
#endif
    int                    count, i;
    char                   *fun_name;
    INVOKE_CALL            Invoke      = InvokePtr;
    CALL_BACK_VARIABLE_SET SetVariable = _SetVariable;
    void                   *HANDLER    = GetHandler(cx);

#ifdef JS_OLDAPI
    /* Differetiate between direct and method-like call */
    if ((JS_TypeOfValue(cx, argv[-2]) == JSTYPE_FUNCTION) && strcmp("call", JS_GetFunctionName(JS_ValueToFunction(cx, argv[-2])))) {
        fun_name = (char *)JS_GetFunctionName((JSFunction *)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[-2])));
        i        = 0;
    } else {
        fun_name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
        i        = 1;
    }
#else
    i = 0;
    JS::CallArgs argv = CallArgsFromVp(argc, vp);
    JS::Value callee = argv.calleev();
    if (callee.isString()) {
        JSString *str = callee.toString();
        fun_name = JS_EncodeString(cx, str);
    } else
    if (callee.isObject()) {
        JS::RootedValue func_val(cx, callee);
        JSFunction* fun = JS_ValueToFunction(cx, func_val);
        if (fun) {
            JSString *str = JS_GetFunctionId(fun);
            fun_name = JS_EncodeString(cx, str);
        }
    }
#endif
    std::map<std::string, void *> *functions = GetFunctions(cx);
    void *ft = NULL;
    if (functions)
        ft = (*functions)[(const char *)fun_name];
#ifndef JS_OLDAPI
    if (fun_name)
        JS_free(cx, fun_name);
#endif
    if (!ft)
        return JS_FALSE;

    int len = argc - i;
    // antibug
    if (len < 0)
        len = 0;
    void **PARAMETERS = new void * [len + 1];
    PARAMETERS[len] = 0;


    int first_arg = i;
    int index     = 0;
    for ( ; i < argc; i++) {
        jsval param = argv[i];
        CREATE_VARIABLE(PARAMETERS[index]);
        JS_TO_CONCEPT(HANDLER, cx, PARAMETERS[index], param);
        index++;
    }

    // -1 in parameter count means that we will provide a parameter list;
    void *EXCEPTION = 0;
    void *RES       = 0;
    Invoke(INVOKE_CALL_DELEGATE, ft, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);
    if (RES) {
#ifdef JS_OLDAPI
        *rval = CONCEPT_TO_JS(HANDLER, cx, RES);
#else
        argv.rval().set(CONCEPT_TO_JS(HANDLER, cx, RES));
#endif
        Invoke(INVOKE_FREE_VARIABLE, RES);
    }
    if (EXCEPTION) {
#ifdef JS_OLDAPI
        ejs_throw_error(cx, obj, CONCEPT_TO_STRING(cx, EXCEPTION).c_str());
#else
        JSObject *global = JS_GetGlobalForObject(cx, &argv.callee());
        if (global)
            ejs_throw_error(cx, global, CONCEPT_TO_STRING(cx, EXCEPTION).c_str());
#endif
        Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
    }

    for (i = 0; i < index; i++) {
        //argv[i+first_arg]=CONCEPT_TO_JS(cx, PARAMETERS[i]);
        FREE_VARIABLE(PARAMETERS[i]);
    }
    if (PARAMETERS)
        delete[] PARAMETERS;

    return JS_TRUE;
}

//---------------------------------------------------------------------------
void printError(JSContext *cx, const char *message, JSErrorReport *report) {
    char *cstr = 0;
    int  where;

    fprintf(stderr, "JSERROR: %s:%d:\n    %s\n",
            (report->filename ? report->filename : "NULL"),
            report->lineno,
            message);
#ifdef JS_OLDAPI
    if (report->linebuf) {
        cstr = (char *)report->linebuf;
        fprintf(stderr, "    \"%s\"\n", cstr);
        if (report->tokenptr) {
            where = report->tokenptr - report->linebuf;
            /* Todo: 80 */
            if ((where >= 0) && (where < 80)) {
                where += 6;
                while (--where > 0) fputc(' ', stderr);
                fprintf(stderr, "^\n");
            }
        }
    }
#endif
    fprintf(stderr, "    Flags:");
    if (JSREPORT_IS_WARNING(report->flags)) fprintf(stderr, " WARNING");
    if (JSREPORT_IS_EXCEPTION(report->flags)) fprintf(stderr, " EXCEPTION");
    if (JSREPORT_IS_STRICT(report->flags)) fprintf(stderr, " STRICT");
    fprintf(stderr, " (Error number: %d)\n", report->errorNumber);
    /* print exception object */
    //printJSException(cx);
}

//---------------------------------------------------------------------------
int char16len(const jschar *s) {
    int len = 0;
    while ((s) && (s[len]))
        len++;
    return len;
}

void ShowError(JSContext *cx, const char *message, JSErrorReport *report) {
    int  where;
    char *cstr;

    void                   *CREPORT;
    INVOKE_CALL            Invoke           = InvokePtr;
    CALL_BACK_VARIABLE_SET SetVariable      = _SetVariable;
    void                   *ERR_DELEGATE    = GetErrorDelegate(cx);

    if (!ERR_DELEGATE) {
        printError(cx, message, report);
        return;
    }

    CREATE_VARIABLE(CREPORT);
    CREATE_ARRAY(CREPORT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "filename", (INTEGER)VARIABLE_STRING, (char *)report->filename, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "lineno", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->lineno);
#ifdef JS_OLDAPI
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "linebuf", (INTEGER)VARIABLE_STRING, (char *)report->linebuf, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "tokenptr", (INTEGER)VARIABLE_STRING, (char *)report->tokenptr, (NUMBER)0);
    // Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "uclinebuf", (INTEGER)VARIABLE_STRING, (char *)report->uclinebuf, (NUMBER)0);
    // Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "uctokenptr", (INTEGER)VARIABLE_STRING, (char *)report->uctokenptr, (NUMBER)0);
#endif
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->flags);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "errorNumber", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->errorNumber);
#ifdef JS_OLDAPI
    // if (report->ucmessage) {
    //     JSString *str = JS_NewUCString(cx, (jschar *)report->ucmessage, char16len(report->ucmessage));
    //     Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "ucmessage", (INTEGER)VARIABLE_STRING, (char *)JS_GetStringBytes(str), (NUMBER)str->length);
    // }
#endif

/*#ifdef JS_OLDAPI
    void *elem_data = NULL;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, CREPORT, "messageArgs", &elem_data);
    if (elem_data) {
        CREATE_ARRAY(elem_data);
    }
    int i = 0;
    const jschar *messageArg = report->messageArgs ? report->messageArgs[i] : 0;
    while (messageArg) {
        JSString *str = JS_NewUCString(cx, (jschar *)messageArg, char16len(messageArg));
        Invoke(INVOKE_SET_ARRAY_ELEMENT, elem_data, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)JS_GetStringBytes(str), (NUMBER)str->length);
        messageArg = report->messageArgs[++i];
    }
#endif*/
    // call delegate
    CALL_DELEGATE(ERR_DELEGATE, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)cx, (INTEGER)VARIABLE_STRING, (char *)message, (NUMBER)0, (INTEGER)VARIABLE_UNDEFINED, CREPORT);
    FREE_VARIABLE(CREPORT);
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr       = Invoke;
#ifndef JS_OLDAPI
    JS_Init();
#endif
    /*DEFINE_CLASS("JSErrorReport",
                 "filename",
                 "lineno",
                 "linebuf",
                 "tokenptr",
                 "uclinebuf",
                 "uctokenptr",
                 "flags",
                 "errorNumber",
                 "ucmessage",
                 "messageArgs"
                 );*/
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewRuntime, 1)
    T_NUMBER(JSNewRuntime, 0)     // uint32
    _SetVariable = SetVariable;

    RETURN_NUMBER((SYS_INT)JS_NewRuntime((SYS_INT)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSDestroyRuntime, 1)
    T_HANDLE(JSDestroyRuntime, 0)     // JSRuntime*

    JS_DestroyRuntime((JSRuntime *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSShutDown, 0)
    JS_ShutDown();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSLock, 1)
    T_HANDLE(JSLock, 0)     // JSRuntime*
#ifdef JS_OLDAPI
    JS_Lock((JSRuntime *)(SYS_INT)PARAM(0));
#endif
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSUnlock, 1)
    T_HANDLE(JSUnlock, 0)     // JSRuntime*
#ifdef JS_OLDAPI
    JS_Unlock((JSRuntime *)(SYS_INT)PARAM(0));
#endif
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewContext, 2)
    T_HANDLE(JSNewContext, 0)     // JSRuntime*
    T_NUMBER(JSNewContext, 1)     // size_t
    _SetVariable = SetVariable;

    JSContext *context = JS_NewContext((JSRuntime *)(SYS_INT)PARAM(0), (size_t)PARAM(1));
    if (context)
        SetHandler(context, PARAMETERS->HANDLER);
    RETURN_NUMBER((SYS_INT)context)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSDestroyContext, 1)
    T_HANDLE(JSDestroyContext, 0)     // JSContext*
    LocalContainer *data = (LocalContainer *)JS_GetContextPrivate((JSContext *)(SYS_INT)PARAM(0));
    if (data) {
#ifndef JS_OLDAPI
        JS::RootedObject *global = (JS::RootedObject *)data->global;
        if (global)
            delete global;
#endif
        data->ReleaseDelegates(Invoke);
        delete data;
    }
    JS_DestroyContext((JSContext *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSInitStandardClasses, 2)
    T_HANDLE(JSInitStandardClasses, 0)     // JSContext*
    T_HANDLE(JSInitStandardClasses, 1)
    _SetVariable = SetVariable;
#ifdef JS_OLDAPI
    RETURN_NUMBER(JS_InitStandardClasses((JSContext *)(SYS_INT)PARAM(0), (JSObject *)(SYS_INT)PARAM(1)))
#else
    JSObject *obj = (JSObject *)(SYS_INT)PARAM(1);
    JS::RootedObject global((JSContext *)(SYS_INT)PARAM(0), obj);
    JSAutoCompartment ac((JSContext *)(SYS_INT)PARAM(0), global);
    RETURN_NUMBER(JS_InitStandardClasses((JSContext *)(SYS_INT)PARAM(0), global))
#endif
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSSetErrorReporter, 2, 3)
    T_HANDLE(JSSetErrorReporter, 0)     // JSContext*
    T_DELEGATE(JSSetErrorReporter, 1)

    bool lock = true;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(JSSetErrorReporter, 2);
        if (!PARAM_INT(2))
            lock = false;
    }
    if (!(PARAM_INT(0)))
        return (void *)"JSSetErrorReporter: Invalid JSContext (is null)";

    void *ERR_DELEGATE = SetErrorDelegate(((JSContext *)(SYS_INT)PARAM(0)), PARAMETER(1), lock);
    if (ERR_DELEGATE)
        FREE_VARIABLE(ERR_DELEGATE);
    if (lock)
        LOCK_VARIABLE(PARAMETER(1));
#ifdef JS_OLDAPI
    JS_SetErrorReporter(((JSContext *)(SYS_INT)PARAM(0)), ShowError);
#else
    JS_SetErrorReporter(JS_GetRuntime(((JSContext *)(SYS_INT)PARAM(0))), ShowError);
#endif
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewObject, 4)
    T_HANDLE(JSNewObject, 0) // JSContext*
    T_NUMBER(JSNewObject, 1) // JSClass*
    T_NUMBER(JSNewObject, 2) // JSObject*
    T_NUMBER(JSNewObject, 3) // JSObject*

#ifdef JS_OLDAPI
    RETURN_NUMBER((SYS_INT)JS_NewObject((JSContext *)(SYS_INT)PARAM(0), (JSClass *)PARAM_INT(1), (JSObject *)(SYS_INT)PARAM(2), (JSObject *)(SYS_INT)PARAM(3)))
#else
    JSObject *parent = (JSObject *)(SYS_INT)PARAM(3);
    if (parent) {
        JSObject *object = JS_NewObject((JSContext *)(SYS_INT)PARAM(0), (JSClass *)PARAM_INT(1));
        RETURN_NUMBER((SYS_INT)object);
        return 0;
    }
    JS::RootedObject *old_global = (JS::RootedObject *)GetGlobal((JSContext *)(SYS_INT)PARAM(0));
    if (old_global) {
        RETURN_NUMBER((SYS_INT)old_global)
        return 0;
    }
    JSObject *global = JS_NewGlobalObject((JSContext *)(SYS_INT)PARAM(0), &global_class, nullptr, JS::FireOnNewGlobalHook);
    SetGlobal((JSContext *)(SYS_INT)PARAM(0), new JS::RootedObject((JSContext *)(SYS_INT)PARAM(0), global));
    RETURN_NUMBER((SYS_INT)global)
#endif
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSGC, 2)
    T_HANDLE(JSGC, 0)
    T_HANDLE(JSGC, 1)
#ifdef JS_OLDAPI
    JSContext *cx = (JSContext *)(SYS_INT)PARAM(0);
#else
    JSRuntime *cx = (JSRuntime *)(SYS_INT)PARAM(1);
#endif
    if (cx) {
        JS_GC(cx);
        RETURN_NUMBER(1);
    } else {
        RETURN_NUMBER(0);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSEvaluateScript, 6)
    T_HANDLE(JSEvaluateScript, 0)     // JSContext*
    T_HANDLE(JSEvaluateScript, 1)
    T_STRING(JSEvaluateScript, 2)     // char*
    T_STRING(JSEvaluateScript, 3)     // char*
    T_NUMBER(JSEvaluateScript, 4)     // uintN

// ... parameter 6 is by reference (jsval*)
    bool is_ok = false;
    JSContext *cx = (JSContext *)(SYS_INT)PARAM(0);
#ifdef JS_OLDAPI
    DECLARE_JSVAL(rval);
    RETURN_NUMBER(is_ok = JS_EvaluateScript(cx, (JSObject *)(SYS_INT)PARAM(1), PARAM(2), (uintN)PARAM_LEN(2), PARAM(3), (uintN)PARAM(4), &rval))
#else
    JSAutoRequest ar(cx);

    JSObject *obj = (JSObject *)(SYS_INT)PARAM(1);
    JS::RootedObject global(cx, obj);
    JS::RootedValue rval(cx);
    JSAutoCompartment ac(cx, global);
    {
        JS::CompileOptions opts(cx);
        opts.setFileAndLine(PARAM(3), PARAM_INT(4));
        is_ok = JS::Evaluate(cx, opts, PARAM(2), PARAM_LEN(2), &rval);
    }
    RETURN_NUMBER(is_ok);
#endif
    SET_NUMBER(5, (SYS_INT)0)
    if (is_ok) {
        if (JSVAL_IS_DOUBLE(rval)) {
#ifdef JS_OLDAPI
            SET_NUMBER(5, *JSVAL_TO_DOUBLE(rval));
#else
            SET_NUMBER(5, rval.toDouble());
#endif
        } else
        if (JSVAL_IS_INT(rval)) {
            SET_NUMBER(5, JSVAL_TO_INT(rval));
        } else
        if (JSVAL_IS_STRING(rval)) {
            struct JSString *str = JSVAL_TO_STRING(rval);
#ifdef JS_OLDAPI
            SET_BUFFER(5, JS_GetStringBytes(str), (NUMBER)str->length);
#else
            char *buf = JS_EncodeString(cx, str);
            SET_STRING(5, buf);
            if (buf)
                JS_free(cx, buf);
#endif
        } else
        if (JSVAL_IS_BOOLEAN(rval)) {
            SET_NUMBER(5, (NUMBER)JSVAL_TO_BOOLEAN(rval));
        } else
        if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
            SET_NUMBER(5, (NUMBER)0);
        } else {
            JS_TO_CONCEPT(PARAMETERS->HANDLER, (JSContext *)(SYS_INT)PARAM(0),PARAMETER(5), rval);
        }
        // SET_NUMBER(5, (NUMBER)1);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSThrow, 3)
    T_HANDLE(JSThrow, 0)     // JSContext*
    T_NUMBER(JSThrow, 1)
    T_STRING(JSThrow, 2)
    _SetVariable = SetVariable;

    RETURN_NUMBER(ejs_throw_error((JSContext *)(SYS_INT)PARAM(0), (JSObject *)(SYS_INT)PARAM(1), PARAM(2)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSEval, 1)
    T_STRING(JSEval, 0)
    JSRuntime * runtime = NULL;
    JSContext *context = NULL;

    // important enclose code in { } (context)
    {
        RETURN_NUMBER(0);
    #ifdef JS_OLDAPI
        JSObject  *global  = NULL;
        DECLARE_JSVAL(rval);

        /* create new runtime, new context, global object */
        if ((!(runtime = JS_NewRuntime(1024L * 1024L))) ||
            (!(context = JS_NewContext(runtime, 8192))) ||
            (!(global = JS_NewObject(context, NULL, NULL, NULL)))
            )
            return (void *)"JSEval: error creating runtime/context/global";

        SetHandler(context, PARAMETERS->HANDLER);
        if (!JS_InitStandardClasses(context, global))
            return (void *)"JSEval: initialising standard classes";

        JS_SetErrorReporter(context, printError);
        bool is_ok = JS_EvaluateScript(context, global, PARAM(0), PARAM_LEN(0), "script.js", 1, &rval);
    #else
        if ((!(runtime = JS_NewRuntime(1024L * 1024L))) || (!(context = JS_NewContext(runtime, 8192))))
            return (void *)"JSEval: error creating runtime/context";

        SetHandler(context, PARAMETERS->HANDLER);
        JSAutoRequest ar(context);
        JS::RootedObject global(context, JS_NewGlobalObject(context, &global_class, nullptr, JS::FireOnNewGlobalHook));
        if (!global)
            return (void *)"JSEval: error creating global object";

        JS::RootedValue rval(context);
        bool is_ok = false;
        JSAutoCompartment ac(context, global);
        {
            JS_InitStandardClasses(context, global);
            JS::CompileOptions opts(context);
            opts.setFileAndLine("script.js", 1);
            is_ok = JS::Evaluate(context, opts, PARAM(0), PARAM_LEN(0), &rval);
        }
    #endif
        if (is_ok) {
            if (JSVAL_IS_DOUBLE(rval)) {
    #ifdef JS_OLDAPI
                RETURN_NUMBER(*JSVAL_TO_DOUBLE(rval));
    #else
                RETURN_NUMBER(JSVAL_TO_DOUBLE(rval));
    #endif
            } else
            if (JSVAL_IS_INT(rval)) {
                RETURN_NUMBER(JSVAL_TO_INT(rval));
            } else
            if (JSVAL_IS_STRING(rval)) {
                struct JSString *str = JSVAL_TO_STRING(rval);
    #ifdef JS_OLDAPI
                RETURN_BUFFER(JS_GetStringBytes(str), str->length);
    #else
                char *buf = JS_EncodeString(context, str);
                RETURN_STRING(buf);
                if (buf)
                    JS_free(context, buf);
    #endif
            } else
            if (JSVAL_IS_BOOLEAN(rval)) {
                RETURN_NUMBER((NUMBER)JSVAL_TO_BOOLEAN(rval));
            } else
            if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
                RETURN_NUMBER(0);
            } else {
                JS_TO_CONCEPT(PARAMETERS->HANDLER, context, RESULT, rval);
            }
        }
    }
    JS_DestroyContext(context);
    JS_DestroyRuntime(runtime);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSWrap, 4)
    T_HANDLE(JSWrap, 0) // JSContext*
    T_HANDLE(JSWrap, 1) // JSObject*
    T_DELEGATE(JSWrap, 2)
    T_STRING(JSWrap, 3) // function name
    _SetVariable = SetVariable;

    void *OLD_DELEGATE = SetFunction(PARAM(3), PARAMETER(2), (JSContext *)PARAM_INT(0), (JSObject *)(SYS_INT)PARAM(1));
    LOCK_VARIABLE(PARAMETER(2));
    if (OLD_DELEGATE) {
        FREE_VARIABLE(OLD_DELEGATE);
    }
    int param_count = Invoke(INVOKE_COUNT_DELEGATE_PARAMS, PARAMETER(2));
#ifdef JS_OLDAPI
    RETURN_NUMBER((JS_DefineFunction((JSContext *)(SYS_INT)PARAM(0), (JSObject *)(SYS_INT)PARAM(1), PARAM(3), function_handler, param_count, 0) != 0));
#else
    JSObject *obj = (JSObject *)(SYS_INT)PARAM(1);
    {
        JS::RootedObject global((JSContext *)(SYS_INT)PARAM(0), obj);
        JSAutoCompartment ac((JSContext *)(SYS_INT)PARAM(0), global);
        RETURN_NUMBER((JS_DefineFunction((JSContext *)(SYS_INT)PARAM(0), global, PARAM(3), function_handler, param_count, 0) != 0));
    }
#endif
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSRecursive, 2) 
    T_HANDLE(JSWrap, 0) // JSContext*
    T_NUMBER(JSWrap, 1)
    SetRecursive((JSContext *)(SYS_INT)PARAM(0), (bool)PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSObjectKey, 4)
    T_HANDLE(JSObjectKey, 0) // JSContext*
    T_HANDLE(JSObjectKey, 1) // JSObject*
    T_STRING(JSObjectKey, 2);

#ifdef JS_OLDAPI
    jsval val = CONCEPT_TO_JS(PARAMETERS->HANDLER, (JSContext *)(SYS_INT)PARAM(0), PARAMETER(3));
    bool prop_set = JS_SetProperty((JSContext *)(SYS_INT)PARAM(0), (JSObject *)(SYS_INT)PARAM(1), PARAM(2), &val);
#else
    JSObject *obj = (JSObject *)(SYS_INT)PARAM(1);
    JS::RootedObject global((JSContext *)(SYS_INT)PARAM(0), obj);
    JSAutoCompartment ac((JSContext *)(SYS_INT)PARAM(0), global);
    jsval val = CONCEPT_TO_JS(PARAMETERS->HANDLER, (JSContext *)(SYS_INT)PARAM(0), PARAMETER(3));
    JS::RootedValue root_val((JSContext *)(SYS_INT)PARAM(0), val);
    bool prop_set = JS_SetProperty((JSContext *)(SYS_INT)PARAM(0), global, (const char *)PARAM(2), root_val);
#endif
    RETURN_NUMBER((int)prop_set);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSCall, 4)
    T_HANDLE(JSCall, 0) // JSContext*
    T_HANDLE(JSCall, 1) // JSObject*
    T_STRING(JSCall, 2) // name
    T_ARRAY(JSCall, 3)  // argv
    RETURN_NUMBER(0);

    JSContext *cx = (JSContext *)(SYS_INT)PARAM(0);
    JSObject *obj = (JSObject *)(SYS_INT)PARAM(1);
    unsigned int argc = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(3));
#ifdef JS_OLDAPI
    jsval *argv = NULL;
    if (argc) {
        argv = (jsval *)malloc(sizeof(jsval) * argc);
        for (INTEGER i = 0; i < argc; i++) {
            argv[i] = JSVAL_VOID;
            void *elem_data = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(3), (INTEGER)i, &elem_data);
            if (elem_data) {
                argv[i] = CONCEPT_TO_JS(PARAMETERS->HANDLER, cx, elem_data);
            }
        }
    }
    DECLARE_JSVAL(rval);
    bool called = JS_CallFunctionName(cx, obj, PARAM(2), argc, argv, &rval);
    if (argv)
        free(argv);
#else
    JS::RootedObject global(cx, obj);
    JSAutoCompartment ac(cx, global);

    jsval *argv = NULL;
    if (argc) {
        argv = new jsval[argc];
        for (INTEGER i = 0; i < argc; i++) {
            void *elem_data = NULL;
            Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(3), (INTEGER)i, &elem_data);
            if (elem_data) {
                argv[i] = CONCEPT_TO_JS(PARAMETERS->HANDLER, cx, elem_data);
            }
        }
    }
    JS::HandleValueArray args = JS::HandleValueArray::fromMarkedLocation(argc, argv);

    JS::RootedValue rval(cx);
    bool called = JS_CallFunctionName(cx, global, PARAM(2), args, &rval);
    if (argv)
        delete[] argv;
#endif
    if (called) {
        JS_TO_CONCEPT(PARAMETERS->HANDLER, cx, RESULT, rval);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(JSModeBinary, 1, 2)
    T_HANDLE(JSModeBinary, 0)
    bool binary_mode = true;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(JSModeBinary, 1)
        if (!PARAM_INT(1))
            binary_mode = false;
    }
    bool old_binary_mode = SetBinary((JSContext *)(SYS_INT)PARAM(0), binary_mode);
    RETURN_NUMBER((int)old_binary_mode);
END_IMPL
//------------------------------------------------------------------------
