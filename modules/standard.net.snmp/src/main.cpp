//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/definitions.h>
#include <net-snmp/library/tools.h>
#include <net-snmp/mib_api.h>
#include <net-snmp/library/mib.h>
#include <net-snmp/library/parse.h>
#include <net-snmp/library/snmp_impl.h>
#include <net-snmp/utilities.h>
//#include <net-snmp/library/cmu_compat.h>

//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
AnsiString  LastError;
//-----------------------------------------------------//
#ifdef _WIN32
AnsiString GetDirectory() {
    char buffer[4096];

    buffer[0] = 0;
    GetModuleFileName(NULL, buffer, 4096);
    for (int i = strlen(buffer) - 1; i >= 0; i--) {
        if ((buffer[i] == '/') || (buffer[i] == '\\')) {
            buffer[i + 1] = 0;
            break;
        }
    }
    return AnsiString(buffer);
}
#endif

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    AnsiString static_path = GetDirectory() + (char *)"..\\share\\snmp\\mibs";
    setenv("MIBDIRS", static_path.c_str(), 0);
#endif
    SOCK_STARTUP;
    DEFINE_ECONSTANT(NETSNMP_DS_MAX_IDS)
    DEFINE_ECONSTANT(NETSNMP_DS_MAX_SUBIDS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIBRARY_ID)
    DEFINE_ECONSTANT(NETSNMP_DS_APPLICATION_ID)
    DEFINE_ECONSTANT(NETSNMP_DS_TOKEN_ID)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIB_ERRORS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SAVE_MIB_DESCRS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIB_COMMENT_TERM)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIB_PARSE_LABEL)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DUMP_PACKET)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_LOG_TIMESTAMP)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_READ_CONFIGS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DISABLE_CONFIG_LOAD)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIB_REPLACE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_NUMERIC_OIDS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_BREAKDOWN_OIDS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_ALARM_DONT_USE_SIG)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_FULL_OID)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_QUICK_PRINT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_RANDOM_ACCESS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_REGEX_ACCESS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_CHECK_RANGE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_NO_TOKEN_WARNINGS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_NUMERIC_TIMETICKS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_ESCAPE_QUOTES)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_REVERSE_ENCODE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_BARE_VALUE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_EXTENDED_INDEX)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_HEX_TEXT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_UCD_STYLE_OID)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_READ_UCD_STYLE_OID)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_HAVE_READ_PREMIB_CONFIG)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_HAVE_READ_CONFIG)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_QUICKE_PRINT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_PRINT_UNITS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_NO_DISPLAY_HINT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_16BIT_IDS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_PERSIST_STATE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_2DIGIT_HEX_OUTPUT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_IGNORE_NO_COMMUNITY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DISABLE_PERSISTENT_LOAD)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DISABLE_PERSISTENT_SAVE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_APPEND_LOGFILES)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_NO_DISCOVERY)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_TSM_USE_PREFIX)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_DONT_LOAD_HOST_FILES)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIB_WARNINGS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SECLEVEL)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SNMPVERSION)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_DEFAULT_PORT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_OID_OUTPUT_FORMAT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRINT_SUFFIX_ONLY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_STRING_OUTPUT_FORMAT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_HEX_OUTPUT_LENGTH)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SERVERSENDBUF)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SERVERRECVBUF)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_CLIENTSENDBUF)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_CLIENTRECVBUF)
    //DEFINE_ECONSTANT(NETSNMP_DS_SSHDOMAIN_SOCK_PERM)
    //DEFINE_ECONSTANT(NETSNMP_DS_SSHDOMAIN_DIR_PERM)
    //DEFINE_ECONSTANT(NETSNMP_DS_SSHDOMAIN_SOCK_USER)
    //DEFINE_ECONSTANT(NETSNMP_DS_SSHDOMAIN_SOCK_GROUP)
#ifndef NETSNMP_DISABLE_SNMPV1
    DEFINE_ECONSTANT(NETSNMP_DS_SNMP_VERSION_1)
#endif
#ifndef NETSNMP_DISABLE_SNMPV2C
    DEFINE_ECONSTANT(NETSNMP_DS_SNMP_VERSION_2c)
#endif
    DEFINE_ECONSTANT(NETSNMP_DS_SNMP_VERSION_3)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SECNAME)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_CONTEXT)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PASSPHRASE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_AUTHPASSPHRASE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRIVPASSPHRASE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_OPTIONALCONFIG)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_APPTYPE)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_COMMUNITY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PERSISTENT_DIR)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_CONFIGURATION_DIR)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_SECMODEL)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_MIBDIRS)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_OIDSUFFIX)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_OIDPREFIX)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_CLIENT_ADDR)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_TEMP_FILE_PATTERN)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_AUTHMASTERKEY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRIVMASTERKEY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_AUTHLOCALIZEDKEY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_PRIVLOCALIZEDKEY)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_APPTYPES)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_KSM_KEYTAB)
    DEFINE_ECONSTANT(NETSNMP_DS_LIB_KSM_SERVICE_NAME)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_X509_CLIENT_PUB)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_X509_SERVER_PUB)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_SSHTOSNMP_SOCKET)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_CERT_EXTRA_SUBDIR)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_HOSTNAME)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_X509_CRL_FILE)
    //DEFINE_ECONSTANT(NETSNMP_DS_LIB_TLS_ALGORITMS)

    DEFINE_ECONSTANT(MAX_CALLBACK_IDS)
    DEFINE_ECONSTANT(MAX_CALLBACK_SUBIDS)
    DEFINE_ECONSTANT(SNMP_CALLBACK_LIBRARY)
    DEFINE_ECONSTANT(SNMP_CALLBACK_APPLICATION)
    DEFINE_ECONSTANT(SNMP_CALLBACK_POST_READ_CONFIG)
    DEFINE_ECONSTANT(SNMP_CALLBACK_STORE_DATA)
    DEFINE_ECONSTANT(SNMP_CALLBACK_SHUTDOWN)
    DEFINE_ECONSTANT(SNMP_CALLBACK_POST_PREMIB_READ_CONFIG)
    DEFINE_ECONSTANT(SNMP_CALLBACK_LOGGING)
    DEFINE_ECONSTANT(SNMP_CALLBACK_SESSION_INIT)
    //DEFINE_ECONSTANT(SNMP_CALLBACK_PRE_READ_CONFIG)
    //DEFINE_ECONSTANT(SNMP_CALLBACK_PRE_PREMIB_READ_CONFIG)
    DEFINE_ECONSTANT(NETSNMP_CALLBACK_HIGHEST_PRIORITY)
    DEFINE_ECONSTANT(NETSNMP_CALLBACK_DEFAULT_PRIORITY)
    DEFINE_ECONSTANT(NETSNMP_CALLBACK_LOWEST_PRIORITY)


    DEFINE_ECONSTANT(SNMP_PORT)
    DEFINE_ECONSTANT(SNMP_TRAP_PORT)
    DEFINE_ECONSTANT(SNMP_MAX_LEN)
    DEFINE_ECONSTANT(SNMP_MIN_MAX_LEN)
#ifndef NETSNMP_DISABLE_SNMPV1
    DEFINE_ECONSTANT(SNMP_VERSION_1)
#endif
#ifndef NETSNMP_DISABLE_SNMPV2C
    DEFINE_ECONSTANT(SNMP_VERSION_2c)
#endif
    DEFINE_ECONSTANT(SNMP_VERSION_2u)
    DEFINE_ECONSTANT(SNMP_VERSION_3)
    DEFINE_ECONSTANT(SNMP_VERSION_sec)
    DEFINE_ECONSTANT(SNMP_VERSION_2p)
    DEFINE_ECONSTANT(SNMP_VERSION_2star)
    DEFINE_ECONSTANT(SNMP_MSG_GET)
    DEFINE_ECONSTANT(SNMP_MSG_GETNEXT)
    DEFINE_ECONSTANT(SNMP_MSG_RESPONSE)
    DEFINE_ECONSTANT(SNMP_MSG_SET)
    DEFINE_ECONSTANT(SNMP_MSG_TRAP)
    DEFINE_ECONSTANT(SNMP_MSG_GETBULK)
    DEFINE_ECONSTANT(SNMP_MSG_INFORM)
    DEFINE_ECONSTANT(SNMP_MSG_TRAP2)
    DEFINE_ECONSTANT(SNMP_MSG_REPORT)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_BEGIN)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_RESERVE1)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_RESERVE2)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_ACTION)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_COMMIT)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_FREE)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_UNDO)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_MAX)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_CHECK_VALUE)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_ROW_CREATE)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_UNDO_SETUP)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_SET_VALUE)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_CHECK_CONSISTENCY)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_UNDO_SET)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_COMMIT)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_UNDO_COMMIT)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_IRREVERSIBLE_COMMIT)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_UNDO_CLEANUP)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_PRE_REQUEST)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_OBJECT_LOOKUP)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_POST_REQUEST)
    DEFINE_ECONSTANT(SNMP_MSG_INTERNAL_GET_STASH)
    DEFINE_ECONSTANT(SNMP_NOSUCHOBJECT)
    DEFINE_ECONSTANT(SNMP_NOSUCHINSTANCE)
    DEFINE_ECONSTANT(SNMP_ENDOFMIBVIEW)
    DEFINE_ECONSTANT(SNMP_ERR_NOERROR)
    DEFINE_ECONSTANT(SNMP_ERR_TOOBIG)
    DEFINE_ECONSTANT(SNMP_ERR_NOSUCHNAME)
    DEFINE_ECONSTANT(SNMP_ERR_BADVALUE)
    DEFINE_ECONSTANT(SNMP_ERR_READONLY)
    DEFINE_ECONSTANT(SNMP_ERR_GENERR)
    DEFINE_ECONSTANT(SNMP_ERR_NOACCESS)
    DEFINE_ECONSTANT(SNMP_ERR_WRONGTYPE)
    DEFINE_ECONSTANT(SNMP_ERR_WRONGLENGTH)
    DEFINE_ECONSTANT(SNMP_ERR_WRONGENCODING)
    DEFINE_ECONSTANT(SNMP_ERR_WRONGVALUE)
    DEFINE_ECONSTANT(SNMP_ERR_NOCREATION)
    DEFINE_ECONSTANT(SNMP_ERR_INCONSISTENTVALUE)
    DEFINE_ECONSTANT(SNMP_ERR_RESOURCEUNAVAILABLE)
    DEFINE_ECONSTANT(SNMP_ERR_COMMITFAILED)
    DEFINE_ECONSTANT(SNMP_ERR_UNDOFAILED)
    DEFINE_ECONSTANT(SNMP_ERR_AUTHORIZATIONERROR)
    DEFINE_ECONSTANT(SNMP_ERR_NOTWRITABLE)
    DEFINE_ECONSTANT(SNMP_ERR_INCONSISTENTNAME)
    DEFINE_ECONSTANT(MAX_SNMP_ERR)
    DEFINE_ECONSTANT(SNMP_TRAP_COLDSTART)
    DEFINE_ECONSTANT(SNMP_TRAP_WARMSTART)
    DEFINE_ECONSTANT(SNMP_TRAP_LINKDOWN)
    DEFINE_ECONSTANT(SNMP_TRAP_LINKUP)
    DEFINE_ECONSTANT(SNMP_TRAP_AUTHFAIL)
    DEFINE_ECONSTANT(SNMP_TRAP_EGPNEIGHBORLOSS)
    DEFINE_ECONSTANT(SNMP_TRAP_ENTERPRISESPECIFIC)
    DEFINE_ECONSTANT(SNMP_ROW_NONEXISTENT)
    DEFINE_ECONSTANT(SNMP_ROW_ACTIVE)
    DEFINE_ECONSTANT(SNMP_ROW_NOTINSERVICE)
    DEFINE_ECONSTANT(SNMP_ROW_NOTREADY)
    DEFINE_ECONSTANT(SNMP_ROW_CREATEANDGO)
    DEFINE_ECONSTANT(SNMP_ROW_CREATEANDWAIT)
    DEFINE_ECONSTANT(SNMP_ROW_DESTROY)
    DEFINE_ECONSTANT(SNMP_STORAGE_NONE)
    DEFINE_ECONSTANT(SNMP_STORAGE_OTHER)
    DEFINE_ECONSTANT(SNMP_STORAGE_VOLATILE)
    DEFINE_ECONSTANT(SNMP_STORAGE_NONVOLATILE)
    DEFINE_ECONSTANT(SNMP_STORAGE_PERMANENT)
    DEFINE_ECONSTANT(SNMP_STORAGE_READONLY)
    DEFINE_ECONSTANT(SNMP_MP_MODEL_SNMPv1)
    DEFINE_ECONSTANT(SNMP_MP_MODEL_SNMPv2c)
    DEFINE_ECONSTANT(SNMP_MP_MODEL_SNMPv2u)
    DEFINE_ECONSTANT(SNMP_MP_MODEL_SNMPv3)
    DEFINE_ECONSTANT(SNMP_MP_MODEL_SNMPv2p)
    DEFINE_ECONSTANT(SNMP_SEC_MODEL_ANY)
    DEFINE_ECONSTANT(SNMP_SEC_MODEL_SNMPv1)
    DEFINE_ECONSTANT(SNMP_SEC_MODEL_SNMPv2c)
    DEFINE_ECONSTANT(SNMP_SEC_MODEL_USM)
    //DEFINE_ECONSTANT(SNMP_SEC_MODEL_TSM)
    DEFINE_ECONSTANT(SNMP_SEC_MODEL_SNMPv2p)
    DEFINE_ECONSTANT(SNMP_SEC_LEVEL_NOAUTH)
    DEFINE_ECONSTANT(SNMP_SEC_LEVEL_AUTHNOPRIV)
    DEFINE_ECONSTANT(SNMP_SEC_LEVEL_AUTHPRIV)
    DEFINE_ECONSTANT(SNMP_MSG_FLAG_AUTH_BIT)
    DEFINE_ECONSTANT(SNMP_MSG_FLAG_PRIV_BIT)
    DEFINE_ECONSTANT(SNMP_MSG_FLAG_RPRT_BIT)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_RESPONSE_PDU)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_EXPECT_RESPONSE)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_FORCE_PDU_COPY)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_ALWAYS_IN_VIEW)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_PDU_TIMEOUT)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_ONE_PASS_ONLY)
    DEFINE_ECONSTANT(UCD_MSG_FLAG_TUNNELED)
    DEFINE_ECONSTANT(SNMP_VIEW_INCLUDED)
    DEFINE_ECONSTANT(SNMP_VIEW_EXCLUDED)
    //DEFINE_ECONSTANT(SNMP_OID_INTERNET)
    //DEFINE_ECONSTANT(SNMP_OID_ENTERPRISES)
    //DEFINE_ECONSTANT(SNMP_OID_MIB2)
    //DEFINE_ECONSTANT(SNMP_OID_SNMPV2)
    //DEFINE_ECONSTANT(SNMP_OID_SNMPMODULES)
    DEFINE_ECONSTANT(SNMPADMINLENGTH)

    DEFINE_ECONSTANT(STAT_SUCCESS)
    DEFINE_ECONSTANT(STAT_ERROR)
    DEFINE_ECONSTANT(STAT_TIMEOUT)

    DEFINE_ECONSTANT(ASN_BOOLEAN)
    DEFINE_ECONSTANT(ASN_INTEGER)
    DEFINE_ECONSTANT(ASN_BIT_STR)
    DEFINE_ECONSTANT(ASN_OCTET_STR)
    DEFINE_ECONSTANT(ASN_NULL)
    DEFINE_ECONSTANT(ASN_OBJECT_ID)
    DEFINE_ECONSTANT(ASN_SEQUENCE)
    DEFINE_ECONSTANT(ASN_SET)

    DEFINE_ECONSTANT(ASN_UNIVERSAL)
    DEFINE_ECONSTANT(ASN_APPLICATION)
    DEFINE_ECONSTANT(ASN_CONTEXT)
    DEFINE_ECONSTANT(ASN_PRIVATE)

    DEFINE_ECONSTANT(ASN_PRIMITIVE)
    DEFINE_ECONSTANT(ASN_CONSTRUCTOR)

    DEFINE_ECONSTANT(ASN_LONG_LEN)
    DEFINE_ECONSTANT(ASN_EXTENSION_ID)
    DEFINE_ECONSTANT(ASN_BIT8)

    DEFINE_ECONSTANT(ASN_APP_OPAQUE)
    DEFINE_ECONSTANT(ASN_APP_COUNTER64)
    DEFINE_ECONSTANT(ASN_APP_FLOAT)
    DEFINE_ECONSTANT(ASN_APP_DOUBLE)
    DEFINE_ECONSTANT(ASN_APP_I64)
    DEFINE_ECONSTANT(ASN_APP_U64)
    DEFINE_ECONSTANT(ASN_APP_UNION)

    //DEFINE_ECONSTANT(SMI_NOSUCHOBJECT)
    //DEFINE_ECONSTANT(SMI_NOSUCHINSTANCE)
    //DEFINE_ECONSTANT(SMI_ENDOFMIBVIEW)

    //DEFINE_ECONSTANT(SNMP_MSG_GET GET_REQ_MSG)
    //DEFINE_ECONSTANT(SNMP_MSG_GETNEXT GETNEXT_REQ_MSG)
    //DEFINE_ECONSTANT(SNMP_MSG_RESPONSE GET_RSP_MSG)
    //DEFINE_ECONSTANT(SNMP_MSG_SET SET_REQ_MSG)
    //DEFINE_ECONSTANT(SNMP_MSG_TRAP TRP_REQ_MSG)
    //DEFINE_ECONSTANT(SNMP_MSG_GETBULK)
    //DEFINE_ECONSTANT(SNMP_MSG_INFORM)
    //DEFINE_ECONSTANT(SNMP_MSG_TRAP2)
    //DEFINE_ECONSTANT(SNMP_MSG_REPORT)
    //DEFINE_ECONSTANT(SNMP_NOSUCHOBJECT)
    //DEFINE_ECONSTANT(SNMP_NOSUCHINSTANCE)
    //DEFINE_ECONSTANT(SNMP_ENDOFMIBVIEW)

    //DEFINE_ECONSTANT(ASN_IPADDRESS)
    //DEFINE_ECONSTANT(ASN_UNSIGNED)
    //DEFINE_ECONSTANT(ASN_TIMETICKS)

    /*DEFINE_ECONSTANT(ACCESS_READONLY)
       DEFINE_ECONSTANT(ACCESS_READWRITE)
       DEFINE_ECONSTANT(ACCESS_WRITEONLY)
       DEFINE_ECONSTANT(ACCESS_NOACCESS)
       DEFINE_ECONSTANT(ACCESS_NOTIFY)
       DEFINE_ECONSTANT(ACCESS_CREATE)
       DEFINE_ECONSTANT(STATUS_MANDATORY)
       DEFINE_ECONSTANT(STATUS_OPTIONAL)
       DEFINE_ECONSTANT(STATUS_OBSOLETE)
       DEFINE_ECONSTANT(STATUS_DEPRECATED)
       DEFINE_ECONSTANT(STATUS_CURRENT)

       DEFINE_ECONSTANT(INTEGER)
       DEFINE_ECONSTANT(STRING)
       DEFINE_ECONSTANT(OBJID)
       DEFINE_ECONSTANT(NULLOBJ)
       DEFINE_ECONSTANT(BITSTRING)
       DEFINE_ECONSTANT(IPADDRESS)
       DEFINE_ECONSTANT(COUNTER)
       DEFINE_ECONSTANT(GAUGE)
       DEFINE_ECONSTANT(UNSIGNED)
       DEFINE_ECONSTANT(TIMETICKS)
       DEFINE_ECONSTANT(ASNT_OPAQUE)
       DEFINE_ECONSTANT(NSAP)
       DEFINE_ECONSTANT(COUNTER64)
       DEFINE_ECONSTANT(UINTEGER)

       DEFINE_ECONSTANT(GET_REQ_MSG)
       DEFINE_ECONSTANT(GETNEXT_REQ_MSG)
       DEFINE_ECONSTANT(GET_RSP_MSG)
       DEFINE_ECONSTANT(SET_REQ_MSG)
       DEFINE_ECONSTANT(TRP_REQ_MSG)
       DEFINE_ECONSTANT(BULK_REQ_MSG)
       DEFINE_ECONSTANT(INFORM_REQ_MSG)
       DEFINE_ECONSTANT(TRP2_REQ_MSG)
       DEFINE_ECONSTANT(REPORT_RSP_MSG)

       DEFINE_ECONSTANT(SNMP_PDU_GET)
       DEFINE_ECONSTANT(SNMP_PDU_GETNEXT)
       DEFINE_ECONSTANT(SNMP_PDU_RESPONSE)
       DEFINE_ECONSTANT(SNMP_PDU_SET)
       DEFINE_ECONSTANT(SNMP_PDU_GETBULK)
       DEFINE_ECONSTANT(SNMP_PDU_INFORM)
       DEFINE_ECONSTANT(SNMP_PDU_V2TRAP)
       DEFINE_ECONSTANT(SNMP_PDU_REPORT)
       DEFINE_ECONSTANT(SNMP_TRAP_AUTHENTICATIONFAILURE)
       DEFINE_ECONSTANT(SMI_INTEGER)
       DEFINE_ECONSTANT(SMI_STRING)
       DEFINE_ECONSTANT(SMI_OBJID)
       DEFINE_ECONSTANT(SMI_NULLOBJ)
       DEFINE_ECONSTANT(SMI_IPADDRESS)
       DEFINE_ECONSTANT(SMI_COUNTER32)
       DEFINE_ECONSTANT(SMI_GAUGE32)
       DEFINE_ECONSTANT(SMI_UNSIGNED32)
       DEFINE_ECONSTANT(SMI_TIMETICKS)
       DEFINE_ECONSTANT(SMI_OPAQUE)
       DEFINE_ECONSTANT(SMI_COUNTER64)*/
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        shutdown_mib();
        netsnmp_ds_shutdown();
        SOCK_CLEANUP;
    }
    return 0;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_init_mib, 0)
    shutdown_mib();
    netsnmp_init_mib();
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(shutdown_mib, 0)
    shutdown_mib();
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(add_mibdir, 1)
    T_STRING(0) // pq
    int res = add_mibdir(PARAM(0));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
void do_tree(void *PARENT, INVOKE_CALL Invoke, struct tree *tree_head) {
    void *RESULT = 0;
    int  count   = Invoke(INVOKE_GET_ARRAY_COUNT, PARENT);

    for (struct tree *tp = tree_head; tp; tp = tp->next_peer) {
        Invoke(INVOKE_ARRAY_VARIABLE, PARENT, (INTEGER)count, &RESULT);
        CREATE_ARRAY(RESULT);
        count++;

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "label", (INTEGER)VARIABLE_STRING, (char *)tp->label, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "augments", (INTEGER)VARIABLE_STRING, (char *)tp->augments, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "hint", (INTEGER)VARIABLE_STRING, (char *)tp->hint, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "units", (INTEGER)VARIABLE_STRING, (char *)tp->units, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "description", (INTEGER)VARIABLE_STRING, (char *)tp->description, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "reference", (INTEGER)VARIABLE_STRING, (char *)tp->reference, (NUMBER)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "defaultValue", (INTEGER)VARIABLE_STRING, (char *)tp->defaultValue, (NUMBER)0);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "subid", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->subid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "modid", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->modid);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "number_modules", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->number_modules);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "tc_index", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->tc_index);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "type", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->type);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "access", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->access);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "status", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->status);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "reported", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)tp->reported);

        if (tp->module_list) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"module_list", &RESULT2);
            CREATE_ARRAY(RESULT2);
            int i     = 0;
            int index = tp->module_list[i];
            while (index) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT2, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)index);
                index = tp->module_list[++i];
            }
        }

        if (tp->enums) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"enums", &RESULT2);
            CREATE_ARRAY(RESULT2);
            struct enum_list *p = tp->enums;
            while (p) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT2, p->label, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)p->value);
                p = p->next;
            }
        }

        if (tp->ranges) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"ranges", &RESULT2);
            CREATE_ARRAY(RESULT2);
            struct range_list *p = tp->ranges;
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT2, "low", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)p->low);
            p = p->next;
            if (p)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT2, "high", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)p->high);
        }

        if (tp->indexes) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"indexes", &RESULT2);
            CREATE_ARRAY(RESULT2);
            struct index_list *p = tp->indexes;
            while (p) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT2, p->ilabel, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)p->isimplied);
                p = p->next;
            }
        }

        if (tp->varbinds) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"varbinds", &RESULT2);
            CREATE_ARRAY(RESULT2);
            struct varbind_list *p = tp->varbinds;
            while (p) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT2, p->vblabel, (INTEGER)VARIABLE_STRING, (char *)p->vblabel, (NUMBER)0);
                p = p->next;
            }
        }

        if (tp->child_list) {
            void *RESULT2 = 0;
            Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, (char *)"children", &RESULT2);

            CREATE_ARRAY(RESULT2);
            do_tree(RESULT2, Invoke, tp->child_list);
        }
    }
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(read_mib, 1)
    T_STRING(0) // pq
    struct tree *res = read_mib(PARAM(0));
    CREATE_ARRAY(RESULT);
    do_tree(RESULT, Invoke, res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(snmp_error, 1, 3)
    T_HANDLE(0)

    netsnmp_session * ss = (netsnmp_session *)PARAM_INT(0);
    int  p_clib_errorno = 0;
    int  p_snmp_errorno = 0;
    char *p_errstring   = 0;

    snmp_error(ss, &p_clib_errorno, &p_snmp_errorno, &p_errstring);

    if (p_errstring) {
        RETURN_STRING(p_errstring);
    } else {
        RETURN_STRING("");
    }
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, p_clib_errorno);
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, p_snmp_errorno);
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(snmp_sess_error, 1, 3)
    T_HANDLE(0)

    netsnmp_session * ss = (netsnmp_session *)PARAM_INT(0);
    int        p_clib_errorno = 0;
    int        p_snmp_errorno = 0;
    const char *p_errstring   = 0;

    if ((ss->s_snmp_errno) || (ss->s_errno)) {
        p_clib_errorno = ss->s_errno;
        p_snmp_errorno = ss->s_snmp_errno;
        if (p_snmp_errorno)
            p_errstring = snmp_api_errstring(p_snmp_errorno);
        else
            p_errstring = snmp_errstring(p_clib_errorno);
        //snmp_sess_error(ss, &p_clib_errorno, &p_snmp_errorno, &p_errstring);
    }
//    crashes on some platforms
//    snmp_sess_error(ss, &p_clib_errorno, &p_snmp_errorno, &p_errstring);

    if (p_errstring) {
        RETURN_STRING(p_errstring);
    } else {
        RETURN_STRING("");
    }
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, p_clib_errorno);
        if (PARAMETERS_COUNT > 2) {
            SET_NUMBER(2, p_snmp_errorno);
        }
    }
END_IMPL
//-----------------------------------------------------//
static void optProc(int argc, char *const *argv, int opt) {
    // nothing
}

CONCEPT_FUNCTION_IMPL(snmp_open, 1)
    T_ARRAY(0)
    static netsnmp_session session; //=(netsnmp_session *)malloc(sizeof(netsnmp_session));
    netsnmp_session *ss = 0;

    char **argv = GetCharList(PARAMETER(0), Invoke);
    int  argc   = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(0));
    snmp_sess_init(&session);
    snmp_parse_args(argc, argv, &session, "C:", optProc);
    delete[] argv;

    ss = snmp_open(&session);
    if (!ss) {
        RETURN_NUMBER(0);
        return 0;
    }
    RETURN_NUMBER((SYS_INT)ss);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_close, 1)
    T_HANDLE(0)
    netsnmp_session * ss = (netsnmp_session *)PARAM_INT(0);

    RETURN_NUMBER(0);
    if (ss) {
        SET_NUMBER(0, 0)
        snmp_close(ss);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_set_string, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_STRING(2)

    int res = netsnmp_ds_set_string(PARAM_INT(0), PARAM_INT(1), PARAM(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_set_int, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    int res = netsnmp_ds_set_int(PARAM_INT(0), PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_set_boolean, 3)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)

    int res = netsnmp_ds_set_boolean(PARAM_INT(0), PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_get_string, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    char *str = netsnmp_ds_get_string(PARAM_INT(0), PARAM_INT(1));
    if (str) {
        RETURN_STRING(str);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_get_int, 3)
    T_NUMBER(0)
    T_NUMBER(1)

    int res = netsnmp_ds_get_int(PARAM_INT(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_get_boolean, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    int res = netsnmp_ds_get_boolean(PARAM_INT(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(netsnmp_ds_toggle_boolean, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    int res = netsnmp_ds_toggle_boolean(PARAM_INT(0), PARAM_INT(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_pdu_create, 1)
    T_NUMBER(0)

    snmp_pdu * res = snmp_pdu_create(PARAM_INT(0));
    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_free_pdu, 1)
    T_HANDLE(0)
    snmp_pdu * ss = (snmp_pdu *)PARAM_INT(0);

    RETURN_NUMBER(0);
    if (ss) {
        SET_NUMBER(0, 0)
        snmp_free_pdu(ss);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_fix_pdu, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    snmp_pdu * ss = (snmp_pdu *)PARAM_INT(0);
    snmp_pdu *res = 0;

    if (ss) {
        res = snmp_fix_pdu(ss, PARAM_INT(1));
    }
    RETURN_NUMBER((SYS_INT)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_clone_pdu, 1)
    T_HANDLE(0)
    snmp_pdu * ss = (snmp_pdu *)PARAM_INT(0);
    snmp_pdu *res = 0;

    if (ss) {
        res = snmp_clone_pdu(ss);
    }
    RETURN_NUMBER((SYS_INT)res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_synch_response, 3)
    T_HANDLE(0)
    T_HANDLE(1)
    SET_NUMBER(2, 0)

    netsnmp_session * ss = (netsnmp_session *)PARAM_INT(0);
    snmp_pdu *pdu      = (snmp_pdu *)PARAM_INT(1);
    snmp_pdu *response = 0;

    int res    = -1;
    int go_out = 0;
    if (ss) {
        res = snmp_synch_response(ss, pdu, &response);
        if (response) {
            void *arr = PARAMETER(2);
            CREATE_ARRAY(arr);

            for (netsnmp_variable_list *vars = response->variables; vars; vars = vars->next_variable) {
                char name[0xFFF];
                char buf[0xFFFF];
                name[0]     = 0;
                name[0xFFE] = 0;
                buf[0]      = 0;
                buf[0xFFFE] = 0;
                int printed_name = snprint_objid(name, 0xFFE, vars->name, vars->name_length);
                if (printed_name > 0) {
                    //AnsiString s_name(name);
                    //Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, (s_name+"@type").c_str(), (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->index);
                    switch (vars->type) {
                        case ASN_BOOLEAN:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.integer[0]);
                            break;

                        case ASN_INTEGER:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.integer[0]);
                            break;

                        case ASN_APP_OPAQUE:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.integer[0]);
                            break;

                        case ASN_APP_COUNTER64:
                            {
                                double val = vars->val.counter64[0].low + vars->val.counter64[0].high * 0xFFFFFFFF;
                                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)val);
                            }
                            break;

                        case ASN_COUNTER:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(unsigned long)vars->val.integer[0]);
                            break;

                        case ASN_APP_FLOAT:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.floatVal[0]);
                            break;

                        case ASN_APP_DOUBLE:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.doubleVal[0]);
                            break;

                        case ASN_APP_I64:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.integer[0]);
                            break;

                        case ASN_APP_U64:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)(unsigned long)vars->val.integer[0]);
                            break;

                        case ASN_APP_UNION:
                            break;

                        case ASN_BIT_STR:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_STRING, (char *)vars->val.bitstring, (NUMBER)vars->val_len);
                            break;

                        case ASN_OCTET_STR:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_STRING, (char *)vars->val.string, (NUMBER)vars->val_len);
                            break;

                        case ASN_NULL:
                            break;

                        case ASN_OBJECT_ID:
                            {
                                int printed_value = snprint_objid(buf, 0xFFFE, vars->val.objid, vars->val_len);
                                if (printed_value > -1) {
                                    buf[printed_value] = 0;
                                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)printed_value);
                                }
                            }
                            break;

                        case ASN_SEQUENCE:
                        case ASN_SET:
                            break;

                        default:
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, name, (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)vars->val.integer[0]);
                    }
                }
                if ((vars->type == SNMP_ENDOFMIBVIEW) ||
                    (vars->type == SNMP_NOSUCHOBJECT) ||
                    (vars->type == SNMP_NOSUCHINSTANCE)) {
                    go_out = 1;
                    break;
                }
            }
            if (response->errstat) {
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, "@error", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)response->errstat);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, "@errstr", (INTEGER)VARIABLE_STRING, (char *)snmp_errstring(response->errstat), (NUMBER)0);
                if (go_out)
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, arr, "@noobjects", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)1);
            }
            snmp_free_pdu(response);
        }
    }
    RETURN_NUMBER(res)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_add_null_var, 2)
    T_HANDLE(0)
    T_STRING(1)

    oid name[MAX_OID_LEN];
    size_t   name_length       = MAX_OID_LEN;
    snmp_pdu *pdu              = (snmp_pdu *)PARAM_INT(0);
    netsnmp_variable_list *res = 0;

    if (snmp_parse_oid(PARAM(1), name, &name_length)) {
        res = snmp_add_null_var(pdu, name, name_length);
    } else
        snmp_perror("snmp_parse_oid");

    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(snmp_add_var, 4)
    T_HANDLE(0)
    T_STRING(1)
    T_NUMBER(2)
    T_STRING(3)

    oid name[MAX_OID_LEN];
    size_t   name_length;
    snmp_pdu *pdu = (snmp_pdu *)PARAM_INT(0);
    int      res  = -1;


    if (snmp_parse_oid(PARAM(1), name, &name_length))
        res = snmp_add_var(pdu, name, name_length, PARAM_INT(2), PARAM(3));
    else
        snmp_perror("snmp_parse_oid");

    RETURN_NUMBER((SYS_INT)res);
END_IMPL
//-----------------------------------------------------//
