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
#include <jsstr.h>
#include <map>
#include <string>
//---------------------------------------------------------------------------
void                   *CONCEPT_HANDLER = 0;
INVOKE_CALL            InvokePtr        = 0;
CALL_BACK_VARIABLE_SET _SetVariable     = 0;

class LocalContainer {
public:
    std::map<std::string, void *> functions;
    void *ERR_DELEGATE;
    bool recursive;

    LocalContainer() {
        ERR_DELEGATE = NULL;
        recursive = true;
    }
};

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

void *SetErrorDelegate(JSContext *cx, void *ERR_DELEGATE) {
    void *OLD_DELEGATE = NULL;
    if (cx) {
        LocalContainer *data = (LocalContainer *)JS_GetContextPrivate(cx);
        if (!data) {
            data = new LocalContainer();
            JS_SetContextPrivate(cx, data);
        }
        OLD_DELEGATE = data->ERR_DELEGATE;
        data->ERR_DELEGATE = ERR_DELEGATE;
    }
    return OLD_DELEGATE;
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
inline JSBool ejs_throw_error(JSContext *cx, JSObject *obj, const char *msg) {
    JSString *jsstr;

    // if we get errors during error reporting we report those
    if (((jsstr = JS_NewStringCopyZ(cx, msg))) &&
        (JS_AddNamedRoot(cx, &jsstr, "jsstr"))) {
        jsval dummy  = JSVAL_VOID;
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

    return JS_FALSE;
}//---------------------------------------------------------------------------
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
void JS_TO_CONCEPT(JSContext *cx, void *member, jsval rval, std::map<void *, void *> *circular_map = NULL, int level = 0) {
    INVOKE_CALL Invoke   = InvokePtr;
    CALL_BACK_VARIABLE_SET SetVariable = _SetVariable;
    void *HANDLER = CONCEPT_HANDLER;
    std::map<void *, void *> *use_map = circular_map;
    if (level > 100)
        return;
    if (!use_map)
        use_map = new std::map<void *, void *>();
    if (JSVAL_IS_DOUBLE(rval)) {
        SET_NUMBER_VARIABLE(member, *JSVAL_TO_DOUBLE(rval));
    } else
    if (JSVAL_IS_INT(rval)) {
        SET_NUMBER_VARIABLE(member, JSVAL_TO_INT(rval));
    } else
    if (JSVAL_IS_STRING(rval)) {
        struct JSString *str = JSVAL_TO_STRING(rval);
        SET_BUFFER_VARIABLE(member, JS_GetStringBytes(str), str->length);
    } else
    if (JSVAL_IS_BOOLEAN(rval)) {
        SET_NUMBER_VARIABLE(member, (NUMBER)JSVAL_TO_BOOLEAN(rval));
    } else
    if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
        SET_NUMBER_VARIABLE(member, 0);
    } else
    if (!JSVAL_IS_PRIMITIVE(rval)) {
        JSObject *object = JSVAL_TO_OBJECT(rval);
        if (object) {
            void *prev_object = (*use_map)[object];
            if (prev_object) {
                if (GetRecursive(cx)) {
                    INTEGER    type     = 0;
                    char       *szValue = 0;
                    NUMBER     nValue   = 0;

                    Invoke(INVOKE_GET_VARIABLE, prev_object, &type, &szValue, &nValue);
                    Invoke(INVOKE_SET_VARIABLE, member, type, szValue, nValue);
                }
            } else {
                CREATE_ARRAY(member);
                (*use_map)[object] = member;
                // bool isArray = false;
                // if ((JS_IsArrayObject(cx, object, &isArray)) && (isArray)) {
                if (JS_IsArrayObject(cx, object)) {
                    // is array
                    jsuint lengthp = 0;
                    if (JS_GetArrayLength(cx, object, &lengthp)) {
                        for (jsuint i = 0; i < lengthp; i++) {
                            jsval rval2 = JSVAL_VOID;
                            if (JS_GetElement(cx, object, i, &rval2)) {
                                void *elem_data = NULL;
                                Invoke(INVOKE_ARRAY_VARIABLE, member, (INTEGER)i, &elem_data);
                                if (elem_data) {
                                    JS_TO_CONCEPT(cx, elem_data, rval2, use_map, level + 1);
                                }
                            }
                        }
                    }
                } else {
                    // is object
                    JSIdArray *arr = JS_Enumerate(cx, object);
                    if (arr) {
                        for (jsint i = 0; i < arr->length; i++) {
                            jsval prop = JSVAL_VOID;
                            if (JS_IdToValue(cx, arr->vector[i], &prop)) {
                                struct JSString *str = JSVAL_TO_STRING(prop);
                                if (str) {
                                    void *elem_data = NULL;
                                    jsval rval2 = JSVAL_VOID;
                                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, member, JS_GetStringBytes(str), &elem_data);
                                    // if ((elem_data) && (JS_GetPropertyById(cx, object, arr->vector[i], &rval2)))
                                    if ((elem_data) && (JS_GetProperty(cx, object, JS_GetStringBytes(str), &rval2))) {
                                        JS_TO_CONCEPT(cx, elem_data, rval2, use_map, level + 1);
                                    }
                                }
                            }
                        }
                        JS_DestroyIdArray(cx, arr);
                    }
                }
            }
        }
    } else {
        SET_NUMBER_VARIABLE(member, 1);
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
            ret = "1";
    }
    return ret;
}

//---------------------------------------------------------------------------
jsval CONCEPT_TO_JS(JSContext *cx, void *member, std::map<void *, int> *circular_map = NULL) {
    INTEGER type     = 0;
    char    *szValue = 0;
    NUMBER  nValue   = 0;
    jsval   ret      = JSVAL_VOID;
    INVOKE_CALL Invoke   = InvokePtr;
    void *HANDLER = CONCEPT_HANDLER;
    std::map<void *, int> *use_map = circular_map;
    if (!use_map)
        use_map = new std::map<void *, int>();

    InvokePtr(INVOKE_GET_VARIABLE, member, &type, &szValue, &nValue);
    switch (type) {
        case VARIABLE_NUMBER:
            ret = DOUBLE_TO_JSVAL(JS_NewDouble(cx, nValue));
            break;

        case VARIABLE_STRING:
            ret = STRING_TO_JSVAL(JS_NewStringCopyN(cx, szValue, (int)nValue));
            break;

        case VARIABLE_ARRAY:
            {
                // circular reference
                if ((*use_map)[(void *)szValue])
                    break;
                (*use_map)[(void *)szValue] = 1;
                int count = Invoke(INVOKE_GET_ARRAY_COUNT, member);
                jsval *values = NULL;
                JSObject *object = NULL;
                if (count) {
                    char *key = NULL;
                    void *elem_data = NULL;
                    Invoke(INVOKE_GET_ARRAY_KEY, member, 0, &key);
                    if (key) {
                        object = JS_NewObject(cx, NULL, NULL, NULL);
                        for (int i = 0; i < count; i++) {
                            InvokePtr(INVOKE_ARRAY_VARIABLE, member, i, &elem_data);
                            InvokePtr(INVOKE_GET_ARRAY_KEY, member, i, &key);
                            if (key) {
                                jsval temp = CONCEPT_TO_JS(cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, &temp);
                            }
                        }
                    } else {
                        values = (jsval *)malloc(sizeof(jsval) * count);
                        if (values) {
                            for (int i = 0; i < count; i++) {
                                values[i] = JSVAL_VOID;
                                Invoke(INVOKE_ARRAY_VARIABLE, member, (INTEGER)i, &elem_data);
                                if (elem_data)
                                    values[i] = CONCEPT_TO_JS(cx, elem_data, use_map);
                            }
                        }
                        object = JS_NewArrayObject(cx, count, values);
                        if (values)
                            free(values);
                    }
                }
                if (object)
                    ret = OBJECT_TO_JSVAL(object);
            }
            break;

        case VARIABLE_CLASS:
            {
                // circular reference
                if ((*use_map)[(void *)szValue])
                    break;
                (*use_map)[(void *)szValue] = 1;
                JSObject * object = JS_NewObject(cx, NULL, NULL, NULL);
                if (object) {
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
                                jsval temp = CONCEPT_TO_JS(cx, elem_data, use_map);
                                JS_SetProperty(cx, object, key, &temp);
                             }
                        }
                    }
                    ret = OBJECT_TO_JSVAL(object);

                    delete[] members;
                    delete[] flags;
                    delete[] access;
                    delete[] types;
                    delete[] szValues;
                    delete[] nValues;
                    delete[] class_data;
                    delete[] variable_data;
                }
            }
            break;

        default:
            ret = DOUBLE_TO_JSVAL(JS_NewDouble(cx, 1.0));
    }
    if ((use_map) && (!circular_map))
        delete use_map;
    return ret;
}

//---------------------------------------------------------------------------
static JSBool function_handler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    int                    count, i;
    char                   *fun_name;
    INVOKE_CALL            Invoke      = InvokePtr;
    CALL_BACK_VARIABLE_SET SetVariable = _SetVariable;
    void                   *HANDLER    = CONCEPT_HANDLER;

    /* Differetiate between direct and method-like call */
    if ((JS_TypeOfValue(cx, argv[-2]) == JSTYPE_FUNCTION) && strcmp("call", JS_GetFunctionName(JS_ValueToFunction(cx, argv[-2])))) {
        fun_name = (char *)JS_GetFunctionName((JSFunction *)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[-2])));
        i        = 0;
    } else {
        fun_name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
        i        = 1;
    }
    //std::cerr << fun_name;
    std::map<std::string, void *> *functions = GetFunctions(cx);
    void *ft = NULL;
    if (functions)
        ft = (*functions)[(const char *)fun_name];
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
        JS_TO_CONCEPT(cx, PARAMETERS[index], param);
        index++;
    }

    // -1 in parameter count means that we will prvide a parameter list;
    void *EXCEPTION = 0;
    void *RES       = 0;
    Invoke(INVOKE_CALL_DELEGATE, ft, &RES, &EXCEPTION, (INTEGER)-1, PARAMETERS);
    if (RES) {
        *rval = CONCEPT_TO_JS(cx, RES);
        Invoke(INVOKE_FREE_VARIABLE, RES);
    }
    if (EXCEPTION) {
        ejs_throw_error(cx, obj, CONCEPT_TO_STRING(cx, EXCEPTION).c_str());
        Invoke(INVOKE_FREE_VARIABLE, EXCEPTION);
    }

    for (i = 0; i < index; i++) {
        //argv[i+first_arg]=CONCEPT_TO_JS(cx, PARAMETERS[i]);
        FREE_VARIABLE(PARAMETERS[i]);
    }
    if (PARAMETERS)
        delete[] PARAMETERS;

    /*jsdouble x, z;
       if (!JS_ValueToNumber(cx, argv[0], &x))
        return JS_FALSE;
       z = (x < 0) ? -x : x;
        return JS_NewDoubleValue(cx, z, rval);*/

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
    void                   *HANDLER         = CONCEPT_HANDLER;
    void                   *ERR_DELEGATE    = GetErrorDelegate(cx);

    if (!ERR_DELEGATE) {
        printError(cx, message, report);
        return;
    }

    CREATE_VARIABLE(CREPORT);
    CREATE_ARRAY(CREPORT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "filename", (INTEGER)VARIABLE_STRING, (char *)report->filename, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "lineno", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->lineno);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "linebuf", (INTEGER)VARIABLE_STRING, (char *)report->linebuf, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "tokenptr", (INTEGER)VARIABLE_STRING, (char *)report->tokenptr, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "uclinebuf", (INTEGER)VARIABLE_STRING, (char *)report->tokenptr, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "uctokenptr", (INTEGER)VARIABLE_STRING, (char *)report->uctokenptr, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "flags", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->flags);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "errorNumber", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)report->errorNumber);
    JSString *str = JS_NewUCString(cx, (jschar *)report->ucmessage, char16len(report->ucmessage));
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, CREPORT, "ucmessage", (INTEGER)VARIABLE_STRING, (char *)JS_GetStringBytes(str), (NUMBER)str->length);

    void *elem_data = NULL;
    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, CREPORT, "messageArgs", &elem_data);
    if (elem_data) {
        CREATE_ARRAY(elem_data);
    }
    int i=0;
    const jschar *messageArg = report->messageArgs ? report->messageArgs[i] : 0;
    while (messageArg) {
        str = JS_NewUCString(cx, (jschar *)messageArg, char16len(messageArg));
        Invoke(INVOKE_SET_ARRAY_ELEMENT, elem_data, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)JS_GetStringBytes(str), (NUMBER)str->length);
        messageArg = report->messageArgs[++i];
    }
    // call delegate
    CALL_DELEGATE(ERR_DELEGATE, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)cx, (INTEGER)VARIABLE_STRING, (char *)message, (NUMBER)0, (INTEGER)VARIABLE_UNDEFINED, CREPORT);
    FREE_VARIABLE(CREPORT);
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr       = Invoke;
    CONCEPT_HANDLER = HANDLER;
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

    RETURN_NUMBER((SYS_INT)JS_NewRuntime((uint32)PARAM(0)))
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

    JS_Lock((JSRuntime *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSUnlock, 1)
    T_HANDLE(JSUnlock, 0)     // JSRuntime*

    JS_Unlock((JSRuntime *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewContext, 2)
    T_HANDLE(JSNewContext, 0)     // JSRuntime*
    T_NUMBER(JSNewContext, 1)     // size_t
    _SetVariable = SetVariable;

    RETURN_NUMBER((SYS_INT)JS_NewContext((JSRuntime *)(SYS_INT)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSDestroyContext, 1)
    T_HANDLE(JSDestroyContext, 0)     // JSContext*
    ClearFunctions((JSContext *)(SYS_INT)PARAM(0));
    JS_DestroyContext((JSContext *)(SYS_INT)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSInitStandardClasses, 2)
    T_NUMBER(JSInitStandardClasses, 0)     // JSContext*
    T_NUMBER(JSInitStandardClasses, 1)
    _SetVariable = SetVariable;

    RETURN_NUMBER(JS_InitStandardClasses((JSContext *)(SYS_INT)PARAM(0), (JSObject *)(SYS_INT)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSSetErrorReporter, 2)
    T_HANDLE(JSSetErrorReporter, 0)     // JSContext*
    T_DELEGATE(JSSetErrorReporter, 1)

    if (!(PARAM_INT(0)))
        return (void *)"JSSetErrorReporter: Invalid JSContext (is null)";

    void *ERR_DELEGATE = SetErrorDelegate(((JSContext *)(SYS_INT)PARAM(0)), PARAMETER(1));
    if (ERR_DELEGATE)
        FREE_VARIABLE(ERR_DELEGATE);

    LOCK_VARIABLE(ERR_DELEGATE);
    JS_SetErrorReporter(((JSContext *)(SYS_INT)PARAM(0)), ShowError);
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSNewObject, 4)
    T_HANDLE(JSNewObject, 0) // JSContext*
    T_NUMBER(JSNewObject, 1) // JSClass*
    T_NUMBER(JSNewObject, 2) // JSObject*
    T_NUMBER(JSNewObject, 3) // JSObject*

    RETURN_NUMBER((SYS_INT)JS_NewObject((JSContext *)PARAM_INT(0), (JSClass *)PARAM_INT(1), (JSObject *)PARAM_INT(2), (JSObject *)PARAM_INT(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSGC, 2)
    T_HANDLE(JSGC, 0)
    T_HANDLE(JSGC, 1)

    JSContext *cx = (JSContext *)(SYS_INT)PARAM(0);
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
    T_NUMBER(JSEvaluateScript, 1)
    T_STRING(JSEvaluateScript, 2)     // char*
    T_STRING(JSEvaluateScript, 3)     // char*
    T_NUMBER(JSEvaluateScript, 4)     // uintN

// ... parameter 6 is by reference (jsval*)
    jsval rval = JSVAL_VOID;
    bool is_ok = false;
    JSContext *cx = (JSContext *)(SYS_INT)PARAM(0);
    RETURN_NUMBER(is_ok = JS_EvaluateScript(cx, (JSObject *)PARAM_INT(1), PARAM(2), (uintN)PARAM_LEN(2), PARAM(3), (uintN)PARAM(4), &rval))
    SET_NUMBER(5, (SYS_INT)0)
    if (is_ok) {
        if (JSVAL_IS_DOUBLE(rval)) {
            SET_NUMBER(5, *JSVAL_TO_DOUBLE(rval));
        } else
        if (JSVAL_IS_INT(rval)) {
            SET_NUMBER(5, JSVAL_TO_INT(rval));
        } else
        if (JSVAL_IS_STRING(rval)) {
            struct JSString *str = JSVAL_TO_STRING(rval);
            SET_BUFFER(5, JS_GetStringBytes(str), (NUMBER)str->length);
        } else
        if (JSVAL_IS_BOOLEAN(rval)) {
            SET_NUMBER(5, (NUMBER)JSVAL_TO_BOOLEAN(rval));
        } else
        if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
            SET_NUMBER(5, (NUMBER)0);
        } else {
            JS_TO_CONCEPT((JSContext *)(SYS_INT)PARAM(0),PARAMETER(5), rval);
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
    JSObject  *global  = NULL;

    jsval rval = JSVAL_VOID;

    RETURN_NUMBER(0);
    /* create new runtime, new context, global object */
    if ((!(runtime = JS_NewRuntime(1024L * 1024L))) ||
        (!(context = JS_NewContext(runtime, 8192))) ||
        (!(global = JS_NewObject(context, NULL, NULL, NULL)))
        )
        return (void *)"JSEval: error creating runtime/context/global";

    if (!JS_InitStandardClasses(context, global))
        return (void *)"JSEval: initialising standard classes";

    JS_SetErrorReporter(context, printError);

    bool is_ok = JS_EvaluateScript(context, global, PARAM(0), PARAM_LEN(0), "script", 1, &rval);

    if (is_ok) {
        if (JSVAL_IS_DOUBLE(rval)) {
            RETURN_NUMBER(*JSVAL_TO_DOUBLE(rval));
        } else
        if (JSVAL_IS_INT(rval)) {
            RETURN_NUMBER(JSVAL_TO_INT(rval));
        } else
        if (JSVAL_IS_STRING(rval)) {
            struct JSString *str = JSVAL_TO_STRING(rval);
            RETURN_BUFFER(JS_GetStringBytes(str), str->length);
        } else
        if (JSVAL_IS_BOOLEAN(rval)) {
            RETURN_NUMBER((NUMBER)JSVAL_TO_BOOLEAN(rval));
        } else
        if ((JSVAL_IS_NULL(rval)) || (JSVAL_IS_VOID(rval))) {
            RETURN_NUMBER(0);
        } else {
            JS_TO_CONCEPT(context, RESULT, rval);
        }
    }
    JS_DestroyContext(context);
    JS_DestroyRuntime(runtime);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSWrap, 4)
    T_HANDLE(JSWrap, 0) // JSContext*
    T_NUMBER(JSWrap, 1) // JSObject*
    T_DELEGATE(JSWrap, 2)
    T_STRING(JSWrap, 3) // function name
    _SetVariable = SetVariable;

    void *OLD_DELEGATE = SetFunction(PARAM(3), PARAMETER(2), (JSContext *)PARAM_INT(0), (JSObject *)PARAM_INT(1));
    LOCK_VARIABLE(PARAMETER(2));
    if (OLD_DELEGATE) {
        FREE_VARIABLE(OLD_DELEGATE);
    }

    int param_count = Invoke(INVOKE_COUNT_DELEGATE_PARAMS, PARAMETER(2));
    RETURN_NUMBER((JS_DefineFunction((JSContext *)(SYS_INT)PARAM(0), (JSObject *)PARAM_INT(1), PARAM(3), function_handler, param_count, 0) != 0));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(JSRecursive, 2) 
    T_HANDLE(JSWrap, 0) // JSContext*
    T_NUMBER(JSWrap, 1)
    SetRecursive((JSContext *)(SYS_INT)PARAM(0), (bool)PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//------------------------------------------------------------------------
