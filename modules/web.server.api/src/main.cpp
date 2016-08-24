//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include "qDecoder/qDecoder.h"

extern "C" {
#include "qDecoder/qInternal.h"

int CONCEPT_AWARE_header(const char *key, const char *value);
int CONCEPT_AWARE_header2N(const char *key, const char *value);
void CONCEPT_AWARE_print(char *data);
int CONCEPT_AWARE_fgetc(FILE *stream);
char *CONCEPT_AWARE_fgets(char *str, int size, FILE *stream);
}
//-----------------------------------------------------------------------------------
typedef char * (*GETENV_EMULATOR)(const char *name, void *userdata);
typedef int (*HEADER_EMULATOR)(const char *key, const char *value, void *userdata);
typedef void (*PRINT_EMULATOR)(char *data, int len, void *userdata);

typedef char * (*FGETS_EMULATOR)(char *str, int size, void *userdata);
typedef int (*FGETC_EMULATOR)(void *userdata);

struct EnvContainer {
    GETENV_EMULATOR getenv;
    HEADER_EMULATOR header;
    PRINT_EMULATOR  print;

    FGETC_EMULATOR  fgetc;
    FGETS_EMULATOR  fgets;

    void            *r;
};
//-----------------------------------------------------------------------------------
INVOKE_CALL  InvokePtr       = 0;
void         *MASTER_HANDLER = 0;
EnvContainer *ec             = 0;
//-----------------------------------------------------------------------------------
extern "C" int concept_printf(__const char *__restrict __format, ...) {
    va_list args;

    va_start(args, __format);
    char    buffer[0xFFFF];
    INTEGER len = vsprintf(buffer, __format, args);

    if ((len > 1) && (len < 0xFFFF - 2)) {
        int i = len - 1;
        if ((buffer[i] == 10) && (buffer[i - 1] == 10)) {
            buffer[i - 1] = 13;
            buffer[i + 1] = 13;
            buffer[i + 2] = 10;
            len          += 2;
        }
    }
    if (len > 0)
        InvokePtr(INVOKE_PRINT, MASTER_HANDLER, buffer, len);
    va_end(args);

    return len;
}

//-----------------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr      = Invoke;
    MASTER_HANDLER = HANDLER;
    ec             = 0;
    //Invoke(INVOKE_GET_USERDATA, HANDLER, &ec);
    return 0;
}
//-----------------------------------------------------------------------------------
char *CONCEPT_AWARE_fgets(char *str, int size, FILE *stream) {
    if (ec) {
        return ec->fgets(str, size, ec);
    } else
        return _fgets(str, size, stream);
}

//-----------------------------------------------------------------------------------
int CONCEPT_AWARE_fgetc(FILE *stream) {
    if (ec) {
        return ec->fgetc(ec);
    } else
        return fgetc(stream);
}

//-----------------------------------------------------------------------------------
void CONCEPT_AWARE_print(char *data) {
    if (ec) {
        ec->print(data, -1, ec);
    } else
        concept_printf("%s", data);
}

//-----------------------------------------------------------------------------------
int CONCEPT_AWARE_header(const char *key, const char *value) {
    if (ec) {
        return ec->header(key, value, ec);
    } else
        return concept_printf("%s: %s\n", key, value);
}

//-----------------------------------------------------------------------------------
int CONCEPT_AWARE_header2N(const char *key, const char *value) {
    if (ec) {
        return ec->header(key, value, ec);
    } else
        return concept_printf("%s: %s\n\n", key, value);
}

//-----------------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(SetHeader, 2)
    T_STRING(SetHeader, 0)
    T_STRING(SetHeader, 1)

    if (ec) {
        RETURN_NUMBER(ec->header(PARAM(0), PARAM(1), ec))
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//---------------------------------------------------------------------------

CONCEPT_DLL_API CONCEPT_WebVar CONCEPT_API_PARAMETERS {
    if ((PARAMETERS->COUNT != 1) && (PARAMETERS->COUNT != 2))
        return (void *)"WebVar takes at least 1 parameter (variable name); usage WebVar(varname[, buffer_size])";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;
    char    *DUMMY;
    NUMBER  BUFFER_SIZE = 0;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"WebVar : parameter 1 should be a string (STATIC STRING)";

    if (PARAMETERS->COUNT == 2) {
        GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &DUMMY, &BUFFER_SIZE);
        if (TYPE != VARIABLE_NUMBER)
            return (void *)"WebVar : parameter 2 should be a number (STATIC NUMBER)";
        if (BUFFER_SIZE < 0)
            BUFFER_SIZE = 0;
    }

    //qDecoder();

    char *data = qValueFirst(CHAR_VAL);


    /*if (!data) {
        qFree();
        return (void *)"WebVar : QUERY_STRING enviroment variable NOT FOUND. Not running in CGI mode ?";
       }*/

    SetVariable(RESULT, VARIABLE_STRING, data ? data : (char *)"", BUFFER_SIZE);

    char *next = qValueNext();
    if (next) {
        Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
        int i = 0;
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i++, (INTEGER)VARIABLE_STRING, (char *)data, (NUMBER)BUFFER_SIZE);
        do {
            Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i++, (INTEGER)VARIABLE_STRING, (char *)next, (NUMBER)BUFFER_SIZE);
            next = qValueNext();
        } while (next);
    }
    //qFree();

    return 0;
}
//---------------------------------------------------------------------------

/*CONCEPT_DLL_API CONCEPT_SessionStart CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT!=0)
        return (void *)"SessionStart takes no parameters";

    qSession(NULL);
    char *sessid=qSessionGetID();

    SetVariable(RESULT,VARIABLE_STRING,sessid,0);

    return 0;
   }*/
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SessionStart, 0, 2)
    char *path = NULL;
    char *ses = NULL;

    if (PARAMETERS_COUNT >= 1) {
        T_STRING(SessionStart, 0);

        path = PARAM(0);
        if (path[0] == 0)
            path = NULL;

        if (PARAMETERS_COUNT == 2) {
            T_STRING(SessionStart, 1)
            ses = PARAM(1);
            if (ses[0] == 0)
                ses = NULL;
        }
    }

    qSession(path, ses);
    char *sessid = qSessionGetID();

//SetVariable(RESULT,VARIABLE_STRING,sessid,0);
    RETURN_STRING(sessid);
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SessionDestroy CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"SessionDestroy takes no parameters";

    char *sessid = qSessionGetID();
    qSessionDestroy();

    SetVariable(RESULT, VARIABLE_STRING, sessid, 0);
    return 0;
}
//-----------------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ContentType CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"ContentType takes 1 parameter (variable name)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"ContentType : parameter 1 should be a string (STATIC STRING)";

    AnsiString ct(CHAR_VAL);
    ct += (char *)"\r\n";
    //Invoke(INVOKE_PRINT, MASTER_HANDLER, ct.c_str(), (INTEGER)ct.Length());
    qContentType(CHAR_VAL);

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SessionVar CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"SessionVar takes 1 parameter (variable name)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"SessionVar : parameter 1 should be a string (STATIC STRING)";

    char *data = qSessionValue(CHAR_VAL);

    /*if (!data)
        return (void *)"SessionVar : Can't get Session Variable. Not running in CGI mode or Session not started ?";
     */

    SetVariable(RESULT, VARIABLE_STRING, data ? data : (char *)"", 0);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SetSessionVar CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 2)
        return (void *)"AddSessionVar takes 2 parameter (variable name, variable value)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL1;
    char    *CHAR_VAL2;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL1, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"SetSessionVar : parameter 1 should be a string (STATIC STRING)";

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[1] - 1], &TYPE, &CHAR_VAL2, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"SetSessionVar : parameter 2 should be a string (STATIC STRING)";

    qSessionAdd(CHAR_VAL1, CHAR_VAL2);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SessionDone CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"SessionDone takes no parameters";

    SetVariable(RESULT, VARIABLE_NUMBER, "", 0);

    qSessionFree();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_GetSessionID CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 0)
        return (void *)"GetSessionID takes no parameters";

    char *sessid = qSessionGetID();

    SetVariable(RESULT, VARIABLE_STRING, sessid ? sessid : (char *)"", 0);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SessionTimeout CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"SessionTimeout takes 1 parameter (timneout in seconds)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_NUMBER)
        return (void *)"SessionTimeout : parameter 1 should be an integer (STATIC NUMBER)";

    qSessionSetTimeout((INTEGER)NUMBER_VALUE);

    SetVariable(RESULT, VARIABLE_NUMBER, "", NUMBER_VALUE);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_ServerVar CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"ServerVar takes 1 parameter (variable name)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"ServerVar : parameter 1 should be a string (STATIC STRING)";

    char *data = getenv(CHAR_VAL);//qGetenvDefault("",CHAR_VAL);

    SetVariable(RESULT, VARIABLE_STRING, data, 0);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_CookieVar CONCEPT_API_PARAMETERS {
    if (PARAMETERS->COUNT != 1)
        return (void *)"CookieVar takes 1 parameter (variable name)";

    NUMBER  NUMBER_VALUE;
    INTEGER TYPE;
    char    *CHAR_VAL;

    GetVariable(LOCAL_CONTEXT[PARAMETERS->PARAM_INDEX[0] - 1], &TYPE, &CHAR_VAL, &NUMBER_VALUE);
    if (TYPE != VARIABLE_STRING)
        return (void *)"CookieVar : parameter 1 should be a string (STATIC STRING)";

    char *data = qCookieValue(CHAR_VAL);

    if (!data) {
        SetVariable(RESULT, VARIABLE_STRING, "", 0);
        return 0;
    }

    SetVariable(RESULT, VARIABLE_STRING, data, 0);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SetCookie CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(6, "SetCookie takes 6 parameters : (char *name, char *value, int exp_days, char *path, char *domain, char *secure)");
    LOCAL_INIT;

    char   *name;
    char   *value;
    NUMBER exp_days;
    char   *path   = 0;
    char   *domain = 0;
    char   *secure = 0;

    GET_CHECK_STRING(0, name, "SetCookie : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, value, "SetCookie : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_NUMBER(2, exp_days, "SetCookie : parameter 3 should be an integer (STATIC NUMBER)");
    //GET_CHECK_STRING(3,path,"SetCookie : parameter 4 should be a string (STATIC STRING)");
    //GET_CHECK_STRING(4,domain,"SetCookie : parameter 5 should be a string (STATIC STRING)");
    //GET_CHECK_STRING(5,secure,"SetCookie : parameter 6 should be a string (STATIC STRING)");
    GET_STRING(3, path);
    if (TYPE != VARIABLE_STRING)
        path = 0;
    GET_STRING(4, domain);
    if (TYPE != VARIABLE_STRING)
        domain = 0;
    GET_STRING(5, secure);
    if (TYPE != VARIABLE_STRING)
        secure = 0;

    qCookieSet(name, value, (int)exp_days, path, domain, secure);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_RemoveCookie CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SetCookie takes 4 parameters : (char *name, char *path, char *domain, char *secure);");
    LOCAL_INIT;

    char *name;
    char *path;
    char *domain;
    char *secure;

    GET_CHECK_STRING(0, name, "SetCookie : parameter 1 should be a string (STATIC STRING)");
    //GET_CHECK_STRING(1,path,"SetCookie : parameter 4 should be a string (STATIC STRING)");
    //GET_CHECK_STRING(2,domain,"SetCookie : parameter 5 should be a string (STATIC STRING)");
    //GET_CHECK_STRING(3,secure,"SetCookie : parameter 6 should be a string (STATIC STRING)");

    GET_STRING(1, path);
    if (TYPE != VARIABLE_STRING)
        path = 0;
    GET_STRING(2, domain);
    if (TYPE != VARIABLE_STRING)
        domain = 0;
    GET_STRING(3, secure);
    if (TYPE != VARIABLE_STRING)
        secure = 0;

    qCookieRemove(name, path, domain, secure);
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_WebFree CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "WebFree takes 0 parameters");
    qFree();
    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_POST, 0)
    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Q_Entry *qe      = qGetFirstEntry();
    Q_Entry *qe_next = 0;
    int     size     = 0;
    while (qe) {
        qe_next = qe->next;
        if (qValueType(qe->name) == 'P') {
            size = 0;
            if (qe_next) {
                AnsiString name = qe->name;
                name += ".length";

                if (name == qe_next->name) {
                    AnsiString value = qe_next->value;
                    size = value.ToInt();
                }
            }

            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, qe->name, (INTEGER)VARIABLE_STRING, (char *)qe->value, (NUMBER)size);
        }
        qe = qe_next;
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_GET, 0)
    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Q_Entry *qe      = qGetFirstEntry();
    Q_Entry *qe_next = 0;
    int     size     = 0;
    while (qe) {
        qe_next = qe->next;
        if (qValueType(qe->name) == 'G') {
            size = 0;
            if (qe_next) {
                AnsiString name = qe->name;
                name += ".length";

                if (name == qe_next->name) {
                    AnsiString value = qe_next->value;
                    size = value.ToInt();
                }
            }

            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, qe->name, (INTEGER)VARIABLE_STRING, (char *)qe->value, (NUMBER)size);
        }
        qe = qe->next;
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_COOKIE, 0)
    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Q_Entry *qe = qGetFirstEntry();
    while (qe) {
        if (qValueType(qe->name) == 'C')
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, qe->name, (INTEGER)VARIABLE_STRING, (char *)qe->value, (NUMBER)0);
        qe = qe->next;
    }
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_VARS, 0)
    Invoke(INVOKE_CREATE_ARRAY, RESULT);

    Q_Entry *qe      = qGetFirstEntry();
    Q_Entry *qe_next = 0;
    int     size     = 0;
    while (qe) {
        qe_next = qe->next;
        size    = 0;
        if (qe_next) {
            AnsiString name = qe->name;
            name += ".length";

            if (name == qe_next->name) {
                AnsiString value = qe_next->value;
                size = value.ToInt();
            }
        }

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, qe->name, (INTEGER)VARIABLE_STRING, (char *)qe->value, (NUMBER)size);
        qe = qe->next;
    }

END_IMPL
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(_PutString, 2)
    T_NUMBER(_PutString, 0)
    T_STRING(_PutString, 1)

    qPuts(PARAM_INT(0), PARAM(1));

    RETURN_NUMBER(0)
END_IMPL
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    qReset();
    return 0;
}
//---------------------------------------------------------------------------

