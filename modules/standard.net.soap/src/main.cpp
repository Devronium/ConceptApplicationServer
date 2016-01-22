//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"

extern "C" {
#include <libcsoap/soap-client.h>
#include <libcsoap/soap-router.h>
#include <libcsoap/soap-server.h>
#include <libcsoap/soap-service.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-ctx.h>
#include <libcsoap/soap-fault.h>
#include <libcsoap/soap-xml.h>
}

INVOKE_CALL InvokePtr = 0;

#define MAX_MEMBER_NAME       0xFF
#define MAX_URN_NAME          0xFF
#define MEMBERS_BLOCK_SIZE    10
typedef struct {
    //char    URN_NAME[MAX_URN_NAME];
    char MEMBER_NAME[MAX_MEMBER_NAME];
    void *DELEGATE;
} EventHandler;

EventHandler *EVENTS      = 0;
int          EVENTS_COUNT = 0;

// SOAP CLIENT

//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT(INTEGER LOAD_ADDR) {
    if (EVENTS)
        delete[] EVENTS;
    return 0;
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPClientInitArgs CONCEPT_API_PARAMETERS {
    LOCAL_INIT;

    char **args = 0;

    if (PARAMETERS_COUNT) {
        args = new char * [PARAMETERS_COUNT];
        for (int i = 0; i < PARAMETERS_COUNT; i++) {
            char *fill_string = 0;
            GET_CHECK_STRING(i, fill_string, "SOAPClientInit : all init parameters should be strings(STATIC STRING)");
            args[i] = fill_string;
        }
    }
    RETURN_NUMBER((long)soap_client_init_args(PARAMETERS_COUNT, args));
    if (args)
        delete[] args;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPClientDestroy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "SOAPClientDestroy takes no parameters");

    LOCAL_INIT;

    soap_client_destroy();

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPClientGetBlockmode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "SOAPClientGetBlockmode takes no parameters");

    LOCAL_INIT;

    RETURN_NUMBER(soap_client_get_blockmode());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPClientBlockSocket CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPClientBlockSocket takes one parameters (int block)");

    LOCAL_INIT;

    NUMBER nBlock;
    GET_CHECK_NUMBER(0, nBlock, "SOAPClientBlockSocket : parameter 0 should be a number (STATIC NUMBER)");

    soap_client_block_socket((int)nBlock);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPClientInvoke CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SOAPClientInvoke takes 4 parameters : herror_t soap_client_invoke(SoapCtx *ctx, SoapCtx** response, const char *url, const char *soap_action);");

    LOCAL_INIT;
    NUMBER nContext;
    char   *urn;
    char   *soap_action;

    GET_CHECK_NUMBER(0, nContext, "SOAPClientInvoke : parameter 0 should be a number (STATIC NUMBER)");
    // pe 1 nu-l iau ... in el returnez !
    GET_CHECK_STRING(2, urn, "SOAPClientInvoke : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, soap_action, "SOAPClientInvoke : parameter 3 should be a string (STATIC STRING)");

    SoapCtx *ctx      = (SoapCtx *)(long)nContext;
    SoapCtx *response = 0;

    RETURN_NUMBER((long)soap_client_invoke(ctx, &response, urn, soap_action));
    SET_NUMBER(1, (long)response);

    return 0;
}
//---------------------------------------------------------------------------

// SOAP CTX

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPCtxGetFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPCtxGetFile takes 2 parameters : part_t *soap_ctx_get_file(SoapCtx* ctx, xmlNodePtr node);");

    LOCAL_INIT;

    NUMBER nContext;
    NUMBER nNode;

    GET_CHECK_NUMBER(0, nContext, "SOAPCtxGetFile : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_NUMBER(1, nNode, "SOAPCtxGetFile : parameter 0 should be a number (STATIC NUMBER)");

    SoapCtx    *ctx = (SoapCtx *)(long)nContext;
    xmlNodePtr node = (xmlNodePtr)(long)nNode;

    RETURN_NUMBER((long)soap_ctx_get_file(ctx, node));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPCtxNewWithMethod CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SOAPCtxNewWithMethod takes 3 parameters : herror_t soap_ctx_new_with_method(const char *urn, const char *method, SoapCtx **out);");

    LOCAL_INIT;

    char *szUrn;
    char *szMethod;

    GET_CHECK_STRING(0, szUrn, "SOAPCtxNewWithMethod : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, szMethod, "SOAPCtxNewWithMethod : parameter 1 should be a string (STATIC STRING)");

    SoapCtx *ctx = 0;

    RETURN_NUMBER((long)soap_ctx_new_with_method(szUrn, szMethod, &ctx));
    SET_NUMBER(2, (long)ctx);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPCtxAddFile CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SOAPCtxAddFile takes 4 parameters : herror_t soap_ctx_add_file(SoapCtx* ctx, const char* filename, const char* content_type, char *dest_href);");

    LOCAL_INIT;

    NUMBER nContext;
    char   *filename;
    char   *content_type;

    GET_CHECK_NUMBER(0, nContext, "SOAPCtxAddFile : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, filename, "SOAPCtxAddFile : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, content_type, "SOAPCtxAddFile : parameter 2 should be a string (STATIC STRING)");

    SoapCtx *ctx       = (SoapCtx *)(long)nContext;
    char    *dest_href = new char[MAX_HREF_SIZE];

    RETURN_NUMBER((long)soap_ctx_add_file(ctx, filename, content_type, dest_href));
    SET_STRING(3, dest_href);
    delete[] dest_href;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPCtxFree CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPCtxFree takes 1 parameters : void soap_ctx_free(SoapCtx* ctx);");

    LOCAL_INIT;

    NUMBER nContext;

    GET_CHECK_NUMBER(0, nContext, "SOAPCtxFree : parameter 0 should be a number (STATIC NUMBER)");

    SoapCtx *ctx = (SoapCtx *)(long)nContext;

    if (ctx)
        soap_ctx_free(ctx);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------

// SOAP ENV

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewWithFault CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(5, "SOAPEnvNewWithFault takes 5 parameters : herror_t soap_env_new_with_fault(fault_code_t faultcode, const char *faultstring, const char *faultactor, const char *detail, SoapEnv **out);");

    LOCAL_INIT;

    NUMBER faultcode;
    char   *faultstring;
    char   *faultactor;
    char   *detail;

    GET_CHECK_NUMBER(0, faultcode, "SOAPEnvNewWithFault : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, faultstring, "SOAPEnvNewWithFault : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, faultactor, "SOAPEnvNewWithFault : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, detail, "SOAPEnvNewWithFault : parameter 3 should be a string (STATIC STRING)");

    SoapEnv *env = 0;


    RETURN_NUMBER((long)soap_env_new_with_fault((fault_code_t)faultcode, faultstring, faultactor, detail, &env));
    SET_NUMBER(4, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewWithMethod CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SOAPEnvNewWithMethod takes 3 parameters : herror_t soap_env_new_with_method(const char *urn, const char *method, SoapEnv **out);");

    LOCAL_INIT;

    char *urn;
    char *method;

    GET_CHECK_STRING(0, urn, "SOAPEnvNewWithMethod : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, method, "SOAPEnvNewWithMethod : parameter 1 should be a string (STATIC STRING)");

    SoapEnv *env = 0;


    RETURN_NUMBER((long)soap_env_new_with_method(urn, method, &env));
    SET_NUMBER(2, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewWithResponse CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvNewWithResponse takes 2 parameters : herror_t soap_env_new_with_response(SoapEnv *req,SoapEnv **out);");

    LOCAL_INIT;

    NUMBER nEnv;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvNewWithResponse : parameter 0 should be a number (STATIC NUMBER)");


    SoapEnv *req = (SoapEnv *)(long)nEnv;
    SoapEnv *env = 0;


    RETURN_NUMBER((long)soap_env_new_with_response(req, &env));
    SET_NUMBER(1, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewFromDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvNewFromDoc takes 2 parameters : herror_t soap_env_new_with_response(SoapEnv *req,SoapEnv **out);");

    LOCAL_INIT;

    NUMBER nDoc;

    GET_CHECK_NUMBER(0, nDoc, "SOAPEnvNewFromDoc : parameter 0 should be a number (STATIC NUMBER)");


    xmlDocPtr doc  = (xmlDocPtr)(long)nDoc;
    SoapEnv   *env = 0;


    RETURN_NUMBER((long)soap_env_new_from_doc(doc, &env));
    SET_NUMBER(1, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewFromBuffer CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvNewFromBuffer takes 2 parameters : herror_t soap_env_new_from_buffer(const char* buffer,SoapEnv **out);");

    LOCAL_INIT;

    char *szBuf;

    GET_CHECK_STRING(0, szBuf, "SOAPEnvNewFromBuffer : parameter 0 should be a string (STATIC STRING)");


    SoapEnv *env = 0;


    RETURN_NUMBER((long)soap_env_new_from_buffer(szBuf, &env));
    SET_NUMBER(1, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvNewFromStream CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvNewFromStream takes 2 parameters : herror_t soap_env_new_from_stream(http_input_stream_t *in, SoapEnv **out);");

    LOCAL_INIT;

    NUMBER nHttpStream;

    GET_CHECK_NUMBER(0, nHttpStream, "SOAPEnvNewFromStream : parameter 0 should be a number (STATIC NUMBER)");


    http_input_stream_t *in  = (http_input_stream_t *)(long)nHttpStream;
    SoapEnv             *env = 0;


    RETURN_NUMBER((long)soap_env_new_from_stream(in, &env));
    SET_NUMBER(1, (long)env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvAddItem CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SOAPEnvAddItem takes 4 parameters : xmlNodePtr soap_env_add_item(SoapEnv* env, const char *type, const char *name, const char *value);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *type;
    char   *name;
    char   *value;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvAddItem : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, type, "SOAPEnvAddItem : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, name, "SOAPEnvAddItem : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, value, "SOAPEnvAddItem : parameter 3 should be a string (STATIC STRING)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;


    RETURN_NUMBER((long)soap_env_add_item(env, type, name, value));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvAttachment CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SOAPEnvAttachment takes 3 parameters : xmlNodePtr soap_env_add_attachment(SoapEnv* env, const char *name, const char *href);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *name;
    char   *href;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvAttachment : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, name, "SOAPEnvAttachment : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, href, "SOAPEnvAttachment : parameter 2 should be a string (STATIC STRING)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;


    RETURN_NUMBER((long)soap_env_add_attachment(env, name, href));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvPushItem CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SOAPEnvPushItem takes 3 parameters : xmlNodePtr soap_env_push_item(SoapEnv *env, const char *type, const char *name);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *type;
    char   *name;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvPushItem : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, type, "SOAPEnvPushItem : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, name, "SOAPEnvPushItem : parameter 2 should be a string (STATIC STRING)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;


    RETURN_NUMBER((long)soap_env_push_item(env, type, name));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvPopItem CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPEnvPopItem takes 1 parameter : void soap_env_pop_item(SoapEnv* env);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *type;
    char   *name;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvPopItem : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    soap_env_pop_item(env);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvFree CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPEnvFree takes 1 parameter : void soap_env_free(SoapEnv *env);");

    LOCAL_INIT;

    NUMBER nContext;

    GET_CHECK_NUMBER(0, nContext, "SOAPEnvFree : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *ctx = (SoapEnv *)(long)nContext;

    if (ctx)
        soap_env_free(ctx);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvGetBody CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPEnvGetBody takes 1 parameter : xmlNodePtr soap_env_get_body(SoapEnv* env);");

    LOCAL_INIT;

    NUMBER nEnv;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvGetBody : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    RETURN_NUMBER((long)soap_env_get_body(env));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvGetMethod CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPEnvGetMethod takes 1 parameter : xmlNodePtr soap_env_get_method(SoapEnv* env);");

    LOCAL_INIT;

    NUMBER nEnv;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvGetMethod : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    RETURN_NUMBER((long)soap_env_get_method(env));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvGetFault CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPEnvGetFault takes 1 parameter : xmlNodePtr soap_env_get_fault(SoapEnv* env);");

    LOCAL_INIT;

    NUMBER nEnv;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvGetFault : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    RETURN_NUMBER((long)soap_env_get_fault(env));
    return 0;
}
//---------------------------------------------------------------------------

/*
   Not referenced ?!?!!?

   CONCEPT_DLL_API CONCEPT_SOAPEnvGetHeader CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1,"SOAPEnvGetHeader takes 1 parameter : xmlNodePtr soap_env_get_header(SoapEnv* env);");

    LOCAL_INIT;

    NUMBER nEnv;

    GET_CHECK_NUMBER(0,nEnv,"SOAPEnvGetHeader : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv*   env=(SoapEnv *)(long)nEnv;

    RETURN_NUMBER((long)soap_env_get_header(env));
    return 0;
   }

 */
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvFindUrn CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvFindUrn takes 2 parameters : int soap_env_find_urn(SoapEnv *env, char *urn);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *urn;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvFindUrn : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, urn, "SOAPEnvFindUrn : parameter 1 should be a string (STATIC STRING)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    RETURN_NUMBER(soap_env_find_urn(env, urn));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPEnvFindMethodname CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPEnvFindMethodname takes 2 parameters : int soap_env_find_methodname(SoapEnv *env, char *filename);");

    LOCAL_INIT;

    NUMBER nEnv;
    char   *filename;

    GET_CHECK_NUMBER(0, nEnv, "SOAPEnvFindMethodname : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, filename, "SOAPEnvFindMethodname : parameter 1 should be a string (STATIC STRING)");

    SoapEnv *env = (SoapEnv *)(long)nEnv;

    RETURN_NUMBER(soap_env_find_methodname(env, filename));
    return 0;
}
//---------------------------------------------------------------------------

// SOAP XML

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPXMLDocPrint CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPXMLDocPrint takes one parameter : void soap_xml_doc_print(xmlDocPtr doc);");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPXMLDocPrint : parameter 0 should be a number (STATIC NUMBER)");

    xmlDocPtr doc = (xmlDocPtr)(long)nPtr;

    soap_xml_doc_print(doc);

    RETURN_NUMBER(0);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPXMLGetChildren CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPXMLGetChildren takes one parameter : xmlNodePtr soap_xml_get_children(xmlNodePtr param);");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPXMLGetChildren : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(long)nPtr;

    RETURN_NUMBER((long)soap_xml_get_children(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPXMLGetNext CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPXMLGetNext takes one parameter : xmlNodePtr soap_xml_get_next(xmlNodePtr param);");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPXMLGetNext : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(long)nPtr;

    RETURN_NUMBER((long)soap_xml_get_next(ptr));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPXMLGetText CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPXMLGetText takes one parameter : char *soap_xml_get_text(xmlNodePtr node);");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPXMLGetText : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr ptr = (xmlNodePtr)(long)nPtr;

    RETURN_STRING(soap_xml_get_text(ptr));
    return 0;
}
//---------------------------------------------------------------------------

// SOAP Fault

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPFaultBuild CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SOAPFaultBuild takes 4 parameters : xmlDocPtr soap_fault_build(fault_code_t faultcode, const char *faultstring, const char *faultactor, const char *detail);");

    LOCAL_INIT;

    NUMBER faultcode;
    char   *faultstring;
    char   *faultactor;
    char   *detail;

    GET_CHECK_NUMBER(0, faultcode, "SOAPFaultBuild : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, faultstring, "SOAPFaultBuild : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, faultactor, "SOAPFaultBuild : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, detail, "SOAPFaultBuild : parameter 2 should be a string (STATIC STRING)");


    RETURN_NUMBER((long)soap_fault_build((fault_code_t)faultcode, faultstring, faultactor, detail));

    return 0;
}
//---------------------------------------------------------------------------

// SOAP Router

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPRouterNew CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "SOAPRouterNew takes no parameters : SoapRouter *soap_router_new();");

    LOCAL_INIT;

    RETURN_NUMBER((long)soap_router_new());

    return 0;
}
//---------------------------------------------------------------------------
herror_t SOAP_HANDLER(SoapCtx *req, SoapCtx *res) {
    xmlNodePtr node    = soap_env_get_method(req->env);
    char       *method = 0;

    if (node)
        method = soap_xml_get_text(node);

    if ((InvokePtr) && (method)) {
        for (int i = 0; i < EVENTS_COUNT; i++) {
            if (!strncmp(EVENTS[i].MEMBER_NAME, method, MAX_MEMBER_NAME)) {
                void *RES       = 0;
                void *EXCEPTION = 0;
                InvokePtr(INVOKE_CALL_DELEGATE, EVENTS[i].DELEGATE, &RES, &EXCEPTION, (INTEGER)2, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)req, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)res);
                InvokePtr(INVOKE_FREE_VARIABLE, RES);
                InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
            }
        }
    }
    return H_OK;
}

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPRouterRegisterService CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(4, "SOAPRouterRegisterService takes 4 parameters : void soap_router_register_service(SoapRouter *router, SoapServiceFunc func, const char* method, const char* urn);");

    LOCAL_INIT;

    NUMBER nRouter;
    char   *urn;
    char   *method;

    char   *cls;
    NUMBER mid;

    GET_CHECK_NUMBER(0, nRouter, "SOAPRouterRegisterService : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_DELEGATE(1, cls, mid, "SOAPRouterRegisterService : parameter 1 should be a function DELEGATE");
    GET_CHECK_STRING(2, method, "SOAPRouterRegisterService : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, urn, "SOAPRouterRegisterService : parameter 3 should be a string (STATIC STRING)");

    if (EVENTS_COUNT % MEMBERS_BLOCK_SIZE == 0) {
        EventHandler *TEMP = new EventHandler[(EVENTS_COUNT / MEMBERS_BLOCK_SIZE + 1) * MEMBERS_BLOCK_SIZE];

        if (EVENTS) {
            memcpy(TEMP, EVENTS, sizeof(EventHandler) * EVENTS_COUNT);
            delete[] EVENTS;
        }

        EVENTS = TEMP;
    }
    InvokePtr = Invoke;
    strncpy(EVENTS[EVENTS_COUNT].MEMBER_NAME, method, MAX_MEMBER_NAME);
    //strncpy(EVENTS[EVENTS_COUNT]->URN_NAME,urn,MAX_URN_NAME);
    EVENTS[EVENTS_COUNT].DELEGATE = PARAMETER(1);
    // incrementing links for parameter 1
    //Invoke(INVOKE_LOCK_VARIABLE,PARAMETER(1));
    EVENTS_COUNT++;

    SoapRouter *router = (SoapRouter *)(long)nRouter;


    soap_router_register_service(router, SOAP_HANDLER, method, urn);
    RETURN_NUMBER(0);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPRouterFindService CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(3, "SOAPRouterFindService takes 3 parameters : SoapService* soap_router_find_service(SoapRouter *router, const char* urn, const char* method);");

    LOCAL_INIT;

    NUMBER nRouter;
    char   *urn;
    char   *method;

    GET_CHECK_NUMBER(0, nRouter, "SOAPRouterFindService : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, urn, "SOAPRouterFindService : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, method, "SOAPRouterFindService : parameter 2 should be a string (STATIC STRING)");

    SoapRouter *router = (SoapRouter *)(long)nRouter;

    RETURN_NUMBER((long)soap_router_find_service(router, urn, method));

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPRouterFree CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPRouterFree takes one parameter : void soap_router_free(SoapRouter *router);");

    LOCAL_INIT;

    NUMBER nRouter;
    char   *urn;
    char   *method;

    GET_CHECK_NUMBER(0, nRouter, "SOAPRouterFree : parameter 0 should be a number (STATIC NUMBER)");

    SoapRouter *router = (SoapRouter *)(long)nRouter;
    soap_router_free(router);

    RETURN_NUMBER(0);

    return 0;
}
//---------------------------------------------------------------------------

// SOAP Server

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPServerInitArgs CONCEPT_API_PARAMETERS {
    LOCAL_INIT;

    char **args = 0;

    if (PARAMETERS_COUNT) {
        args = new char * [PARAMETERS_COUNT];
        for (int i = 0; i < PARAMETERS_COUNT; i++) {
            char *fill_string = 0;
            GET_CHECK_STRING(i, fill_string, "SOAPServerInitArgs : all init parameters should be strings(STATIC STRING)");
            args[i] = fill_string;
        }
    }
    RETURN_NUMBER((long)soap_server_init_args(PARAMETERS_COUNT, args));
    if (args)
        delete[] args;
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPServerRegisterRouter CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "SOAPServerRegisterRouter takes 2 parameters : int soap_server_register_router(SoapRouter *router, const char* context);");

    LOCAL_INIT;

    NUMBER nRouter;
    char   *context;

    GET_CHECK_NUMBER(0, nRouter, "SOAPServerRegisterRouter : parameter 0 should be a number (STATIC NUMBER)");
    GET_CHECK_STRING(1, context, "SOAPServerRegisterRouter : parameter 1 should be a string (STATIC STRING)");

    SoapRouter *router = (SoapRouter *)(long)nRouter;

    RETURN_NUMBER(soap_server_register_router(router, context));

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPServerRun CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "SOAPServerRun takes no parameters : herror_t soap_server_run();");

    LOCAL_INIT;

    RETURN_NUMBER((long)soap_server_run());

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPServerDestroy CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "SOAPServerDestroy takes no parameters : void soap_server_destroy();");

    LOCAL_INIT;

    soap_server_destroy();
    RETURN_NUMBER(0);

    return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// SOAP - Compatibility (extra)

//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetRootDoc CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetRootDoc takes one parameter : envelope");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetRootDoc : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nPtr;

    RETURN_NUMBER((long)env->root->doc);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetCurrentNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetCurrentNode takes one parameter : envelope");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetCurrentNode : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nPtr;

    RETURN_NUMBER((long)env->cur);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetRootNode CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetRootNode takes one parameter : envelope");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetRootNode : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nPtr;

    RETURN_NUMBER((long)env->root);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetEnv CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetEnv takes one parameter : context");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetEnv : parameter 0 should be a number (STATIC NUMBER)");

    SoapCtx *ctx = (SoapCtx *)(long)nPtr;

    RETURN_NUMBER((long)ctx->env);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPError CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPError takes one parameter : error_id");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetEnviroment : parameter 0 should be a number (STATIC NUMBER)");

    herror_t *err = (herror_t *)(long)nPtr;

    AnsiString result;

    if (err) {
        result  = "[";
        result += AnsiString(herror_code(err));
        result += "]";
        result += herror_func(err);
        result += ": ";
        result += herror_message(err);
        herror_release(err);
    }

    RETURN_STRING(result);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetServiceUrn CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetServiceUrn takes one parameter : Service");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetServiceUrn : parameter 0 should be a number (STATIC NUMBER)");

    SoapService *env = (SoapService *)(long)nPtr;

    RETURN_STRING(env->urn);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetServiceMethod CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetServiceMethod takes one parameter : Service");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetServiceMethod : parameter 0 should be a number (STATIC NUMBER)");

    SoapService *env = (SoapService *)(long)nPtr;

    RETURN_STRING(env->method);
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPXMLGetNodeArray CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPXMLGetNodeArray takes one parameter : nodeID");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPXMLGetNodeArray : parameter 0 should be a number (STATIC NUMBER)");

    xmlNodePtr node = (xmlNodePtr)(long)nPtr;

    if (!node)
        return 0;

    void *array_var;
    void *array_data;

    Invoke(INVOKE_CREATE_VARIABLE, &array_var);
    Invoke(INVOKE_CREATE_ARRAY, array_var);

    GET_ARRAY_FROM_VARIABLE(array_var, array_data);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "name", (INTEGER)VARIABLE_STRING, (char *)node->name, (NUMBER)0);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "children", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->children);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "last", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->last);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "parent", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->parent);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "next", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->next);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "prev", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->prev);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "doc", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->doc);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "properties", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)node->properties);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "text", (INTEGER)VARIABLE_STRING, (char *)soap_xml_get_text(node), (NUMBER)0);

    RETURN_ARRAY(array_data);
    Invoke(INVOKE_FREE_VARIABLE, array_var);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetEnvArray CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetEnvArray takes one parameter : envelope");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetEnvArray : parameter 0 should be a number (STATIC NUMBER)");

    SoapEnv *env = (SoapEnv *)(long)nPtr;


    void *array_var;
    void *array_data;

    Invoke(INVOKE_CREATE_VARIABLE, &array_var);
    Invoke(INVOKE_CREATE_ARRAY, array_var);

    GET_ARRAY_FROM_VARIABLE(array_var, array_data);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "root", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)env->root);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "cur", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)env->cur);

    RETURN_ARRAY(array_data);
    Invoke(INVOKE_FREE_VARIABLE, array_var);

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_SOAPGetCtxArray CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "SOAPGetCtxArray takes one parameter : context");

    LOCAL_INIT;

    NUMBER nPtr;

    GET_CHECK_NUMBER(0, nPtr, "SOAPGetCtxArray : parameter 0 should be a number (STATIC NUMBER)");

    SoapCtx *ctx = (SoapCtx *)(long)nPtr;


    void *array_var;
    void *array_data;

    Invoke(INVOKE_CREATE_VARIABLE, &array_var);
    Invoke(INVOKE_CREATE_ARRAY, array_var);

    GET_ARRAY_FROM_VARIABLE(array_var, array_data);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "env", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)ctx->env);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, array_var, "attachments", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(long)ctx->attachments);

    RETURN_ARRAY(array_data);
    Invoke(INVOKE_FREE_VARIABLE, array_var);

    return 0;
}
//---------------------------------------------------------------------------
