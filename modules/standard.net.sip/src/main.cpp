//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <map>
extern "C" {
#define ENABLE_TRACE
#include <osip2/osip.h>
#include <osip2/osip_mt.h>
#include <osip2/osip_dialog.h>
#include <osipparser2/osip_parser.h>
#include <osipparser2/sdp_message.h>
}
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
static std::map<int, std::map<int, void *> > EventsMap;
#define det_osip_strdup(a)    (((a) && ((a)[0])) ? osip_strdup(a) : NULL)
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(ICT_PRE_CALLING)
    DEFINE_ECONSTANT(ICT_CALLING)
    DEFINE_ECONSTANT(ICT_PROCEEDING)
    DEFINE_ECONSTANT(ICT_COMPLETED)
    DEFINE_ECONSTANT(ICT_TERMINATED)

    DEFINE_ECONSTANT(IST_PRE_PROCEEDING)
    DEFINE_ECONSTANT(IST_PROCEEDING)
    DEFINE_ECONSTANT(IST_COMPLETED)
    DEFINE_ECONSTANT(IST_CONFIRMED)
    DEFINE_ECONSTANT(IST_TERMINATED)

    DEFINE_ECONSTANT(NICT_PRE_TRYING)
    DEFINE_ECONSTANT(NICT_TRYING)
    DEFINE_ECONSTANT(NICT_PROCEEDING)
    DEFINE_ECONSTANT(NICT_COMPLETED)
    DEFINE_ECONSTANT(NICT_TERMINATED)

    DEFINE_ECONSTANT(NIST_PRE_TRYING)
    DEFINE_ECONSTANT(NIST_TRYING)
    DEFINE_ECONSTANT(NIST_PROCEEDING)
    DEFINE_ECONSTANT(NIST_COMPLETED)
    DEFINE_ECONSTANT(NIST_TERMINATED)

    DEFINE_ECONSTANT(TIMEOUT_A)
    DEFINE_ECONSTANT(TIMEOUT_B)
    DEFINE_ECONSTANT(TIMEOUT_D)
    DEFINE_ECONSTANT(TIMEOUT_E)
    DEFINE_ECONSTANT(TIMEOUT_F)
    DEFINE_ECONSTANT(TIMEOUT_K)
    DEFINE_ECONSTANT(TIMEOUT_G)
    DEFINE_ECONSTANT(TIMEOUT_H)
    DEFINE_ECONSTANT(TIMEOUT_I)
    DEFINE_ECONSTANT(TIMEOUT_J)
    DEFINE_ECONSTANT(RCV_REQINVITE)
    DEFINE_ECONSTANT(RCV_REQACK)
    DEFINE_ECONSTANT(RCV_REQUEST)
    DEFINE_ECONSTANT(RCV_STATUS_1XX)
    DEFINE_ECONSTANT(RCV_STATUS_2XX)
    DEFINE_ECONSTANT(RCV_STATUS_3456XX)
    DEFINE_ECONSTANT(SND_REQINVITE)
    DEFINE_ECONSTANT(SND_REQACK)
    DEFINE_ECONSTANT(SND_REQUEST)
    DEFINE_ECONSTANT(SND_STATUS_1XX)
    DEFINE_ECONSTANT(SND_STATUS_2XX)
    DEFINE_ECONSTANT(SND_STATUS_3456XX)
    DEFINE_ECONSTANT(KILL_TRANSACTION)
    DEFINE_ECONSTANT(UNKNOWN_EVT)

    DEFINE_ECONSTANT(ICT)
    DEFINE_ECONSTANT(IST)
    DEFINE_ECONSTANT(NICT)
    DEFINE_ECONSTANT(NIST)
    DEFINE_ECONSTANT(DEFAULT_T1)
    DEFINE_ECONSTANT(DEFAULT_T2)
    DEFINE_ECONSTANT(DEFAULT_T4)
    DEFINE_ECONSTANT(OSIP_SRV_STATE_UNKNOWN)
    DEFINE_ECONSTANT(OSIP_SRV_STATE_RETRYLATER)
    DEFINE_ECONSTANT(OSIP_SRV_STATE_COMPLETED)
    DEFINE_ECONSTANT(OSIP_SRV_STATE_NOTSUPPORTED)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_UNKNOWN)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_INPROGRESS)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_NAPTRDONE)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_SRVINPROGRESS)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_SRVDONE)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_RETRYLATER)
    DEFINE_ECONSTANT(OSIP_NAPTR_STATE_NOTSUPPORTED)

    DEFINE_ECONSTANT(OSIP_ICT_INVITE_SENT)
    DEFINE_ECONSTANT(OSIP_ICT_INVITE_SENT_AGAIN)
    DEFINE_ECONSTANT(OSIP_ICT_ACK_SENT)
    DEFINE_ECONSTANT(OSIP_ICT_ACK_SENT_AGAIN)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_1XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_2XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_2XX_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_3XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_4XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_5XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_6XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_ICT_STATUS_3456XX_RECEIVED_AGAIN)

    DEFINE_ECONSTANT(OSIP_IST_INVITE_RECEIVED)
    DEFINE_ECONSTANT(OSIP_IST_INVITE_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_IST_ACK_RECEIVED)
    DEFINE_ECONSTANT(OSIP_IST_ACK_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_1XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_2XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_2XX_SENT_AGAIN)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_3XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_4XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_5XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_6XX_SENT)
    DEFINE_ECONSTANT(OSIP_IST_STATUS_3456XX_SENT_AGAIN)

    DEFINE_ECONSTANT(OSIP_NICT_REGISTER_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_BYE_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_OPTIONS_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_INFO_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_CANCEL_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_NOTIFY_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_SUBSCRIBE_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_UNKNOWN_REQUEST_SENT)
    DEFINE_ECONSTANT(OSIP_NICT_REQUEST_SENT_AGAIN)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_1XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_2XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_2XX_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_3XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_4XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_5XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_6XX_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_3456XX_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_NIST_REGISTER_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_BYE_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_OPTIONS_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_INFO_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_CANCEL_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_NOTIFY_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_SUBSCRIBE_RECEIVED)

    DEFINE_ECONSTANT(OSIP_NIST_UNKNOWN_REQUEST_RECEIVED)
    DEFINE_ECONSTANT(OSIP_NIST_REQUEST_RECEIVED_AGAIN)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_1XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_2XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_2XX_SENT_AGAIN)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_3XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_4XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_5XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_6XX_SENT)
    DEFINE_ECONSTANT(OSIP_NIST_STATUS_3456XX_SENT_AGAIN)

    DEFINE_ECONSTANT(OSIP_ICT_STATUS_TIMEOUT)
    DEFINE_ECONSTANT(OSIP_NICT_STATUS_TIMEOUT)

    DEFINE_ECONSTANT(OSIP_MESSAGE_CALLBACK_COUNT)

    DEFINE_ECONSTANT(OSIP_ICT_KILL_TRANSACTION)
    DEFINE_ECONSTANT(OSIP_IST_KILL_TRANSACTION)
    DEFINE_ECONSTANT(OSIP_NICT_KILL_TRANSACTION)
    DEFINE_ECONSTANT(OSIP_NIST_KILL_TRANSACTION)

    DEFINE_ECONSTANT(OSIP_KILL_CALLBACK_COUNT)
    DEFINE_ECONSTANT(OSIP_ICT_TRANSPORT_ERROR)
    DEFINE_ECONSTANT(OSIP_IST_TRANSPORT_ERROR)
    DEFINE_ECONSTANT(OSIP_NICT_TRANSPORT_ERROR)
    DEFINE_ECONSTANT(OSIP_NIST_TRANSPORT_ERROR)

    DEFINE_ECONSTANT(OSIP_TRANSPORT_ERROR_CALLBACK_COUNT)

    DEFINE_ECONSTANT(OSIP_SUCCESS)
    DEFINE_ECONSTANT(OSIP_UNDEFINED_ERROR)
    DEFINE_ECONSTANT(OSIP_BADPARAMETER)
    DEFINE_ECONSTANT(OSIP_WRONG_STATE)
    DEFINE_ECONSTANT(OSIP_NOMEM)
    DEFINE_ECONSTANT(OSIP_SYNTAXERROR)
    DEFINE_ECONSTANT(OSIP_NOTFOUND)
    DEFINE_ECONSTANT(OSIP_API_NOT_INITIALIZED)
    DEFINE_ECONSTANT(OSIP_NO_NETWORK)
    DEFINE_ECONSTANT(OSIP_PORT_BUSY)
    DEFINE_ECONSTANT(OSIP_UNKNOWN_HOST)
    DEFINE_ECONSTANT(OSIP_DISK_FULL)
    DEFINE_ECONSTANT(OSIP_NO_RIGHTS)
    DEFINE_ECONSTANT(OSIP_FILE_NOT_EXIST)
    DEFINE_ECONSTANT(OSIP_TIMEOUT)
    DEFINE_ECONSTANT(OSIP_TOOMUCHCALL)
    DEFINE_ECONSTANT(OSIP_WRONG_FORMAT)
    DEFINE_ECONSTANT(OSIP_NOCOMMONCODEC)

    DEFINE_ESCONSTANT(ACCEPT)
    DEFINE_ESCONSTANT(ACCEPT_ENCODING)
    DEFINE_ESCONSTANT(ACCEPT_LANGUAGE)
    DEFINE_ESCONSTANT(ALERT_INFO)
    DEFINE_ESCONSTANT(ALLOW)
    DEFINE_ESCONSTANT(AUTHENTICATION_INFO)
    DEFINE_ESCONSTANT(AUTHORIZATION)
    DEFINE_ESCONSTANT(CALL_ID)
    DEFINE_ESCONSTANT(CALL_ID_SHORT)
    DEFINE_ESCONSTANT(CALL_INFO)
    DEFINE_ESCONSTANT(CONTACT)
    DEFINE_ESCONSTANT(CONTACT_SHORT)
    DEFINE_ESCONSTANT(CONTENT_DISPOSITION)
    DEFINE_ESCONSTANT(CONTENT_ENCODING_SHORT)
    DEFINE_ESCONSTANT(CONTENT_ENCODING)
    DEFINE_ESCONSTANT(CONTENT_LANGUAGE)
    DEFINE_ESCONSTANT(CONTENT_LENGTH_SHORT)
    DEFINE_ESCONSTANT(CONTENT_LENGTH)
    DEFINE_ESCONSTANT(CONTENT_TYPE_SHORT)
    DEFINE_ESCONSTANT(CONTENT_TYPE)
    DEFINE_ESCONSTANT(CSEQ)
    DEFINE_ESCONSTANT(SIPDATE)
    DEFINE_ESCONSTANT(ERROR_INFO)
    DEFINE_ESCONSTANT(EXPIRES)
    DEFINE_ESCONSTANT(FROM)
    DEFINE_ESCONSTANT(FROM_SHORT)
    DEFINE_ESCONSTANT(IN_REPLY_TO)
    DEFINE_ESCONSTANT(MAX_FORWARDS)
    DEFINE_ESCONSTANT(MIME_VERSION)
    DEFINE_ESCONSTANT(MIN_EXPIRES)
    DEFINE_ESCONSTANT(ORGANIZATION)
    DEFINE_ESCONSTANT(PRIORITY)
    DEFINE_ESCONSTANT(PROXY_AUTHENTICATE)
    DEFINE_ESCONSTANT(PROXY_AUTHENTICATION_INFO)
    DEFINE_ESCONSTANT(PROXY_AUTHORIZATION)
    DEFINE_ESCONSTANT(PROXY_REQUIRE)
    DEFINE_ESCONSTANT(RECORD_ROUTE)
    DEFINE_ESCONSTANT(REPLY_TO)
    DEFINE_ESCONSTANT(REQUIRE)
    DEFINE_ESCONSTANT(RETRY_AFTER)
    DEFINE_ESCONSTANT(ROUTE)
    DEFINE_ESCONSTANT(SERVER)
    DEFINE_ESCONSTANT(SUBJECT)
    DEFINE_ESCONSTANT(SUBJECT_SHORT)
    DEFINE_ESCONSTANT(SUPPORTED)
    DEFINE_ESCONSTANT(TIMESTAMP)
    DEFINE_ESCONSTANT(TO)
    DEFINE_ESCONSTANT(TO_SHORT)
    DEFINE_ESCONSTANT(UNSUPPORTED)
    DEFINE_ESCONSTANT(USER_AGENT)
    DEFINE_ESCONSTANT(VIA)
    DEFINE_ESCONSTANT(VIA_SHORT)
    DEFINE_ESCONSTANT(WARNING)
    DEFINE_ESCONSTANT(WWW_AUTHENTICATE)


    DEFINE_ECONSTANT(RESPONSE_CODES)

    DEFINE_ECONSTANT(SIP_TRYING)
    DEFINE_ECONSTANT(SIP_RINGING)
    DEFINE_ECONSTANT(SIP_CALL_IS_BEING_FORWARDED)
    DEFINE_ECONSTANT(SIP_QUEUED)
    DEFINE_ECONSTANT(SIP_SESSION_PROGRESS)
    DEFINE_ECONSTANT(SIP_OK)
    DEFINE_ECONSTANT(SIP_ACCEPTED)
    DEFINE_ECONSTANT(SIP_MULTIPLE_CHOICES)
    DEFINE_ECONSTANT(SIP_MOVED_PERMANENTLY)
    DEFINE_ECONSTANT(SIP_MOVED_TEMPORARILY)
    DEFINE_ECONSTANT(SIP_USE_PROXY)
    DEFINE_ECONSTANT(SIP_ALTERNATIVE_SERVICE)
    DEFINE_ECONSTANT(SIP_BAD_REQUEST)
    DEFINE_ECONSTANT(SIP_UNAUTHORIZED)
    DEFINE_ECONSTANT(SIP_PAYMENT_REQUIRED)
    DEFINE_ECONSTANT(SIP_FORBIDDEN)
    DEFINE_ECONSTANT(SIP_NOT_FOUND)
    DEFINE_ECONSTANT(SIP_METHOD_NOT_ALLOWED)
    DEFINE_ECONSTANT(SIP_406_NOT_ACCEPTABLE)
    DEFINE_ECONSTANT(SIP_PROXY_AUTHENTICATION_REQUIRED)
    DEFINE_ECONSTANT(SIP_REQUEST_TIME_OUT)
    DEFINE_ECONSTANT(SIP_GONE)
    DEFINE_ECONSTANT(SIP_REQUEST_ENTITY_TOO_LARGE)
    DEFINE_ECONSTANT(SIP_REQUEST_URI_TOO_LARGE)
    DEFINE_ECONSTANT(SIP_UNSUPPORTED_MEDIA_TYPE)
    DEFINE_ECONSTANT(SIP_UNSUPPORTED_URI_SCHEME)
    DEFINE_ECONSTANT(SIP_BAD_EXTENSION)
    DEFINE_ECONSTANT(SIP_EXTENSION_REQUIRED)
    DEFINE_ECONSTANT(SIP_INTERVAL_TOO_BRIEF)
    DEFINE_ECONSTANT(SIP_TEMPORARILY_UNAVAILABLE)
    DEFINE_ECONSTANT(SIP_CALL_TRANSACTION_DOES_NOT_EXIST)
    DEFINE_ECONSTANT(SIP_LOOP_DETECTED)
    DEFINE_ECONSTANT(SIP_TOO_MANY_HOPS)
    DEFINE_ECONSTANT(SIP_ADDRESS_INCOMPLETE)
    DEFINE_ECONSTANT(SIP_AMBIGUOUS)
    DEFINE_ECONSTANT(SIP_BUSY_HERE)
    DEFINE_ECONSTANT(SIP_REQUEST_TERMINATED)
    DEFINE_ECONSTANT(SIP_NOT_ACCEPTABLE_HERE)
    DEFINE_ECONSTANT(SIP_BAD_EVENT)
    DEFINE_ECONSTANT(SIP_REQUEST_PENDING)
    DEFINE_ECONSTANT(SIP_UNDECIPHERABLE)
    DEFINE_ECONSTANT(SIP_INTERNAL_SERVER_ERROR)
    DEFINE_ECONSTANT(SIP_NOT_IMPLEMENTED)
    DEFINE_ECONSTANT(SIP_BAD_GATEWAY)
    DEFINE_ECONSTANT(SIP_SERVICE_UNAVAILABLE)
    DEFINE_ECONSTANT(SIP_SERVER_TIME_OUT)
    DEFINE_ECONSTANT(SIP_VERSION_NOT_SUPPORTED)
    DEFINE_ECONSTANT(SIP_MESSAGE_TOO_LARGE)
    DEFINE_ECONSTANT(SIP_BUSY_EVRYWHERE)
    DEFINE_ECONSTANT(SIP_DECLINE)
    DEFINE_ECONSTANT(SIP_DOES_NOT_EXIST_ANYWHERE)
    DEFINE_ECONSTANT(SIP_606_NOT_ACCEPTABLE)

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    parser_init();

    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    EventsMap.clear();
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_init, 1)
    osip_t * osip = 0;
    int res = osip_init(&osip);
    SET_NUMBER(0, (SYS_INT)osip)
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_release, 1)
    T_NUMBER(osip_release, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    if (osip) {
        if (osip->application_context) {
            delete (std::map<int, void *> *)osip->application_context;
            osip->application_context = 0;
        }
        osip_release(osip);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_ict_execute, 1)
    T_HANDLE(osip_ict_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    int res = osip_ict_execute(osip);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_ist_execute, 1)
    T_HANDLE(osip_ist_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    int res = osip_ist_execute(osip);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_nict_execute, 1)
    T_HANDLE(osip_nict_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    int res = osip_nict_execute(osip);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_nist_execute, 1)
    T_HANDLE(osip_nist_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    int res = osip_nist_execute(osip);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_timers_ict_execute, 1)
    T_HANDLE(osip_timers_ict_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_timers_ict_execute(osip);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_timers_ist_execute, 1)
    T_HANDLE(osip_timers_ist_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_timers_ist_execute(osip);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_timers_nict_execute, 1)
    T_HANDLE(osip_timers_nict_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_timers_nict_execute(osip);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_timers_nist_execute, 1)
    T_HANDLE(osip_timers_nist_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_timers_nist_execute(osip);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_parse, 1)
    T_STRING(osip_parse, 0)
    osip_event_t * evt = osip_parse(PARAM(0), PARAM_LEN(0));
    RETURN_NUMBER((SYS_INT)evt);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_event_free, 1)
    T_NUMBER(osip_event_free, 0)
    osip_event_t * evt = (osip_event_t *)PARAM_INT(0);
    if (evt) {
        osip_event_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_find_transaction_and_add_event, 2)
    T_HANDLE(osip_find_transaction_and_add_event, 0)
    T_HANDLE(osip_find_transaction_and_add_event, 1)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_event_t *evt = (osip_event_t *)PARAM_INT(1);
    int          res  = osip_find_transaction_and_add_event(osip, evt);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_create_transaction, 2)
    T_HANDLE(osip_create_transaction, 0)
    T_HANDLE(osip_create_transaction, 1)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_event_t       *evt = (osip_event_t *)PARAM_INT(1);
    osip_transaction_t *t   = osip_create_transaction(osip, evt);
    RETURN_NUMBER((SYS_INT)t);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_retransmissions_execute, 2)
    T_HANDLE(osip_retransmissions_execute, 0)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_retransmissions_execute(osip);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_free, 1)
    T_NUMBER(osip_transaction_free, 0)
    osip_transaction_t * evt = (osip_transaction_t *)PARAM_INT(0);
    int res = 0;
    if (evt) {
        res = osip_transaction_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_free2, 1)
    T_NUMBER(osip_transaction_free2, 0)
    osip_transaction_t * evt = (osip_transaction_t *)PARAM_INT(0);
    int res = 0;
    if (evt) {
        res = osip_transaction_free2(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_add_event, 2)
    T_HANDLE(osip_transaction_add_event, 0)
    T_HANDLE(osip_transaction_add_event, 1)
    osip_transaction_t * osip = (osip_transaction_t *)PARAM_INT(0);
    osip_event_t *evt = (osip_event_t *)PARAM_INT(1);
    int          res  = osip_transaction_add_event(osip, evt);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_init, 4)
//T_HANDLE(0)
    T_NUMBER(osip_transaction_init, 1)
    T_HANDLE(osip_transaction_init, 2)
    T_HANDLE(osip_transaction_init, 3)

    osip_transaction_t * t = NULL;
    osip_t         *osip    = (osip_t *)PARAM_INT(2);
    osip_message_t *request = (osip_message_t *)PARAM_INT(3);
    if ((request) && (request->allows)) {
        int res = osip_transaction_init(&t, (osip_fsm_type_t)PARAM_INT(1), osip, request);
        SET_NUMBER(0, (SYS_INT)t);
        RETURN_NUMBER(res);
    } else {
        SET_NUMBER(0, (SYS_INT)t);
        RETURN_NUMBER(-2);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_execute, 2)
    T_HANDLE(osip_transaction_execute, 0)
    T_HANDLE(osip_transaction_execute, 1)

    osip_transaction_t * osip = (osip_transaction_t *)PARAM_INT(0);
    osip_event_t *request = (osip_event_t *)PARAM_INT(1);

    SYS_INT i = osip_transaction_execute(osip, request);
    RETURN_NUMBER(i);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_set_out_socket, 2)
    T_HANDLE(osip_transaction_set_out_socket, 0)
    T_NUMBER(osip_transaction_set_out_socket, 1)

    int res = osip_transaction_set_out_socket((osip_transaction_t *)PARAM_INT(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_set_your_instance, 2)
    T_HANDLE(osip_transaction_set_your_instance, 0)
    T_NUMBER(osip_transaction_set_your_instance, 1)

    int res = osip_transaction_set_your_instance((osip_transaction_t *)PARAM_INT(0), (void *)PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_get_your_instance, 1)
    T_HANDLE(osip_transaction_get_your_instance, 0)

    SYS_INT res = (SYS_INT)osip_transaction_get_your_instance((osip_transaction_t *)PARAM_INT(0));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_transaction_get_destination, 3)
    T_HANDLE(osip_transaction_get_destination, 0)

    char *ip = "";
    int port = 0;
    int res  = osip_transaction_get_destination((osip_transaction_t *)PARAM_INT(0), &ip, &port);
    SET_STRING(1, ip);
    SET_NUMBER(2, port);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_nict_set_destination, 3)
    T_HANDLE(osip_nict_set_destination, 0)
    T_STRING(osip_nict_set_destination, 1)
    T_NUMBER(osip_nict_set_destination, 2)

    int res = osip_nict_set_destination((osip_nict_t *)PARAM_INT(0), PARAM(1), PARAM_INT(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_event_describe, 1)
    T_HANDLE(osip_event_describe, 0)

    osip_event_t * evt = (osip_event_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)evt->type);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "transactionid", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)evt->transactionid);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sip", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)evt->sip);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_init, 1)
    osip_message_t * msg = 0;
    int res = osip_message_init(&msg);
    SET_NUMBER(0, (SYS_INT)msg);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_free, 1)
    T_NUMBER(osip_message_free, 0)
    osip_message_t * evt = (osip_message_t *)PARAM_INT(0);
    if (evt) {
        osip_message_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
int do_osip_set_cb_send_message(osip_transaction_t *tr, osip_message_t *msg, char *buf, int port, int outsocket) {
    void *DELEGATE = EventsMap[0][0];

    if (!DELEGATE)
        return 0;

    int  result     = 0;
    void *RES       = 0;
    void *EXCEPTION = 0;
    InvokePtr(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)5, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)tr, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)msg, (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)outsocket);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;
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

    return result;
}

void do_osip_set_message_callback(int type, osip_transaction_t *tr, osip_message_t *msg) {
    void *DELEGATE = EventsMap[1][type];

    if (!DELEGATE)
        return;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  res        = InvokePtr(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)type, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)tr, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)msg);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    InvokePtr(INVOKE_FREE_VARIABLE, RES);
}

void do_osip_set_kill_transaction_callback(int type, osip_transaction_t *tr) {
    void *DELEGATE = EventsMap[2][type];

    if (!DELEGATE)
        return;
    void *RES       = 0;
    void *EXCEPTION = 0;
    InvokePtr(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)2, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)type, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)tr);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    InvokePtr(INVOKE_FREE_VARIABLE, RES);
}

void do_osip_set_transport_error_callback(int type, osip_transaction_t *tr, int error) {
    void *DELEGATE = EventsMap[3][type];

    if (!DELEGATE)
        return;
    void *RES       = 0;
    void *EXCEPTION = 0;
    InvokePtr(INVOKE_CALL_DELEGATE, DELEGATE, &RES, &EXCEPTION, (INTEGER)3, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)type, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)tr, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)error);
    InvokePtr(INVOKE_FREE_VARIABLE, EXCEPTION);
    InvokePtr(INVOKE_FREE_VARIABLE, RES);
}

CONCEPT_FUNCTION_IMPL(osip_set_cb_send_message, 2)
    T_HANDLE(osip_set_cb_send_message, 0)
    T_DELEGATE(osip_set_cb_send_message, 1)

    osip_t * osip = (osip_t *)PARAM_INT(0);
    if (EventsMap[0][0])
        Invoke(INVOKE_FREE_VARIABLE, EventsMap[0][0]);
    void *var = NULL;
    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(1), var);
    EventsMap[0][0] = var;
    osip_set_cb_send_message(osip, do_osip_set_cb_send_message);

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_set_message_callback, 3)
    T_HANDLE(osip_set_message_callback, 0)
    T_NUMBER(osip_set_message_callback, 1)
    T_DELEGATE(osip_set_message_callback, 2)

    osip_t * osip = (osip_t *)PARAM_INT(0);
    if (EventsMap[1][PARAM_INT(1)])
        Invoke(INVOKE_FREE_VARIABLE, EventsMap[1][PARAM_INT(1)]);
    void *var = NULL;
    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), var);
    EventsMap[1][PARAM_INT(1)] = var;

    osip_set_message_callback(osip, PARAM_INT(1), do_osip_set_message_callback);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_set_kill_transaction_callback, 3)
    T_HANDLE(osip_set_kill_transaction_callback, 0)
    T_NUMBER(osip_set_kill_transaction_callback, 1)
    T_DELEGATE(osip_set_kill_transaction_callback, 2)

    osip_t * osip = (osip_t *)PARAM_INT(0);
    if (EventsMap[2][PARAM_INT(1)])
        Invoke(INVOKE_FREE_VARIABLE, EventsMap[2][PARAM_INT(1)]);
    void *var = NULL;
    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), var);
    EventsMap[2][PARAM_INT(1)] = var;

    osip_set_kill_transaction_callback(osip, PARAM_INT(1), do_osip_set_kill_transaction_callback);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_set_transport_error_callback, 3)
    T_HANDLE(osip_set_transport_error_callback, 0)
    T_NUMBER(osip_set_transport_error_callback, 1)
    T_DELEGATE(osip_set_transport_error_callback, 2)

    osip_t * osip = (osip_t *)PARAM_INT(0);
    if (EventsMap[3][PARAM_INT(1)])
        Invoke(INVOKE_FREE_VARIABLE, EventsMap[3][PARAM_INT(1)]);
    void *var = NULL;
    DUPLICATE_VARIABLE_GC(PARAMETERS->HANDLER, PARAMETER(2), var);
    EventsMap[3][PARAM_INT(1)] = PARAMETER(2);

    osip_set_transport_error_callback(osip, PARAM_INT(1), do_osip_set_transport_error_callback);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_to_str, 2)
    T_HANDLE(osip_message_to_str, 0)
    SET_STRING(1, "");
    osip_message_t *sip   = (osip_message_t *)PARAM_INT(0);
    char           *msgP  = 0;
    size_t         msgLen = 0;
    int            res    = osip_message_to_str(sip, &msgP, &msgLen);
    if (msgLen > 0) {
        SET_BUFFER(1, msgP, msgLen);
    }
    if (msgP)
        osip_free(msgP);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_init, 1)
    osip_via_t * header = 0;
    int res = osip_via_init(&header);
    SET_NUMBER(0, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_free, 1)
    T_NUMBER(osip_via_free, 0)
    osip_via_t * evt = (osip_via_t *)PARAM_INT(0);
    if (evt) {
        osip_via_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_parse, 2)
    T_HANDLE(osip_via_parse, 0)
    T_STRING(osip_via_parse, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    int res = osip_via_parse(header, PARAM(1));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_to_str, 2)
    T_HANDLE(osip_via_to_str, 0)
//T_STRING(1)
    SET_STRING(1, "");

    osip_via_t *header = (osip_via_t *)PARAM_INT(0);
    char       *buf    = 0;
    int        res     = osip_via_to_str(header, &buf);
    if (buf) {
        SET_STRING(1, buf);
        osip_free(buf);
    }
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_clone, 2)
    T_HANDLE(osip_via_clone, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_t *clone = 0;
    int        res    = osip_via_clone(header, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_set_version, 2)
    T_HANDLE(via_set_version, 0)
    T_STRING(via_set_version, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    via_set_version(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_get_version, 1)
    T_HANDLE(via_get_version, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    char *res = via_get_version(header);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_set_protocol, 2)
    T_HANDLE(via_set_protocol, 0)
    T_STRING(via_set_protocol, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    via_set_protocol(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_get_protocol, 1)
    T_HANDLE(via_get_protocol, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    char *res = via_get_protocol(header);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_set_host, 2)
    T_HANDLE(via_set_host, 0)
    T_STRING(via_set_host, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    via_set_host(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_get_host, 1)
    T_HANDLE(via_get_host, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    char *res = via_get_host(header);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_set_port, 2)
    T_HANDLE(via_set_port, 0)
    T_STRING(via_set_port, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    via_set_port(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_get_port, 1)
    T_HANDLE(via_get_port, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    char *res = via_get_port(header);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_set_comment, 2)
    T_HANDLE(via_set_comment, 0)
    T_STRING(via_set_comment, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    via_set_comment(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(via_get_comment, 1)
    T_HANDLE(via_get_comment, 0)
    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    char *res = via_get_comment(header);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_set_ttl, 2)
    T_HANDLE(osip_via_set_ttl, 0)
    T_STRING(osip_via_set_ttl, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_set_ttl(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_set_maddr, 2)
    T_HANDLE(osip_via_set_maddr, 0)
    T_STRING(osip_via_set_maddr, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_set_maddr(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_set_received, 2)
    T_HANDLE(osip_via_set_received, 0)
    T_STRING(osip_via_set_received, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_set_received(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_set_branch, 2)
    T_HANDLE(osip_via_set_branch, 0)
    T_STRING(osip_via_set_branch, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_set_branch(header, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_param_add, 3)
    T_HANDLE(osip_via_param_add, 0)
    T_STRING(osip_via_param_add, 1)
    T_STRING(osip_via_param_add, 2)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_param_add(header, osip_strdup(PARAM(1)), osip_strdup(PARAM(2)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_via_match, 2)
    T_HANDLE(osip_via_match, 0)
    T_HANDLE(osip_via_match, 1)

    osip_via_t * header = (osip_via_t *)PARAM_INT(0);
    osip_via_t *target = (osip_via_t *)PARAM_INT(1);
    int        res     = osip_via_match(header, target);

    RETURN_NUMBER(res);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_parse, 2)
    T_HANDLE(osip_message_parse, 0)
    T_STRING(osip_message_parse, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    int res = osip_message_parse(message, PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//

/*CONCEPT_FUNCTION_IMPL(osip_message_parse_sipfrag, 2)
    T_HANDLE(osip_message_parse, 0)
    T_STRING(osip_message_parse, 1)
    osip_message_t *message=(osip_message_t *)PARAM_INT(0);
    int res=osip_message_parse_sipfrag(message, PARAM(1), PARAM_LEN(1));

    RETURN_NUMBER(res)
   END_IMPL
   //-----------------------------------------------------//
   CONCEPT_FUNCTION_IMPL(osip_message_to_str_sipfrag, 1)
    T_HANDLE(osip_message_to_str_sipfrag, 0)
    SET_STRING(1,"");
    osip_message_t *sip=(osip_message_t *)PARAM_INT(0);
    char *msgP=0;
    size_t msgLen=0;
    int res=osip_message_to_str_sipfrag(sip, &msgP, &msgLen);
    if (msgLen) {
        SET_BUFFER(1, msgP, msgLen);
    }
    if (msgP)
        osip_free(msgP);
    RETURN_NUMBER(res);
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_clone, 2)
    T_HANDLE(osip_message_clone, 0)

    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_t *clone = 0;

    int res = osip_message_clone(message, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_reason_phrase, 2)
    T_HANDLE(osip_message_set_reason_phrase, 0)
    T_STRING(osip_message_set_reason_phrase, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_reason_phrase(message, osip_strdup(PARAM(1)));

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_reason_phrase, 1)
    T_HANDLE(osip_message_get_reason_phrase, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);

    char *res = osip_message_get_reason_phrase(message);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_status_code, 2)
    T_HANDLE(osip_message_set_status_code, 0)
    T_NUMBER(osip_message_set_status_code, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_status_code(message, PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_status_code, 1)
    T_HANDLE(osip_message_get_status_code, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    int res = osip_message_get_status_code(message);

    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_method, 2)
    T_HANDLE(osip_message_set_method, 0)
    T_STRING(osip_message_set_method, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);

    osip_message_set_method(message, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_method, 1)
    T_HANDLE(osip_message_get_method, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);

    char *res = osip_message_get_method(message);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_version, 2)
    T_HANDLE(osip_message_set_version, 0)
    T_STRING(osip_message_set_version, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);

    osip_message_set_version(message, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_version, 1)
    T_HANDLE(osip_message_get_version, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);

    char *res = osip_message_get_version(message);
    if (res) {
        RETURN_STRING(res);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_uri, 2)
    T_HANDLE(osip_message_set_uri, 0)
    T_HANDLE(osip_message_set_uri, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_uri_t *uri = (osip_uri_t *)PARAM_INT(1);

    osip_message_set_uri(message, uri);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_uri, 1)
    T_HANDLE(osip_message_get_uri, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_uri_t *url = osip_message_get_uri(message);
    RETURN_NUMBER((SYS_INT)url)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_init, 1)
    osip_uri_t * url = 0;
    int res = osip_uri_init(&url);
    SET_NUMBER(0, (SYS_INT)url);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_free, 1)
    T_NUMBER(osip_uri_free, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    if (url) {
        osip_uri_free(url);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_parse, 2)
    T_HANDLE(osip_uri_parse, 0)
    T_STRING(osip_uri_parse, 1)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    int res = osip_uri_parse(url, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_to_str, 2)
    T_HANDLE(osip_uri_to_str, 0)
    SET_STRING(1, "");

    osip_uri_t *url  = (osip_uri_t *)PARAM_INT(0);
    char       *msgP = 0;
    int        res   = osip_uri_to_str(url, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_clone, 2)
    T_HANDLE(osip_uri_clone, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_t *clone = 0;
    int        res    = osip_uri_clone(url, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_scheme, 2)
    T_HANDLE(osip_uri_set_scheme, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_get_scheme, 1)
    T_HANDLE(osip_uri_get_scheme, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    char *res = osip_uri_get_scheme(url);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_host, 2)
    T_HANDLE(osip_uri_set_host, 0)
    T_STRING(osip_uri_set_host, 1)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_host(url, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_get_host, 1)
    T_HANDLE(osip_uri_get_host, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    char *res = osip_uri_get_host(url);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_username, 2)
    T_HANDLE(osip_uri_set_username, 0)
    T_STRING(osip_uri_set_username, 1)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_username(url, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_get_username, 1)
    T_HANDLE(osip_uri_get_username, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    char *res = osip_uri_get_username(url);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_password, 2)
    T_HANDLE(osip_uri_set_password, 0)
    T_STRING(osip_uri_set_password, 1)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_password(url, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_get_password, 1)
    T_HANDLE(osip_uri_get_password, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    char *res = osip_uri_get_password(url);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_port, 2)
    T_HANDLE(osip_uri_set_port, 0)
    T_STRING(osip_uri_set_port, 1)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_port(url, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_get_port, 1)
    T_HANDLE(osip_uri_get_port, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    char *res = osip_uri_get_port(url);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_transport_udp, 1)
    T_HANDLE(osip_uri_set_transport_udp, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_transport_udp(url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_transport_tcp, 1)
    T_HANDLE(osip_uri_set_transport_tcp, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_transport_tcp(url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_transport_sctp, 1)
    T_HANDLE(osip_uri_set_transport_sctp, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_transport_sctp(url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_uri_set_transport_tls, 1)
    T_HANDLE(osip_uri_set_transport_tls, 0)
    osip_uri_t * url = (osip_uri_t *)PARAM_INT(0);
    osip_uri_set_transport_tls(url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_force_update, 1)
    T_HANDLE(osip_message_force_update, 0)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    int res = osip_message_force_update(message);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_date, 2)
    T_HANDLE(osip_message_set_date, 0)
    T_STRING(osip_message_set_date, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_date(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_date, 3)
    T_HANDLE(osip_message_get_date, 0)
    T_NUMBER(osip_message_get_date, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_date(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_encryption, 2)
    T_HANDLE(osip_message_set_encryption, 0)
    T_STRING(osip_message_set_encryption, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_encryption(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_encryption, 3)
    T_HANDLE(osip_message_get_encryption, 0)
    T_NUMBER(osip_message_get_encryption, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_encryption(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_organization, 2)
    T_HANDLE(osip_message_set_organization, 0)
    T_STRING(osip_message_set_organization, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_organization(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_organization, 3)
    T_HANDLE(osip_message_get_organization, 0)
    T_NUMBER(osip_message_get_organization, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_organization(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_require, 2)
    T_HANDLE(osip_message_set_require, 0)
    T_STRING(osip_message_set_require, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_require(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_require, 3)
    T_HANDLE(osip_message_get_require, 0)
    T_NUMBER(osip_message_get_require, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_require(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_supported, 2)
    T_HANDLE(osip_message_set_supported, 0)
    T_STRING(osip_message_set_supported, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_supported(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_supported, 3)
    T_HANDLE(osip_message_get_supported, 0)
    T_NUMBER(osip_message_get_supported, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_supported(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_timestamp, 2)
    T_HANDLE(osip_message_set_timestamp, 0)
    T_STRING(osip_message_set_timestamp, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_timestamp(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_timestamp, 3)
    T_HANDLE(osip_message_get_timestamp, 0)
    T_NUMBER(osip_message_get_timestamp, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_timestamp(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_user_agent, 2)
    T_HANDLE(osip_message_set_user_agent, 0)
    T_STRING(osip_message_set_user_agent, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_user_agent(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_user_agent, 3)
    T_HANDLE(osip_message_get_user_agent, 0)
    T_NUMBER(osip_message_get_user_agent, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_user_agent(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_content_language, 2)
    T_HANDLE(osip_message_set_content_language, 0)
    T_STRING(osip_message_set_content_language, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_content_language(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_content_language, 3)
    T_HANDLE(osip_message_get_content_language, 0)
    T_NUMBER(osip_message_get_content_language, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_content_language(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_expires, 2)
    T_HANDLE(osip_message_set_expires, 0)
    T_STRING(osip_message_set_expires, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_expires(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_expires, 3)
    T_HANDLE(osip_message_get_expires, 0)
    T_NUMBER(osip_message_get_expires, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_expires(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_in_reply_to, 2)
    T_HANDLE(osip_message_set_in_reply_to, 0)
    T_STRING(osip_message_set_in_reply_to, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_in_reply_to(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_in_reply_to, 3)
    T_HANDLE(osip_message_get_in_reply_to, 0)
    T_NUMBER(osip_message_get_in_reply_to, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_in_reply_to(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_max_forwards, 2)
    T_HANDLE(osip_message_set_max_forwards, 0)
    T_STRING(osip_message_set_max_forwards, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_max_forwards(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_max_forwards, 3)
    T_HANDLE(osip_message_get_max_forwards, 0)
    T_NUMBER(osip_message_get_max_forwards, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_max_forwards(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_priority, 2)
    T_HANDLE(osip_message_set_priority, 0)
    T_STRING(osip_message_set_priority, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_priority(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_priority, 3)
    T_HANDLE(osip_message_get_priority, 0)
    T_NUMBER(osip_message_get_priority, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_priority(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_proxy_require, 2)
    T_HANDLE(osip_message_set_proxy_require, 0)
    T_STRING(osip_message_set_proxy_require, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_proxy_require(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_proxy_require, 3)
    T_HANDLE(osip_message_get_proxy_require, 0)
    T_NUMBER(osip_message_get_proxy_require, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_proxy_require(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_response_key, 2)
    T_HANDLE(osip_message_set_response_key, 0)
    T_STRING(osip_message_set_response_key, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_response_key(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_response_key, 3)
    T_HANDLE(osip_message_get_response_key, 0)
    T_NUMBER(osip_message_get_response_key, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_response_key(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_subject, 2)
    T_HANDLE(osip_message_set_subject, 0)
    T_STRING(osip_message_set_subject, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_subject(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_subject, 3)
    T_HANDLE(osip_message_get_subject, 0)
    T_NUMBER(osip_message_get_subject, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_subject(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_retry_after, 2)
    T_HANDLE(osip_message_set_retry_after, 0)
    T_STRING(osip_message_set_retry_after, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_retry_after(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_retry_after, 3)
    T_HANDLE(osip_message_get_retry_after, 0)
    T_NUMBER(osip_message_get_retry_after, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_retry_after(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_server, 2)
    T_HANDLE(osip_message_set_server, 0)
    T_STRING(osip_message_set_server, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_server(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_server, 3)
    T_HANDLE(osip_message_get_server, 0)
    T_NUMBER(osip_message_get_server, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_server(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_unsupported, 2)
    T_HANDLE(osip_message_set_unsupported, 0)
    T_STRING(osip_message_set_unsupported, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_unsupported(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_unsupported, 3)
    T_HANDLE(osip_message_get_unsupported, 0)
    T_NUMBER(osip_message_get_unsupported, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_unsupported(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_set_warning, 2)
    T_HANDLE(osip_message_set_warning, 0)
    T_STRING(osip_message_set_warning, 1)
    osip_message_t * message = (osip_message_t *)PARAM_INT(0);
    osip_message_set_warning(message, PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_warning, 3)
    T_HANDLE(osip_message_get_warning, 0)
    T_NUMBER(osip_message_get_warning, 1)
    osip_header_t * header = 0;
    osip_message_t *message = (osip_message_t *)PARAM_INT(0);
    int            res      = osip_message_get_warning(message, PARAM_INT(1), &header);
    SET_NUMBER(2, (SYS_INT)header);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_fix_last_via_header, 3)
    T_HANDLE(osip_message_fix_last_via_header, 0)     // osip_message_t*
    T_STRING(osip_message_fix_last_via_header, 1)     // char*
    T_NUMBER(osip_message_fix_last_via_header, 2)     // int

    RETURN_NUMBER(osip_message_fix_last_via_header((osip_message_t *)(long)PARAM(0), PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get__property, 1)
    T_HANDLE(osip_message_get__property, 0)     // osip_message_t*

    RETURN_NUMBER(osip_message_get__property((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_reason, 1)
    T_NUMBER(osip_message_get_reason, 0)     // int

    RETURN_STRING((char *)osip_message_get_reason((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_accept, 2)
    T_HANDLE(osip_message_set_accept, 0)     // osip_message_t*
    T_STRING(osip_message_set_accept, 1)     // char*

    RETURN_NUMBER(osip_message_set_accept((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_accept, 3)
    T_HANDLE(osip_message_get_accept, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_accept, 1)     // int

// ... parameter 2 is by reference (osip_accept_t**)
    osip_accept_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_accept((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_accept_encoding, 2)
    T_HANDLE(osip_message_set_accept_encoding, 0)     // osip_message_t*
    T_STRING(osip_message_set_accept_encoding, 1)     // char*

    RETURN_NUMBER(osip_message_set_accept_encoding((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_accept_encoding, 3)
    T_HANDLE(osip_message_get_accept_encoding, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_accept_encoding, 1)     // int

// ... parameter 2 is by reference (osip_accept_encoding_t**)
    osip_accept_encoding_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_accept_encoding((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_accept_language, 2)
    T_HANDLE(osip_message_set_accept_language, 0)     // osip_message_t*
    T_STRING(osip_message_set_accept_language, 1)     // char*

    RETURN_NUMBER(osip_message_set_accept_language((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_accept_language, 3)
    T_HANDLE(osip_message_get_accept_language, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_accept_language, 1)     // int

// ... parameter 2 is by reference (osip_accept_language_t**)
    osip_accept_language_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_accept_language((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_alert_info, 2)
    T_HANDLE(osip_message_set_alert_info, 0)     // osip_message_t*
    T_STRING(osip_message_set_alert_info, 1)     // char*

    RETURN_NUMBER(osip_message_set_alert_info((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_alert_info, 3)
    T_HANDLE(osip_message_get_alert_info, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_alert_info, 1)     // int

// ... parameter 2 is by reference (osip_alert_info_t**)
    osip_alert_info_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_alert_info((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_allow, 2)
    T_HANDLE(osip_message_set_allow, 0)     // osip_message_t*
    T_STRING(osip_message_set_allow, 1)     // char*

    RETURN_NUMBER(osip_message_set_allow((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_allow, 3)
    T_HANDLE(osip_message_get_allow, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_allow, 1)     // int

// ... parameter 2 is by reference (osip_allow_t**)
    osip_allow_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_allow((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_authentication_info, 2)
    T_HANDLE(osip_message_set_authentication_info, 0)     // osip_message_t*
    T_STRING(osip_message_set_authentication_info, 1)     // char*

    RETURN_NUMBER(osip_message_set_authentication_info((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_authentication_info, 3)
    T_HANDLE(osip_message_get_authentication_info, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_authentication_info, 1)     // int

// ... parameter 2 is by reference (osip_authentication_info_t**)
    osip_authentication_info_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_authentication_info((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_authorization, 2)
    T_HANDLE(osip_message_set_authorization, 0)     // osip_message_t*
    T_STRING(osip_message_set_authorization, 1)     // char*

    RETURN_NUMBER(osip_message_set_authorization((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_authorization, 3)
    T_HANDLE(osip_message_get_authorization, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_authorization, 1)     // int

// ... parameter 2 is by reference (osip_authorization_t**)
    osip_authorization_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_authorization((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_call_id, 2)
    T_HANDLE(osip_message_set_call_id, 0)     // osip_message_t*
    T_STRING(osip_message_set_call_id, 1)     // char*

    RETURN_NUMBER(osip_message_set_call_id((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_call_id, 1)
    T_HANDLE(osip_message_get_call_id, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_call_id((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_call_info, 2)
    T_HANDLE(osip_message_set_call_info, 0)     // osip_message_t*
    T_STRING(osip_message_set_call_info, 1)     // char*

    RETURN_NUMBER(osip_message_set_call_info((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_call_info, 3)
    T_HANDLE(osip_message_get_call_info, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_call_info, 1)     // int

// ... parameter 2 is by reference (osip_call_info_t**)
    osip_call_info_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_call_info((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_contact, 2)
    T_HANDLE(osip_message_set_contact, 0)     // osip_message_t*
    T_STRING(osip_message_set_contact, 1)     // char*

    RETURN_NUMBER(osip_message_set_contact((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_contact, 3)
    T_HANDLE(osip_message_get_contact, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_contact, 1)     // int

// ... parameter 2 is by reference (osip_contact_t**)
    osip_contact_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_contact((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_content_encoding, 2)
    T_HANDLE(osip_message_set_content_encoding, 0)     // osip_message_t*
    T_STRING(osip_message_set_content_encoding, 1)     // char*

    RETURN_NUMBER(osip_message_set_content_encoding((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_content_encoding, 3)
    T_HANDLE(osip_message_get_content_encoding, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_content_encoding, 1)     // int

// ... parameter 2 is by reference (osip_content_encoding_t**)
    osip_content_encoding_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_content_encoding((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_content_length, 2)
    T_HANDLE(osip_message_set_content_length, 0)     // osip_message_t*
    T_STRING(osip_message_set_content_length, 1)     // char*

    RETURN_NUMBER(osip_message_set_content_length((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_content_length, 1)
    T_HANDLE(osip_message_get_content_length, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_content_length((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_content_type, 2)
    T_HANDLE(osip_message_set_content_type, 0)     // osip_message_t*
    T_STRING(osip_message_set_content_type, 1)     // char*

    RETURN_NUMBER(osip_message_set_content_type((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_content_type, 1)
    T_HANDLE(osip_message_get_content_type, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_content_type((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_cseq, 2)
    T_HANDLE(osip_message_set_cseq, 0)     // osip_message_t*
    T_STRING(osip_message_set_cseq, 1)     // char*

    RETURN_NUMBER(osip_message_set_cseq((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_cseq, 1)
    T_HANDLE(osip_message_get_cseq, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_cseq((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_error_info, 2)
    T_HANDLE(osip_message_set_error_info, 0)     // osip_message_t*
    T_STRING(osip_message_set_error_info, 1)     // char*

    RETURN_NUMBER(osip_message_set_error_info((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_error_info, 3)
    T_HANDLE(osip_message_get_error_info, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_error_info, 1)     // int

// ... parameter 2 is by reference (osip_error_info_t**)
    osip_error_info_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_error_info((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_from, 2)
    T_HANDLE(osip_message_set_from, 0)     // osip_message_t*
    T_STRING(osip_message_set_from, 1)     // char*

    RETURN_NUMBER(osip_message_set_from((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_from, 1)
    T_HANDLE(osip_message_get_from, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_from((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_mime_version, 2)
    T_HANDLE(osip_message_set_mime_version, 0)     // osip_message_t*
    T_STRING(osip_message_set_mime_version, 1)     // char*

    RETURN_NUMBER(osip_message_set_mime_version((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_mime_version, 1)
    T_HANDLE(osip_message_get_mime_version, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_mime_version((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_proxy_authenticate, 2)
    T_HANDLE(osip_message_set_proxy_authenticate, 0)     // osip_message_t*
    T_STRING(osip_message_set_proxy_authenticate, 1)     // char*

    RETURN_NUMBER(osip_message_set_proxy_authenticate((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_proxy_authenticate, 3)
    T_HANDLE(osip_message_get_proxy_authenticate, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_proxy_authenticate, 1)     // int

// ... parameter 2 is by reference (osip_proxy_authenticate_t**)
    osip_proxy_authenticate_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_proxy_authenticate((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_proxy_authorization, 2)
    T_HANDLE(osip_message_set_proxy_authorization, 0)     // osip_message_t*
    T_STRING(osip_message_set_proxy_authorization, 1)     // char*

    RETURN_NUMBER(osip_message_set_proxy_authorization((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_proxy_authorization, 3)
    T_HANDLE(osip_message_get_proxy_authorization, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_proxy_authorization, 1)     // int

// ... parameter 2 is by reference (osip_proxy_authorization_t**)
    osip_proxy_authorization_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_proxy_authorization((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_proxy_authentication_info, 2)
    T_HANDLE(osip_message_set_proxy_authentication_info, 0)     // osip_message_t*
    T_STRING(osip_message_set_proxy_authentication_info, 1)     // char*

    RETURN_NUMBER(osip_message_set_proxy_authentication_info((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_proxy_authentication_info, 3)
    T_HANDLE(osip_message_get_proxy_authentication_info, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_proxy_authentication_info, 1)     // int

// ... parameter 2 is by reference (osip_proxy_authentication_info_t**)
    osip_proxy_authentication_info_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_proxy_authentication_info((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_record_route, 2)
    T_HANDLE(osip_message_set_record_route, 0)     // osip_message_t*
    T_STRING(osip_message_set_record_route, 1)     // char*

    RETURN_NUMBER(osip_message_set_record_route((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_record_route, 3)
    T_HANDLE(osip_message_get_record_route, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_record_route, 1)     // int

// ... parameter 2 is by reference (osip_record_route_t**)
    osip_record_route_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_record_route((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_route, 2)
    T_HANDLE(osip_message_set_route, 0)     // osip_message_t*
    T_STRING(osip_message_set_route, 1)     // char*

    RETURN_NUMBER(osip_message_set_route((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_route, 3)
    T_HANDLE(osip_message_get_route, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_route, 1)     // int

// ... parameter 2 is by reference (osip_route_t**)
    osip_route_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_route((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_to, 2)
    T_HANDLE(osip_message_set_to, 0)     // osip_message_t*
    T_STRING(osip_message_set_to, 1)     // char*

    RETURN_NUMBER(osip_message_set_to((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_to, 1)
    T_HANDLE(osip_message_get_to, 0)     // osip_message_t*

    RETURN_NUMBER((SYS_INT)osip_message_get_to((osip_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_via, 2)
    T_HANDLE(osip_message_set_via, 0)     // osip_message_t*
    T_STRING(osip_message_set_via, 1)     // char*

    RETURN_NUMBER(osip_message_set_via((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(osip_message_append_via,2)
        T_HANDLE(osip_message_set_via, 0) // osip_message_t*
        T_STRING(osip_message_set_via, 1) // char*

        RETURN_NUMBER(osip_message_append_via((osip_message_t*)(long)PARAM(0), PARAM(1)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_via, 3)
    T_HANDLE(osip_message_get_via, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_via, 1)     // int

// ... parameter 2 is by reference (osip_via_t**)
    osip_via_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_via((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_www_authenticate, 2)
    T_HANDLE(osip_message_set_www_authenticate, 0)     // osip_message_t*
    T_STRING(osip_message_set_www_authenticate, 1)     // char*

    RETURN_NUMBER(osip_message_set_www_authenticate((osip_message_t *)(long)PARAM(0), PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_www_authenticate, 3)
    T_HANDLE(osip_message_get_www_authenticate, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_www_authenticate, 1)     // int

// ... parameter 2 is by reference (osip_www_authenticate_t**)
    osip_www_authenticate_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_www_authenticate((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_header, 3)
    T_HANDLE(osip_message_set_header, 0)     // osip_message_t*
    T_STRING(osip_message_set_header, 1)     // char*
    T_STRING(osip_message_set_header, 2)     // char*

    RETURN_NUMBER(osip_message_set_header((osip_message_t *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(osip_message_replace_header,3)
        T_HANDLE(osip_message_set_header, 0) // osip_message_t*
        T_STRING(osip_message_set_header, 1) // char*
        T_STRING(osip_message_set_header, 2) // char*

        RETURN_NUMBER(osip_message_replace_header((osip_message_t*)(long)PARAM(0), PARAM(1), PARAM(2)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_topheader, 3)
    T_HANDLE(osip_message_set_topheader, 0)     // osip_message_t*
    T_STRING(osip_message_set_topheader, 1)     // char*
    T_STRING(osip_message_set_topheader, 2)     // char*

    RETURN_NUMBER(osip_message_set_topheader((osip_message_t *)(long)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_header_get_byname, 4)
    T_HANDLE(osip_message_header_get_byname, 0)     // osip_message_t*
    T_STRING(osip_message_header_get_byname, 1)     // char*
    T_NUMBER(osip_message_header_get_byname, 2)     // int

// ... parameter 3 is by reference (osip_header_t**)
    osip_header_t * local_parameter_3 = 0;

    RETURN_NUMBER(osip_message_header_get_byname((osip_message_t *)(long)PARAM(0), PARAM(1), (int)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_header, 3)
    T_HANDLE(osip_message_get_header, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_header, 1)     // int

// ... parameter 2 is by reference (osip_header_t**)
    osip_header_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_header((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_body, 2)
    T_HANDLE(osip_message_set_body, 0)     // osip_message_t*
    T_STRING(osip_message_set_body, 1)     // char*

    RETURN_NUMBER(osip_message_set_body((osip_message_t *)(long)PARAM(0), PARAM(1), (size_t)PARAM_LEN(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_set_body_mime, 2)
    T_HANDLE(osip_message_set_body_mime, 0)     // osip_message_t*
    T_STRING(osip_message_set_body_mime, 1)     // char*

    RETURN_NUMBER(osip_message_set_body_mime((osip_message_t *)(long)PARAM(0), PARAM(1), (size_t)PARAM_LEN(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_message_get_body, 3)
    T_HANDLE(osip_message_get_body, 0)     // osip_message_t*
    T_NUMBER(osip_message_get_body, 1)     // int

// ... parameter 2 is by reference (osip_body_t**)
    osip_body_t * local_parameter_2 = 0;

    RETURN_NUMBER(osip_message_get_body((osip_message_t *)(long)PARAM(0), (int)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_describe, 1)
    T_HANDLE(osip_message_describe, 0)

    osip_message_t * msg = (osip_message_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sip_version", (INTEGER)VARIABLE_STRING, (char *)msg->sip_version, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "req_uri", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->req_uri);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sip_method", (INTEGER)VARIABLE_STRING, (char *)msg->sip_method, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "status_code", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)msg->status_code);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "reason_phrase", (INTEGER)VARIABLE_STRING, (char *)msg->reason_phrase, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "authorizations", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->authorizations);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "call_id", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->call_id);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "contacts", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->contacts);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "content_length", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->content_length);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "content_type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->content_type);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cseq", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->cseq);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "from", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->from);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "mime_version", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->mime_version);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "proxy_authenticates", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->proxy_authenticates);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "record_routes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->record_routes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "routes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->routes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "to", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->to);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "vias", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->vias);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "www_authenticates", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->www_authenticates);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "headers", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->www_authenticates);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "bodies", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->www_authenticates);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "message_property", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)msg->message_property);
    if (msg->message_length)
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "message", (INTEGER)VARIABLE_STRING, (char *)msg->message, (NUMBER)msg->message_length);
    else
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "message", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "message_length", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)msg->message_length);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "application_data", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->application_data);
#ifndef MINISIZE
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "accepts", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->accepts);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "accept_encodings", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->accept_encodings);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "accept_languages", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->accept_languages);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "alert_infos", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->alert_infos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "allows", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->allows);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "authentication_infos", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->authentication_infos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "call_infos", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->call_infos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "content_encodings", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->content_encodings);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "error_infos", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->error_infos);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "proxy_authentication_infos", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->proxy_authentication_infos);
#endif
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_add, 3)
    T_HANDLE(osip_list_add, 0)
    T_HANDLE(osip_list_add, 1)
    T_NUMBER(osip_list_add, 2)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);
    void *handle = (void *)PARAM_INT(1);
    int  res     = osip_list_add(list, handle, PARAM_INT(2));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_add_string, 3)
    T_HANDLE(osip_list_add_string, 0)
    T_STRING(osip_list_add_string, 1)
    T_NUMBER(osip_list_add_string, 2)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);

    int res = osip_list_add(list, osip_strdup(PARAM(1)), PARAM_INT(2));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_size, 1)
    T_HANDLE(osip_list_size, 0)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);
    int res = osip_list_size(list);
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_remove, 2)
    T_HANDLE(osip_list_remove, 0)
    T_NUMBER(osip_list_remove, 1)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);
    int res = osip_list_remove(list, PARAM_INT(1));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_eol, 2)
    T_HANDLE(osip_list_eol, 0)
    T_NUMBER(osip_list_eol, 1)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);
    int res = osip_list_eol(list, PARAM_INT(1));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_list_get, 2)
    T_HANDLE(osip_list_get, 0)
    T_NUMBER(osip_list_get, 1)
    osip_list_t * list = (osip_list_t *)PARAM_INT(0);
    SYS_INT res = (SYS_INT)osip_list_get(list, PARAM_INT(1));
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_init, 1)
    osip_from_t * from = 0;
    int res = osip_from_init(&from);
    SET_NUMBER(0, (SYS_INT)from);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_free, 1)
    T_NUMBER(osip_from_free, 0)
    osip_from_t * evt = (osip_from_t *)PARAM_INT(0);
    if (evt) {
        osip_from_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_parse, 2)
    T_HANDLE(osip_from_parse, 0)
    T_STRING(osip_from_parse, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);

    int res = osip_from_parse(from, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_to_str, 2)
    T_HANDLE(osip_from_to_str, 0)
    SET_STRING(1, "");

    osip_from_t *from  = (osip_from_t *)PARAM_INT(0);
    char        *msgP  = 0;
    size_t      msgLen = 0;
    int         res    = osip_from_to_str(from, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_clone, 2)
    T_HANDLE(osip_from_clone, 0)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_from_t *clone = 0;
    int         res    = osip_from_clone(from, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_set_displayname, 2)
    T_HANDLE(osip_from_set_displayname, 0)
    T_STRING(osip_from_set_displayname, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_from_set_displayname(from, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_get_displayname, 1)
    T_HANDLE(osip_from_get_displayname, 0)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    char *res = osip_from_get_displayname(from);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_set_url, 2)
    T_HANDLE(osip_from_set_url, 0)
    T_HANDLE(osip_from_set_url, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_uri_t *url = (osip_uri_t *)PARAM_INT(1);
    osip_from_set_url(from, url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_get_url, 1)
    T_HANDLE(osip_from_get_url, 0)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_uri_t *url = osip_from_get_url(from);
    RETURN_NUMBER((SYS_INT)url);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_get_tag, 1)
    T_HANDLE(osip_from_get_tag, 0)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_uri_t *res = osip_from_get_url(from);
    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_set_tag, 2)
    T_HANDLE(osip_from_set_tag, 0)
    T_STRING(osip_from_set_tag, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_from_set_tag(from, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_compare, 2)
    T_HANDLE(osip_from_compare, 0)
    T_HANDLE(osip_from_compare, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_from_t *other = (osip_from_t *)PARAM_INT(0);
    int         res    = osip_from_compare(from, other);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_tag_match, 2)
    T_HANDLE(osip_from_tag_match, 0)
    T_HANDLE(osip_from_tag_match, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_from_t *other = (osip_from_t *)PARAM_INT(0);
    int         res    = osip_from_compare(from, other);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_from_message, 2)
    T_HANDLE(osip_from_message, 1)
    osip_message_t * msg = (osip_message_t *)PARAM_INT(1);
    int res = osip_from_init(&msg->from);
    SET_NUMBER(0, (SYS_INT)msg->from);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_message, 2)
    T_HANDLE(osip_to_message, 1)
    osip_message_t * msg = (osip_message_t *)PARAM_INT(1);
    int res = osip_to_init(&msg->to);
    SET_NUMBER(0, (SYS_INT)msg->to);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_init, 1)
    osip_to_t * to = 0;
    int res = osip_to_init(&to);
    SET_NUMBER(0, (SYS_INT)to);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_free, 1)
    T_NUMBER(osip_to_free, 0)
    osip_to_t * evt = (osip_to_t *)PARAM_INT(0);
    if (evt) {
        osip_to_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_parse, 2)
    T_HANDLE(osip_to_parse, 0)
    T_STRING(osip_to_parse, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);

    int res = osip_to_parse(to, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_to_str, 2)
    T_HANDLE(osip_to_to_str, 0)
    SET_STRING(1, "");

    osip_to_t *to    = (osip_to_t *)PARAM_INT(0);
    char      *msgP  = 0;
    size_t    msgLen = 0;
    int       res    = osip_to_to_str(to, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_clone, 2)
    T_HANDLE(osip_to_clone, 0)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_to_t *clone = 0;
    int       res    = osip_to_clone(to, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_set_displayname, 2)
    T_HANDLE(osip_to_set_displayname, 0)
    T_STRING(osip_to_set_displayname, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_to_set_displayname(to, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_get_displayname, 1)
    T_HANDLE(osip_to_get_displayname, 0)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    char *res = osip_to_get_displayname(to);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_set_url, 2)
    T_HANDLE(osip_to_set_url, 0)
    T_HANDLE(osip_to_set_url, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_uri_t *url = (osip_uri_t *)PARAM_INT(1);
    osip_to_set_url(to, url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_get_url, 1)
    T_HANDLE(osip_to_get_url, 0)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_uri_t *url = osip_to_get_url(to);
    RETURN_NUMBER((SYS_INT)url);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_get_tag, 1)
    T_HANDLE(osip_to_get_tag, 0)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_uri_t *res = osip_to_get_url(to);
    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_set_tag, 2)
    T_HANDLE(osip_to_set_tag, 0)
    T_STRING(osip_to_set_tag, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_to_set_tag(to, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_compare, 2)
    T_HANDLE(osip_to_compare, 0)
    T_HANDLE(osip_to_compare, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_to_t *other = (osip_to_t *)PARAM_INT(0);
    int       res    = osip_to_compare(to, other);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_to_tag_match, 2)
    T_HANDLE(osip_to_tag_match, 0)
    T_HANDLE(osip_to_tag_match, 1)
    osip_to_t * to = (osip_to_t *)PARAM_INT(0);
    osip_to_t *other = (osip_to_t *)PARAM_INT(0);
    int       res    = osip_to_compare(to, other);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_init, 1)
    osip_call_id_t * handle = 0;
    int res = osip_call_id_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_free, 1)
    T_NUMBER(osip_call_id_free, 0)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    if (handle) {
        osip_call_id_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_parse, 2)
    T_HANDLE(osip_call_id_parse, 0)
    T_STRING(osip_call_id_parse, 1)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    int res = osip_call_id_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_clone, 2)
    T_HANDLE(osip_call_id_clone, 0)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    osip_call_id_t *clone = 0;
    int            res    = osip_call_id_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_match, 2)
    T_HANDLE(osip_call_id_match, 0)
    T_HANDLE(osip_call_id_match, 1)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    osip_call_id_t *handle2 = (osip_call_id_t *)PARAM_INT(1);
    int            res      = osip_call_id_match(handle, handle2);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_info_init, 1)
    osip_call_info_t * handle = 0;
    int res = osip_call_info_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_info_free, 1)
    T_NUMBER(osip_call_info_free, 0)
    osip_call_info_t * handle = (osip_call_info_t *)PARAM_INT(0);
    if (handle) {
        osip_call_info_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_info_parse, 2)
    T_HANDLE(osip_call_info_parse, 0)
    T_STRING(osip_call_info_parse, 1)
    osip_call_info_t * handle = (osip_call_info_t *)PARAM_INT(0);
    int res = osip_call_info_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_info_clone, 2)
    T_HANDLE(osip_call_info_clone, 0)
    osip_call_info_t * handle = (osip_call_info_t *)PARAM_INT(0);
    osip_call_info_t *clone = 0;
    int res = osip_call_info_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_free, 1)
    T_NUMBER(osip_accept_free, 0)
    osip_accept_t * handle = (osip_accept_t *)PARAM_INT(0);
    if (handle) {
        osip_accept_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_parse, 2)
    T_HANDLE(osip_accept_parse, 0)
    T_STRING(osip_accept_parse, 1)
    osip_accept_t * handle = (osip_accept_t *)PARAM_INT(0);
    int res = osip_accept_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_clone, 2)
    T_HANDLE(osip_accept_clone, 0)
    osip_accept_t * handle = (osip_accept_t *)PARAM_INT(0);
    osip_accept_t *clone = 0;
    int           res    = osip_accept_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_encoding_init, 1)
    osip_accept_encoding_t * handle = 0;
    int res = osip_accept_encoding_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_encoding_free, 1)
    T_NUMBER(osip_accept_encoding_free, 0)
    osip_accept_encoding_t * handle = (osip_accept_encoding_t *)PARAM_INT(0);
    if (handle) {
        osip_accept_encoding_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_encoding_parse, 2)
    T_HANDLE(osip_accept_encoding_parse, 0)
    T_STRING(osip_accept_encoding_parse, 1)
    osip_accept_encoding_t * handle = (osip_accept_encoding_t *)PARAM_INT(0);
    int res = osip_accept_encoding_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_encoding_clone, 2)
    T_HANDLE(osip_accept_encoding_clone, 0)
    osip_accept_encoding_t * handle = (osip_accept_encoding_t *)PARAM_INT(0);
    osip_accept_encoding_t *clone = 0;
    int res = osip_accept_encoding_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_language_init, 1)
    osip_accept_language_t * handle = 0;
    int res = osip_accept_language_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_language_free, 1)
    T_NUMBER(osip_accept_language_free, 0)
    osip_accept_language_t * handle = (osip_accept_language_t *)PARAM_INT(0);
    if (handle) {
        osip_accept_language_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_language_parse, 2)
    T_HANDLE(osip_accept_language_parse, 0)
    T_STRING(osip_accept_language_parse, 1)
    osip_accept_language_t * handle = (osip_accept_language_t *)PARAM_INT(0);
    int res = osip_accept_language_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_accept_language_clone, 2)
    T_HANDLE(osip_accept_language_clone, 0)
    osip_accept_language_t * handle = (osip_accept_language_t *)PARAM_INT(0);
    osip_accept_language_t *clone = 0;
    int res = osip_accept_language_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_alert_info_init, 1)
    osip_alert_info_t * handle = 0;
    int res = osip_alert_info_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_alert_info_free, 1)
    T_NUMBER(osip_alert_info_free, 0)
    osip_alert_info_t * handle = (osip_alert_info_t *)PARAM_INT(0);
    if (handle) {
        osip_alert_info_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_alert_info_parse, 2)
    T_HANDLE(osip_alert_info_parse, 0)
    T_STRING(osip_alert_info_parse, 1)
    osip_alert_info_t * handle = (osip_alert_info_t *)PARAM_INT(0);
    int res = osip_alert_info_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_alert_info_clone, 2)
    T_HANDLE(osip_alert_info_clone, 0)
    osip_alert_info_t * handle = (osip_alert_info_t *)PARAM_INT(0);
    osip_alert_info_t *clone = 0;
    int res = osip_alert_info_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_allow_init, 1)
    osip_allow_t * handle = 0;
    int res = osip_allow_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_allow_free, 1)
    T_NUMBER(osip_allow_free, 0)
    osip_allow_t * handle = (osip_allow_t *)PARAM_INT(0);
    if (handle) {
        osip_allow_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_allow_parse, 2)
    T_HANDLE(osip_allow_parse, 0)
    T_STRING(osip_allow_parse, 1)
    osip_allow_t * handle = (osip_allow_t *)PARAM_INT(0);
    int res = osip_allow_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_allow_clone, 2)
    T_HANDLE(osip_allow_clone, 0)
    osip_allow_t * handle = (osip_allow_t *)PARAM_INT(0);
    osip_allow_t *clone = 0;
    int          res    = osip_allow_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_init, 1)
    osip_authentication_info_t * handle = 0;
    int res = osip_authentication_info_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_free, 1)
    T_NUMBER(osip_authentication_info_free, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    if (handle) {
        osip_authentication_info_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_parse, 2)
    T_HANDLE(osip_authentication_info_parse, 0)
    T_STRING(osip_authentication_info_parse, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    int res = osip_authentication_info_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_clone, 2)
    T_HANDLE(osip_authentication_info_clone, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_t *clone = 0;
    int res = osip_authentication_info_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_init, 1)
    osip_authorization_t * handle = 0;
    int res = osip_authorization_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_free, 1)
    T_NUMBER(osip_authorization_free, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    if (handle) {
        osip_authorization_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_parse, 2)
    T_HANDLE(osip_authorization_parse, 0)
    T_STRING(osip_authorization_parse, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    int res = osip_authorization_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_clone, 2)
    T_HANDLE(osip_authorization_clone, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_t *clone = 0;
    int res = osip_authorization_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_init, 1)
    osip_contact_t * handle = 0;
    int res = osip_contact_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_free, 1)
    T_NUMBER(osip_contact_free, 0)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    if (handle) {
        osip_contact_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_parse, 2)
    T_HANDLE(osip_contact_parse, 0)
    T_STRING(osip_contact_parse, 1)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    int res = osip_contact_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_clone, 2)
    T_HANDLE(osip_contact_clone, 0)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    osip_contact_t *clone = 0;
    int            res    = osip_contact_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_disposition_init, 1)
    osip_content_disposition_t * handle = 0;
    int res = osip_content_disposition_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_disposition_free, 1)
    T_NUMBER(osip_content_disposition_free, 0)
    osip_content_disposition_t * handle = (osip_content_disposition_t *)PARAM_INT(0);
    if (handle) {
        osip_content_disposition_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_disposition_parse, 2)
    T_HANDLE(osip_content_disposition_parse, 0)
    T_STRING(osip_content_disposition_parse, 1)
    osip_content_disposition_t * handle = (osip_content_disposition_t *)PARAM_INT(0);
    int res = osip_content_disposition_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_disposition_clone, 2)
    T_HANDLE(osip_content_disposition_clone, 0)
    osip_content_disposition_t * handle = (osip_content_disposition_t *)PARAM_INT(0);
    osip_content_disposition_t *clone = 0;
    int res = osip_content_disposition_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_encoding_init, 1)
    osip_content_encoding_t * handle = 0;
    int res = osip_content_encoding_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_encoding_free, 1)
    T_NUMBER(osip_content_encoding_free, 0)
    osip_content_encoding_t * handle = (osip_content_encoding_t *)PARAM_INT(0);
    if (handle) {
        osip_content_encoding_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_encoding_parse, 2)
    T_HANDLE(osip_content_encoding_parse, 0)
    T_STRING(osip_content_encoding_parse, 1)
    osip_content_encoding_t * handle = (osip_content_encoding_t *)PARAM_INT(0);
    int res = osip_content_encoding_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_encoding_clone, 2)
    T_HANDLE(osip_content_encoding_clone, 0)
    osip_content_encoding_t * handle = (osip_content_encoding_t *)PARAM_INT(0);
    osip_content_encoding_t *clone = 0;
    int res = osip_content_encoding_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_length_init, 1)
    osip_content_length_t * handle = 0;
    int res = osip_content_length_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_length_free, 1)
    T_NUMBER(osip_content_length_free, 0)
    osip_content_length_t * handle = (osip_content_length_t *)PARAM_INT(0);
    if (handle) {
        osip_content_length_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_length_parse, 2)
    T_HANDLE(osip_content_length_parse, 0)
    T_STRING(osip_content_length_parse, 1)
    osip_content_length_t * handle = (osip_content_length_t *)PARAM_INT(0);
    int res = osip_content_length_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_length_clone, 2)
    T_HANDLE(osip_content_length_clone, 0)
    osip_content_length_t * handle = (osip_content_length_t *)PARAM_INT(0);
    osip_content_length_t *clone = 0;
    int res = osip_content_length_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_type_init, 1)
    osip_content_type_t * handle = 0;
    int res = osip_content_type_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_type_free, 1)
    T_NUMBER(osip_content_type_free, 0)
    osip_content_type_t * handle = (osip_content_type_t *)PARAM_INT(0);
    if (handle) {
        osip_content_type_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_type_parse, 2)
    T_HANDLE(osip_content_type_parse, 0)
    T_STRING(osip_content_type_parse, 1)
    osip_content_type_t * handle = (osip_content_type_t *)PARAM_INT(0);
    int res = osip_content_type_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_content_type_clone, 2)
    T_HANDLE(osip_content_type_clone, 0)
    osip_content_type_t * handle = (osip_content_type_t *)PARAM_INT(0);
    osip_content_type_t *clone = 0;
    int res = osip_content_type_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_init, 1)
    osip_cseq_t * handle = 0;
    int res = osip_cseq_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_message, 2)
    T_HANDLE(osip_cseq_message, 1)
    osip_message_t * msg = (osip_message_t *)PARAM_INT(1);
    int res = osip_cseq_init(&msg->cseq);
    SET_NUMBER(0, (SYS_INT)msg->cseq);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_message, 2)
    T_HANDLE(osip_call_id_message, 1)
    osip_message_t * msg = (osip_message_t *)PARAM_INT(1);
    int res = osip_call_id_init(&msg->call_id);
    SET_NUMBER(0, (SYS_INT)msg->call_id);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_free, 1)
    T_NUMBER(osip_cseq_free, 0)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    if (handle) {
        osip_cseq_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_parse, 2)
    T_HANDLE(osip_cseq_parse, 0)
    T_STRING(osip_cseq_parse, 1)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    int res = osip_cseq_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_clone, 2)
    T_HANDLE(osip_cseq_clone, 0)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    osip_cseq_t *clone = 0;
    int         res    = osip_cseq_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_match, 2)
    T_HANDLE(osip_cseq_match, 0)
    T_HANDLE(osip_cseq_match, 1)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    osip_cseq_t *handle2 = (osip_cseq_t *)PARAM_INT(1);
    int         res      = osip_cseq_match(handle, handle2);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_error_info_init, 1)
    osip_error_info_t * handle = 0;
    int res = osip_error_info_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_error_info_free, 1)
    T_NUMBER(osip_error_info_free, 0)
    osip_error_info_t * handle = (osip_error_info_t *)PARAM_INT(0);
    if (handle) {
        osip_error_info_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_error_info_parse, 2)
    T_HANDLE(osip_error_info_parse, 0)
    T_STRING(osip_error_info_parse, 1)
    osip_error_info_t * handle = (osip_error_info_t *)PARAM_INT(0);
    int res = osip_error_info_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_error_info_clone, 2)
    T_HANDLE(osip_error_info_clone, 0)
    osip_error_info_t * handle = (osip_error_info_t *)PARAM_INT(0);
    osip_error_info_t *clone = 0;
    int res = osip_error_info_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_header_init, 1)
    osip_header_t * handle = 0;
    int res = osip_header_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_header_free, 1)
    T_NUMBER(osip_header_free, 0)
    osip_header_t * handle = (osip_header_t *)PARAM_INT(0);
    if (handle) {
        osip_header_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_header_clone, 2)
    T_HANDLE(osip_header_clone, 0)
    osip_header_t * handle = (osip_header_t *)PARAM_INT(0);
    osip_header_t *clone = 0;
    int           res    = osip_header_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_mime_version_init, 1)
    osip_mime_version_t * handle = 0;
    int res = osip_mime_version_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_mime_version_free, 1)
    T_NUMBER(osip_mime_version_free, 0)
    osip_mime_version_t * handle = (osip_mime_version_t *)PARAM_INT(0);
    if (handle) {
        osip_mime_version_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_mime_version_parse, 2)
    T_HANDLE(osip_mime_version_parse, 0)
    T_STRING(osip_mime_version_parse, 1)
    osip_mime_version_t * handle = (osip_mime_version_t *)PARAM_INT(0);
    int res = osip_mime_version_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_mime_version_clone, 2)
    T_HANDLE(osip_mime_version_clone, 0)
    osip_mime_version_t * handle = (osip_mime_version_t *)PARAM_INT(0);
    osip_mime_version_t *clone = 0;
    int res = osip_mime_version_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_init, 1)
    osip_proxy_authenticate_t * handle = 0;
    int res = osip_proxy_authenticate_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_free, 1)
    T_NUMBER(osip_proxy_authenticate_free, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    if (handle) {
        osip_proxy_authenticate_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_parse, 2)
    T_HANDLE(osip_proxy_authenticate_parse, 0)
    T_STRING(osip_proxy_authenticate_parse, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    int res = osip_proxy_authenticate_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_clone, 2)
    T_HANDLE(osip_proxy_authenticate_clone, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_t *clone = 0;
    int res = osip_proxy_authenticate_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authentication_info_init, 1)
    osip_proxy_authentication_info_t * handle = 0;
    int res = osip_proxy_authentication_info_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authentication_info_free, 1)
    T_NUMBER(osip_proxy_authentication_info_free, 0)
    osip_proxy_authentication_info_t * handle = (osip_proxy_authentication_info_t *)PARAM_INT(0);
    if (handle) {
        osip_proxy_authentication_info_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authentication_info_parse, 2)
    T_HANDLE(osip_proxy_authentication_info_parse, 0)
    T_STRING(osip_proxy_authentication_info_parse, 1)
    osip_proxy_authentication_info_t * handle = (osip_proxy_authentication_info_t *)PARAM_INT(0);
    int res = osip_proxy_authentication_info_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authentication_info_clone, 2)
    T_HANDLE(osip_proxy_authentication_info_clone, 0)
    osip_proxy_authentication_info_t * handle = (osip_proxy_authentication_info_t *)PARAM_INT(0);
    osip_proxy_authentication_info_t *clone = 0;
    int res = osip_proxy_authentication_info_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_init, 1)
    osip_proxy_authorization_t * handle = 0;
    int res = osip_proxy_authorization_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_free, 1)
    T_NUMBER(osip_proxy_authorization_free, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    if (handle) {
        osip_proxy_authorization_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_parse, 2)
    T_HANDLE(osip_proxy_authorization_parse, 0)
    T_STRING(osip_proxy_authorization_parse, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    int res = osip_proxy_authorization_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_clone, 2)
    T_HANDLE(osip_proxy_authorization_clone, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_t *clone = 0;
    int res = osip_proxy_authorization_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_record_route_init, 1)
    osip_record_route_t * handle = 0;
    int res = osip_record_route_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_record_route_free, 1)
    T_NUMBER(osip_record_route_free, 0)
    osip_record_route_t * handle = (osip_record_route_t *)PARAM_INT(0);
    if (handle) {
        osip_record_route_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_record_route_parse, 2)
    T_HANDLE(osip_record_route_parse, 0)
    T_STRING(osip_record_route_parse, 1)
    osip_record_route_t * handle = (osip_record_route_t *)PARAM_INT(0);
    int res = osip_record_route_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_record_route_clone, 2)
    T_HANDLE(osip_record_route_clone, 0)
    osip_record_route_t * handle = (osip_record_route_t *)PARAM_INT(0);
    osip_record_route_t *clone = 0;
    int res = osip_record_route_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_route_init, 1)
    osip_route_t * handle = 0;
    int res = osip_route_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_route_free, 1)
    T_NUMBER(osip_route_free, 0)
    osip_route_t * handle = (osip_route_t *)PARAM_INT(0);
    if (handle) {
        osip_route_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_route_parse, 2)
    T_HANDLE(osip_route_parse, 0)
    T_STRING(osip_route_parse, 1)
    osip_route_t * handle = (osip_route_t *)PARAM_INT(0);
    int res = osip_route_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_route_clone, 2)
    T_HANDLE(osip_route_clone, 0)
    osip_route_t * handle = (osip_route_t *)PARAM_INT(0);
    osip_route_t *clone = 0;
    int          res    = osip_route_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_www_authenticate_init, 1)
    osip_www_authenticate_t * handle = 0;
    int res = osip_www_authenticate_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_www_authenticate_free, 1)
    T_NUMBER(osip_www_authenticate_free, 0)
    osip_www_authenticate_t * handle = (osip_www_authenticate_t *)PARAM_INT(0);
    if (handle) {
        osip_www_authenticate_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_www_authenticate_parse, 2)
    T_HANDLE(osip_www_authenticate_parse, 0)
    T_STRING(osip_www_authenticate_parse, 1)
    osip_www_authenticate_t * handle = (osip_www_authenticate_t *)PARAM_INT(0);
    int res = osip_www_authenticate_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_www_authenticate_clone, 2)
    T_HANDLE(osip_www_authenticate_clone, 0)
    osip_www_authenticate_t * handle = (osip_www_authenticate_t *)PARAM_INT(0);
    osip_www_authenticate_t *clone = 0;
    int res = osip_www_authenticate_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_init, 1)
    sdp_message_t * handle = 0;
    int res = sdp_message_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_free, 1)
    T_NUMBER(sdp_message_free, 0)
    sdp_message_t * handle = (sdp_message_t *)PARAM_INT(0);
    if (handle) {
        sdp_message_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_parse, 2)
    T_HANDLE(sdp_message_parse, 0)
    T_STRING(sdp_message_parse, 1)
    sdp_message_t * handle = (sdp_message_t *)PARAM_INT(0);
    int res = sdp_message_parse(handle, PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_clone, 2)
    T_HANDLE(sdp_message_clone, 0)
    sdp_message_t * handle = (sdp_message_t *)PARAM_INT(0);
    sdp_message_t *clone = 0;
    int           res    = sdp_message_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_to_str, 2)
    T_HANDLE(sdp_message_to_str, 0);
    SET_STRING(1, "");
    sdp_message_t *to    = (sdp_message_t *)PARAM_INT(0);
    char          *msgP  = 0;
    size_t        msgLen = 0;
    int           res    = sdp_message_to_str(to, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_set_host, 2)
    T_HANDLE(osip_call_id_set_host, 0)
    T_STRING(osip_call_id_set_host, 1)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    osip_call_id_set_host(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_get_host, 1)
    T_HANDLE(osip_call_id_get_host, 0)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    char *res = osip_call_id_get_host(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_set_number, 2)
    T_HANDLE(osip_call_id_set_number, 0)
    T_STRING(osip_call_id_set_number, 1)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    osip_call_id_set_number(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_get_number, 1)
    T_HANDLE(osip_call_id_get_number, 0)
    osip_call_id_t * handle = (osip_call_id_t *)PARAM_INT(0);
    char *res = osip_call_id_get_number(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_set_number, 2)
    T_HANDLE(osip_cseq_set_number, 0)
    T_STRING(osip_cseq_set_number, 1)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    osip_cseq_set_number(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_get_number, 1)
    T_HANDLE(osip_cseq_get_number, 0)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    char *res = osip_cseq_get_number(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_set_method, 2)
    T_HANDLE(osip_cseq_set_method, 0)
    T_STRING(osip_cseq_set_method, 1)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    osip_cseq_set_method(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_get_method, 1)
    T_HANDLE(osip_cseq_get_method, 0)
    osip_cseq_t * handle = (osip_cseq_t *)PARAM_INT(0);
    char *res = osip_cseq_get_method(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_set_displayname, 2)
    T_HANDLE(osip_contact_set_displayname, 0)
    T_STRING(osip_contact_set_displayname, 1)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    osip_contact_set_displayname(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_get_displayname, 1)
    T_HANDLE(osip_contact_get_displayname, 0)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    char *res = osip_contact_get_displayname(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_param_add, 3)
    T_HANDLE(osip_contact_param_add, 0)
    T_STRING(osip_contact_param_add, 1)
    T_STRING(osip_contact_param_add, 2)
    osip_contact_t * handle = (osip_contact_t *)PARAM_INT(0);
    int res = osip_contact_param_add(handle, osip_strdup(PARAM(1)), osip_strdup(PARAM(2)));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_attribute_init, 1)
    sdp_attribute_t * handle = 0;
    int res = sdp_attribute_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_attribute_free, 1)
    T_NUMBER(sdp_attribute_free, 0)
    sdp_attribute_t * handle = (sdp_attribute_t *)PARAM_INT(0);
    if (handle) {
        sdp_attribute_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_connection_init, 1)
    sdp_connection_t * handle = 0;
    int res = sdp_connection_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_connection_free, 1)
    T_NUMBER(sdp_connection_free, 0)
    sdp_connection_t * handle = (sdp_connection_t *)PARAM_INT(0);
    if (handle) {
        sdp_connection_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_media_init, 1)
    sdp_media_t * handle = 0;
    int res = sdp_media_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_media_free, 1)
    T_NUMBER(sdp_media_free, 0)
    sdp_media_t * handle = (sdp_media_t *)PARAM_INT(0);
    if (handle) {
        sdp_media_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_v_version_set, 2)
    T_HANDLE(sdp_message_v_version_set, 0)     // sdp_message_t*
    T_STRING(sdp_message_v_version_set, 1)     // char*

    RETURN_NUMBER(sdp_message_v_version_set((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_v_version_get, 1)
    T_HANDLE(sdp_message_v_version_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_v_version_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_origin_set, 7)
    T_HANDLE(sdp_message_o_origin_set, 0)     // sdp_message_t*
    T_STRING(sdp_message_o_origin_set, 1)     // char*
    T_STRING(sdp_message_o_origin_set, 2)     // char*
    T_STRING(sdp_message_o_origin_set, 3)     // char*
    T_STRING(sdp_message_o_origin_set, 4)     // char*
    T_STRING(sdp_message_o_origin_set, 5)     // char*
    T_STRING(sdp_message_o_origin_set, 6)     // char*

    RETURN_NUMBER(sdp_message_o_origin_set((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1)), osip_strdup(PARAM(2)), osip_strdup(PARAM(3)), osip_strdup(PARAM(4)), osip_strdup(PARAM(5)), osip_strdup(PARAM(6))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_username_get, 1)
    T_HANDLE(sdp_message_o_username_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_username_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_sess_id_get, 1)
    T_HANDLE(sdp_message_o_sess_id_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_sess_id_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_sess_version_get, 1)
    T_HANDLE(sdp_message_o_sess_version_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_sess_version_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_nettype_get, 1)
    T_HANDLE(sdp_message_o_nettype_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_nettype_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_addrtype_get, 1)
    T_HANDLE(sdp_message_o_addrtype_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_addrtype_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_o_addr_get, 1)
    T_HANDLE(sdp_message_o_addr_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_o_addr_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_s_name_set, 2)
    T_HANDLE(sdp_message_s_name_set, 0)     // sdp_message_t*
    T_STRING(sdp_message_s_name_set, 1)     // char*

    RETURN_NUMBER(sdp_message_s_name_set((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_s_name_get, 1)
    T_HANDLE(sdp_message_s_name_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_s_name_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_i_info_set, 3)
    T_HANDLE(sdp_message_i_info_set, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_i_info_set, 1)     // int
    T_STRING(sdp_message_i_info_set, 2)     // char*

    RETURN_NUMBER(sdp_message_i_info_set((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_i_info_get, 2)
    T_HANDLE(sdp_message_i_info_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_i_info_get, 1)     // int

    RETURN_STRING((char *)sdp_message_i_info_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_u_uri_set, 2)
    T_HANDLE(sdp_message_u_uri_set, 0)     // sdp_message_t*
    T_STRING(sdp_message_u_uri_set, 1)     // char*

    RETURN_NUMBER(sdp_message_u_uri_set((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_u_uri_get, 1)
    T_HANDLE(sdp_message_u_uri_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_u_uri_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_e_email_add, 2)
    T_HANDLE(sdp_message_e_email_add, 0)     // sdp_message_t*
    T_STRING(sdp_message_e_email_add, 1)     // char*

    RETURN_NUMBER(sdp_message_e_email_add((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_e_email_get, 2)
    T_HANDLE(sdp_message_e_email_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_e_email_get, 1)     // int

    RETURN_STRING((char *)sdp_message_e_email_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_p_phone_add, 2)
    T_HANDLE(sdp_message_p_phone_add, 0)     // sdp_message_t*
    T_STRING(sdp_message_p_phone_add, 1)     // char*

    RETURN_NUMBER(sdp_message_p_phone_add((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_p_phone_get, 2)
    T_HANDLE(sdp_message_p_phone_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_p_phone_get, 1)     // int

    RETURN_STRING((char *)sdp_message_p_phone_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_connection_add, 7)
    T_HANDLE(sdp_message_c_connection_add, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_connection_add, 1)     // int
    T_STRING(sdp_message_c_connection_add, 2)     // char*
    T_STRING(sdp_message_c_connection_add, 3)     // char*
    T_STRING(sdp_message_c_connection_add, 4)     // char*
    T_STRING(sdp_message_c_connection_add, 5)     // char*
    T_STRING(sdp_message_c_connection_add, 6)     // char*

    RETURN_NUMBER(sdp_message_c_connection_add((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), det_osip_strdup(PARAM(2)), det_osip_strdup(PARAM(3)), det_osip_strdup(PARAM(4)), det_osip_strdup(PARAM(5)), det_osip_strdup(PARAM(6))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_nettype_get, 3)
    T_HANDLE(sdp_message_c_nettype_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_nettype_get, 1)     // int
    T_NUMBER(sdp_message_c_nettype_get, 2)     // int

    RETURN_STRING((char *)sdp_message_c_nettype_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_addrtype_get, 3)
    T_HANDLE(sdp_message_c_addrtype_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_addrtype_get, 1)     // int
    T_NUMBER(sdp_message_c_addrtype_get, 2)     // int

    RETURN_STRING((char *)sdp_message_c_addrtype_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_addr_get, 3)
    T_HANDLE(sdp_message_c_addr_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_addr_get, 1)     // int
    T_NUMBER(sdp_message_c_addr_get, 2)     // int

    RETURN_STRING((char *)sdp_message_c_addr_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_addr_multicast_ttl_get, 3)
    T_HANDLE(sdp_message_c_addr_multicast_ttl_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_addr_multicast_ttl_get, 1)     // int
    T_NUMBER(sdp_message_c_addr_multicast_ttl_get, 2)     // int

    RETURN_STRING((char *)sdp_message_c_addr_multicast_ttl_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_c_addr_multicast_int_get, 3)
    T_HANDLE(sdp_message_c_addr_multicast_int_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_c_addr_multicast_int_get, 1)     // int
    T_NUMBER(sdp_message_c_addr_multicast_int_get, 2)     // int

    RETURN_STRING((char *)sdp_message_c_addr_multicast_int_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_b_bandwidth_add, 4)
    T_HANDLE(sdp_message_b_bandwidth_add, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_b_bandwidth_add, 1)     // int
    T_STRING(sdp_message_b_bandwidth_add, 2)     // char*
    T_STRING(sdp_message_b_bandwidth_add, 3)     // char*

    RETURN_NUMBER(sdp_message_b_bandwidth_add((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), det_osip_strdup(PARAM(2)), det_osip_strdup(PARAM(3))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_bandwidth_get, 3)
    T_HANDLE(sdp_message_bandwidth_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_bandwidth_get, 1)     // int
    T_NUMBER(sdp_message_bandwidth_get, 2)     // int

    RETURN_NUMBER((SYS_INT)sdp_message_bandwidth_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_b_bwtype_get, 3)
    T_HANDLE(sdp_message_b_bwtype_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_b_bwtype_get, 1)     // int
    T_NUMBER(sdp_message_b_bwtype_get, 2)     // int

    RETURN_STRING((char *)sdp_message_b_bwtype_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_b_bandwidth_get, 3)
    T_HANDLE(sdp_message_b_bandwidth_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_b_bandwidth_get, 1)     // int
    T_NUMBER(sdp_message_b_bandwidth_get, 2)     // int

    RETURN_STRING((char *)sdp_message_b_bandwidth_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_t_time_descr_add, 3)
    T_HANDLE(sdp_message_t_time_descr_add, 0)     // sdp_message_t*
    T_STRING(sdp_message_t_time_descr_add, 1)     // char*
    T_STRING(sdp_message_t_time_descr_add, 2)     // char*

    RETURN_NUMBER(sdp_message_t_time_descr_add((sdp_message_t *)(long)PARAM(0), det_osip_strdup(PARAM(1)), det_osip_strdup(PARAM(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_t_start_time_get, 2)
    T_HANDLE(sdp_message_t_start_time_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_t_start_time_get, 1)     // int

    RETURN_STRING((char *)sdp_message_t_start_time_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_t_stop_time_get, 2)
    T_HANDLE(sdp_message_t_stop_time_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_t_stop_time_get, 1)     // int

    RETURN_STRING((char *)sdp_message_t_stop_time_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_r_repeat_add, 3)
    T_HANDLE(sdp_message_r_repeat_add, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_r_repeat_add, 1)     // int
    T_STRING(sdp_message_r_repeat_add, 2)     // char*

    RETURN_NUMBER(sdp_message_r_repeat_add((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_r_repeat_get, 3)
    T_HANDLE(sdp_message_r_repeat_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_r_repeat_get, 1)     // int
    T_NUMBER(sdp_message_r_repeat_get, 2)     // int

    RETURN_STRING((char *)sdp_message_r_repeat_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_z_adjustments_set, 2)
    T_HANDLE(sdp_message_z_adjustments_set, 0)     // sdp_message_t*
    T_STRING(sdp_message_z_adjustments_set, 1)     // char*

    RETURN_NUMBER(sdp_message_z_adjustments_set((sdp_message_t *)(long)PARAM(0), osip_strdup(PARAM(1))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_z_adjustments_get, 1)
    T_HANDLE(sdp_message_z_adjustments_get, 0)     // sdp_message_t*

    RETURN_STRING((char *)sdp_message_z_adjustments_get((sdp_message_t *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_k_key_set, 4)
    T_HANDLE(sdp_message_k_key_set, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_k_key_set, 1)     // int
    T_STRING(sdp_message_k_key_set, 2)     // char*
    T_STRING(sdp_message_k_key_set, 3)     // char*

    RETURN_NUMBER(sdp_message_k_key_set((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2)), osip_strdup(PARAM(3))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_k_keytype_get, 2)
    T_HANDLE(sdp_message_k_keytype_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_k_keytype_get, 1)     // int

    RETURN_STRING((char *)sdp_message_k_keytype_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_k_keydata_get, 2)
    T_HANDLE(sdp_message_k_keydata_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_k_keydata_get, 1)     // int

    RETURN_STRING((char *)sdp_message_k_keydata_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_a_attribute_add, 4)
    T_HANDLE(sdp_message_a_attribute_add, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_a_attribute_add, 1)     // int
    T_STRING(sdp_message_a_attribute_add, 2)     // char*
    T_STRING(sdp_message_a_attribute_add, 3)     // char*

    RETURN_NUMBER(sdp_message_a_attribute_add((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2)), osip_strdup(PARAM(3))))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(sdp_message_a_attribute_del,3)
        T_HANDLE(sdp_message_a_attribute_add, 0) // sdp_message_t*
        T_NUMBER(sdp_message_a_attribute_add, 1) // int
        T_STRING(sdp_message_a_attribute_add, 2) // char*

        RETURN_NUMBER(sdp_message_a_attribute_del((sdp_message_t*)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2))))
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(sdp_message_a_attribute_del_at_index,4)
        T_HANDLE(sdp_message_a_attribute_del_at_index, 0) // sdp_message_t*
        T_NUMBER(sdp_message_a_attribute_del_at_index, 1) // int
        T_STRING(sdp_message_a_attribute_del_at_index, 2) // char*
        T_NUMBER(sdp_message_a_attribute_del_at_index, 3) // int

        RETURN_NUMBER(sdp_message_a_attribute_del_at_index((sdp_message_t*)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2)), (int)PARAM(3)))
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_attribute_get, 3)
    T_HANDLE(sdp_message_attribute_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_attribute_get, 1)     // int
    T_NUMBER(sdp_message_attribute_get, 2)     // int

    RETURN_NUMBER((SYS_INT)sdp_message_attribute_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_a_att_field_get, 3)
    T_HANDLE(sdp_message_a_att_field_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_a_att_field_get, 1)     // int
    T_NUMBER(sdp_message_a_att_field_get, 2)     // int

    RETURN_STRING((char *)sdp_message_a_att_field_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_a_att_value_get, 3)
    T_HANDLE(sdp_message_a_att_value_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_a_att_value_get, 1)     // int
    T_NUMBER(sdp_message_a_att_value_get, 2)     // int

    RETURN_STRING((char *)sdp_message_a_att_value_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_endof_media, 2)
    T_HANDLE(sdp_message_endof_media, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_endof_media, 1)     // int

    RETURN_NUMBER(sdp_message_endof_media((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_media_add, 5)
    T_HANDLE(sdp_message_m_media_add, 0)     // sdp_message_t*
    T_STRING(sdp_message_m_media_add, 1)     // char*
    T_STRING(sdp_message_m_media_add, 2)     // char*
    T_STRING(sdp_message_m_media_add, 3)     // char*
    T_STRING(sdp_message_m_media_add, 4)     // char*

    RETURN_NUMBER(sdp_message_m_media_add((sdp_message_t *)(long)PARAM(0), det_osip_strdup(PARAM(1)), det_osip_strdup(PARAM(2)), det_osip_strdup(PARAM(3)), det_osip_strdup(PARAM(4))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_media_get, 2)
    T_HANDLE(sdp_message_m_media_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_media_get, 1)     // int

    RETURN_STRING((char *)sdp_message_m_media_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_port_get, 2)
    T_HANDLE(sdp_message_m_port_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_port_get, 1)     // int

    RETURN_STRING((char *)sdp_message_m_port_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(sdp_message_m_port_set,3)
        T_HANDLE(sdp_message_m_port_get, 0) // sdp_message_t*
        T_NUMBER(sdp_message_m_port_get, 1) // int
        T_STRING(sdp_message_m_port_get, 2) // char*

        RETURN_NUMBER(sdp_message_m_port_set((sdp_message_t*)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2))))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_number_of_port_get, 2)
    T_HANDLE(sdp_message_m_number_of_port_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_number_of_port_get, 1)     // int

    RETURN_STRING((char *)sdp_message_m_number_of_port_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_proto_get, 2)
    T_HANDLE(sdp_message_m_proto_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_proto_get, 1)     // int

    RETURN_STRING((char *)sdp_message_m_proto_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_payload_add, 3)
    T_HANDLE(sdp_message_m_payload_add, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_payload_add, 1)     // int
    T_STRING(sdp_message_m_payload_add, 2)     // char*

    RETURN_NUMBER(sdp_message_m_payload_add((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), osip_strdup(PARAM(2))))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(sdp_message_m_payload_get, 3)
    T_HANDLE(sdp_message_m_payload_get, 0)     // sdp_message_t*
    T_NUMBER(sdp_message_m_payload_get, 1)     // int
    T_NUMBER(sdp_message_m_payload_get, 2)     // int

    RETURN_STRING((char *)sdp_message_m_payload_get((sdp_message_t *)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(sdp_message_m_payload_del,3)
        T_HANDLE(sdp_message_m_payload_get, 0) // sdp_message_t*
        T_NUMBER(sdp_message_m_payload_get, 1) // int
        T_NUMBER(sdp_message_m_payload_get, 2) // int

        RETURN_NUMBER(sdp_message_m_payload_del((sdp_message_t*)(long)PARAM(0), (int)PARAM(1), (int)PARAM(2)))
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_bandwidth_init, 1)
    sdp_bandwidth_t * handle = 0;
    int res = sdp_bandwidth_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_bandwidth_free, 1)
    T_NUMBER(sdp_bandwidth_free, 0)
    sdp_bandwidth_t * handle = (sdp_bandwidth_t *)PARAM_INT(0);
    if (handle) {
        sdp_bandwidth_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_message_describe, 1)
    T_HANDLE(sdp_message_describe, 0)

    sdp_media_t * msg = (sdp_media_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_media", (INTEGER)VARIABLE_STRING, (char *)msg->m_media, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_port", (INTEGER)VARIABLE_STRING, (char *)msg->m_port, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_number_of_port", (INTEGER)VARIABLE_STRING, (char *)msg->m_number_of_port, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_proto", (INTEGER)VARIABLE_STRING, (char *)msg->m_proto, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_payloads", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->m_payloads);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "i_info", (INTEGER)VARIABLE_STRING, (char *)msg->i_info, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_connections", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->c_connections);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "b_bandwidths", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->b_bandwidths);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "a_attributes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->a_attributes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "k_key", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->k_key);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_media_describe, 1)
    T_HANDLE(sdp_media_describe, 0)

    sdp_message_t * msg = (sdp_message_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "v_version", (INTEGER)VARIABLE_STRING, (char *)msg->v_version, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_username", (INTEGER)VARIABLE_STRING, (char *)msg->o_username, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_sess_id", (INTEGER)VARIABLE_STRING, (char *)msg->o_sess_id, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_sess_version", (INTEGER)VARIABLE_STRING, (char *)msg->o_sess_version, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_nettype", (INTEGER)VARIABLE_STRING, (char *)msg->o_nettype, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_addrtype", (INTEGER)VARIABLE_STRING, (char *)msg->o_addrtype, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "o_addr", (INTEGER)VARIABLE_STRING, (char *)msg->o_addr, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "s_name", (INTEGER)VARIABLE_STRING, (char *)msg->s_name, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "i_info", (INTEGER)VARIABLE_STRING, (char *)msg->i_info, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "u_uri", (INTEGER)VARIABLE_STRING, (char *)msg->u_uri, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "e_emails", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->e_emails);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "p_phones", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->p_phones);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_connection", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->c_connection);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "b_bandwidths", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->b_bandwidths);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "t_descrs", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->t_descrs);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "z_adjustments", (INTEGER)VARIABLE_STRING, (char *)msg->z_adjustments, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "k_key", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)msg->k_key);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "a_attributes", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->a_attributes);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "m_medias", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&msg->m_medias);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_attribute_describe, 1)
    T_HANDLE(sdp_attribute_describe, 0)

    sdp_attribute_t * msg = (sdp_attribute_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "a_att_field", (INTEGER)VARIABLE_STRING, (char *)msg->a_att_field, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "a_att_value", (INTEGER)VARIABLE_STRING, (char *)msg->a_att_value, (NUMBER)0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_connection_describe, 1)
    T_HANDLE(sdp_connection_describe, 0)

    sdp_connection_t * msg = (sdp_connection_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_nettype", (INTEGER)VARIABLE_STRING, (char *)msg->c_nettype, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_addrtype", (INTEGER)VARIABLE_STRING, (char *)msg->c_addrtype, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_addr", (INTEGER)VARIABLE_STRING, (char *)msg->c_addr, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_addr_multicast_ttl", (INTEGER)VARIABLE_STRING, (char *)msg->c_addr_multicast_ttl, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "c_addr_multicast_int", (INTEGER)VARIABLE_STRING, (char *)msg->c_addr_multicast_int, (NUMBER)0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(sdp_bandwidth_describe, 1)
    T_HANDLE(sdp_bandwidth_describe, 0)

    sdp_bandwidth_t * msg = (sdp_bandwidth_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "b_bwtype", (INTEGER)VARIABLE_STRING, (char *)msg->b_bwtype, (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "b_bandwidth", (INTEGER)VARIABLE_STRING, (char *)msg->b_bandwidth, (NUMBER)0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_set_url, 2)
    T_HANDLE(osip_contact_set_url, 0)
    T_HANDLE(osip_contact_set_url, 1)
    osip_from_t * from = (osip_from_t *)PARAM_INT(0);
    osip_uri_t *url = (osip_uri_t *)PARAM_INT(1);
    osip_contact_set_url(from, url);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_get_url, 1)
    T_HANDLE(osip_contact_get_url, 0)
    osip_contact_t * from = (osip_contact_t *)PARAM_INT(0);
    osip_uri_t *url = osip_contact_get_url(from);
    RETURN_NUMBER((SYS_INT)url);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_contact_to_str, 2)
    T_HANDLE(osip_contact_to_str, 0)
    SET_STRING(1, "");

    osip_contact_t *from  = (osip_contact_t *)PARAM_INT(0);
    char           *msgP  = 0;
    size_t         msgLen = 0;
    int            res    = osip_contact_to_str(from, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_init, 1)
    osip_body_t * handle = 0;
    int res = osip_body_init(&handle);
    SET_NUMBER(0, (SYS_INT)handle);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_free, 1)
    T_NUMBER(osip_body_free, 0)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    if (handle) {
        osip_body_free(handle);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_parse, 2)
    T_HANDLE(osip_body_parse, 0)
    T_STRING(osip_body_parse, 1)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    int res = osip_body_parse(handle, PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_parse_mime, 2)
    T_HANDLE(osip_body_parse_mime, 0)
    T_STRING(osip_body_parse_mime, 1)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    int res = osip_body_parse_mime(handle, PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_clone, 2)
    T_HANDLE(osip_body_clone, 0)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    osip_body_t *clone = 0;
    int         res    = osip_body_clone(handle, &clone);
    SET_NUMBER(1, (SYS_INT)clone);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_to_str, 2)
    T_HANDLE(osip_body_to_str, 0);
    SET_STRING(1, "");
    osip_body_t *to    = (osip_body_t *)PARAM_INT(0);
    char        *msgP  = 0;
    size_t      msgLen = 0;
    int         res    = osip_body_to_str(to, &msgP, &msgLen);
    if (msgP) {
        if (msgLen > 0) {
            SET_BUFFER(1, msgP, msgLen);
        }
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_set_header, 3)
    T_HANDLE(osip_body_set_header, 0)
    T_STRING(osip_body_set_header, 1)
    T_STRING(osip_body_set_header, 2)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    int res = osip_body_set_header(handle, osip_strdup(PARAM(1)), osip_strdup(PARAM(2)));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_body_set_contenttype, 2)
    T_HANDLE(osip_body_set_contenttype, 0)
    T_STRING(osip_body_set_contenttype, 1)
    osip_body_t * handle = (osip_body_t *)PARAM_INT(0);
    int res = osip_body_set_contenttype(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_auth_type, 2)
    T_HANDLE(osip_proxy_authenticate_set_auth_type, 0)
    T_STRING(osip_proxy_authenticate_set_auth_type, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_auth_type(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_auth_type, 1)
    T_HANDLE(osip_proxy_authenticate_get_auth_type, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_auth_type(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_realm, 2)
    T_HANDLE(osip_proxy_authenticate_set_realm, 0)
    T_STRING(osip_proxy_authenticate_set_realm, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_realm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_realm, 1)
    T_HANDLE(osip_proxy_authenticate_get_realm, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_realm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_domain, 2)
    T_HANDLE(osip_proxy_authenticate_set_domain, 0)
    T_STRING(osip_proxy_authenticate_set_domain, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_domain(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_domain, 1)
    T_HANDLE(osip_proxy_authenticate_get_domain, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_domain(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_nonce, 2)
    T_HANDLE(osip_proxy_authenticate_set_nonce, 0)
    T_STRING(osip_proxy_authenticate_set_nonce, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_nonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_nonce, 1)
    T_HANDLE(osip_proxy_authenticate_get_nonce, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_nonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_opaque, 2)
    T_HANDLE(osip_proxy_authenticate_set_opaque, 0)
    T_STRING(osip_proxy_authenticate_set_opaque, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_opaque(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_opaque, 1)
    T_HANDLE(osip_proxy_authenticate_get_opaque, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_opaque(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_stale, 2)
    T_HANDLE(osip_proxy_authenticate_set_stale, 0)
    T_STRING(osip_proxy_authenticate_set_stale, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_stale(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_stale, 1)
    T_HANDLE(osip_proxy_authenticate_get_stale, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_stale(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_algorithm, 2)
    T_HANDLE(osip_proxy_authenticate_set_algorithm, 0)
    T_STRING(osip_proxy_authenticate_set_algorithm, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_algorithm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_algorithm, 1)
    T_HANDLE(osip_proxy_authenticate_get_algorithm, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_algorithm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_set_qop_options, 2)
    T_HANDLE(osip_proxy_authenticate_set_qop_options, 0)
    T_STRING(osip_proxy_authenticate_set_qop_options, 1)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    osip_proxy_authenticate_set_qop_options(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authenticate_get_qop_options, 1)
    T_HANDLE(osip_proxy_authenticate_get_qop_options, 0)
    osip_proxy_authenticate_t * handle = (osip_proxy_authenticate_t *)PARAM_INT(0);
    char *res = osip_proxy_authenticate_get_qop_options(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_auth_type, 2)
    T_HANDLE(osip_authorization_set_auth_type, 0)
    T_STRING(osip_authorization_set_auth_type, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_auth_type(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_auth_type, 1)
    T_HANDLE(osip_authorization_get_auth_type, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_auth_type(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_realm, 2)
    T_HANDLE(osip_authorization_set_realm, 0)
    T_STRING(osip_authorization_set_realm, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_realm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_realm, 1)
    T_HANDLE(osip_authorization_get_realm, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_realm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_nonce, 2)
    T_HANDLE(osip_authorization_set_nonce, 0)
    T_STRING(osip_authorization_set_nonce, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_nonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_nonce, 1)
    T_HANDLE(osip_authorization_get_nonce, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_nonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_opaque, 2)
    T_HANDLE(osip_authorization_set_opaque, 0)
    T_STRING(osip_authorization_set_opaque, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_opaque(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_opaque, 1)
    T_HANDLE(osip_authorization_get_opaque, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_opaque(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_algorithm, 2)
    T_HANDLE(osip_authorization_set_algorithm, 0)
    T_STRING(osip_authorization_set_algorithm, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_algorithm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_algorithm, 1)
    T_HANDLE(osip_authorization_get_algorithm, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_algorithm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_username, 2)
    T_HANDLE(osip_authorization_set_username, 0)
    T_STRING(osip_authorization_set_username, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_username(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_username, 1)
    T_HANDLE(osip_authorization_get_username, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_username(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_uri, 2)
    T_HANDLE(osip_authorization_set_uri, 0)
    T_STRING(osip_authorization_set_uri, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_uri(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_uri, 1)
    T_HANDLE(osip_authorization_get_uri, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_uri(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_response, 2)
    T_HANDLE(osip_authorization_set_response, 0)
    T_STRING(osip_authorization_set_response, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_response(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_response, 1)
    T_HANDLE(osip_authorization_get_response, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_response(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_digest, 2)
    T_HANDLE(osip_authorization_set_digest, 0)
    T_STRING(osip_authorization_set_digest, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_digest(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_digest, 1)
    T_HANDLE(osip_authorization_get_digest, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_digest(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_cnonce, 2)
    T_HANDLE(osip_authorization_set_cnonce, 0)
    T_STRING(osip_authorization_set_cnonce, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_cnonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_cnonce, 1)
    T_HANDLE(osip_authorization_get_cnonce, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_cnonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_message_qop, 2)
    T_HANDLE(osip_authorization_set_message_qop, 0)
    T_STRING(osip_authorization_set_message_qop, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_message_qop(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_message_qop, 1)
    T_HANDLE(osip_authorization_get_message_qop, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_message_qop(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_set_nonce_count, 2)
    T_HANDLE(osip_authorization_set_nonce_count, 0)
    T_STRING(osip_authorization_set_nonce_count, 1)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    osip_authorization_set_nonce_count(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authorization_get_nonce_count, 1)
    T_HANDLE(osip_authorization_get_nonce_count, 0)
    osip_authorization_t * handle = (osip_authorization_t *)PARAM_INT(0);
    char *res = osip_authorization_get_nonce_count(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_set_nextnonce, 2)
    T_HANDLE(osip_authentication_info_set_nextnonce, 0)
    T_STRING(osip_authentication_info_set_nextnonce, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_set_nextnonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_get_nextnonce, 1)
    T_HANDLE(osip_authentication_info_get_nextnonce, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    char *res = osip_authentication_info_get_nextnonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_set_cnonce, 2)
    T_HANDLE(osip_authentication_info_set_cnonce, 0)
    T_STRING(osip_authentication_info_set_cnonce, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_set_cnonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_get_cnonce, 1)
    T_HANDLE(osip_authentication_info_get_cnonce, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    char *res = osip_authentication_info_get_cnonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_set_qop_options, 2)
    T_HANDLE(osip_authentication_info_set_qop_options, 0)
    T_STRING(osip_authentication_info_set_qop_options, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_set_qop_options(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_get_qop_options, 1)
    T_HANDLE(osip_authentication_info_get_qop_options, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    char *res = osip_authentication_info_get_qop_options(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_set_rspauth, 2)
    T_HANDLE(osip_authentication_info_set_rspauth, 0)
    T_STRING(osip_authentication_info_set_rspauth, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_set_rspauth(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_get_rspauth, 1)
    T_HANDLE(osip_authentication_info_get_rspauth, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    char *res = osip_authentication_info_get_rspauth(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_set_nonce_count, 2)
    T_HANDLE(osip_authentication_info_set_nonce_count, 0)
    T_STRING(osip_authentication_info_set_nonce_count, 1)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    osip_authentication_info_set_nonce_count(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_authentication_info_get_nonce_count, 1)
    T_HANDLE(osip_authentication_info_get_nonce_count, 0)
    osip_authentication_info_t * handle = (osip_authentication_info_t *)PARAM_INT(0);
    char *res = osip_authentication_info_get_nonce_count(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_auth_type, 2)
    T_HANDLE(osip_proxy_authorization_set_auth_type, 0)
    T_STRING(osip_proxy_authorization_set_auth_type, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_auth_type(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_auth_type, 1)
    T_HANDLE(osip_proxy_authorization_get_auth_type, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_auth_type(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_username, 2)
    T_HANDLE(osip_proxy_authorization_set_username, 0)
    T_STRING(osip_proxy_authorization_set_username, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_username(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_username, 1)
    T_HANDLE(osip_proxy_authorization_get_username, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_username(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_realm, 2)
    T_HANDLE(osip_proxy_authorization_set_realm, 0)
    T_STRING(osip_proxy_authorization_set_realm, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_realm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_realm, 1)
    T_HANDLE(osip_proxy_authorization_get_realm, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_realm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_nonce, 2)
    T_HANDLE(osip_proxy_authorization_set_nonce, 0)
    T_STRING(osip_proxy_authorization_set_nonce, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_nonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_nonce, 1)
    T_HANDLE(osip_proxy_authorization_get_nonce, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_nonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_uri, 2)
    T_HANDLE(osip_proxy_authorization_set_uri, 0)
    T_STRING(osip_proxy_authorization_set_uri, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_uri(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_uri, 1)
    T_HANDLE(osip_proxy_authorization_get_uri, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_uri(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_response, 2)
    T_HANDLE(osip_proxy_authorization_set_response, 0)
    T_STRING(osip_proxy_authorization_set_response, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_response(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_response, 1)
    T_HANDLE(osip_proxy_authorization_get_response, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_response(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_digest, 2)
    T_HANDLE(osip_proxy_authorization_set_digest, 0)
    T_STRING(osip_proxy_authorization_set_digest, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_digest(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_digest, 1)
    T_HANDLE(osip_proxy_authorization_get_digest, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_digest(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_algorithm, 2)
    T_HANDLE(osip_proxy_authorization_set_algorithm, 0)
    T_STRING(osip_proxy_authorization_set_algorithm, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_algorithm(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_algorithm, 1)
    T_HANDLE(osip_proxy_authorization_get_algorithm, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_algorithm(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_cnonce, 2)
    T_HANDLE(osip_proxy_authorization_set_cnonce, 0)
    T_STRING(osip_proxy_authorization_set_cnonce, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_cnonce(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_cnonce, 1)
    T_HANDLE(osip_proxy_authorization_get_cnonce, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_cnonce(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_opaque, 2)
    T_HANDLE(osip_proxy_authorization_set_opaque, 0)
    T_STRING(osip_proxy_authorization_set_opaque, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_opaque(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_opaque, 1)
    T_HANDLE(osip_proxy_authorization_get_opaque, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_opaque(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_message_qop, 2)
    T_HANDLE(osip_proxy_authorization_set_message_qop, 0)
    T_STRING(osip_proxy_authorization_set_message_qop, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_message_qop(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_message_qop, 1)
    T_HANDLE(osip_proxy_authorization_get_message_qop, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_message_qop(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_set_nonce_count, 2)
    T_HANDLE(osip_proxy_authorization_set_nonce_count, 0)
    T_STRING(osip_proxy_authorization_set_nonce_count, 1)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    osip_proxy_authorization_set_nonce_count(handle, osip_strdup(PARAM(1)));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_proxy_authorization_get_nonce_count, 1)
    T_HANDLE(osip_proxy_authorization_get_nonce_count, 0)
    osip_proxy_authorization_t * handle = (osip_proxy_authorization_t *)PARAM_INT(0);
    char *res = osip_proxy_authorization_get_nonce_count(handle);
    RETURN_STRING(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_init_as_uac, 2)
    T_HANDLE(osip_dialog_init_as_uac, 1)
    osip_message_t * request = (osip_message_t *)PARAM_INT(1);
    osip_dialog_t *dialog = 0;
    int           res     = osip_dialog_init_as_uac(&dialog, request);
    SET_NUMBER(0, (SYS_INT)dialog);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_init_as_uac_with_remote_request, 3)
    T_HANDLE(osip_dialog_init_as_uac_with_remote_request, 1)
    T_NUMBER(osip_dialog_init_as_uac_with_remote_request, 2)
    osip_message_t * request = (osip_message_t *)PARAM_INT(1);
    osip_dialog_t *dialog = 0;
    int           res     = osip_dialog_init_as_uac_with_remote_request(&dialog, request, PARAM_INT(2));
    SET_NUMBER(0, (SYS_INT)dialog);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_init_as_uas, 3)
    T_HANDLE(osip_dialog_init_as_uas, 1)
    T_HANDLE(osip_dialog_init_as_uas, 2)
    osip_message_t * request = (osip_message_t *)PARAM_INT(1);
    osip_message_t *request2 = (osip_message_t *)PARAM_INT(2);
    osip_dialog_t  *dialog   = 0;
    int            res       = osip_dialog_init_as_uas(&dialog, request, request2);
    SET_NUMBER(0, (SYS_INT)dialog);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_free, 1)
    T_NUMBER(osip_dialog_free, 0)
    osip_dialog_t * evt = (osip_dialog_t *)PARAM_INT(0);
    if (evt) {
        osip_dialog_free(evt);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_set_state, 2)
    T_HANDLE(osip_dialog_set_state, 0)
    T_NUMBER(osip_dialog_set_state, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_dialog_set_state(dialog, (state_t)PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_update_route_set_as_uas, 2)
    T_HANDLE(osip_dialog_update_route_set_as_uas, 0)
    T_HANDLE(osip_dialog_update_route_set_as_uas, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_update_route_set_as_uas(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_update_osip_cseq_as_uas, 2)
    T_HANDLE(osip_dialog_update_osip_cseq_as_uas, 0)
    T_HANDLE(osip_dialog_update_osip_cseq_as_uas, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_update_osip_cseq_as_uas(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_match_as_uac, 2)
    T_HANDLE(osip_dialog_match_as_uac, 0)
    T_HANDLE(osip_dialog_match_as_uac, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_match_as_uac(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_update_tag_as_uac, 2)
    T_HANDLE(osip_dialog_update_tag_as_uac, 0)
    T_HANDLE(osip_dialog_update_tag_as_uac, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_update_tag_as_uac(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_update_route_set_as_uac, 2)
    T_HANDLE(osip_dialog_update_route_set_as_uac, 0)
    T_HANDLE(osip_dialog_update_route_set_as_uac, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_update_route_set_as_uac(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_dialog_match_as_uas, 2)
    T_HANDLE(osip_dialog_match_as_uas, 0)
    T_HANDLE(osip_dialog_match_as_uas, 1)
    osip_dialog_t * dialog = (osip_dialog_t *)PARAM_INT(0);
    osip_message_t *request = (osip_message_t *)PARAM_INT(1);
    int            res      = osip_dialog_match_as_uas(dialog, request);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//

/*CONCEPT_FUNCTION_IMPL(osip_dialog_is_originator, 1)
    T_HANDLE(osip_dialog_match_as_uas, 0)
    osip_dialog_t *dialog=(osip_dialog_t *)PARAM_INT(0);
    int res=osip_dialog_is_originator(dialog);
    RETURN_NUMBER(res);
   END_IMPL
   //-----------------------------------------------------//
   CONCEPT_FUNCTION_IMPL(osip_dialog_is_callee, 1)
    T_HANDLE(osip_dialog_is_callee, 0)
    osip_dialog_t *dialog=(osip_dialog_t *)PARAM_INT(0);
    int res=osip_dialog_is_callee(dialog);
    RETURN_NUMBER(res);
   END_IMPL*/
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_start_200ok_retransmissions, 4)
    T_HANDLE(osip_start_200ok_retransmissions, 0)
    T_HANDLE(osip_start_200ok_retransmissions, 1)
    T_HANDLE(osip_start_200ok_retransmissions, 2)
    T_NUMBER(osip_start_200ok_retransmissions, 3)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_dialog_t  *dialog   = (osip_dialog_t *)PARAM_INT(1);
    osip_message_t *msg200ok = (osip_message_t *)PARAM_INT(2);
    osip_start_200ok_retransmissions(osip, dialog, msg200ok, PARAM_INT(3));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_start_ack_retransmissions, 6)
    T_HANDLE(osip_start_ack_retransmissions, 0)
    T_HANDLE(osip_start_ack_retransmissions, 1)
    T_HANDLE(osip_start_ack_retransmissions, 2)
    T_STRING(osip_start_ack_retransmissions, 3)
    T_NUMBER(osip_start_ack_retransmissions, 4)
    T_NUMBER(osip_start_ack_retransmissions, 5)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_dialog_t  *dialog = (osip_dialog_t *)PARAM_INT(1);
    osip_message_t *ack    = (osip_message_t *)PARAM_INT(2);
    osip_start_ack_retransmissions(osip, dialog, ack, PARAM(3), PARAM_INT(4), PARAM_INT(5));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_stop_200ok_retransmissions, 2)
    T_HANDLE(osip_stop_200ok_retransmissions, 0)
    T_HANDLE(osip_stop_200ok_retransmissions, 1)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_message_t *ack = (osip_message_t *)PARAM_INT(1);
    osip_dialog    *res = osip_stop_200ok_retransmissions(osip, ack);
    RETURN_NUMBER((SYS_INT)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_stop_retransmissions_from_dialog, 2)
    T_HANDLE(osip_stop_retransmissions_from_dialog, 0)
    T_HANDLE(osip_stop_retransmissions_from_dialog, 1)
    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_dialog_t *dialog = (osip_dialog_t *)PARAM_INT(1);
    osip_stop_retransmissions_from_dialog(osip, dialog);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_new_outgoing_sipmessage, 1)
    T_HANDLE(osip_new_outgoing_sipmessage, 0)
    osip_message_t * request = (osip_message_t *)PARAM_INT(0);
    osip_event_t *evt = osip_new_outgoing_sipmessage(request);
    RETURN_NUMBER((SYS_INT)evt);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_cseq_to_str, 2)
    T_HANDLE(osip_cseq_to_str, 0);
    SET_STRING(1, "");
    osip_cseq_t *to    = (osip_cseq_t *)PARAM_INT(0);
    char        *msgP  = 0;
    size_t      msgLen = 0;
    int         res    = osip_cseq_to_str(to, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_call_id_to_str, 2)
    T_HANDLE(osip_call_id_to_str, 0);
    SET_STRING(1, "");
    osip_call_id_t *to    = (osip_call_id_t *)PARAM_INT(0);
    char           *msgP  = 0;
    size_t         msgLen = 0;
    int            res    = osip_call_id_to_str(to, &msgP);
    if (msgP) {
        SET_STRING(1, msgP);
        osip_free(msgP);
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_message_get_header_str, 3)
    T_HANDLE(osip_message_get_header_str, 0)     // osip_message_t*
    T_STRING(osip_message_get_header_str, 1)
    T_NUMBER(osip_message_get_header_str, 2)

// ... parameter 2 is by reference (osip_header_t**)
    osip_header_t * local_parameter_2 = 0;

    osip_message_header_get_byname((osip_message_t *)(long)PARAM(0), PARAM(1), (int)PARAM(2), &local_parameter_2);
    if (local_parameter_2) {
        char *value = osip_header_get_value(local_parameter_2);
        if (value) {
            RETURN_STRING(value);
        } else {
            RETURN_STRING("");
        }
        osip_header_free(local_parameter_2);
    } else {
        RETURN_STRING("");
    }
//SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(osip_transaction_describe, 1)
    T_HANDLE(osip_transaction_describe, 0)

    osip_transaction_t * tran = (osip_transaction_t *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);

    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "your_instance", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->your_instance);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "transactionid", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->transactionid);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "transactionff", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->transactionff);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "from", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->from);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "to", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->to);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "callid", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->callid);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "cseq", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->cseq);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "orig_request", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->orig_request);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "last_response", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->last_response);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ack", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->ack);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "state", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->state);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "birth_time", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->birth_time);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "completed_time", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->completed_time);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "in_socket", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->in_socket);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "out_socket", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->out_socket);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "config", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->config);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ctx_type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->ctx_type);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ict_context", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->ict_context);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ist_context", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->ist_context);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nict_context", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->nict_context);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "nist_context", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->nist_context);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "record", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)&tran->record);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "naptr_record", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(SYS_INT)tran->naptr_record);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(osip_remove_transaction, 2)
    T_HANDLE(osip_remove_transaction, 0)
    T_HANDLE(osip_remove_transaction, 1)

    osip_t * osip = (osip_t *)PARAM_INT(0);
    osip_transaction_t *tran = (osip_transaction_t *)PARAM_INT(1);

    int res = osip_remove_transaction(osip, tran);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//

