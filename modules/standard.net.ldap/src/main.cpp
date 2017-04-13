//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#define LDAP_DEPRECATED 1
#include <lber.h>
#include <ldap.h>

INVOKE_CALL InvokePtr = 0;


//struct berval sz_value;

#define SET_BERVAL(index, sz_value)         SET_BUFFER(index, sz_value.bv_val, sz_value.bv_len);
#define GET_BERVAL(index, sz_value)         T_STRING(INTERNAL_GETBERVAL, index); sz_value.bv_val = PARAM(index); sz_value.bv_len = PARAM_LEN(index);

#define GET_DECL_BERVAL(index, sz_value)    berval sz_value; GET_BERVAL(index, sz_value)

//-----------------------------------------------------------------------------------
void ReturnCharList(void *PARAMETER, char **list) {
    InvokePtr(INVOKE_CREATE_ARRAY, PARAMETER);
    if (list) {
        int  i  = 0;
        char *e = list[i++];
        while (e) {
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT, PARAMETER, i, (INTEGER)VARIABLE_NUMBER, e, (NUMBER)0);
            e = list[i++];
        }
    }
}

//-----------------------------------------------------------------------------------
void WRAP_timeval(void *RESULT, struct timeval *timeinfo) {
    InvokePtr(INVOKE_CREATE_ARRAY, RESULT);

    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tv_sec", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tv_sec);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tv_usec", (INTEGER)VARIABLE_NUMBER, (char *)0, (double)timeinfo->tv_usec);
}

//-----------------------------------------------------------------------------------
void WRAP_LDAPControl(void *PARAMETER, LDAPControl *ctl) {
    InvokePtr(INVOKE_CREATE_ARRAY, PARAMETER);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "handle", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)ctl);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldctl_oid", (INTEGER)VARIABLE_STRING, (char *)ctl->ldctl_oid, (NUMBER)0);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldctl_value", (INTEGER)VARIABLE_STRING, (char *)ctl->ldctl_value.bv_val, (NUMBER)ctl->ldctl_value.bv_len);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldctl_iscritical", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldctl_iscritical);
}

LDAPControl ctl_wrap;
LDAPControl *UNWRAP_LDAPControl(void *arr) {
    INTEGER     type = 0;
    char        *str = 0;
    NUMBER      nr   = 0;
    LDAPControl *ctl = 0;

    if (!InvokePtr)
        return ctl;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "handle", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        //mytime->tm_hour=nr;
        ctl = (LDAPControl *)(SYS_INT)nr;
    else
        ctl = &ctl_wrap;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldctl_oid", &type, &str, &nr);
    if (type == VARIABLE_STRING)
        ctl->ldctl_oid = str;
    else
        ctl->ldctl_oid = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldctl_value", &type, &str, &nr);
    if (type == VARIABLE_STRING) {
        ctl->ldctl_value.bv_val = str;
        ctl->ldctl_value.bv_len = (int)nr;
    }

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldctl_iscritical", &type, &str, &nr);
    if (type == VARIABLE_NUMBER)
        ctl->ldctl_iscritical = (char)(int)nr;
    else
        ctl->ldctl_iscritical = 0;

    return ctl;
}

//-----------------------------------------------------------------------------------
void WRAP_LDAPVLVInfo(void *PARAMETER, LDAPVLVInfo *ctl) {
    InvokePtr(INVOKE_CREATE_ARRAY, PARAMETER);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "handle", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)ctl);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_version", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_version);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_before_count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_before_count);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_after_count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_after_count);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_offset", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_offset);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_count", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_count);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_version", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)ctl->ldvlv_version);
    if (ctl->ldvlv_attrvalue)
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_attrvalue", (INTEGER)VARIABLE_STRING, (char *)ctl->ldvlv_attrvalue->bv_val, (NUMBER)ctl->ldvlv_attrvalue->bv_len);
    if (ctl->ldvlv_context)
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_context", (INTEGER)VARIABLE_STRING, (char *)ctl->ldvlv_context->bv_val, (NUMBER)ctl->ldvlv_context->bv_len);
    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER, "ldvlv_extradata", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)ctl->ldvlv_extradata);
}

LDAPVLVInfo vlv_wrap;
berval      vlv_context;
berval      vlv_attrvalue;
LDAPVLVInfo *UNWRAP_LDAPVLVInfo(void *arr) {
    INTEGER     type = 0;
    char        *str = 0;
    NUMBER      nr   = 0;
    LDAPVLVInfo *ctl = 0;

    if (!InvokePtr)
        return ctl;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "handle", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl = (LDAPVLVInfo *)(SYS_INT)nr;
    else
        ctl = &vlv_wrap;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_version", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_version = (SYS_INT)nr;
    else
        ctl->ldvlv_version = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_before_count", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_before_count = (SYS_INT)nr;
    else
        ctl->ldvlv_before_count = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_after_count", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_after_count = (SYS_INT)nr;
    else
        ctl->ldvlv_after_count = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_offset", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_offset = (SYS_INT)nr;
    else
        ctl->ldvlv_offset = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_count", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_count = (SYS_INT)nr;
    else
        ctl->ldvlv_count = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_version", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_version = (SYS_INT)nr;
    else
        ctl->ldvlv_version = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_extradata", &type, &str, &nr);
    if ((type == VARIABLE_NUMBER) && (nr))
        ctl->ldvlv_extradata = (void *)(SYS_INT)nr;
    else
        ctl->ldvlv_extradata = 0;

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_attrvalue", &type, &str, &nr);
    if (type == VARIABLE_STRING) {
        ctl->ldvlv_attrvalue         = &vlv_attrvalue;
        ctl->ldvlv_attrvalue->bv_val = str;
        ctl->ldvlv_attrvalue->bv_len = (int)nr;
    }

    InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, "ldvlv_context", &type, &str, &nr);
    if (type == VARIABLE_STRING) {
        ctl->ldvlv_context         = &vlv_context;
        ctl->ldvlv_context->bv_val = str;
        ctl->ldvlv_context->bv_len = (int)nr;
    }

    return ctl;
}

//-----------------------------------------------------------------------------------
CONCEPT_INIT {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(LDAP_VERSION1)
    DEFINE_ECONSTANT(LDAP_VERSION2)
    DEFINE_ECONSTANT(LDAP_VERSION3)
    DEFINE_ECONSTANT(LDAP_VERSION_MIN)
    DEFINE_ECONSTANT(LDAP_VERSION)
    DEFINE_ECONSTANT(LDAP_VERSION_MAX)
    DEFINE_ECONSTANT(LDAP_API_VERSION)
    DEFINE_ECONSTANT(LDAP_VENDOR_NAME)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_X_OPENLDAP)
    DEFINE_ECONSTANT(LDAP_PORT)
    DEFINE_ECONSTANT(LDAPS_PORT)
    DEFINE_ECONSTANT(LDAP_ROOT_DSE)
    DEFINE_ECONSTANT(LDAP_NO_ATTRS)
    DEFINE_ECONSTANT(LDAP_ALL_USER_ATTRIBUTES)
    DEFINE_ECONSTANT(LDAP_ALL_OPERATIONAL_ATTRIBUTES)
    DEFINE_ECONSTANT(LDAP_MAXINT)
    DEFINE_ECONSTANT(LDAP_OPT_API_INFO)
    DEFINE_ECONSTANT(LDAP_OPT_DESC)
    DEFINE_ECONSTANT(LDAP_OPT_DEREF)
    DEFINE_ECONSTANT(LDAP_OPT_SIZELIMIT)
    DEFINE_ECONSTANT(LDAP_OPT_TIMELIMIT)
    DEFINE_ECONSTANT(LDAP_OPT_REFERRALS)
    DEFINE_ECONSTANT(LDAP_OPT_RESTART)
    DEFINE_ECONSTANT(LDAP_OPT_PROTOCOL_VERSION)
    DEFINE_ECONSTANT(LDAP_OPT_SERVER_CONTROLS)
    DEFINE_ECONSTANT(LDAP_OPT_CLIENT_CONTROLS)
    DEFINE_ECONSTANT(LDAP_OPT_API_FEATURE_INFO)
    DEFINE_ECONSTANT(LDAP_OPT_HOST_NAME)
    DEFINE_ECONSTANT(LDAP_OPT_RESULT_CODE)
    DEFINE_ECONSTANT(LDAP_OPT_ERROR_NUMBER)
    DEFINE_ECONSTANT(LDAP_OPT_DIAGNOSTIC_MESSAGE)
    DEFINE_ECONSTANT(LDAP_OPT_ERROR_STRING)
    DEFINE_ECONSTANT(LDAP_OPT_MATCHED_DN)
    DEFINE_ECONSTANT(LDAP_OPT_SSPI_FLAGS)
    DEFINE_ECONSTANT(LDAP_OPT_SIGN)
    DEFINE_ECONSTANT(LDAP_OPT_ENCRYPT)
    DEFINE_ECONSTANT(LDAP_OPT_SASL_METHOD)
    DEFINE_ECONSTANT(LDAP_OPT_SECURITY_CONTEXT)
    DEFINE_ECONSTANT(LDAP_OPT_API_EXTENSION_BASE)
    DEFINE_ECONSTANT(LDAP_OPT_DEBUG_LEVEL)
    DEFINE_ECONSTANT(LDAP_OPT_TIMEOUT)
    DEFINE_ECONSTANT(LDAP_OPT_REFHOPLIMIT)
    DEFINE_ECONSTANT(LDAP_OPT_NETWORK_TIMEOUT)
    DEFINE_ECONSTANT(LDAP_OPT_URI)
    DEFINE_ECONSTANT(LDAP_OPT_REFERRAL_URLS)
    DEFINE_ECONSTANT(LDAP_OPT_SOCKBUF)
    DEFINE_ECONSTANT(LDAP_OPT_DEFBASE)
    DEFINE_ECONSTANT(LDAP_OPT_CONNECT_ASYNC)
    DEFINE_ECONSTANT(LDAP_OPT_CONNECT_CB)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CTX)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CACERTFILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CACERTDIR)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CERTFILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_KEYFILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_REQUIRE_CERT)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_MIN)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CIPHER_SUITE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_RANDOM_FILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_SSL_CTX)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CRLCHECK)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CONNECT_CB)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CONNECT_ARG)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_DHFILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_NEWCTX)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CRLFILE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_NEVER)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_HARD)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_DEMAND)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_ALLOW)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_TRY)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CRL_NONE)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CRL_PEER)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_CRL_ALL)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_SSL2)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_SSL3)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_TLS1_0)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_TLS1_1)
    DEFINE_ECONSTANT(LDAP_OPT_X_TLS_PROTOCOL_TLS1_2)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_MECH)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_REALM)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_AUTHCID)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_AUTHZID)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_SSF)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_SSF_EXTERNAL)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_SECPROPS)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_SSF_MIN)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_SSF_MAX)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_MAXBUFSIZE)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_MECHLIST)
    DEFINE_ECONSTANT(LDAP_OPT_X_SASL_NOCANON)
    DEFINE_ECONSTANT(LDAP_OPT_X_GSSAPI_DO_NOT_FREE_CONTEXT)
    DEFINE_ECONSTANT(LDAP_OPT_X_GSSAPI_ALLOW_REMOTE_PRINCIPAL)
    DEFINE_ECONSTANT(LDAP_OPT_PRIVATE_EXTENSION_BASE)
    DEFINE_ECONSTANT(LDAP_OPT_SUCCESS)
    DEFINE_ECONSTANT(LDAP_OPT_ERROR)
    DEFINE_ECONSTANT(LDAP_OPT_ON)
    DEFINE_ECONSTANT(LDAP_OPT_OFF)
    DEFINE_ECONSTANT(LDAP_API_INFO_VERSION)
    DEFINE_ECONSTANT(LDAP_FEATURE_INFO_VERSION)
    DEFINE_ECONSTANT(LDAP_CONTROL_MANAGEDSAIT)
    DEFINE_ECONSTANT(LDAP_CONTROL_PROXY_AUTHZ)
    DEFINE_ECONSTANT(LDAP_CONTROL_SUBENTRIES)
    DEFINE_ECONSTANT(LDAP_CONTROL_VALUESRETURNFILTER)
    DEFINE_ECONSTANT(LDAP_CONTROL_ASSERT)
    DEFINE_ECONSTANT(LDAP_CONTROL_PRE_READ)
    DEFINE_ECONSTANT(LDAP_CONTROL_POST_READ)
    DEFINE_ECONSTANT(LDAP_CONTROL_SORTREQUEST)
    DEFINE_ECONSTANT(LDAP_CONTROL_SORTRESPONSE)
    DEFINE_ECONSTANT(LDAP_CONTROL_PAGEDRESULTS)
    DEFINE_ECONSTANT(LDAP_SYNC_OID)
    DEFINE_ECONSTANT(LDAP_CONTROL_SYNC)
    DEFINE_ECONSTANT(LDAP_CONTROL_SYNC_STATE)
    DEFINE_ECONSTANT(LDAP_CONTROL_SYNC_DONE)
    DEFINE_ECONSTANT(LDAP_SYNC_INFO)
    DEFINE_ECONSTANT(LDAP_SYNC_NONE)
    DEFINE_ECONSTANT(LDAP_SYNC_REFRESH_ONLY)
    DEFINE_ECONSTANT(LDAP_SYNC_RESERVED)
    DEFINE_ECONSTANT(LDAP_SYNC_REFRESH_AND_PERSIST)
    DEFINE_ECONSTANT(LDAP_SYNC_REFRESH_PRESENTS)
    DEFINE_ECONSTANT(LDAP_SYNC_REFRESH_DELETES)
    DEFINE_ECONSTANT(LDAP_TAG_SYNC_NEW_COOKIE)
    DEFINE_ECONSTANT(LDAP_TAG_SYNC_REFRESH_DELETE)
    DEFINE_ECONSTANT(LDAP_TAG_SYNC_REFRESH_PRESENT)
    DEFINE_ECONSTANT(LDAP_TAG_SYNC_ID_SET)
    DEFINE_ECONSTANT(LDAP_TAG_SYNC_COOKIE)
    DEFINE_ECONSTANT(LDAP_TAG_REFRESHDELETES)
    DEFINE_ECONSTANT(LDAP_TAG_REFRESHDONE)
    DEFINE_ECONSTANT(LDAP_TAG_RELOAD_HINT)
    DEFINE_ECONSTANT(LDAP_SYNC_PRESENT)
    DEFINE_ECONSTANT(LDAP_SYNC_ADD)
    DEFINE_ECONSTANT(LDAP_SYNC_MODIFY)
    DEFINE_ECONSTANT(LDAP_SYNC_DELETE)
    DEFINE_ECONSTANT(LDAP_SYNC_NEW_COOKIE)
    DEFINE_ECONSTANT(LDAP_CONTROL_PASSWORDPOLICYREQUEST)
    DEFINE_ECONSTANT(LDAP_CONTROL_PASSWORDPOLICYRESPONSE)
    DEFINE_ECONSTANT(LDAP_CONTROL_NOOP)
    DEFINE_ECONSTANT(LDAP_CONTROL_NO_SUBORDINATES)
    DEFINE_ECONSTANT(LDAP_CONTROL_RELAX)
    DEFINE_ECONSTANT(LDAP_CONTROL_MANAGEDIT)
    DEFINE_ECONSTANT(LDAP_CONTROL_SLURP)
    DEFINE_ECONSTANT(LDAP_CONTROL_VALSORT)
    DEFINE_ECONSTANT(LDAP_CONTROL_DONTUSECOPY)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_DEREF)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_WHATFAILED)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_CHAINING_BEHAVIOR)
    DEFINE_ECONSTANT(LDAP_CHAINING_PREFERRED)
    DEFINE_ECONSTANT(LDAP_CHAINING_REQUIRED)
    DEFINE_ECONSTANT(LDAP_REFERRALS_PREFERRED)
    DEFINE_ECONSTANT(LDAP_REFERRALS_REQUIRED)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_INCREMENTAL_VALUES)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_DOMAIN_SCOPE)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_PERMISSIVE_MODIFY)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_SEARCH_OPTIONS)
    DEFINE_ECONSTANT(LDAP_SEARCH_FLAG_DOMAIN_SCOPE)
    DEFINE_ECONSTANT(LDAP_SEARCH_FLAG_PHANTOM_ROOT)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_TREE_DELETE)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_EXTENDED_DN)
#ifdef LDAP_DEVEL
    DEFINE_ECONSTANT(LDAP_CONTROL_X_SESSION_TRACKING)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_SESSION_TRACKING_RADIUS_ACCT_SESSION_ID)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_SESSION_TRACKING_RADIUS_ACCT_MULTI_SESSION_ID)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_SESSION_TRACKING_USERNAME)
#endif
    DEFINE_ECONSTANT(LDAP_CONTROL_DUPENT_REQUEST)
    DEFINE_ECONSTANT(LDAP_CONTROL_DUPENT_RESPONSE)
    DEFINE_ECONSTANT(LDAP_CONTROL_DUPENT_ENTRY)
    DEFINE_ECONSTANT(LDAP_CONTROL_DUPENT)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_REQUEST)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_ENTRY_CHANGE_NOTICE)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_ENTRY_CHANGE_ADD)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_ENTRY_CHANGE_DELETE)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_ENTRY_CHANGE_MODIFY)
    DEFINE_ECONSTANT(LDAP_CONTROL_PERSIST_ENTRY_CHANGE_RENAME)
    DEFINE_ECONSTANT(LDAP_CONTROL_VLVREQUEST)
    DEFINE_ECONSTANT(LDAP_CONTROL_VLVRESPONSE)
    DEFINE_ECONSTANT(LDAP_NOTICE_OF_DISCONNECTION)
    DEFINE_ECONSTANT(LDAP_NOTICE_DISCONNECT)
    DEFINE_ECONSTANT(LDAP_EXOP_START_TLS)
    DEFINE_ECONSTANT(LDAP_EXOP_MODIFY_PASSWD)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_MODIFY_PASSWD_ID)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_MODIFY_PASSWD_OLD)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_MODIFY_PASSWD_NEW)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_MODIFY_PASSWD_GEN)
    DEFINE_ECONSTANT(LDAP_EXOP_CANCEL)
    DEFINE_ECONSTANT(LDAP_EXOP_X_CANCEL)
    DEFINE_ECONSTANT(LDAP_EXOP_REFRESH)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_REFRESH_REQ_DN)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_REFRESH_REQ_TTL)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_REFRESH_RES_TTL)
    DEFINE_ECONSTANT(LDAP_EXOP_WHO_AM_I)
    DEFINE_ECONSTANT(LDAP_EXOP_X_WHO_AM_I)
    DEFINE_ECONSTANT(LDAP_EXOP_TURN)
    DEFINE_ECONSTANT(LDAP_EXOP_X_TURN)
    DEFINE_ECONSTANT(LDAP_X_DISTPROC_BASE)
    DEFINE_ECONSTANT(LDAP_EXOP_X_CHAINEDREQUEST)
    DEFINE_ECONSTANT(LDAP_FEATURE_X_CANCHAINOPS)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_RETURNCONTREF)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_LOCALREFOID)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_REFTYPEOID)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_SEARCHEDSUBTREEOID)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_FAILEDNAMEOID)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_LOCALREF)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_REFTYPE)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_SEARCHEDSUBTREE)
    DEFINE_ECONSTANT(LDAP_URLEXT_X_FAILEDNAME)
#ifdef LDAP_DEVEL
    DEFINE_ECONSTANT(LDAP_X_TXN)
    DEFINE_ECONSTANT(LDAP_EXOP_X_TXN_START)
    DEFINE_ECONSTANT(LDAP_CONTROL_X_TXN_SPEC)
    DEFINE_ECONSTANT(LDAP_EXOP_X_TXN_END)
    DEFINE_ECONSTANT(LDAP_EXOP_X_TXN_ABORTED_NOTICE)
#endif
    DEFINE_ECONSTANT(LDAP_FEATURE_ALL_OP_ATTRS)
    DEFINE_ECONSTANT(LDAP_FEATURE_OBJECTCLASS_ATTRS)
    DEFINE_ECONSTANT(LDAP_FEATURE_ABSOLUTE_FILTERS)
    DEFINE_ECONSTANT(LDAP_FEATURE_LANGUAGE_TAG_OPTIONS)
    DEFINE_ECONSTANT(LDAP_FEATURE_LANGUAGE_RANGE_OPTIONS)
    DEFINE_ECONSTANT(LDAP_FEATURE_MODIFY_INCREMENT)
    DEFINE_ECONSTANT(LDAP_FEATURE_SUBORDINATE_SCOPE)
    DEFINE_ECONSTANT(LDAP_FEATURE_CHILDREN_SCOPE)
    DEFINE_ECONSTANT(LDAP_TAG_MESSAGE)
    DEFINE_ECONSTANT(LDAP_TAG_MSGID)
    DEFINE_ECONSTANT(LDAP_TAG_LDAPDN)
    DEFINE_ECONSTANT(LDAP_TAG_LDAPCRED)
    DEFINE_ECONSTANT(LDAP_TAG_CONTROLS)
    DEFINE_ECONSTANT(LDAP_TAG_REFERRAL)
    DEFINE_ECONSTANT(LDAP_TAG_NEWSUPERIOR)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_REQ_OID)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_REQ_VALUE)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_RES_OID)
    DEFINE_ECONSTANT(LDAP_TAG_EXOP_RES_VALUE)
    DEFINE_ECONSTANT(LDAP_TAG_IM_RES_OID)
    DEFINE_ECONSTANT(LDAP_TAG_IM_RES_VALUE)
    DEFINE_ECONSTANT(LDAP_TAG_SASL_RES_CREDS)
    DEFINE_ECONSTANT(LDAP_REQ_BIND)
    DEFINE_ECONSTANT(LDAP_REQ_UNBIND)
    DEFINE_ECONSTANT(LDAP_REQ_SEARCH)
    DEFINE_ECONSTANT(LDAP_REQ_MODIFY)
    DEFINE_ECONSTANT(LDAP_REQ_ADD)
    DEFINE_ECONSTANT(LDAP_REQ_DELETE)
    DEFINE_ECONSTANT(LDAP_REQ_MODDN)
    DEFINE_ECONSTANT(LDAP_REQ_MODRDN)
    DEFINE_ECONSTANT(LDAP_REQ_RENAME)
    DEFINE_ECONSTANT(LDAP_REQ_COMPARE)
    DEFINE_ECONSTANT(LDAP_REQ_ABANDON)
    DEFINE_ECONSTANT(LDAP_REQ_EXTENDED)
    DEFINE_ECONSTANT(LDAP_RES_BIND)
    DEFINE_ECONSTANT(LDAP_RES_SEARCH_ENTRY)
    DEFINE_ECONSTANT(LDAP_RES_SEARCH_REFERENCE)
    DEFINE_ECONSTANT(LDAP_RES_SEARCH_RESULT)
    DEFINE_ECONSTANT(LDAP_RES_MODIFY)
    DEFINE_ECONSTANT(LDAP_RES_ADD)
    DEFINE_ECONSTANT(LDAP_RES_DELETE)
    DEFINE_ECONSTANT(LDAP_RES_MODDN)
    DEFINE_ECONSTANT(LDAP_RES_MODRDN)
    DEFINE_ECONSTANT(LDAP_RES_RENAME)
    DEFINE_ECONSTANT(LDAP_RES_COMPARE)
    DEFINE_ECONSTANT(LDAP_RES_EXTENDED)
    DEFINE_ECONSTANT(LDAP_RES_INTERMEDIATE)
    DEFINE_ECONSTANT(LDAP_RES_ANY)
    DEFINE_ECONSTANT(LDAP_RES_UNSOLICITED)
    DEFINE_ECONSTANT(LDAP_SASL_SIMPLE)
    DEFINE_ECONSTANT(LDAP_SASL_NULL)
    DEFINE_ECONSTANT(LDAP_AUTH_NONE)
    DEFINE_ECONSTANT(LDAP_AUTH_SIMPLE)
    DEFINE_ECONSTANT(LDAP_AUTH_SASL)
    DEFINE_ECONSTANT(LDAP_AUTH_KRBV4)
    DEFINE_ECONSTANT(LDAP_AUTH_KRBV41)
    DEFINE_ECONSTANT(LDAP_AUTH_KRBV42)
    DEFINE_ECONSTANT(LDAP_AUTH_NEGOTIATE)
    DEFINE_ECONSTANT(LDAP_FILTER_AND)
    DEFINE_ECONSTANT(LDAP_FILTER_OR)
    DEFINE_ECONSTANT(LDAP_FILTER_NOT)
    DEFINE_ECONSTANT(LDAP_FILTER_EQUALITY)
    DEFINE_ECONSTANT(LDAP_FILTER_SUBSTRINGS)
    DEFINE_ECONSTANT(LDAP_FILTER_GE)
    DEFINE_ECONSTANT(LDAP_FILTER_LE)
    DEFINE_ECONSTANT(LDAP_FILTER_PRESENT)
    DEFINE_ECONSTANT(LDAP_FILTER_APPROX)
    DEFINE_ECONSTANT(LDAP_FILTER_EXT)
    DEFINE_ECONSTANT(LDAP_FILTER_EXT_OID)
    DEFINE_ECONSTANT(LDAP_FILTER_EXT_TYPE)
    DEFINE_ECONSTANT(LDAP_FILTER_EXT_VALUE)
    DEFINE_ECONSTANT(LDAP_FILTER_EXT_DNATTRS)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_INITIAL)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_ANY)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_FINAL)
    DEFINE_ECONSTANT(LDAP_SCOPE_BASE)
    DEFINE_ECONSTANT(LDAP_SCOPE_BASEOBJECT)
    DEFINE_ECONSTANT(LDAP_SCOPE_ONELEVEL)
    DEFINE_ECONSTANT(LDAP_SCOPE_ONE)
    DEFINE_ECONSTANT(LDAP_SCOPE_SUBTREE)
    DEFINE_ECONSTANT(LDAP_SCOPE_SUB)
    DEFINE_ECONSTANT(LDAP_SCOPE_SUBORDINATE)
    DEFINE_ECONSTANT(LDAP_SCOPE_CHILDREN)
    DEFINE_ECONSTANT(LDAP_SCOPE_DEFAULT)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_INITIAL)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_ANY)
    DEFINE_ECONSTANT(LDAP_SUBSTRING_FINAL)
    DEFINE_ECONSTANT(LDAP_SUCCESS)
    DEFINE_ECONSTANT(LDAP_OPERATIONS_ERROR)
    DEFINE_ECONSTANT(LDAP_PROTOCOL_ERROR)
    DEFINE_ECONSTANT(LDAP_TIMELIMIT_EXCEEDED)
    DEFINE_ECONSTANT(LDAP_SIZELIMIT_EXCEEDED)
    DEFINE_ECONSTANT(LDAP_COMPARE_FALSE)
    DEFINE_ECONSTANT(LDAP_COMPARE_TRUE)
    DEFINE_ECONSTANT(LDAP_AUTH_METHOD_NOT_SUPPORTED)
    DEFINE_ECONSTANT(LDAP_STRONG_AUTH_NOT_SUPPORTED)
    DEFINE_ECONSTANT(LDAP_STRONG_AUTH_REQUIRED)
    DEFINE_ECONSTANT(LDAP_STRONGER_AUTH_REQUIRED)
    DEFINE_ECONSTANT(LDAP_PARTIAL_RESULTS)
    DEFINE_ECONSTANT(LDAP_REFERRAL)
    DEFINE_ECONSTANT(LDAP_ADMINLIMIT_EXCEEDED)
    DEFINE_ECONSTANT(LDAP_UNAVAILABLE_CRITICAL_EXTENSION)
    DEFINE_ECONSTANT(LDAP_CONFIDENTIALITY_REQUIRED)
    DEFINE_ECONSTANT(LDAP_SASL_BIND_IN_PROGRESS)
    DEFINE_ECONSTANT(LDAP_NO_SUCH_ATTRIBUTE)
    DEFINE_ECONSTANT(LDAP_UNDEFINED_TYPE)
    DEFINE_ECONSTANT(LDAP_INAPPROPRIATE_MATCHING)
    DEFINE_ECONSTANT(LDAP_CONSTRAINT_VIOLATION)
    DEFINE_ECONSTANT(LDAP_TYPE_OR_VALUE_EXISTS)
    DEFINE_ECONSTANT(LDAP_INVALID_SYNTAX)
    DEFINE_ECONSTANT(LDAP_NO_SUCH_OBJECT)
    DEFINE_ECONSTANT(LDAP_ALIAS_PROBLEM)
    DEFINE_ECONSTANT(LDAP_INVALID_DN_SYNTAX)
    DEFINE_ECONSTANT(LDAP_IS_LEAF)
    DEFINE_ECONSTANT(LDAP_ALIAS_DEREF_PROBLEM)
    DEFINE_ECONSTANT(LDAP_X_PROXY_AUTHZ_FAILURE)
    DEFINE_ECONSTANT(LDAP_INAPPROPRIATE_AUTH)
    DEFINE_ECONSTANT(LDAP_INVALID_CREDENTIALS)
    DEFINE_ECONSTANT(LDAP_INSUFFICIENT_ACCESS)
    DEFINE_ECONSTANT(LDAP_BUSY)
    DEFINE_ECONSTANT(LDAP_UNAVAILABLE)
    DEFINE_ECONSTANT(LDAP_UNWILLING_TO_PERFORM)
    DEFINE_ECONSTANT(LDAP_LOOP_DETECT)
    DEFINE_ECONSTANT(LDAP_NAMING_VIOLATION)
    DEFINE_ECONSTANT(LDAP_OBJECT_CLASS_VIOLATION)
    DEFINE_ECONSTANT(LDAP_NOT_ALLOWED_ON_NONLEAF)
    DEFINE_ECONSTANT(LDAP_NOT_ALLOWED_ON_RDN)
    DEFINE_ECONSTANT(LDAP_ALREADY_EXISTS)
    DEFINE_ECONSTANT(LDAP_NO_OBJECT_CLASS_MODS)
    DEFINE_ECONSTANT(LDAP_RESULTS_TOO_LARGE)
    DEFINE_ECONSTANT(LDAP_AFFECTS_MULTIPLE_DSAS)
    DEFINE_ECONSTANT(LDAP_OTHER)
    DEFINE_ECONSTANT(LDAP_CUP_RESOURCES_EXHAUSTED)
    DEFINE_ECONSTANT(LDAP_CUP_SECURITY_VIOLATION)
    DEFINE_ECONSTANT(LDAP_CUP_INVALID_DATA)
    DEFINE_ECONSTANT(LDAP_CUP_UNSUPPORTED_SCHEME)
    DEFINE_ECONSTANT(LDAP_CUP_RELOAD_REQUIRED)
    DEFINE_ECONSTANT(LDAP_CANCELLED)
    DEFINE_ECONSTANT(LDAP_NO_SUCH_OPERATION)
    DEFINE_ECONSTANT(LDAP_TOO_LATE)
    DEFINE_ECONSTANT(LDAP_CANNOT_CANCEL)
    DEFINE_ECONSTANT(LDAP_ASSERTION_FAILED)
    DEFINE_ECONSTANT(LDAP_PROXIED_AUTHORIZATION_DENIED)
    DEFINE_ECONSTANT(LDAP_SYNC_REFRESH_REQUIRED)
    DEFINE_ECONSTANT(LDAP_X_SYNC_REFRESH_REQUIRED)
    DEFINE_ECONSTANT(LDAP_X_ASSERTION_FAILED)
    DEFINE_ECONSTANT(LDAP_X_NO_OPERATION)
    DEFINE_ECONSTANT(LDAP_X_NO_REFERRALS_FOUND)
    DEFINE_ECONSTANT(LDAP_X_CANNOT_CHAIN)
    DEFINE_ECONSTANT(LDAP_X_INVALIDREFERENCE)
#ifdef LDAP_X_TXN
    DEFINE_ECONSTANT(LDAP_X_TXN_SPECIFY_OKAY)
    DEFINE_ECONSTANT(LDAP_X_TXN_ID_INVALID)
#endif
    DEFINE_ECONSTANT(LDAP_SERVER_DOWN)
    DEFINE_ECONSTANT(LDAP_LOCAL_ERROR)
    DEFINE_ECONSTANT(LDAP_ENCODING_ERROR)
    DEFINE_ECONSTANT(LDAP_DECODING_ERROR)
    DEFINE_ECONSTANT(LDAP_TIMEOUT)
    DEFINE_ECONSTANT(LDAP_AUTH_UNKNOWN)
    DEFINE_ECONSTANT(LDAP_FILTER_ERROR)
    DEFINE_ECONSTANT(LDAP_USER_CANCELLED)
    DEFINE_ECONSTANT(LDAP_PARAM_ERROR)
    DEFINE_ECONSTANT(LDAP_NO_MEMORY)
    DEFINE_ECONSTANT(LDAP_CONNECT_ERROR)
    DEFINE_ECONSTANT(LDAP_NOT_SUPPORTED)
    DEFINE_ECONSTANT(LDAP_CONTROL_NOT_FOUND)
    DEFINE_ECONSTANT(LDAP_NO_RESULTS_RETURNED)
    DEFINE_ECONSTANT(LDAP_MORE_RESULTS_TO_RETURN)
    DEFINE_ECONSTANT(LDAP_CLIENT_LOOP)
    DEFINE_ECONSTANT(LDAP_REFERRAL_LIMIT_EXCEEDED)
    DEFINE_ECONSTANT(LDAP_X_CONNECTING)
    DEFINE_ECONSTANT(LDAP_MOD_OP)
    DEFINE_ECONSTANT(LDAP_MOD_ADD)
    DEFINE_ECONSTANT(LDAP_MOD_DELETE)
    DEFINE_ECONSTANT(LDAP_MOD_REPLACE)
    DEFINE_ECONSTANT(LDAP_MOD_INCREMENT)
    DEFINE_ECONSTANT(LDAP_MOD_BVALUES)
    //DEFINE_ECONSTANT(mod_values)
    //DEFINE_ECONSTANT(mod_bvalues)
    DEFINE_ECONSTANT(LDAP_DEREF_NEVER)
    DEFINE_ECONSTANT(LDAP_DEREF_SEARCHING)
    DEFINE_ECONSTANT(LDAP_DEREF_FINDING)
    DEFINE_ECONSTANT(LDAP_DEREF_ALWAYS)
    DEFINE_ECONSTANT(LDAP_NO_LIMIT)
    DEFINE_ECONSTANT(LDAP_MSG_ONE)
    DEFINE_ECONSTANT(LDAP_MSG_ALL)
    DEFINE_ECONSTANT(LDAP_MSG_RECEIVED)
    DEFINE_ECONSTANT(LDAP_URL_SUCCESS)
    DEFINE_ECONSTANT(LDAP_URL_ERR_MEM)
    DEFINE_ECONSTANT(LDAP_URL_ERR_PARAM)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADSCHEME)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADENCLOSURE)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADURL)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADHOST)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADATTRS)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADSCOPE)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADFILTER)
    DEFINE_ECONSTANT(LDAP_URL_ERR_BADEXTS)
    DEFINE_ECONSTANT(LDAP_SASL_AUTOMATIC)
    DEFINE_ECONSTANT(LDAP_SASL_INTERACTIVE)
    DEFINE_ECONSTANT(LDAP_SASL_QUIET)
    DEFINE_ECONSTANT(LDAP_AVA_NULL)
    DEFINE_ECONSTANT(LDAP_AVA_STRING)
    DEFINE_ECONSTANT(LDAP_AVA_BINARY)
    DEFINE_ECONSTANT(LDAP_AVA_NONPRINTABLE)
    DEFINE_ECONSTANT(LDAP_AVA_FREE_ATTR)
    DEFINE_ECONSTANT(LDAP_AVA_FREE_VALUE)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_LDAP)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_LDAPV3)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_LDAPV2)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_DCE)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_UFN)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_AD_CANONICAL)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_LBER)
    DEFINE_ECONSTANT(LDAP_DN_FORMAT_MASK)
    DEFINE_ECONSTANT(LDAP_DN_PRETTY)
    DEFINE_ECONSTANT(LDAP_DN_SKIP)
    DEFINE_ECONSTANT(LDAP_DN_P_NOLEADTRAILSPACES)
    DEFINE_ECONSTANT(LDAP_DN_P_NOSPACEAFTERRDN)
    DEFINE_ECONSTANT(LDAP_DN_PEDANTIC)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_CANCEL)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_TURN)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_PAGED_RESULTS)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_SERVER_SIDE_SORT)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_VIRTUAL_LIST_VIEW)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_WHOAMI)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_PASSWD_MODIFY)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_PASSWORD_POLICY)
    DEFINE_ECONSTANT(LDAP_API_FEATURE_REFRESH)

#ifdef LDAP_CONTROL_PASSWORDPOLICYREQUEST
    DEFINE_ECONSTANT(PP_passwordExpired)
    DEFINE_ECONSTANT(PP_accountLocked)
    DEFINE_ECONSTANT(PP_changeAfterReset)
    DEFINE_ECONSTANT(PP_passwordModNotAllowed)
    DEFINE_ECONSTANT(PP_mustSupplyOldPassword)
    DEFINE_ECONSTANT(PP_insufficientPasswordQuality)
    DEFINE_ECONSTANT(PP_passwordTooShort)
    DEFINE_ECONSTANT(PP_passwordTooYoung)
    DEFINE_ECONSTANT(PP_passwordInHistory)
    DEFINE_ECONSTANT(PP_noError)
#endif

    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_NONE)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_PHASE_FLAG)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_IDSET_FLAG)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_DONE_FLAG)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_PRESENT)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_ADD)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_MODIFY)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_DELETE)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_PRESENTS)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_DELETES)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_PRESENTS_IDSET)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_DELETES_IDSET)
    DEFINE_ECONSTANT(LDAP_SYNC_CAPI_DONE)

    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_DESTROY {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_control_create, 5)
    T_STRING(ldap_control_create, 0)     // char*
    T_NUMBER(ldap_control_create, 1)     // int
    T_NUMBER(ldap_control_create, 3)     // int

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;

    RETURN_NUMBER(ldap_control_create(PARAM(0), (int)PARAM(1), &local_parameter_2, (int)PARAM(3), &local_parameter_4))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_control_find, 3)
    T_STRING(ldap_control_find, 0)     // char*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl***)
    LDAPControl **local_parameter_2;

    RETURN_NUMBER((long)ldap_control_find(PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_control_free, 1)
    T_NUMBER(ldap_control_free, 0)     // LDAPControl*

    ldap_control_free((LDAPControl *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_controls_free, 1)

// ... parameter 0 is by reference (LDAPControl**)
    LDAPControl * local_parameter_0;

    ldap_controls_free(&local_parameter_0);
    SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_controls_dup, 1)

// ... parameter 0 is by reference (LDAPControl**)
    LDAPControl * local_parameter_0;

    RETURN_NUMBER((long)ldap_controls_dup(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_control_dup, 1)
    T_HANDLE(ldap_control_dup, 0)     // LDAPControl*

    RETURN_NUMBER((long)ldap_control_dup((LDAPControl *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_domain2dn, 2)
    T_STRING(ldap_domain2dn, 0)     // char*

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(ldap_domain2dn(PARAM(0), &local_parameter_1))
    SET_STRING(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2domain, 2)
    T_STRING(ldap_dn2domain, 0)     // char*

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(ldap_dn2domain(PARAM(0), &local_parameter_1))
    SET_STRING(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_domain2hostlist, 2)
    T_STRING(ldap_domain2hostlist, 0)     // char*

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(ldap_domain2hostlist(PARAM(0), &local_parameter_1))
    SET_STRING(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_extended_operation, 6)
    T_NUMBER(ldap_extended_operation, 0)     // LDAP*
    T_STRING(ldap_extended_operation, 1)     // char*

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_extended_operation((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)

    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_extended_operation_s, 7)
    T_NUMBER(ldap_extended_operation_s, 0)     // LDAP*
    T_STRING(ldap_extended_operation_s, 1)     // char*

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (char**)
    char *local_parameter_5;
// ... parameter 6 is by reference (berval**)
    berval *local_parameter_6;

    RETURN_NUMBER(ldap_extended_operation_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_STRING(5, local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_extended_result, 5)
    T_NUMBER(ldap_parse_extended_result, 0)     // LDAP*
    T_NUMBER(ldap_parse_extended_result, 1)     // LDAPMessage*
    T_NUMBER(ldap_parse_extended_result, 4)     // int

// ... parameter 2 is by reference (char**)
    char *local_parameter_2;
// ... parameter 3 is by reference (berval**)
    berval *local_parameter_3;

    RETURN_NUMBER(ldap_parse_extended_result((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, &local_parameter_3, (int)PARAM(4)))
    SET_STRING(2, local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_intermediate, 6)
    T_NUMBER(ldap_parse_intermediate, 0)     // LDAP*
    T_NUMBER(ldap_parse_intermediate, 1)     // LDAPMessage*
    T_NUMBER(ldap_parse_intermediate, 5)     // int

// ... parameter 2 is by reference (char**)
    char *local_parameter_2;
// ... parameter 3 is by reference (berval**)
    berval *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl***)
    LDAPControl **local_parameter_4;

    RETURN_NUMBER(ldap_parse_intermediate((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, (int)PARAM(5)))
    SET_STRING(2, local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_abandon_ext, 4)
    T_NUMBER(ldap_abandon_ext, 0)     // LDAP*
    T_NUMBER(ldap_abandon_ext, 1)     // int

// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;

    RETURN_NUMBER(ldap_abandon_ext((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_add_ext, 6)
    T_NUMBER(ldap_add_ext, 0)     // LDAP*
    T_STRING(ldap_add_ext, 1)     // char*

// ... parameter 2 is by reference (LDAPMod**)
    LDAPMod * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_add_ext((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_add_ext_s, 5)
    T_NUMBER(ldap_add_ext_s, 0)     // LDAP*
    T_STRING(ldap_add_ext_s, 1)     // char*

// ... parameter 2 is by reference (LDAPMod**)
    LDAPMod * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;

    RETURN_NUMBER(ldap_add_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sasl_bind, 7)
    T_NUMBER(ldap_sasl_bind, 0)     // LDAP*
    T_STRING(ldap_sasl_bind, 1)     // char*
    T_STRING(ldap_sasl_bind, 2)     // char*

// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;
// ... parameter 6 is by reference (int*)
    int local_parameter_6;

    RETURN_NUMBER(ldap_sasl_bind((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
void *deleg = 0;

int MyDeleg(LDAP *ld, unsigned flags, void *defaults, void *interact) {
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    if (!deleg)
        return 0;


    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, deleg, &RES, &EXCEPTION, (INTEGER)4,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)ld,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)flags,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)defaults,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)interact
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

CONCEPT_FUNCTION_IMPL(ldap_sasl_interactive_bind_s, 8)
    T_NUMBER(ldap_sasl_interactive_bind_s, 0)     // LDAP*
    T_STRING(ldap_sasl_interactive_bind_s, 1)     // char*
    T_STRING(ldap_sasl_interactive_bind_s, 2)     // char*
    T_NUMBER(ldap_sasl_interactive_bind_s, 5)     // unsigned
    T_DELEGATE(ldap_sasl_interactive_bind_s, 6)
    T_NUMBER(ldap_sasl_interactive_bind_s, 7)     // void*

    deleg = PARAMETER(6);

// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 6 is by reference (LDAP_SASL_INTERACT_PROC*)
//LDAP_SASL_INTERACT_PROC local_parameter_6;

    RETURN_NUMBER(ldap_sasl_interactive_bind_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, (unsigned)PARAM(5), MyDeleg, (void *)(long)PARAM(7)))
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sasl_bind_s, 7)
    T_NUMBER(ldap_sasl_bind_s, 0)     // LDAP*
    T_STRING(ldap_sasl_bind_s, 1)     // char*
    T_STRING(ldap_sasl_bind_s, 2)     // char*

// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;
// ... parameter 6 is by reference (berval**)
    berval *local_parameter_6;

    RETURN_NUMBER(ldap_sasl_bind_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_sasl_bind_result, 4)
    T_NUMBER(ldap_parse_sasl_bind_result, 0)     // LDAP*
    T_NUMBER(ldap_parse_sasl_bind_result, 1)     // LDAPMessage*
    T_NUMBER(ldap_parse_sasl_bind_result, 3)     // int

// ... parameter 2 is by reference (berval**)
    berval * local_parameter_2;

    RETURN_NUMBER(ldap_parse_sasl_bind_result((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, (int)PARAM(3)))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_compare_ext, 7)
    T_NUMBER(ldap_compare_ext, 0)     // LDAP*
    T_STRING(ldap_compare_ext, 1)     // char*
    T_STRING(ldap_compare_ext, 2)     // char*

// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;
// ... parameter 6 is by reference (int*)
    int local_parameter_6;

    RETURN_NUMBER(ldap_compare_ext((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_compare_ext_s, 6)
    T_NUMBER(ldap_compare_ext_s, 0)     // LDAP*
    T_STRING(ldap_compare_ext_s, 1)     // char*
    T_STRING(ldap_compare_ext_s, 2)     // char*

// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;

    RETURN_NUMBER(ldap_compare_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_delete_ext, 5)
    T_NUMBER(ldap_delete_ext, 0)     // LDAP*
    T_STRING(ldap_delete_ext, 1)     // char*

// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (int*)
    int local_parameter_4;

    RETURN_NUMBER(ldap_delete_ext((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_delete_ext_s, 4)
    T_NUMBER(ldap_delete_ext_s, 0)     // LDAP*
    T_STRING(ldap_delete_ext_s, 1)     // char*

// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;

    RETURN_NUMBER(ldap_delete_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_result, 8)
    T_NUMBER(ldap_parse_result, 0)     // LDAP*
    T_NUMBER(ldap_parse_result, 1)     // LDAPMessage*
    T_NUMBER(ldap_parse_result, 7)     // int

// ... parameter 2 is by reference (int*)
    int local_parameter_2;
// ... parameter 3 is by reference (char**)
    char *local_parameter_3;
// ... parameter 4 is by reference (char**)
    char *local_parameter_4;
// ... parameter 5 is by reference (char***)
    char **local_parameter_5;
// ... parameter 6 is by reference (LDAPControl***)
    LDAPControl **local_parameter_6;

    RETURN_NUMBER(ldap_parse_result((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6, (int)PARAM(7)))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_STRING(3, local_parameter_3)
    SET_STRING(4, local_parameter_4)
    ReturnCharList(PARAMETER(5), local_parameter_5);
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_err2string, 1)
    T_NUMBER(ldap_err2string, 0)     // int

    RETURN_STRING((char *)ldap_err2string((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_gssapi_bind, 3)
    T_NUMBER(ldap_gssapi_bind, 0)     // LDAP*
    T_STRING(ldap_gssapi_bind, 1)     // char*
    T_STRING(ldap_gssapi_bind, 2)     // char*

    RETURN_NUMBER(ldap_gssapi_bind((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_gssapi_bind_s, 3)
    T_NUMBER(ldap_gssapi_bind_s, 0)     // LDAP*
    T_STRING(ldap_gssapi_bind_s, 1)     // char*
    T_STRING(ldap_gssapi_bind_s, 2)     // char*

    RETURN_NUMBER(ldap_gssapi_bind_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_modify_ext, 6)
    T_NUMBER(ldap_modify_ext, 0)     // LDAP*
    T_STRING(ldap_modify_ext, 1)     // char*

// ... parameter 2 is by reference (LDAPMod**)
    LDAPMod * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_modify_ext((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_modify_ext_s, 5)
    T_NUMBER(ldap_modify_ext_s, 0)     // LDAP*
    T_STRING(ldap_modify_ext_s, 1)     // char*

// ... parameter 2 is by reference (LDAPMod**)
    LDAPMod * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;

    RETURN_NUMBER(ldap_modify_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_rename, 8)
    T_NUMBER(ldap_rename, 0)     // LDAP*
    T_STRING(ldap_rename, 1)     // char*
    T_STRING(ldap_rename, 2)     // char*
    T_STRING(ldap_rename, 3)     // char*
    T_NUMBER(ldap_rename, 4)     // int

// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl * local_parameter_5;
// ... parameter 6 is by reference (LDAPControl**)
    LDAPControl *local_parameter_6;
// ... parameter 7 is by reference (int*)
    int local_parameter_7;

    RETURN_NUMBER(ldap_rename((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), PARAM(3), (int)PARAM(4), &local_parameter_5, &local_parameter_6, &local_parameter_7))
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
    SET_NUMBER(7, (long)local_parameter_7)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_rename_s, 7)
    T_NUMBER(ldap_rename_s, 0)     // LDAP*
    T_STRING(ldap_rename_s, 1)     // char*
    T_STRING(ldap_rename_s, 2)     // char*
    T_STRING(ldap_rename_s, 3)     // char*
    T_NUMBER(ldap_rename_s, 4)     // int

// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl * local_parameter_5;
// ... parameter 6 is by reference (LDAPControl**)
    LDAPControl *local_parameter_6;

    RETURN_NUMBER(ldap_rename_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), PARAM(3), (int)PARAM(4), &local_parameter_5, &local_parameter_6))
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create, 1)

// ... parameter 0 is by reference (LDAP**)
    LDAP * local_parameter_0;

    RETURN_NUMBER(ldap_create(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ldap_initialize, 2, 3)
    T_STRING(ldap_initialize, 1)     // char*

// ... parameter 0 is by reference (LDAP**)
    LDAP * local_parameter_0 = NULL;
    int version = LDAP_VERSION3;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(ldap_initialize, 2);
        version = PARAM_INT(2);
    }

    RETURN_NUMBER(ldap_initialize(&local_parameter_0, PARAM(1)))
    if (local_parameter_0)
        ldap_set_option(local_parameter_0, LDAP_OPT_PROTOCOL_VERSION, &version);

    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(ldap_tls_inplace,1)
        T_NUMBER(ldap_initialize, 0) // LDAP*

        RETURN_NUMBER(ldap_tls_inplace((LDAP*)(long)PARAM(0)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_start_tls, 1)
    T_HANDLE(ldap_start_tls, 0)     // LDAP*
    RETURN_NUMBER(ldap_start_tls_s((LDAP *)(uintptr_t)PARAM(0), NULL, NULL));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_install_tls, 1)
    T_NUMBER(ldap_install_tls, 0)     // LDAP*

    RETURN_NUMBER(ldap_install_tls((LDAP *)(uintptr_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_start_tls_s, 3)
    T_NUMBER(ldap_start_tls_s, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;

    RETURN_NUMBER(ldap_start_tls_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_first_message, 2)
    T_NUMBER(ldap_first_message, 0)     // LDAP*
    T_NUMBER(ldap_first_message, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_first_message((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_next_message, 2)
    T_NUMBER(ldap_next_message, 0)     // LDAP*
    T_NUMBER(ldap_next_message, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_next_message((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_count_messages, 2)
    T_NUMBER(ldap_count_messages, 0)     // LDAP*
    T_NUMBER(ldap_count_messages, 1)     // LDAPMessage*

    RETURN_NUMBER(ldap_count_messages((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_first_reference, 2)
    T_NUMBER(ldap_first_reference, 0)     // LDAP*
    T_NUMBER(ldap_first_reference, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_first_reference((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_next_reference, 2)
    T_NUMBER(ldap_next_reference, 0)     // LDAP*
    T_NUMBER(ldap_next_reference, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_next_reference((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_count_references, 2)
    T_NUMBER(ldap_count_references, 0)     // LDAP*
    T_NUMBER(ldap_count_references, 1)     // LDAPMessage*

    RETURN_NUMBER(ldap_count_references((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_reference, 5)
    T_NUMBER(ldap_parse_reference, 0)     // LDAP*
    T_NUMBER(ldap_parse_reference, 1)     // LDAPMessage*
    T_NUMBER(ldap_parse_reference, 4)     // int

// ... parameter 2 is by reference (char***)
    char **local_parameter_2;
// ... parameter 3 is by reference (LDAPControl***)
    LDAPControl **local_parameter_3;

    RETURN_NUMBER(ldap_parse_reference((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, &local_parameter_3, (int)PARAM(4)))
//SET_NUMBER(2, (long)local_parameter_2)
    ReturnCharList(PARAMETER(2), local_parameter_2);
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_first_entry, 2)
    T_NUMBER(ldap_first_entry, 0)     // LDAP*
    T_NUMBER(ldap_first_entry, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_first_entry((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_next_entry, 2)
    T_NUMBER(ldap_next_entry, 0)     // LDAP*
    T_NUMBER(ldap_next_entry, 1)     // LDAPMessage*

    RETURN_NUMBER((long)ldap_next_entry((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_count_entries, 2)
    T_NUMBER(ldap_count_entries, 0)     // LDAP*
    T_NUMBER(ldap_count_entries, 1)     // LDAPMessage*

    RETURN_NUMBER(ldap_count_entries((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_get_entry_controls, 3)
    T_NUMBER(ldap_get_entry_controls, 0)     // LDAP*
    T_NUMBER(ldap_get_entry_controls, 1)     // LDAPMessage*

// ... parameter 2 is by reference (LDAPControl***)
    LDAPControl * *local_parameter_2;

    RETURN_NUMBER(ldap_get_entry_controls((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_delete_result_entry, 2)
    T_NUMBER(ldap_delete_result_entry, 1)     // LDAPMessage*

// ... parameter 0 is by reference (LDAPMessage**)
    LDAPMessage * local_parameter_0;

    RETURN_NUMBER((long)ldap_delete_result_entry(&local_parameter_0, (LDAPMessage *)(uintptr_t)PARAM(1)))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_add_result_entry, 2)
    T_NUMBER(ldap_add_result_entry, 1)     // LDAPMessage*

// ... parameter 0 is by reference (LDAPMessage**)
    LDAPMessage * local_parameter_0;

    ldap_add_result_entry(&local_parameter_0, (LDAPMessage *)(uintptr_t)PARAM(1));
    SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_get_dn, 2)
    T_HANDLE(ldap_get_dn, 0)     // LDAP*
    T_HANDLE(ldap_get_dn, 1)     // LDAPMessage*

    RETURN_STRING((char *)ldap_get_dn((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_rdnfree, 1)
    T_HANDLE(ldap_rdnfree, 0)     // LDAPRDN

    ldap_rdnfree((LDAPRDN)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dnfree, 1)
    T_NUMBER(ldap_dnfree, 0)     // LDAPDN

    ldap_dnfree((LDAPDN)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_bv2dn, 3)
    T_NUMBER(ldap_bv2dn, 1)     // LDAPDN*
    T_NUMBER(ldap_bv2dn, 2)     // unsigned

// ... parameter 0 is by reference (berval*)
    GET_DECL_BERVAL(0, local_parameter_0);

    RETURN_NUMBER(ldap_bv2dn(&local_parameter_0, (LDAPDN *)(long)PARAM(1), (unsigned)PARAM(2)))
//SET_NUMBER(0, (long)local_parameter_0)
    SET_BERVAL(0, local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_str2dn, 3)
    T_STRING(ldap_str2dn, 0)     // char*
    T_NUMBER(ldap_str2dn, 1)     // LDAPDN*
    T_NUMBER(ldap_str2dn, 2)     // unsigned

    RETURN_NUMBER(ldap_str2dn(PARAM(0), (LDAPDN *)(long)PARAM(1), (unsigned)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2bv, 3)
    T_NUMBER(ldap_dn2bv, 0)     // LDAPDN
    T_NUMBER(ldap_dn2bv, 2)     // unsigned

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);

    RETURN_NUMBER(ldap_dn2bv((LDAPDN)PARAM_INT(0), &local_parameter_1, (unsigned)PARAM(2)))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_BERVAL(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2str, 3)
    T_NUMBER(ldap_dn2str, 0)     // LDAPDN
    T_NUMBER(ldap_dn2str, 2)     // unsigned

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(ldap_dn2str((LDAPDN)PARAM_INT(0), &local_parameter_1, (unsigned)PARAM(2)))
    SET_STRING(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_bv2rdn, 4)
    T_NUMBER(ldap_bv2rdn, 3)     // unsigned

// ... parameter 0 is by reference (berval*)
    GET_DECL_BERVAL(0, local_parameter_0);
// ... parameter 1 is by reference (LDAPRDN*)
    LDAPRDN local_parameter_1;
// ... parameter 2 is by reference (char**)
    char *local_parameter_2;

    RETURN_NUMBER(ldap_bv2rdn(&local_parameter_0, &local_parameter_1, &local_parameter_2, (unsigned)PARAM(3)))
//SET_NUMBER(0, (long)local_parameter_0)
    SET_BERVAL(0, local_parameter_0)
    SET_NUMBER(1, (long)local_parameter_1)
    SET_STRING(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_str2rdn, 4)
    T_STRING(ldap_str2rdn, 0)     // char*
    T_NUMBER(ldap_str2rdn, 3)     // unsigned

// ... parameter 1 is by reference (LDAPRDN*)
    LDAPRDN local_parameter_1;
// ... parameter 2 is by reference (char**)
    char *local_parameter_2;

    RETURN_NUMBER(ldap_str2rdn(PARAM(0), &local_parameter_1, &local_parameter_2, (unsigned)PARAM(3)))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_STRING(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_rdn2bv, 3)
    T_NUMBER(ldap_rdn2bv, 0)     // LDAPRDN
    T_NUMBER(ldap_rdn2bv, 2)     // unsigned

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);

    RETURN_NUMBER(ldap_rdn2bv((LDAPRDN)PARAM_INT(0), &local_parameter_1, (unsigned)PARAM(2)))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_BERVAL(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_rdn2str, 3)
    T_NUMBER(ldap_rdn2str, 0)     // LDAPRDN
    T_NUMBER(ldap_rdn2str, 2)     // unsigned

// ... parameter 1 is by reference (char**)
    char *local_parameter_1;

    RETURN_NUMBER(ldap_rdn2str((LDAPRDN)PARAM_INT(0), &local_parameter_1, (unsigned)PARAM(2)))
    SET_STRING(1, local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn_normalize, 4)
    T_STRING(ldap_dn_normalize, 0)     // char*
    T_NUMBER(ldap_dn_normalize, 1)     // unsigned
    T_NUMBER(ldap_dn_normalize, 3)     // unsigned

// ... parameter 2 is by reference (char**)
    char *local_parameter_2;

    RETURN_NUMBER(ldap_dn_normalize(PARAM(0), (unsigned)PARAM(1), &local_parameter_2, (unsigned)PARAM(3)))
    SET_STRING(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2ufn, 1)
    T_STRING(ldap_dn2ufn, 0)     // char*

    RETURN_STRING((char *)ldap_dn2ufn(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_explode_dn, 2)
    T_STRING(ldap_explode_dn, 0)     // char*
    T_NUMBER(ldap_explode_dn, 1)     // int

    ReturnCharList(RESULT, ldap_explode_dn(PARAM(0), (int)PARAM(1)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_explode_rdn, 2)
    T_STRING(ldap_explode_rdn, 0)     // char*
    T_NUMBER(ldap_explode_rdn, 1)     // int

    ReturnCharList(RESULT, ldap_explode_rdn(PARAM(0), (int)PARAM(1)));
END_IMPL
//------------------------------------------------------------------------
void *deleg2 = 0;

int MyDeleg2(LDAPDN dn, unsigned flags, void *ctx) {
    if (!deleg2)
        return 0;
    void *RES       = 0;
    void *EXCEPTION = 0;
    int  result     = 0;

    NUMBER  ndata   = 0;
    char    *szdata = 0;
    INTEGER TYPE    = 0;

    InvokePtr(INVOKE_CALL_DELEGATE, deleg, &RES, &EXCEPTION, (INTEGER)3,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)dn,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(int)flags,
              (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)ctx
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

CONCEPT_FUNCTION_IMPL(ldap_X509dn2bv, 4)
    T_NUMBER(ldap_X509dn2bv, 0)     // void*
    T_DELEGATE(ldap_X509dn2bv, 2)
    T_NUMBER(ldap_X509dn2bv, 3)     // unsigned

    deleg2 = PARAMETER(2);
// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);
// ... parameter 2 is by reference (LDAPDN_rewrite_func*)
//LDAPDN_rewrite_func local_parameter_2;

    RETURN_NUMBER(ldap_X509dn2bv((void *)(long)PARAM(0), &local_parameter_1, MyDeleg2, (unsigned)PARAM(3)))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_BERVAL(1, local_parameter_1)
//SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2dcedn, 1)
    T_STRING(ldap_dn2dcedn, 0)     // char*

    RETURN_STRING((char *)ldap_dn2dcedn(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dcedn2dn, 1)
    T_STRING(ldap_dcedn2dn, 0)     // char*

    RETURN_STRING((char *)ldap_dcedn2dn(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_dn2ad_canonical, 1)
    T_STRING(ldap_dn2ad_canonical, 0)     // char*

    RETURN_STRING(ldap_dn2ad_canonical(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_get_dn_ber, 4)
    T_NUMBER(ldap_get_dn_ber, 0)     // LDAP*
    T_NUMBER(ldap_get_dn_ber, 1)     // LDAPMessage*
//T_HANDLE(2)

// ... parameter 2 is by reference (BerElement**)
    BerElement * local_parameter_2;
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);

    RETURN_NUMBER(ldap_get_dn_ber((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_get_attribute_ber, 5)
    T_NUMBER(ldap_get_attribute_ber, 0)     // LDAP*
    T_NUMBER(ldap_get_attribute_ber, 1)     // LDAPMessage*
    T_HANDLE(ldap_get_attribute_ber, 2)

// ... parameter 2 is by reference (BerElement*)
//struct BerElement local_parameter_2;
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (berval**)
    berval *local_parameter_4;

    RETURN_NUMBER(ldap_get_attribute_ber((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), (BerElement *)PARAM_INT(2), &local_parameter_3, &local_parameter_4))
//SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_first_attribute, 3)
    T_NUMBER(ldap_first_attribute, 0)     // LDAP*
    T_NUMBER(ldap_first_attribute, 1)     // LDAPMessage*

// ... parameter 2 is by reference (BerElement**)
    BerElement * local_parameter_2;

    RETURN_STRING((char *)ldap_first_attribute((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_next_attribute, 3)
    T_NUMBER(ldap_next_attribute, 0)     // LDAP*
    T_NUMBER(ldap_next_attribute, 1)     // LDAPMessage*
    T_HANDLE(ldap_next_attribute, 2)

// ... parameter 2 is by reference (BerElement*)
//struct BerElement local_parameter_2;

    RETURN_STRING((char *)ldap_next_attribute((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), (BerElement *)PARAM_INT(2)))
//SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_count_values_len, 1)

// ... parameter 0 is by reference (berval**)
    berval * local_parameter_0;

    RETURN_NUMBER(ldap_count_values_len(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
//SET_BERVAL(0, local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_value_free_len, 1)

// ... parameter 0 is by reference (berval**)
    berval * local_parameter_0;

    ldap_value_free_len(&local_parameter_0);
    SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_result, 5)
    T_NUMBER(ldap_result, 0)     // LDAP*
    T_NUMBER(ldap_result, 1)     // int
    T_NUMBER(ldap_result, 2)     // int

// ... parameter 3 is by reference (timeval*)
    timeval local_parameter_3;
// ... parameter 4 is by reference (LDAPMessage**)
    LDAPMessage *local_parameter_4;

    RETURN_NUMBER(ldap_result((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), (int)PARAM(2), &local_parameter_3, &local_parameter_4))
//SET_NUMBER(3, (long)local_parameter_3)
    WRAP_timeval(PARAMETER(3), &local_parameter_3);
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_msgtype, 1)
    T_NUMBER(ldap_msgtype, 0)     // LDAPMessage*

    RETURN_NUMBER(ldap_msgtype((LDAPMessage *)(uintptr_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_msgid, 1)
    T_NUMBER(ldap_msgid, 0)     // LDAPMessage*

    RETURN_NUMBER(ldap_msgid((LDAPMessage *)(uintptr_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_msgfree, 1)
    T_NUMBER(ldap_msgfree, 0)     // LDAPMessage*

    RETURN_NUMBER(ldap_msgfree((LDAPMessage *)(uintptr_t)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_msgdelete, 2)
    T_NUMBER(ldap_msgdelete, 0)     // LDAP*
    T_NUMBER(ldap_msgdelete, 1)     // int

    RETURN_NUMBER(ldap_msgdelete((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_bv2escaped_filter_value, 2)

// ... parameter 0 is by reference (berval*)
    GET_DECL_BERVAL(0, local_parameter_0);
// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);

    RETURN_NUMBER(ldap_bv2escaped_filter_value(&local_parameter_0, &local_parameter_1))
    SET_BERVAL(0, local_parameter_0)
    SET_BERVAL(1, local_parameter_1)
//SET_NUMBER(0, (long)local_parameter_0)
//SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_search_ext, 11)
    T_NUMBER(ldap_search_ext, 0)     // LDAP*
    T_STRING(ldap_search_ext, 1)     // char*
    T_NUMBER(ldap_search_ext, 2)     // int
    T_STRING(ldap_search_ext, 3)     // char*
    T_NUMBER(ldap_search_ext, 5)     // int
    T_NUMBER(ldap_search_ext, 9)     // int

// ... parameter 4 is by reference (char**)
    char *local_parameter_4;
// ... parameter 6 is by reference (LDAPControl**)
    LDAPControl *local_parameter_6;
// ... parameter 7 is by reference (LDAPControl**)
    LDAPControl *local_parameter_7;
// ... parameter 8 is by reference (timeval*)
    timeval local_parameter_8;
// ... parameter 10 is by reference (int*)
    int local_parameter_10;

    RETURN_NUMBER(ldap_search_ext((LDAP *)(uintptr_t)PARAM(0), PARAM(1), (int)PARAM(2), PARAM(3), &local_parameter_4, (int)PARAM(5), &local_parameter_6, &local_parameter_7, &local_parameter_8, (int)PARAM(9), &local_parameter_10))
    SET_STRING(4, local_parameter_4)
    SET_NUMBER(6, (long)local_parameter_6)
    SET_NUMBER(7, (long)local_parameter_7)
//SET_NUMBER(8, (long)local_parameter_8)
    WRAP_timeval(PARAMETER(8), &local_parameter_8);
    SET_NUMBER(10, (long)local_parameter_10)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_search_ext_s, 11)
    T_NUMBER(ldap_search_ext_s, 0)     // LDAP*
    T_STRING(ldap_search_ext_s, 1)     // char*
    T_NUMBER(ldap_search_ext_s, 2)     // int
    T_STRING(ldap_search_ext_s, 3)     // char*
    T_NUMBER(ldap_search_ext_s, 5)     // int
    T_NUMBER(ldap_search_ext_s, 9)     // int

// ... parameter 4 is by reference (char**)
    char *local_parameter_4 = NULL;
// ... parameter 6 is by reference (LDAPControl**)
    LDAPControl *local_parameter_6 = NULL;
// ... parameter 7 is by reference (LDAPControl**)
    LDAPControl *local_parameter_7 = NULL;
// ... parameter 8 is by reference (timeval*)
    timeval local_parameter_8 = {0, 0};
// ... parameter 10 is by reference (LDAPMessage**)
    LDAPMessage *local_parameter_10 = NULL;

    RETURN_NUMBER(ldap_search_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), (int)PARAM(2), PARAM(3), &local_parameter_4, (int)PARAM(5), &local_parameter_6, &local_parameter_7, &local_parameter_8, (int)PARAM(9), &local_parameter_10))
    SET_STRING(4, local_parameter_4)
    SET_NUMBER(6, (long)local_parameter_6)
    SET_NUMBER(7, (long)local_parameter_7)
//SET_NUMBER(8, (long)local_parameter_8)
    WRAP_timeval(PARAMETER(8), &local_parameter_8);
    SET_NUMBER(10, (long)local_parameter_10)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_unbind_ext, 3)
    T_NUMBER(ldap_unbind_ext, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;

    RETURN_NUMBER(ldap_unbind_ext((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_unbind_ext_s, 3)
    T_NUMBER(ldap_unbind_ext_s, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;

    RETURN_NUMBER(ldap_unbind_ext_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_put_vrFilter, 2)
    T_STRING(ldap_put_vrFilter, 1)     // char*
    T_HANDLE(ldap_put_vrFilter, 0)

// ... parameter 0 is by reference (BerElement*)
//BerElement local_parameter_0;

    RETURN_NUMBER(ldap_put_vrFilter((BerElement *)PARAM_INT(0), PARAM(1)))
//SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_mods_free, 2)
    T_NUMBER(ldap_mods_free, 1)     // int

// ... parameter 0 is by reference (LDAPMod**)
    LDAPMod * local_parameter_0;

    ldap_mods_free(&local_parameter_0, (int)PARAM(1));
    SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_is_ldap_url, 1)
    T_STRING(ldap_is_ldap_url, 0)     // char*

    RETURN_NUMBER(ldap_is_ldap_url(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_is_ldaps_url, 1)
    T_STRING(ldap_is_ldaps_url, 0)     // char*

    RETURN_NUMBER(ldap_is_ldaps_url(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_is_ldapi_url, 1)
    T_STRING(ldap_is_ldapi_url, 0)     // char*

    RETURN_NUMBER(ldap_is_ldapi_url(PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_url_parse, 2)
    T_STRING(ldap_url_parse, 0)     // char*

// ... parameter 1 is by reference (LDAPURLDesc**)
    LDAPURLDesc * local_parameter_1;

    RETURN_NUMBER(ldap_url_parse(PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_url_desc2str, 1)
    T_HANDLE(ldap_url_desc2str, 0)
// ... parameter 0 is by reference (LDAPURLDesc*)
//LDAPURLDesc local_parameter_0;

    RETURN_STRING((char *)ldap_url_desc2str((LDAPURLDesc *)PARAM_INT(0)))
//SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_free_urldesc, 1)
    T_HANDLE(ldap_free_urldesc, 0)
// ... parameter 0 is by reference (LDAPURLDesc*)
//LDAPURLDesc local_parameter_0;

    ldap_free_urldesc((LDAPURLDesc *)PARAM_INT(0));
//SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_cancel, 5)
    T_NUMBER(ldap_cancel, 0)     // LDAP*
    T_NUMBER(ldap_cancel, 1)     // int

// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (int*)
    int local_parameter_4;

    RETURN_NUMBER(ldap_cancel((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_cancel_s, 4)
    T_NUMBER(ldap_cancel_s, 0)     // LDAP*
    T_NUMBER(ldap_cancel_s, 1)     // int

// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;

    RETURN_NUMBER(ldap_cancel_s((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_turn, 6)
    T_NUMBER(ldap_turn, 0)     // LDAP*
    T_NUMBER(ldap_turn, 1)     // int
    T_STRING(ldap_turn, 2)     // char*

// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl * local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_turn((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5))
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_turn_s, 5)
    T_NUMBER(ldap_turn_s, 0)     // LDAP*
    T_NUMBER(ldap_turn_s, 1)     // int
    T_STRING(ldap_turn_s, 2)     // char*

// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl * local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;

    RETURN_NUMBER(ldap_turn_s((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), PARAM(2), &local_parameter_3, &local_parameter_4))
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_page_control_value, 4)
    T_NUMBER(ldap_create_page_control_value, 0)     // LDAP*
    T_NUMBER(ldap_create_page_control_value, 1)     // ber_int_t

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);

    RETURN_NUMBER(ldap_create_page_control_value((LDAP *)(uintptr_t)PARAM(0), (ber_int_t)PARAM(1), &local_parameter_2, &local_parameter_3))
//SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(2, local_parameter_2)
    SET_BERVAL(3, local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_page_control, 5)
    T_NUMBER(ldap_create_page_control, 0)     // LDAP*
    T_NUMBER(ldap_create_page_control, 1)     // ber_int_t
    T_NUMBER(ldap_create_page_control, 3)     // int

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;

    RETURN_NUMBER(ldap_create_page_control((LDAP *)(uintptr_t)PARAM(0), (ber_int_t)PARAM(1), &local_parameter_2, (int)PARAM(3), &local_parameter_4))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_pageresponse_control, 4)
    T_NUMBER(ldap_parse_pageresponse_control, 0)     // LDAP*
    T_NUMBER(ldap_parse_pageresponse_control, 1)     // LDAPControl*

// ... parameter 2 is by reference (ber_int_t*)
    ber_int_t local_parameter_2;
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);

    RETURN_NUMBER(ldap_parse_pageresponse_control((LDAP *)(uintptr_t)PARAM(0), (LDAPControl *)(long)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(3, local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_sort_keylist, 2)
    T_STRING(ldap_create_sort_keylist, 1)     // char*

// ... parameter 0 is by reference (LDAPSortKey***)
    LDAPSortKey * *local_parameter_0;

    RETURN_NUMBER(ldap_create_sort_keylist(&local_parameter_0, PARAM(1)))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_free_sort_keylist, 1)

// ... parameter 0 is by reference (LDAPSortKey**)
    LDAPSortKey * local_parameter_0;

    ldap_free_sort_keylist(&local_parameter_0);
    SET_NUMBER(0, (long)local_parameter_0)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_sort_control_value, 3)
    T_NUMBER(ldap_create_sort_control_value, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPSortKey**)
    LDAPSortKey * local_parameter_1;
// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);

    RETURN_NUMBER(ldap_create_sort_control_value((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_sort_control, 4)
    T_NUMBER(ldap_create_sort_control, 0)     // LDAP*
    T_NUMBER(ldap_create_sort_control, 2)     // int

// ... parameter 1 is by reference (LDAPSortKey**)
    LDAPSortKey * local_parameter_1;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;

    RETURN_NUMBER(ldap_create_sort_control((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, (int)PARAM(2), &local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_sortresponse_control, 4)
    T_NUMBER(ldap_parse_sortresponse_control, 0)     // LDAP*
    T_NUMBER(ldap_parse_sortresponse_control, 1)     // LDAPControl*

// ... parameter 2 is by reference (ber_int_t*)
    ber_int_t local_parameter_2;
// ... parameter 3 is by reference (char**)
    char *local_parameter_3;

    RETURN_NUMBER(ldap_parse_sortresponse_control((LDAP *)(uintptr_t)PARAM(0), (LDAPControl *)(long)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (SYS_INT)local_parameter_2)
    SET_STRING(3, local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_vlv_control_value, 3)
    T_NUMBER(ldap_create_vlv_control_value, 0)     // LDAP*
    T_ARRAY(ldap_create_vlv_control_value, 1)

// ... parameter 1 is by reference (LDAPVLVInfo*)
//LDAPVLVInfo local_parameter_1;
// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);

    RETURN_NUMBER(ldap_create_vlv_control_value((LDAP *)(uintptr_t)PARAM(0), UNWRAP_LDAPVLVInfo(PARAMETER(1)), &local_parameter_2))
//SET_NUMBER(1, (long)local_parameter_1)
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_vlv_control, 3)
    T_NUMBER(ldap_create_vlv_control, 0)     // LDAP*
    T_ARRAY(ldap_create_vlv_control, 1)

// ... parameter 1 is by reference (LDAPVLVInfo*)
//LDAPVLVInfo local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl * local_parameter_2;

    RETURN_NUMBER(ldap_create_vlv_control((LDAP *)(uintptr_t)PARAM(0), UNWRAP_LDAPVLVInfo(PARAMETER(1)), &local_parameter_2))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_vlvresponse_control, 6)
    T_NUMBER(ldap_parse_vlvresponse_control, 0)     // LDAP*
    T_NUMBER(ldap_parse_vlvresponse_control, 1)     // LDAPControl*

// ... parameter 2 is by reference (ber_int_t*)
    ber_int_t local_parameter_2;
// ... parameter 3 is by reference (ber_int_t*)
    ber_int_t local_parameter_3;
// ... parameter 4 is by reference (berval**)
    berval *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_parse_vlvresponse_control((LDAP *)(uintptr_t)PARAM(0), (LDAPControl *)(long)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_whoami, 3)
    T_NUMBER(ldap_parse_whoami, 0)     // LDAP*
    T_NUMBER(ldap_parse_whoami, 1)     // LDAPMessage*

// ... parameter 2 is by reference (berval**)
    berval * local_parameter_2;

    RETURN_NUMBER(ldap_parse_whoami((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_whoami, 4)
    T_NUMBER(ldap_whoami, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;
// ... parameter 3 is by reference (int*)
    int local_parameter_3;

    RETURN_NUMBER(ldap_whoami((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_whoami_s, 4)
    T_NUMBER(ldap_whoami_s, 0)     // LDAP*

// ... parameter 1 is by reference (berval**)
    berval * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;

    RETURN_NUMBER(ldap_whoami_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_passwd, 3)
    T_NUMBER(ldap_parse_passwd, 0)     // LDAP*
    T_NUMBER(ldap_parse_passwd, 1)     // LDAPMessage*

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);

    RETURN_NUMBER(ldap_parse_passwd((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_passwd, 7)
    T_NUMBER(ldap_passwd, 0)     // LDAP*

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);
// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;
// ... parameter 6 is by reference (int*)
    int local_parameter_6;

    RETURN_NUMBER(ldap_passwd((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(1, (long)local_parameter_1)
//SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(1, local_parameter_1)
    SET_BERVAL(2, local_parameter_2)
    SET_BERVAL(3, local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_passwd_s, 7)
    T_NUMBER(ldap_passwd_s, 0)     // LDAP*

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);
// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (berval*)
    GET_DECL_BERVAL(4, local_parameter_4);
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;
// ... parameter 6 is by reference (LDAPControl**)
    LDAPControl *local_parameter_6;

    RETURN_NUMBER(ldap_passwd_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5, &local_parameter_6))
//SET_NUMBER(1, (long)local_parameter_1)
//SET_NUMBER(2, (long)local_parameter_2)
//SET_NUMBER(3, (long)local_parameter_3)
    SET_BERVAL(1, local_parameter_1)
    SET_BERVAL(2, local_parameter_2)
    SET_BERVAL(3, local_parameter_3)
    SET_BERVAL(4, local_parameter_4)
//SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
    SET_NUMBER(6, (long)local_parameter_6)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(ldap_create_passwordpolicy_control,2)
        T_NUMBER(ldap_passwd_s, 0) // LDAP*

        // ... parameter 1 is by reference (LDAPControl**)
        LDAPControl* local_parameter_1;

        RETURN_NUMBER(ldap_create_passwordpolicy_control((LDAP*)(long)PARAM(0), &local_parameter_1))
        SET_NUMBER(1, (long)local_parameter_1)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(ldap_parse_passwordpolicy_control,5)
        T_NUMBER(ldap_parse_passwordpolicy_control, 0) // LDAP*
        T_NUMBER(ldap_parse_passwordpolicy_control, 1) // LDAPControl*

        // ... parameter 2 is by reference (ber_int_t*)
        ber_int_t local_parameter_2;
        // ... parameter 3 is by reference (ber_int_t*)
        ber_int_t local_parameter_3;
        // ... parameter 4 is by reference (LDAPPasswordPolicyError*)
        LDAPPasswordPolicyError local_parameter_4;

        RETURN_NUMBER(ldap_parse_passwordpolicy_control((LDAP*)(long)PARAM(0), (LDAPControl*)(long)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4))
        SET_NUMBER(2, (long)local_parameter_2)
        SET_NUMBER(3, (long)local_parameter_3)
        SET_NUMBER(4, (long)local_parameter_4)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(ldap_passwordpolicy_err2txt,0)

        RETURN_STRING((char*)ldap_passwordpolicy_err2txt())
   END_IMPL
 */
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_refresh, 3)
    T_NUMBER(ldap_parse_refresh, 0)     // LDAP*
    T_NUMBER(ldap_parse_refresh, 1)     // LDAPMessage*

// ... parameter 2 is by reference (ber_int_t*)
    ber_int_t local_parameter_2;

    RETURN_NUMBER(ldap_parse_refresh((LDAP *)(uintptr_t)PARAM(0), (LDAPMessage *)(uintptr_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_refresh, 6)
    T_NUMBER(ldap_refresh, 0)     // LDAP*
    T_NUMBER(ldap_refresh, 2)     // ber_int_t

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_refresh((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, (ber_int_t)PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_BERVAL(1, local_parameter_1)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_refresh_s, 6)
    T_NUMBER(ldap_refresh_s, 0)     // LDAP*
    T_NUMBER(ldap_refresh_s, 2)     // ber_int_t

// ... parameter 1 is by reference (berval*)
    GET_DECL_BERVAL(1, local_parameter_1);
// ... parameter 3 is by reference (ber_int_t*)
    ber_int_t local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;

    RETURN_NUMBER(ldap_refresh_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, (ber_int_t)PARAM(2), &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(1, (long)local_parameter_1)
    SET_BERVAL(1, local_parameter_1)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
#ifdef LDAP_X_TXN
CONCEPT_FUNCTION_IMPL(ldap_txn_start, 4)
    T_NUMBER(ldap_txn_start, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;
// ... parameter 3 is by reference (int*)
    int local_parameter_3;

    RETURN_NUMBER(ldap_txn_start((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_txn_start_s, 4)
    T_NUMBER(ldap_txn_start_s, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPControl**)
    LDAPControl *local_parameter_2;
// ... parameter 3 is by reference (berval**)
    berval *local_parameter_3;

    RETURN_NUMBER(ldap_txn_start_s((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_txn_end, 6)
    T_NUMBER(ldap_txn_end, 0)     // LDAP*
    T_NUMBER(ldap_txn_end, 1)     // int

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_txn_end((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_txn_end_s, 6)
    T_NUMBER(ldap_txn_end_s, 0)     // LDAP*
    T_NUMBER(ldap_txn_end_s, 1)     // int

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl *local_parameter_3;
// ... parameter 4 is by reference (LDAPControl**)
    LDAPControl *local_parameter_4;
// ... parameter 5 is by reference (int*)
    int local_parameter_5;

    RETURN_NUMBER(ldap_txn_end_s((LDAP *)(uintptr_t)PARAM(0), (int)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
#endif
CONCEPT_FUNCTION_IMPL(ldap_sync_initialize, 0)

    ldap_sync_t * local_parameter_0 = (ldap_sync_t *)ldap_memalloc(sizeof(ldap_sync_t));

    RETURN_NUMBER((SYS_INT)ldap_sync_initialize(local_parameter_0))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sync_destroy, 2)
    T_NUMBER(ldap_sync_destroy, 1)     // int
    T_HANDLE(ldap_sync_destroy, 0)


    ldap_sync_destroy((ldap_sync_t *)PARAM_INT(0), (int)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sync_init, 2)
    T_NUMBER(ldap_sync_init, 1)     // int
    T_HANDLE(ldap_sync_init, 0)

    RETURN_NUMBER(ldap_sync_init((ldap_sync_t *)PARAM_INT(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sync_init_refresh_only, 1)
    T_HANDLE(ldap_sync_init_refresh_only, 0)

    RETURN_NUMBER(ldap_sync_init_refresh_only((ldap_sync_t *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sync_init_refresh_and_persist, 1)
    T_HANDLE(ldap_sync_init_refresh_and_persist, 0)

    RETURN_NUMBER(ldap_sync_init_refresh_and_persist((ldap_sync_t *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_sync_poll, 1)
    T_HANDLE(ldap_sync_poll, 0)

    RETURN_NUMBER(ldap_sync_poll((ldap_sync_t *)PARAM_INT(0)))
END_IMPL
//------------------------------------------------------------------------
#ifdef LDAP_CONTROL_X_SESSION_TRACKING
CONCEPT_FUNCTION_IMPL(ldap_create_session_tracking_value, 6)
    T_NUMBER(ldap_create_session_tracking_value, 0)     // LDAP*
    T_STRING(ldap_create_session_tracking_value, 1)     // char*
    T_STRING(ldap_create_session_tracking_value, 2)     // char*
    T_STRING(ldap_create_session_tracking_value, 3)     // char*

// ... parameter 4 is by reference (berval*)
    GET_DECL_BERVAL(4, local_parameter_4);
// ... parameter 5 is by reference (berval*)
    GET_DECL_BERVAL(5, local_parameter_5);

    RETURN_NUMBER(ldap_create_session_tracking_value((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), PARAM(3), &local_parameter_4, &local_parameter_5))
//SET_NUMBER(4, (long)local_parameter_4)
//SET_NUMBER(5, (long)local_parameter_5)
    SET_BERVAL(4, local_parameter_4)
    SET_BERVAL(5, local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_session_tracking, 6)
    T_NUMBER(ldap_create_session_tracking, 0)     // LDAP*
    T_STRING(ldap_create_session_tracking, 1)     // char*
    T_STRING(ldap_create_session_tracking, 2)     // char*
    T_STRING(ldap_create_session_tracking, 3)     // char*

// ... parameter 4 is by reference (berval*)
    GET_DECL_BERVAL(4, local_parameter_4);
// ... parameter 5 is by reference (LDAPControl**)
    LDAPControl *local_parameter_5;

    RETURN_NUMBER(ldap_create_session_tracking((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2), PARAM(3), &local_parameter_4, &local_parameter_5))
//SET_NUMBER(4, (long)local_parameter_4)
    SET_BERVAL(4, local_parameter_4)
    SET_NUMBER(5, (long)local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_session_tracking_control, 6)
    T_NUMBER(ldap_parse_session_tracking_control, 0)     // LDAP*
    T_NUMBER(ldap_parse_session_tracking_control, 1)     // LDAPControl*

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);
// ... parameter 3 is by reference (berval*)
    GET_DECL_BERVAL(3, local_parameter_3);
// ... parameter 4 is by reference (berval*)
    GET_DECL_BERVAL(4, local_parameter_4);
// ... parameter 5 is by reference (berval*)
    GET_DECL_BERVAL(5, local_parameter_5);

    RETURN_NUMBER(ldap_parse_session_tracking_control((LDAP *)(uintptr_t)PARAM(0), (LDAPControl *)(long)PARAM(1), &local_parameter_2, &local_parameter_3, &local_parameter_4, &local_parameter_5))

/*SET_NUMBER(2, (long)local_parameter_2)
   SET_NUMBER(3, (long)local_parameter_3)
   SET_NUMBER(4, (long)local_parameter_4)
   SET_NUMBER(5, (long)local_parameter_5)*/
    SET_BERVAL(2, local_parameter_2)
    SET_BERVAL(3, local_parameter_3)
    SET_BERVAL(4, local_parameter_4)
    SET_BERVAL(5, local_parameter_5)
END_IMPL
//------------------------------------------------------------------------
#endif
CONCEPT_FUNCTION_IMPL(ldap_create_assertion_control_value, 3)
    T_NUMBER(ldap_create_assertion_control_value, 0)     // LDAP*
    T_STRING(ldap_create_assertion_control_value, 1)     // char*

// ... parameter 2 is by reference (berval*)
    GET_DECL_BERVAL(2, local_parameter_2);

    RETURN_NUMBER(ldap_create_assertion_control_value((LDAP *)(uintptr_t)PARAM(0), PARAM(1), &local_parameter_2))
//SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_create_assertion_control, 4)
    T_NUMBER(ldap_create_assertion_control, 0)     // LDAP*
    T_STRING(ldap_create_assertion_control, 1)     // char*
    T_NUMBER(ldap_create_assertion_control, 2)     // int

// ... parameter 3 is by reference (LDAPControl**)
    LDAPControl * local_parameter_3;

    RETURN_NUMBER(ldap_create_assertion_control((LDAP *)(uintptr_t)PARAM(0), PARAM(1), (int)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(ldap_create_deref_control_value,3)
        T_NUMBER(ldap_create_assertion_control, 0) // LDAP*

        // ... parameter 1 is by reference (LDAPDerefSpec*)
        LDAPDerefSpec local_parameter_1;
        // ... parameter 2 is by reference (berval*)
        GET_DECL_BERVAL(2,local_parameter_2);

        RETURN_NUMBER(ldap_create_deref_control_value((LDAP*)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
        SET_NUMBER(1, (long)local_parameter_1)
        //SET_NUMBER(2, (long)local_parameter_2)
    SET_BERVAL(2, local_parameter_2)
   END_IMPL
   //------------------------------------------------------------------------
   CONCEPT_FUNCTION_IMPL(ldap_create_deref_control,4)
        T_NUMBER(ldap_create_deref_control, 0) // LDAP*
        T_NUMBER(ldap_create_deref_control, 2) // int

        // ... parameter 1 is by reference (LDAPDerefSpec*)
        LDAPDerefSpec local_parameter_1;
        // ... parameter 3 is by reference (LDAPControl**)
        LDAPControl* local_parameter_3;

        RETURN_NUMBER(ldap_create_deref_control((LDAP*)(long)PARAM(0), &local_parameter_1, (int)PARAM(2), &local_parameter_3))
        SET_NUMBER(1, (long)local_parameter_1)
        SET_NUMBER(3, (long)local_parameter_3)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_derefresponse_free, 1)
    T_NUMBER(ldap_derefresponse_free, 0)     // LDAPDerefRes*

    ldap_derefresponse_free((LDAPDerefRes *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_derefresponse_control, 3)
    T_NUMBER(ldap_parse_derefresponse_control, 0)     // LDAP*
    T_NUMBER(ldap_parse_derefresponse_control, 1)     // LDAPControl*

// ... parameter 2 is by reference (LDAPDerefRes**)
    LDAPDerefRes * local_parameter_2;

    RETURN_NUMBER(ldap_parse_derefresponse_control((LDAP *)(uintptr_t)PARAM(0), (LDAPControl *)(long)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_parse_deref_control, 3)
    T_NUMBER(ldap_parse_deref_control, 0)     // LDAP*

// ... parameter 1 is by reference (LDAPControl**)
    LDAPControl * local_parameter_1;
// ... parameter 2 is by reference (LDAPDerefRes**)
    LDAPDerefRes *local_parameter_2;

    RETURN_NUMBER(ldap_parse_deref_control((LDAP *)(uintptr_t)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_simple_bind_s, 3)
    T_HANDLE(ldap_simple_bind_s, 0)     // LDAP*
    T_STRING(ldap_simple_bind_s, 1)     // char*
    T_STRING(ldap_simple_bind_s, 2)     // char*

    RETURN_NUMBER(ldap_simple_bind_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_simple_bind, 3)
    T_HANDLE(ldap_simple_bind_s, 0)     // LDAP*
    T_STRING(ldap_simple_bind_s, 1)     // char*
    T_STRING(ldap_simple_bind_s, 2)     // char*

    RETURN_NUMBER(ldap_simple_bind((LDAP *)(uintptr_t)PARAM(0), PARAM(1), PARAM(2)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_search, 6)
    T_HANDLE(ldap_search, 0)     // LDAP*
    T_STRING(ldap_search, 1)     // char*
    T_NUMBER(ldap_search, 2)     // int
    T_STRING(ldap_search, 3)     // char*
    T_NUMBER(ldap_search, 4)

    LDAPMessage *msg = NULL;
    char *filter = PARAM(3);
    if ((filter) && (!filter[0]))
        filter = NULL;
    RETURN_NUMBER(ldap_search_ext_s((LDAP *)(uintptr_t)PARAM(0), PARAM(1), (int)PARAM(2), filter, NULL, 0, NULL, NULL, NULL, PARAM_INT(4), &msg));
    SET_NUMBER(5, (long)msg)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_done, 1)
    T_HANDLE(ldap_done, 0)
    ldap_unbind_ext_s((LDAP *)(uintptr_t)PARAM(0), NULL, NULL);
    SET_NUMBER(0, 0);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ldap_array, 2)
    T_HANDLE(ldap_to_array, 0)
    T_HANDLE(ldap_to_array, 1)

    LDAP *ld = (LDAP *)(uintptr_t)PARAM(0);
    LDAPMessage *msg = (LDAPMessage *)(uintptr_t)PARAM(1);
    CREATE_ARRAY(RESULT);
    
    LDAPMessage  *e = ldap_first_entry(ld, msg);
    BerElement *ber = NULL;
    INTEGER index = 0;
    while (e) {
        void *record = NULL;
        Invoke(INVOKE_ARRAY_VARIABLE, RESULT, index++, &record);
        CREATE_ARRAY(record);
        for (char *a = ldap_first_attribute(ld, e, &ber); a != NULL; a = ldap_next_attribute(ld, e, ber)) { 
            char **vals = ldap_get_values(ld, e, a);
            if ((vals != NULL) && (vals[0]))  {
                if (!vals[1]) {
                    InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, record, a, (INTEGER)VARIABLE_STRING, (char *)vals[0], (NUMBER)0);
                } else {
                    void *values = NULL;
                    Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, record, a, &values);
                    CREATE_ARRAY(values);
                    for (INTEGER j = 0; vals[j] != NULL; j++) {
                        InvokePtr(INVOKE_SET_ARRAY_ELEMENT, values, j, (INTEGER)VARIABLE_STRING, (char *)vals[j], (NUMBER)0);
                    }
                }
            } else
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, record, a, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)0);
            if (vals)
                ldap_value_free(vals);
            ldap_memfree(a);
        }
        if (ber)
            ber_free(ber, 0); //  or ldap_ber_free
        e = ldap_next_entry(ld, e);
    }
END_IMPL
//------------------------------------------------------------------------
