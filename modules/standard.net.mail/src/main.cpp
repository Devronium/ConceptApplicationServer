//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include <string.h>
#include "AnsiString.h"
#include "mail.h"
#ifdef WITH_GSASL
 #include <gsasl.h>
#endif


#ifdef _WIN32
WSADATA wsa;
#endif

#ifdef WITH_GSASL
Gsasl *ctx = NULL;
#endif
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSAStartup(MAKEWORD(1, 1), &wsa);
#endif
#ifdef WITH_GSASL
    int rc;
    gsasl_init(&ctx);
    if ((rc = gsasl_init(&ctx)) != GSASL_OK) {
        //printf ("Cannot initialize libgsasl (%d): %s", rc, gsasl_strerror (rc));
        return (void *)gsasl_strerror(rc);
    }
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSACleanup();
#endif
#ifdef WITH_GSASL
 #ifndef _WIN32
    if (ctx)
        gsasl_done(ctx);
 #endif
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_mail CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(4, 6, "mail takes 4 to 6 parameters: static integer mail(string from, string to, string headers, string content [, string smtp_server=\"localhost\"][,number smtp_port=25])");

    LOCAL_INIT;

    char   *from    = (char *)empty_string;
    char   *to      = (char *)empty_string;
    char   *headers = (char *)empty_string;
    char   *content = (char *)empty_string;
    char   *server  = (char *)"localhost";
    NUMBER port     = 25;

    GET_CHECK_STRING(0, from, "mail : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, to, "mail : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, headers, "mail : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3, content, "mail : parameter 3 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT > 4) {
        GET_CHECK_STRING(4, server, "mail : parameter 4 should be a string (STATIC STRING)");
    }

    if (PARAMETERS_COUNT > 5) {
        GET_CHECK_NUMBER(5, port, "mail : parameter 5 should be a number (STATIC NUMBER)");
    }

    RETURN_NUMBER(send_mail(from, to, headers, content, server, (int)port));
    return 0;
}
//---------------------------------------------------------------------------
/*CONCEPT_DLL_API CONCEPT_mail_login CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(6,8,"mail_login takes 6 to 8 parameters: static integer mail_login(string from, string to, string headers, string content, string username, string password[, string smtp_server=\"localhost\"][,number smtp_port=25])");

    LOCAL_INIT;

    char *from="";
    char *to="";
    char *headers="";
    char *content="";
    char *server="localhost";
        char *username="";
        char *password="";
    NUMBER port=25;

    GET_CHECK_STRING(0,from,"mail_login : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1,to,"mail_login : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2,headers,"mail_login : parameter 2 should be a string (STATIC STRING)");
    GET_CHECK_STRING(3,content,"mail_login : parameter 3 should be a string (STATIC STRING)");
    GET_CHECK_STRING(4,username,"mail_login : parameter 4 should be a string (STATIC STRING)");
    GET_CHECK_STRING(5,password,"mail_login : parameter 5 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT>6) {
        GET_CHECK_STRING(6,server,"mail_login : parameter 6 should be a string (STATIC STRING)");
    }

    if (PARAMETERS_COUNT>7) {
        GET_CHECK_NUMBER(7,port,"mail_login : parameter 7 should be a number (STATIC NUMBER)");
    }

    RETURN_NUMBER(send_mail_login(ctx, from, to, headers, content, server, (int)port, username, password));
    return 0;
   }*/
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_newsletter CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(4, 6, "newsletter takes 4 to 6 parameters: static integer newsletter(string from, array<string> to, array<string>|string headers, array<string>|string content [, dont_wait_confirmation][, string smtp_server=\"localhost\"][,number smtp_port=25])");

    LOCAL_INIT;

    char *from = (char *)empty_string;

    char *to      = 0;
    char *headers = 0;
    char *content = 0;

    char *headers_s = (char *)empty_string;
    char *content_s = (char *)empty_string;

    char   *server = (char *)"localhost";
    NUMBER port    = 25;
    NUMBER fast    = 0;

    GET_CHECK_STRING(0, from, "newsletter : parameter 0 should be a string (STATIC STRING)");

    GET_CHECK_ARRAY(1, to, "newsletter : parameter 1 should be an array of strings");

    GET_ARRAY(2, headers);
    if (TYPE != VARIABLE_ARRAY) {
        headers = 0;
        GET_CHECK_STRING(2, headers_s, "newsletter : parameter 2 should be an array of strings or a string (STATIC STRING)");
    }

    GET_ARRAY(3, content);
    if (TYPE != VARIABLE_ARRAY) {
        content = 0;
        GET_CHECK_STRING(3, content_s, "newsletter : parameter 3 should be an array of strings or a string (STATIC STRING)");
    }

    if (PARAMETERS_COUNT > 4) {
        GET_CHECK_NUMBER(4, fast, "newsletter : parameter 4 should be a number (STATIC NUMBER)");
    }

    if (PARAMETERS_COUNT > 5) {
        GET_CHECK_STRING(5, server, "newsletter : parameter 5 should be a string (STATIC STRING)");
    }

    if (PARAMETERS_COUNT > 6) {
        GET_CHECK_NUMBER(6, port, "newsletter : parameter 6 should be a number (STATIC NUMBER)");
    }


    int count_to = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));

    int count_headers = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(2));
    int count_content = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(3));

    if (count_to == 0) {
        RETURN_NUMBER(0);
        return 0;
    } else
    if (count_to < 0) {
        RETURN_NUMBER(-1);
        return 0;
    }

    if ((headers) && (count_headers != count_to))
        return (void *)"newsletter : length of 'to' array parameter should be equal with length of 'headers' array (if 'headers' is an array)";

    if ((content) && (count_content != count_to))
        return (void *)"newsletter : length of 'to' array parameter should be equal with length of 'content' array (if 'content' is an array)";

    char **to_arr      = new char * [count_to];
    char **headers_arr = new char * [count_to];
    char **content_arr = new char * [count_to];

    for (INTEGER i = 0; i < count_to; i++) {
        char    *str = 0;
        NUMBER  nDummy;
        INTEGER type;

        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(1), i, &type, &str, &nDummy);

        if (type != VARIABLE_STRING) {
            delete[] to_arr;
            delete[] headers_arr;
            delete[] content_arr;

            return (void *)"newsletter: all ellements of 'to' should be strings (STATIC STRINGS)";
        }

        to_arr[i] = str;
    }

    if (!headers) {
        for (int i = 0; i < count_to; i++)
            headers_arr[i] = headers_s;
    } else {
        for (INTEGER i = 0; i < count_to; i++) {
            char    *str = 0;
            NUMBER  nDummy;
            INTEGER type;

            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(2), i, &type, &str, &nDummy);

            if (type != VARIABLE_STRING) {
                delete[] to_arr;
                delete[] headers_arr;
                delete[] content_arr;

                return (void *)"newsletter: all ellements of 'headers' should be strings (STATIC STRINGS)";
            }

            headers_arr[i] = str;
        }
    }

    if (!content) {
        for (int i = 0; i < count_to; i++)
            content_arr[i] = content_s;
    } else {
        for (INTEGER i = 0; i < count_to; i++) {
            char    *str = 0;
            NUMBER  nDummy;
            INTEGER type;

            Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(3), i, &type, &str, &nDummy);

            if (type != VARIABLE_STRING) {
                delete[] to_arr;
                delete[] headers_arr;
                delete[] content_arr;

                return (void *)"newsletter: all ellements of 'content' should be strings (STATIC STRINGS)";
            }

            content_arr[i] = str;
        }
    }

    RETURN_NUMBER(send_newsletter(from, to_arr, headers_arr, content_arr, count_to, (char)fast, server, (int)port));

    delete[] to_arr;
    delete[] headers_arr;
    delete[] content_arr;

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_mail_error CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "mail_error takes no parameters");

    LOCAL_INIT;

    RETURN_STRING(send_mail_error().c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT_sent_count CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "sent_count takes no parameters");

    LOCAL_INIT;

    RETURN_NUMBER(send_mails_count());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_open CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(3, 4, "pop3_open takes 3 to 4 parameters: (szHostname, szUsername, szPassword[, nPort=110]");
    char   *szHostname;
    char   *szUsername;
    char   *szPassword;
    NUMBER nPort = 110;

    LOCAL_INIT;

    GET_CHECK_STRING(0, szHostname, "pop3_open : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, szUsername, "pop3_open : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, szPassword, "pop3_open : parameter 2 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_NUMBER(3, nPort, "pop3_open : parameter 3 should be a number (STATIC NUMBER)");
    }

    RETURN_NUMBER(pop3_open(szHostname, szUsername, szPassword, (int)nPort));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_open_secured CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK_MIN_MAX(3, 4, "pop3_open_secured takes 3 to 4 parameters: (szHostname, szUsername, szPassMD5[, nPort=110]");
    char   *szHostname;
    char   *szUsername;
    char   *szPassword;
    NUMBER nPort = 110;

    LOCAL_INIT;

    GET_CHECK_STRING(0, szHostname, "pop3_open_secured : parameter 0 should be a string (STATIC STRING)");
    GET_CHECK_STRING(1, szUsername, "pop3_open_secured : parameter 1 should be a string (STATIC STRING)");
    GET_CHECK_STRING(2, szPassword, "pop3_open_secured : parameter 2 should be a string (STATIC STRING)");

    if (PARAMETERS_COUNT > 3) {
        GET_CHECK_NUMBER(3, nPort, "pop3_open_secured : parameter 3 should be a number (STATIC NUMBER)");
    }

    RETURN_NUMBER(pop3_open_secured(szHostname, szUsername, szPassword, (int)nPort));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_count_messages CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "pop3_count_messages takes one parameter: connection_handle (as returned by pop3_open)");

    NUMBER handle;

    LOCAL_INIT;
    GET_CHECK_NUMBER(0, handle, "pop3_count_messages : parameter 0 should be a number (STATIC NUMBER) => representing the connection handle");

    RETURN_NUMBER(pop3_count_messages((int)handle));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_get_message CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "pop3_get_message takes 2 parameters: connection_handle (as returned by pop3_open), message_index");

    NUMBER handle;
    NUMBER index;

    LOCAL_INIT;
    GET_CHECK_NUMBER(0, handle, "pop3_get_message : parameter 0 should be a number (STATIC NUMBER) => representing the connection handle");
    GET_CHECK_NUMBER(1, index, "pop3_get_message : parameter 1 should be a number (STATIC NUMBER) => representing the message index");

    RETURN_STRING(pop3_get_message((int)handle, (int)index).c_str());
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_delete_message CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(2, "pop3_delete_message takes 2 parameters: connection_handle (as returned by pop3_open), message_index");

    NUMBER handle;
    NUMBER index;

    LOCAL_INIT;
    GET_CHECK_NUMBER(0, handle, "pop3_delete_message : parameter 0 should be a number (STATIC NUMBER) => representing the connection handle");
    GET_CHECK_NUMBER(1, index, "pop3_delete_message : parameter 1 should be a number (STATIC NUMBER) => representing the message index");

    RETURN_NUMBER(pop3_delete_message((int)handle, (int)index));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_close CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(1, "pop3_close takes one parameter: connection_handle (as returned by pop3_open)");

    NUMBER handle;

    LOCAL_INIT;
    GET_CHECK_NUMBER(0, handle, "pop3_close : parameter 0 should be a number (STATIC NUMBER) => representing the connection handle");

    RETURN_NUMBER(pop3_close((int)handle));
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DLL_API CONCEPT__pop3_error CONCEPT_API_PARAMETERS {
    PARAMETERS_CHECK(0, "pop3_error takes no parameters");

    LOCAL_INIT;

    RETURN_STRING(pop3_last_error().c_str());
    return 0;
}
//---------------------------------------------------------------------------
