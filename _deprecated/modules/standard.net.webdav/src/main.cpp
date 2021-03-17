//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <ne_socket.h>
#include <ne_session.h>
#include <ne_utils.h>
#include <ne_auth.h>
#include <ne_basic.h>
#include <ne_locks.h>
#include <ne_redirect.h>
#include <ne_i18n.h>

INVOKE_CALL InvokePtr = 0;

//-----------------------------------------------------------------------------------
int f1(void *userdata, ne_request *p1, const ne_status *p2) {
    if (!userdata)
        return 0;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, userdata, &RES, &EXCEPTION, (INTEGER)2,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p1,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p2
              );

    if (RES) {
        InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

        switch (TYPE) {
            case VARIABLE_NUMBER:
                result = (int)ndata;
                break;

            case VARIABLE_STRING:
                result = AnsiString(szdata).ToInt();
                break;

            case VARIABLE_ARRAY:
            case VARIABLE_DELEGATE:
            case VARIABLE_CLASS:
                result = (long)szdata;
                break;
        }

        InvokePtr(INVOKE_FREE_VARIABLE, RES);
    }
    if (EXCEPTION)
        InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    return result;
}

int f2(void *userdata, const char *p1, size_t p2) {
    if (!userdata)
        return 0;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, userdata, &RES, &EXCEPTION, (INTEGER)2,
              (INTEGER)VARIABLE_STRING, (char *)p1, (double)0,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p2
              );

    if (RES) {
        InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

        switch (TYPE) {
            case VARIABLE_NUMBER:
                result = (int)ndata;
                break;

            case VARIABLE_STRING:
                result = AnsiString(szdata).ToInt();
                break;

            case VARIABLE_ARRAY:
            case VARIABLE_DELEGATE:
            case VARIABLE_CLASS:
                result = (long)szdata;
                break;
        }

        InvokePtr(INVOKE_FREE_VARIABLE, RES);
    }
    if (EXCEPTION)
        InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    return result;
}

/*int f3(void* userdata, char* p1, int* p2, char** p3, double* p4) {
   }*/

int f4(void *userdata, const char *p1, int p2, char *p3, char *p4) {
    if (!userdata)
        return 0;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, userdata, &RES, &EXCEPTION, (INTEGER)4,
              (INTEGER)VARIABLE_STRING, (char *)p1, (double)0,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p2,
              (INTEGER)VARIABLE_STRING, (char *)p3, (double)0,
              (INTEGER)VARIABLE_STRING, (char *)p4, (double)0
              );

    if (RES) {
        InvokePtr(INVOKE_GET_VARIABLE, RES, &TYPE, &szdata, &ndata);

        switch (TYPE) {
            case VARIABLE_NUMBER:
                result = (int)ndata;
                break;

            case VARIABLE_STRING:
                result = AnsiString(szdata).ToInt();
                break;

            case VARIABLE_ARRAY:
            case VARIABLE_DELEGATE:
            case VARIABLE_CLASS:
                result = (long)szdata;
                break;
        }

        InvokePtr(INVOKE_FREE_VARIABLE, RES);
    }
    if (EXCEPTION)
        InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    return result;
}

void f5(void *userdata, const struct ne_lock *p1, const ne_uri *p2, const ne_status *p3) {
    if (!userdata)
        return;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, userdata, &RES, &EXCEPTION, (INTEGER)3,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p1,
              (INTEGER)VARIABLE_STRING, (char *)ne_uri_unparse(p2), (double)0,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)p3
              );

    if (RES)
        InvokePtr(INVOKE_FREE_VARIABLE, RES);
    if (EXCEPTION)
        InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
}

//-----------------------------------------------------------------------------------
CONCEPT_INIT {
    InvokePtr = Invoke;
    //DEFINE_ECONSTANT(FO_ENUM_FORMAT_SVG);
    ne_sock_init();
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DESTROY {
    ne_sock_exit();
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_accept_2xx, 3)
    T_NUMBER(ne_accept_2xx, 0)     // void*
    T_NUMBER(ne_accept_2xx, 1)     // ne_request*

// ... parameter 2 is by reference (ne_status*)
    ne_status local_parameter_2;

    RETURN_NUMBER(ne_accept_2xx((void *)(long)PARAM(0), (ne_request *)(long)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2.code)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_accept_always, 3)
    T_NUMBER(ne_accept_always, 0)     // void*
    T_NUMBER(ne_accept_always, 1)     // ne_request*

// ... parameter 2 is by reference (ne_status*)
    ne_status local_parameter_2;

    RETURN_NUMBER(ne_accept_always((void *)(long)PARAM(0), (ne_request *)(long)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2.code)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_add_response_body_reader, 3)
    T_NUMBER(ne_add_response_body_reader, 0)     // ne_request*
    T_DELEGATE(ne_add_response_body_reader, 1)
    T_DELEGATE(ne_add_response_body_reader, 2)
//T_NUMBER(1) // ne_accept_response
//T_NUMBER(2) // ne_block_reader
//T_NUMBER(3) // void*

    ne_add_response_body_reader((ne_request *)(long)PARAM(0), f1, f2, (void *)PARAMETER(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_response_header, 2)
    T_NUMBER(ne_get_response_header, 0)     // ne_request*
    T_STRING(ne_get_response_header, 1)     // char*

    RETURN_STRING((char *)ne_get_response_header((ne_request *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_response_header_iterate, 4)
    T_NUMBER(ne_response_header_iterate, 0)     // ne_request*
    T_NUMBER(ne_response_header_iterate, 1)     // void*

// ... parameter 2 is by reference (char**)
    char *local_parameter_2;
// ... parameter 3 is by reference (char**)
    char *local_parameter_3;

    RETURN_NUMBER((long)ne_response_header_iterate((ne_request *)(long)PARAM(0), (void *)(long)PARAM(1), (const char **)&local_parameter_2, (const char **)&local_parameter_3))
    SET_STRING(2, local_parameter_2)
    SET_STRING(3, local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_add_request_header, 3)
    T_NUMBER(ne_add_request_header, 0)     // ne_request*
    T_STRING(ne_add_request_header, 1)     // char*
    T_STRING(ne_add_request_header, 2)     // char*

    ne_add_request_header((ne_request *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_request_dispatch, 1)
    T_NUMBER(ne_request_dispatch, 0)     // ne_request*

    RETURN_NUMBER(ne_request_dispatch((ne_request *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_status, 1)
    T_NUMBER(ne_get_status, 0)     // ne_request*

    const ne_status * st = ne_get_status((ne_request *)(long)PARAM(0));
    if (st) {
        RETURN_NUMBER((long)st->code)
    } else {
        RETURN_NUMBER((long)0)
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_session, 1)
    T_NUMBER(ne_get_session, 0)     // ne_request*

    RETURN_NUMBER((long)ne_get_session((ne_request *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_request_destroy, 1)
    T_NUMBER(ne_request_destroy, 0)     // ne_request*

    ne_request_destroy((ne_request *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_begin_request, 1)
    T_NUMBER(ne_begin_request, 0)     // ne_request*

    RETURN_NUMBER(ne_begin_request((ne_request *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_end_request, 1)
    T_NUMBER(ne_end_request, 0)     // ne_request*

    RETURN_NUMBER(ne_end_request((ne_request *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_read_response_block, 3)
    T_NUMBER(ne_read_response_block, 0)     // ne_request*
    T_STRING(ne_read_response_block, 1)     // char*
    T_NUMBER(ne_read_response_block, 2)     // int

    RETURN_NUMBER(ne_read_response_block((ne_request *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_discard_response, 1)
    T_NUMBER(ne_discard_response, 0)     // ne_request*

    RETURN_NUMBER(ne_discard_response((ne_request *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_read_response_to_fd, 2)
    T_NUMBER(ne_read_response_to_fd, 0)     // ne_request*
    T_NUMBER(ne_read_response_to_fd, 1)     // int

    RETURN_NUMBER(ne_read_response_to_fd((ne_request *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_request_flag, 3)
    T_NUMBER(ne_set_request_flag, 0)     // ne_request*
    T_NUMBER(ne_set_request_flag, 1)     // ne_request_flag
    T_NUMBER(ne_set_request_flag, 2)     // int

    ne_set_request_flag((ne_request *)(long)PARAM(0), (ne_request_flag)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_request_flag, 2)
    T_NUMBER(ne_get_request_flag, 0)     // ne_request*
    T_NUMBER(ne_get_request_flag, 1)     // ne_request_flag

    RETURN_NUMBER(ne_get_request_flag((ne_request *)(long)PARAM(0), (ne_request_flag)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_redirect_register, 1)
    T_NUMBER(ne_redirect_register, 0)     // ne_session*

    ne_redirect_register((ne_session *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_redirect_location, 1)
    T_NUMBER(ne_redirect_location, 0)     // ne_session*

    const ne_uri * uri = ne_redirect_location((ne_session *)(long)PARAM(0));
    if (uri) {
        RETURN_STRING(ne_uri_unparse(uri))
    } else {
        RETURN_STRING("")
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_copy, 5)
    T_NUMBER(ne_copy, 0)     // ne_session*
    T_NUMBER(ne_copy, 1)     // int
    T_NUMBER(ne_copy, 2)     // int
    T_STRING(ne_copy, 3)     // char*
    T_STRING(ne_copy, 4)     // char*

    RETURN_NUMBER(ne_copy((ne_session *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2), PARAM(3), PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_move, 4)
    T_NUMBER(ne_move, 0)     // ne_session*
    T_NUMBER(ne_move, 1)     // int
    T_STRING(ne_move, 2)     // char*
    T_STRING(ne_move, 3)     // char*

    RETURN_NUMBER(ne_move((ne_session *)(long)PARAM(0), (int)PARAM(1), PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_delete, 2)
    T_NUMBER(ne_delete, 0)     // ne_session*
    T_STRING(ne_delete, 1)     // char*

    RETURN_NUMBER(ne_delete((ne_session *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_mkcol, 2)
    T_NUMBER(ne_mkcol, 0)     // ne_session*
    T_STRING(ne_mkcol, 1)     // char*

    RETURN_NUMBER(ne_mkcol((ne_session *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_add_depth_header, 2)
    T_NUMBER(ne_add_depth_header, 0)     // ne_request*
    T_NUMBER(ne_add_depth_header, 1)     // int

    ne_add_depth_header((ne_request *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_getmodtime, 3)
    T_NUMBER(ne_getmodtime, 0)     // ne_session*
    T_STRING(ne_getmodtime, 1)     // char*

// ... parameter 2 is by reference (time_t*)
    time_t local_parameter_2;

    RETURN_NUMBER(ne_getmodtime((ne_session *)(long)PARAM(0), PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_content_type, 2)
    T_NUMBER(ne_get_content_type, 0)     // ne_request*

// ... parameter 1 is by reference (ne_content_type*)
    ne_content_type local_parameter_1;

    RETURN_NUMBER(ne_get_content_type((ne_request *)(long)PARAM(0), &local_parameter_1))

    AnsiString res = (char *)local_parameter_1.type;
    res           += (char *)"/";
    res           += (char *)local_parameter_1.subtype;

    SET_STRING(1, res.c_str())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_options2, 3)
    T_NUMBER(ne_options2, 0)     // ne_session*
    T_STRING(ne_options2, 1)     // char*
    T_NUMBER(ne_options2, 2)     // unsigned

    unsigned int opt = (unsigned int)PARAM_INT(2);
    RETURN_NUMBER(ne_options2((ne_session *)(long)PARAM(0), PARAM(1), &opt))
    SET_NUMBER(2, opt);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_range, 4)
    T_NUMBER(ne_get_range, 0)     // ne_session*
    T_STRING(ne_get_range, 1)     // char*
    T_NUMBER(ne_get_range, 3)     // int

// ... parameter 2 is by reference (ne_content_range*)
    ne_content_range local_parameter_2;

    RETURN_NUMBER(ne_get_range((ne_session *)(long)PARAM(0), PARAM(1), &local_parameter_2, (int)PARAM(3)))
    SET_NUMBER(2, (long)local_parameter_2.start)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_post, 4)
    T_NUMBER(ne_post, 0)     // ne_session*
    T_STRING(ne_post, 1)     // char*
    T_NUMBER(ne_post, 2)     // int
    T_STRING(ne_post, 3)     // char*

    RETURN_NUMBER(ne_post((ne_session *)(long)PARAM(0), PARAM(1), (int)PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_server_auth, 2)
    T_HANDLE(ne_set_server_auth, 0)     // ne_session*
    T_DELEGATE(ne_set_server_auth, 1)
//T_NUMBER(1) // ne_auth_creds
//T_NUMBER(2) // void*

    ne_set_server_auth((ne_session *)(long)PARAM(0), f4, (void *)PARAMETER(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_proxy_auth, 2)
    T_NUMBER(ne_set_proxy_auth, 0)     // ne_session*
    T_DELEGATE(ne_set_proxy_auth, 1)   // ne_auth_creds
//T_NUMBER(2) // void*

    ne_set_proxy_auth((ne_session *)(long)PARAM(0), f4, (void *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_add_server_auth, 3)
    T_NUMBER(ne_add_server_auth, 0)     // ne_session*
    T_NUMBER(ne_add_server_auth, 1)     // unsigned
    T_DELEGATE(ne_add_server_auth, 2)
//T_NUMBER(2) // ne_auth_creds
//T_NUMBER(3) // void*

    ne_add_server_auth((ne_session *)(long)PARAM(0), (unsigned)PARAM(1), f4, (void *)PARAMETER(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_add_proxy_auth, 3)
    T_NUMBER(ne_add_proxy_auth, 0)     // ne_session*
    T_NUMBER(ne_add_proxy_auth, 1)     // unsigned
    T_DELEGATE(ne_add_proxy_auth, 2)   // ne_auth_creds
//T_NUMBER(3) // void*

    ne_add_proxy_auth((ne_session *)(long)PARAM(0), (unsigned)PARAM(1), f4, (void *)PARAMETER(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_forget_auth, 1)
    T_NUMBER(ne_forget_auth, 0)     // ne_session*

    ne_forget_auth((ne_session *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_i18n_init, 1)
    T_STRING(ne_i18n_init, 0)     // char*

    ne_i18n_init(PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_create, 0)

    RETURN_NUMBER((long)ne_lock_create())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_copy, 1)
    T_NUMBER(ne_lock_copy, 0)     // ne_lock*

    RETURN_NUMBER((long)ne_lock_copy((struct ne_lock *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_free, 1)
    T_NUMBER(ne_lock_free, 0)     // ne_lock*

    ne_lock_free((struct ne_lock *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_destroy, 1)
    T_NUMBER(ne_lock_destroy, 0)     // ne_lock*

    ne_lock_destroy((struct ne_lock *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_create, 0)

    RETURN_NUMBER((long)ne_lockstore_create())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_register, 2)
    T_HANDLE(ne_lockstore_register, 0)
    T_HANDLE(ne_lockstore_register, 1)     // ne_session*

// ... parameter 0 is by reference (ne_lock_store*)
//ne_lock_store local_parameter_0;

    ne_lockstore_register((ne_lock_store *)(long)PARAM(0), (ne_session *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_destroy, 1)

// ... parameter 0 is by reference (ne_lock_store*)
    T_HANDLE(ne_lockstore_destroy, 0)

    ne_lockstore_destroy((ne_lock_store *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_add, 2)
    T_HANDLE(ne_lockstore_add, 0)
    T_HANDLE(ne_lockstore_add, 1)     // ne_lock*

// ... parameter 0 is by reference (ne_lock_store*)
//ne_lock_store local_parameter_0;

    ne_lockstore_add((ne_lock_store *)(long)PARAM(0), (struct ne_lock *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_remove, 2)
    T_HANDLE(ne_lockstore_remove, 0)
    T_HANDLE(ne_lockstore_remove, 1)     // ne_lock*

// ... parameter 0 is by reference (ne_lock_store*)
//ne_lock_store local_parameter_0;

    ne_lockstore_remove((ne_lock_store *)(long)PARAM(0), (struct ne_lock *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_first, 1)
    T_NUMBER(ne_lockstore_first, 0)
// ... parameter 0 is by reference (ne_lock_store*)
//ne_lock_store local_parameter_0;

    RETURN_NUMBER((long)ne_lockstore_first((ne_lock_store *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_next, 1)
    T_HANDLE(ne_lockstore_next, 0)

    RETURN_NUMBER((long)ne_lockstore_next((ne_lock_store *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lockstore_findbyuri, 2)
    T_NUMBER(ne_lockstore_findbyuri, 0)
    T_STRING(ne_lockstore_findbyuri, 1)
// ... parameter 0 is by reference (ne_lock_store*)
//ne_lock_store local_parameter_0;
// ... parameter 1 is by reference (ne_uri*)
//ne_uri local_parameter_1;

    ne_uri uri;
    ne_uri_parse(PARAM(1), &uri);

    RETURN_NUMBER((long)ne_lockstore_findbyuri((ne_lock_store *)PARAM_INT(0), &uri))

//SET_NUMBER(0, (long)local_parameter_0)
//SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock, 2)
    T_NUMBER(ne_lock, 0)     // ne_session*
    T_NUMBER(ne_lock, 1)     // ne_lock*

    RETURN_NUMBER(ne_lock((ne_session *)(long)PARAM(0), (struct ne_lock *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_unlock, 2)
    T_NUMBER(ne_unlock, 0)     // ne_session*
    T_NUMBER(ne_unlock, 1)     // ne_lock*

    RETURN_NUMBER(ne_unlock((ne_session *)(long)PARAM(0), (struct ne_lock *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_refresh, 2)
    T_NUMBER(ne_lock_refresh, 0)     // ne_session*
    T_NUMBER(ne_lock_refresh, 1)     // ne_lock*

    RETURN_NUMBER(ne_lock_refresh((ne_session *)(long)PARAM(0), (struct ne_lock *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_discover, 3)
    T_NUMBER(ne_lock_discover, 0)     // ne_session*
    T_STRING(ne_lock_discover, 1)     // char*
    T_DELEGATE(ne_lock_discover, 2)   // ne_lock_result
//T_NUMBER(3) // void*

    RETURN_NUMBER(ne_lock_discover((ne_session *)(long)PARAM(0), PARAM(1), f5, (void *)PARAMETER(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_using_resource, 3)
    T_NUMBER(ne_lock_using_resource, 0)     // ne_request*
    T_STRING(ne_lock_using_resource, 1)     // char*
    T_NUMBER(ne_lock_using_resource, 2)     // int

    ne_lock_using_resource((ne_request *)(long)PARAM(0), PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_lock_using_parent, 2)
    T_NUMBER(ne_lock_using_parent, 0)     // ne_request*
    T_STRING(ne_lock_using_parent, 1)     // char*

    ne_lock_using_parent((ne_request *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_session_create, 3)
    T_STRING(ne_session_create, 0)     // char*
    T_STRING(ne_session_create, 1)     // char*
    T_NUMBER(ne_session_create, 2)     // unsigned

    RETURN_NUMBER((long)ne_session_create(PARAM(0), PARAM(1), (unsigned)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_session_destroy, 1)
    T_NUMBER(ne_session_destroy, 0)     // ne_session*

    ne_session_destroy((ne_session *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_close_connection, 1)
    T_NUMBER(ne_close_connection, 0)     // ne_session*

    ne_close_connection((ne_session *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_session_proxy, 3)
    T_NUMBER(ne_session_proxy, 0)     // ne_session*
    T_STRING(ne_session_proxy, 1)     // char*
    T_NUMBER(ne_session_proxy, 2)     // unsigned

    ne_session_proxy((ne_session *)(long)PARAM(0), PARAM(1), (unsigned)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_session_flag, 3)
    T_NUMBER(ne_set_session_flag, 0)     // ne_session*
    T_NUMBER(ne_set_session_flag, 1)     // ne_session_flag
    T_NUMBER(ne_set_session_flag, 2)     // int

    ne_set_session_flag((ne_session *)(long)PARAM(0), (ne_session_flag)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_session_flag, 2)
    T_NUMBER(ne_get_session_flag, 0)     // ne_session*
    T_NUMBER(ne_get_session_flag, 1)     // ne_session_flag

    RETURN_NUMBER(ne_get_session_flag((ne_session *)(long)PARAM(0), (ne_session_flag)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_addrlist, 3)
    T_NUMBER(ne_set_addrlist, 0)     // ne_session*
    T_NUMBER(ne_set_addrlist, 2)     // int

// ... parameter 1 is by reference (ne_inet_addr**)
    const ne_inet_addr * local_parameter_1;

    ne_set_addrlist((ne_session *)(long)PARAM(0), &local_parameter_1, (int)PARAM(2));
    SET_NUMBER(1, (long)local_parameter_1)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_localaddr, 2)
    T_NUMBER(ne_set_localaddr, 0)     // ne_session*
    T_NUMBER(ne_set_localaddr, 1)     // ne_inet_addr*

    ne_set_localaddr((ne_session *)(long)PARAM(0), (ne_inet_addr *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_session_private, 3)
    T_NUMBER(ne_set_session_private, 0)     // ne_session*
    T_STRING(ne_set_session_private, 1)     // char*
    T_NUMBER(ne_set_session_private, 2)     // void*

    ne_set_session_private((ne_session *)(long)PARAM(0), PARAM(1), (void *)(long)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_session_private, 2)
    T_NUMBER(ne_get_session_private, 0)     // ne_session*
    T_STRING(ne_get_session_private, 1)     // char*

    RETURN_NUMBER((long)ne_get_session_private((ne_session *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_set_clicert, 2)
    T_NUMBER(ne_ssl_set_clicert, 0)     // ne_session*
    T_NUMBER(ne_ssl_set_clicert, 1)     // ne_ssl_client_cert*

    ne_ssl_set_clicert((ne_session *)(long)PARAM(0), (ne_ssl_client_cert *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_trust_cert, 2)
    T_NUMBER(ne_ssl_trust_cert, 0)     // ne_session*
    T_NUMBER(ne_ssl_trust_cert, 1)     // ne_ssl_certificate*

    ne_ssl_trust_cert((ne_session *)(long)PARAM(0), (ne_ssl_certificate *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_trust_default_ca, 1)
    T_NUMBER(ne_ssl_trust_default_ca, 0)     // ne_session*

    ne_ssl_trust_default_ca((ne_session *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_read_timeout, 2)
    T_NUMBER(ne_set_read_timeout, 0)     // ne_session*
    T_NUMBER(ne_set_read_timeout, 1)     // int

    ne_set_read_timeout((ne_session *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_connect_timeout, 2)
    T_NUMBER(ne_set_connect_timeout, 0)     // ne_session*
    T_NUMBER(ne_set_connect_timeout, 1)     // int

    ne_set_connect_timeout((ne_session *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_set_useragent, 2)
    T_NUMBER(ne_set_useragent, 0)     // ne_session*
    T_STRING(ne_set_useragent, 1)     // char*

    ne_set_useragent((ne_session *)(long)PARAM(0), PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_version_pre_http11, 1)
    T_NUMBER(ne_version_pre_http11, 0)     // ne_session*

    RETURN_NUMBER(ne_version_pre_http11((ne_session *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_server_hostport, 1)
    T_NUMBER(ne_get_server_hostport, 0)     // ne_session*

    RETURN_STRING((char *)ne_get_server_hostport((ne_session *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_scheme, 1)
    T_NUMBER(ne_get_scheme, 0)     // ne_session*

    RETURN_STRING((char *)ne_get_scheme((ne_session *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_fill_server_uri, 2)
    T_NUMBER(ne_fill_server_uri, 0)     // ne_session*
//T_STRING(1)

    ne_uri uri;

    ne_fill_server_uri((ne_session *)(long)PARAM(0), &uri);
    char *parse = ne_uri_unparse((const ne_uri *)&uri);
    SET_STRING(1, parse)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_proxy_server_uri, 2)
    T_NUMBER(ne_proxy_server_uri, 0)     // ne_session*
//T_STRING(1)

    ne_uri uri;

    ne_fill_proxy_uri((ne_session *)(long)PARAM(0), &uri);
    char *parse = ne_uri_unparse((const ne_uri *)&uri);
    SET_STRING(1, parse)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_get_error, 1)
    T_NUMBER(ne_get_error, 0)     // ne_session*

    RETURN_STRING((char *)ne_get_error((ne_session *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_init, 0)

    RETURN_NUMBER(ne_sock_init())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_exit, 0)

    ne_sock_exit();
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_resolve, 2)
    T_STRING(ne_addr_resolve, 0)     // char*
    T_NUMBER(ne_addr_resolve, 1)     // int

    RETURN_NUMBER((long)ne_addr_resolve(PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_result, 1)
    T_HANDLE(ne_addr_result, 0)

    RETURN_NUMBER(ne_addr_result((ne_sock_addr *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_first, 1)
    T_HANDLE(ne_addr_first, 0)

    RETURN_NUMBER((long)ne_addr_first((ne_sock_addr *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_next, 1)
    T_HANDLE(ne_addr_next, 0)

    RETURN_NUMBER((long)ne_addr_next((ne_sock_addr *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_error, 2)
//T_STRING(1) // char*
//T_NUMBER(2) // int
    T_HANDLE(ne_addr_error, 0)
    char buffer[0xFFFF];

    RETURN_STRING((char *)ne_addr_error((ne_sock_addr *)PARAM_INT(0), buffer, (long)0xFFFF))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_addr_destroy, 1)
    T_HANDLE(ne_addr_destroy, 0)

    ne_addr_destroy((ne_sock_addr *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_make, 2)
    T_NUMBER(ne_iaddr_make, 0)     // ne_iaddr_type
    T_STRING(ne_iaddr_make, 1)     // unsigned

    RETURN_NUMBER((long)ne_iaddr_make((ne_iaddr_type)PARAM(0), (unsigned char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_cmp, 2)
    T_NUMBER(ne_iaddr_cmp, 0)     // ne_inet_addr*
    T_NUMBER(ne_iaddr_cmp, 1)     // ne_inet_addr*

    RETURN_NUMBER(ne_iaddr_cmp((ne_inet_addr *)(long)PARAM(0), (ne_inet_addr *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_typeof, 1)
    T_NUMBER(ne_iaddr_typeof, 0)     // ne_inet_addr*

    RETURN_NUMBER(ne_iaddr_typeof((ne_inet_addr *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_print, 3)
    T_NUMBER(ne_iaddr_print, 0)     // ne_inet_addr*
    T_STRING(ne_iaddr_print, 1)     // char*
    T_NUMBER(ne_iaddr_print, 2)     // int

    RETURN_STRING((char *)ne_iaddr_print((ne_inet_addr *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_reverse, 3)
    T_NUMBER(ne_iaddr_reverse, 0)     // ne_inet_addr*
    T_STRING(ne_iaddr_reverse, 1)     // char*
    T_NUMBER(ne_iaddr_reverse, 2)     // int

    RETURN_NUMBER(ne_iaddr_reverse((ne_inet_addr *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_iaddr_free, 1)
    T_NUMBER(ne_iaddr_free, 0)     // ne_inet_addr*

    ne_iaddr_free((ne_inet_addr *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_create, 0)

    RETURN_NUMBER((long)ne_sock_create())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_prebind, 3)
    T_NUMBER(ne_sock_prebind, 0)     // ne_socket*
    T_NUMBER(ne_sock_prebind, 1)     // ne_inet_addr*
    T_NUMBER(ne_sock_prebind, 2)     // unsigned

    ne_sock_prebind((ne_socket *)(long)PARAM(0), (ne_inet_addr *)(long)PARAM(1), (unsigned)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_connect, 3)
    T_NUMBER(ne_sock_connect, 0)     // ne_socket*
    T_NUMBER(ne_sock_connect, 1)     // ne_inet_addr*
    T_NUMBER(ne_sock_connect, 2)     // unsigned

    RETURN_NUMBER(ne_sock_connect((ne_socket *)(long)PARAM(0), (ne_inet_addr *)(long)PARAM(1), (unsigned)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_read, 3)
    T_NUMBER(ne_sock_read, 0)     // ne_socket*
    T_STRING(ne_sock_read, 1)     // char*
    T_NUMBER(ne_sock_read, 2)     // int

    RETURN_NUMBER(ne_sock_read((ne_socket *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_peek, 3)
    T_NUMBER(ne_sock_peek, 0)     // ne_socket*
    T_STRING(ne_sock_peek, 1)     // char*
    T_NUMBER(ne_sock_peek, 2)     // int

    RETURN_NUMBER(ne_sock_peek((ne_socket *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_block, 2)
    T_NUMBER(ne_sock_block, 0)     // ne_socket*
    T_NUMBER(ne_sock_block, 1)     // int

    RETURN_NUMBER(ne_sock_block((ne_socket *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_fullwrite, 3)
    T_NUMBER(ne_sock_fullwrite, 0)     // ne_socket*
    T_STRING(ne_sock_fullwrite, 1)     // char*
    T_NUMBER(ne_sock_fullwrite, 2)     // int

    RETURN_NUMBER(ne_sock_fullwrite((ne_socket *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_readline, 3)
    T_NUMBER(ne_sock_readline, 0)     // ne_socket*
    T_STRING(ne_sock_readline, 1)     // char*
    T_NUMBER(ne_sock_readline, 2)     // int

    RETURN_NUMBER(ne_sock_readline((ne_socket *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_fullread, 3)
    T_NUMBER(ne_sock_fullread, 0)     // ne_socket*
    T_STRING(ne_sock_fullread, 1)     // char*
    T_NUMBER(ne_sock_fullread, 2)     // int

    RETURN_NUMBER(ne_sock_fullread((ne_socket *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_accept, 2)
    T_NUMBER(ne_sock_accept, 0)     // ne_socket*
    T_NUMBER(ne_sock_accept, 1)     // int

    RETURN_NUMBER(ne_sock_accept((ne_socket *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_fd, 1)
    T_NUMBER(ne_sock_fd, 0)     // ne_socket*

    RETURN_NUMBER(ne_sock_fd((ne_socket *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_peer, 2)
    T_NUMBER(ne_sock_peer, 0)     // ne_socket*
    T_NUMBER(ne_sock_peer, 1)     // unsigned int

    int port = PARAM_INT(1);

    RETURN_NUMBER((long)ne_sock_peer((ne_socket *)(long)PARAM(0), (unsigned int *)&port))
    SET_NUMBER(1, port)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_close, 1)
    T_NUMBER(ne_sock_close, 0)     // ne_socket*

    RETURN_NUMBER(ne_sock_close((ne_socket *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_error, 1)
    T_NUMBER(ne_sock_error, 0)     // ne_socket*

    RETURN_STRING((char *)ne_sock_error((ne_socket *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_read_timeout, 2)
    T_NUMBER(ne_sock_read_timeout, 0)     // ne_socket*
    T_NUMBER(ne_sock_read_timeout, 1)     // int

    ne_sock_read_timeout((ne_socket *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_connect_timeout, 2)
    T_NUMBER(ne_sock_connect_timeout, 0)     // ne_socket*
    T_NUMBER(ne_sock_connect_timeout, 1)     // int

    ne_sock_connect_timeout((ne_socket *)(long)PARAM(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_accept_ssl, 2)
    T_NUMBER(ne_sock_accept_ssl, 0)     // ne_socket*
    T_NUMBER(ne_sock_accept_ssl, 1)     // ne_ssl_context*

    RETURN_NUMBER(ne_sock_accept_ssl((ne_socket *)(long)PARAM(0), (ne_ssl_context *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_connect_ssl, 3)
    T_NUMBER(ne_sock_connect_ssl, 0)     // ne_socket*
    T_NUMBER(ne_sock_connect_ssl, 1)     // ne_ssl_context*
    T_NUMBER(ne_sock_connect_ssl, 2)     // void*

    RETURN_NUMBER(ne_sock_connect_ssl((ne_socket *)(long)PARAM(0), (ne_ssl_context *)(long)PARAM(1), (void *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_sessid, 3)
    T_HANDLE(ne_sock_sessid, 0)     // ne_socket*
//T_NUMBER(1)
    T_NUMBER(ne_sock_sessid, 2)
//T_STRING(1)

// ... parameter 2 is by reference (int*)
    size_t local_parameter_2 = (size_t)PARAM_INT(2);
    char *buf = new char[local_parameter_2 + 1];
    buf[local_parameter_2] = 0;

    RETURN_NUMBER(ne_sock_sessid((ne_socket *)(long)PARAM(0), (unsigned char *)buf, &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_BUFFER(1, (char *)buf, (long)local_parameter_2);
    delete[] buf;
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_sock_cipher, 1)
    T_NUMBER(ne_sock_cipher, 0)     // ne_socket*

    RETURN_STRING((char *)ne_sock_cipher((ne_socket *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_readable_dname, 1)
    T_NUMBER(ne_ssl_readable_dname, 0)     // ne_ssl_dname*

    RETURN_STRING((char *)ne_ssl_readable_dname((ne_ssl_dname *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_dname_cmp, 2)
    T_NUMBER(ne_ssl_dname_cmp, 0)     // ne_ssl_dname*
    T_NUMBER(ne_ssl_dname_cmp, 1)     // ne_ssl_dname*

    RETURN_NUMBER(ne_ssl_dname_cmp((ne_ssl_dname *)(long)PARAM(0), (ne_ssl_dname *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_read, 1)
    T_STRING(ne_ssl_cert_read, 0)     // char*

    RETURN_NUMBER((long)ne_ssl_cert_read(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_write, 2)
    T_NUMBER(ne_ssl_cert_write, 0)     // ne_ssl_certificate*
    T_STRING(ne_ssl_cert_write, 1)     // char*

    RETURN_NUMBER(ne_ssl_cert_write((ne_ssl_certificate *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_export, 1)
    T_NUMBER(ne_ssl_cert_export, 0)     // ne_ssl_certificate*

    RETURN_STRING((char *)ne_ssl_cert_export((ne_ssl_certificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_import, 1)
    T_STRING(ne_ssl_cert_import, 0)     // char*

    RETURN_NUMBER((long)ne_ssl_cert_import(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_identity, 1)
    T_NUMBER(ne_ssl_cert_identity, 0)     // ne_ssl_certificate*

    RETURN_STRING((char *)ne_ssl_cert_identity((ne_ssl_certificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_signedby, 1)
    T_NUMBER(ne_ssl_cert_signedby, 0)     // ne_ssl_certificate*

    RETURN_NUMBER((long)ne_ssl_cert_signedby((ne_ssl_certificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_issuer, 1)
    T_NUMBER(ne_ssl_cert_issuer, 0)     // ne_ssl_certificate*

    RETURN_NUMBER((long)ne_ssl_cert_issuer((ne_ssl_certificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_subject, 1)
    T_NUMBER(ne_ssl_cert_subject, 0)     // ne_ssl_certificate*

    RETURN_NUMBER((long)ne_ssl_cert_subject((ne_ssl_certificate *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_digest, 2)
    T_NUMBER(ne_ssl_cert_digest, 0)     // ne_ssl_certificate*
    T_STRING(ne_ssl_cert_digest, 1)     // char*

    RETURN_NUMBER(ne_ssl_cert_digest((ne_ssl_certificate *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_validity_time, 3)
    T_NUMBER(ne_ssl_cert_validity_time, 0)     // ne_ssl_certificate*

// ... parameter 1 is by reference (time_t*)
    time_t local_parameter_1;
// ... parameter 2 is by reference (time_t*)
    time_t local_parameter_2;

    ne_ssl_cert_validity_time((ne_ssl_certificate *)(long)PARAM(0), &local_parameter_1, &local_parameter_2);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_validity, 3)
    T_NUMBER(ne_ssl_cert_validity, 0)     // ne_ssl_certificate*
    T_STRING(ne_ssl_cert_validity, 1)     // char*
    T_STRING(ne_ssl_cert_validity, 2)     // char*

    ne_ssl_cert_validity((ne_ssl_certificate *)(long)PARAM(0), PARAM(1), PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_cmp, 2)
    T_NUMBER(ne_ssl_cert_cmp, 0)     // ne_ssl_certificate*
    T_NUMBER(ne_ssl_cert_cmp, 1)     // ne_ssl_certificate*

    RETURN_NUMBER(ne_ssl_cert_cmp((ne_ssl_certificate *)(long)PARAM(0), (ne_ssl_certificate *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_cert_free, 1)
    T_NUMBER(ne_ssl_cert_free, 0)     // ne_ssl_certificate*

    ne_ssl_cert_free((ne_ssl_certificate *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_read, 1)
    T_STRING(ne_ssl_clicert_read, 0)     // char*

    RETURN_NUMBER((long)ne_ssl_clicert_read(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_name, 1)
    T_NUMBER(ne_ssl_clicert_name, 0)     // ne_ssl_client_cert*

    RETURN_STRING((char *)ne_ssl_clicert_name((ne_ssl_client_cert *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_encrypted, 1)
    T_NUMBER(ne_ssl_clicert_encrypted, 0)     // ne_ssl_client_cert*

    RETURN_NUMBER(ne_ssl_clicert_encrypted((ne_ssl_client_cert *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_decrypt, 2)
    T_NUMBER(ne_ssl_clicert_decrypt, 0)     // ne_ssl_client_cert*
    T_STRING(ne_ssl_clicert_decrypt, 1)     // char*

    RETURN_NUMBER(ne_ssl_clicert_decrypt((ne_ssl_client_cert *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_owner, 1)
    T_NUMBER(ne_ssl_clicert_owner, 0)     // ne_ssl_client_cert*

    RETURN_NUMBER((long)ne_ssl_clicert_owner((ne_ssl_client_cert *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_clicert_free, 1)
    T_NUMBER(ne_ssl_clicert_free, 0)     // ne_ssl_client_cert*

    ne_ssl_clicert_free((ne_ssl_client_cert *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_create, 1)
    T_NUMBER(ne_ssl_context_create, 0)     // int

    RETURN_NUMBER((long)ne_ssl_context_create((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_trustcert, 2)
    T_NUMBER(ne_ssl_context_trustcert, 0)     // ne_ssl_context*
    T_NUMBER(ne_ssl_context_trustcert, 1)     // ne_ssl_certificate*

    ne_ssl_context_trustcert((ne_ssl_context *)(long)PARAM(0), (ne_ssl_certificate *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_keypair, 3)
    T_NUMBER(ne_ssl_context_keypair, 0)     // ne_ssl_context*
    T_STRING(ne_ssl_context_keypair, 1)     // char*
    T_STRING(ne_ssl_context_keypair, 2)     // char*

    RETURN_NUMBER(ne_ssl_context_keypair((ne_ssl_context *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_set_verify, 4)
    T_NUMBER(ne_ssl_context_set_verify, 0)     // ne_ssl_context*
    T_NUMBER(ne_ssl_context_set_verify, 1)     // int
    T_STRING(ne_ssl_context_set_verify, 2)     // char*
    T_STRING(ne_ssl_context_set_verify, 3)     // char*

    RETURN_NUMBER(ne_ssl_context_set_verify((ne_ssl_context *)(long)PARAM(0), (int)PARAM(1), PARAM(2), PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_set_flag, 3)
    T_NUMBER(ne_ssl_context_set_flag, 0)     // ne_ssl_context*
    T_NUMBER(ne_ssl_context_set_flag, 1)     // int
    T_NUMBER(ne_ssl_context_set_flag, 2)     // int

    ne_ssl_context_set_flag((ne_ssl_context *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ne_ssl_context_destroy, 1)
    T_NUMBER(ne_ssl_context_destroy, 0)     // ne_ssl_context*

    ne_ssl_context_destroy((ne_ssl_context *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL

