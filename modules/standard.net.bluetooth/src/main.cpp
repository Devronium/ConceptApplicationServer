//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT    0x0600

 #define _WINSOCKAPI_
 #include <windows.h>
 #include <io.h>
 #include <winsock2.h>
 #include <ws2bth.h>
#else
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/un.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <string.h>
 #include <arpa/inet.h>
 #include <bluetooth/bluetooth.h>
 #include <bluetooth/rfcomm.h>
 #include <bluetooth/sdp.h>
 #include <bluetooth/sdp_lib.h>
 #include <bluetooth/uuid.h>
 #include <bluetooth/hci.h>
 #include <bluetooth/hci_lib.h>
 #include <bluetooth/l2cap.h>
 #include <signal.h>

 #include <map>
 #include <string>
std::map<std::string, sdp_session_t *> RegisteredSessions;
#endif

#ifndef AF_BTH
 #define AF_BTH    32
#endif

#ifndef BTPROTO_RFCOMM
 #define BTPROTO_RFCOMM    BTHPROTO_RFCOMM
#endif

#ifndef BT_PORT_ANY
 #define BT_PORT_ANY    -1
#endif

#ifndef BTPROTO_RFCOMM
 #define BTPROTO_RFCOMM    0x0003
#endif

#ifndef BTPROTO_L2CAP
 #define BTPROTO_L2CAP    0x0100
#endif

#ifdef _WIN32
int StringToUUID(char *s, GUID *guid) {
    unsigned long p0;
    int           p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;

    int err = sscanf(s, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);

    if (err != 11) {
        err = sscanf(s, "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
        if (err != 11) {
            sscanf(s, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
            if (err != 11) {
                sscanf(s, "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
            }
        }
    }
    guid->Data1    = p0;
    guid->Data2    = p1;
    guid->Data3    = p2;
    guid->Data4[0] = p3;
    guid->Data4[1] = p4;
    guid->Data4[2] = p5;
    guid->Data4[3] = p6;
    guid->Data4[4] = p7;
    guid->Data4[5] = p8;
    guid->Data4[6] = p9;
    guid->Data4[7] = p10;
    if (err != 11)
        return -1;
    return 0;
}

int AddrStringToBtAddr(const char *pszRemoteAddr, BTH_ADDR *pRemoteBtAddr, int& port) {
    ULONG    ulAddrData[6];
    BTH_ADDR BtAddrTemp = 0;

    port = 0;
    if ((pszRemoteAddr == NULL) || (pRemoteBtAddr == NULL))
        return 1;
    *pRemoteBtAddr = 0;
    if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x):%02i", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5], &port)) {
        if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x)", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5])) {
            if (sscanf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5]))
                return 1;
        }
    }
    for (int i = 0; i < 6; i++) {
        BtAddrTemp     = (BTH_ADDR)(ulAddrData[i] & 0xFF);
        *pRemoteBtAddr = ((*pRemoteBtAddr) << 8) + BtAddrTemp;
    }
    return 0;
}

int BtAddrToAddrString(char *pszRemoteAddr, SOCKADDR_BTH *address) {
    if (pszRemoteAddr == NULL)
        return 1;

    unsigned char *s = (unsigned char *)&address->btAddr;
    sprintf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", (int)s[5], (int)s[4], (int)s[3], (int)s[2], (int)s[1], (int)s[0]);
    return 0;
}

#else

int StringToUUID(char *s, uuid_t *guid) {
    if ((s) && (s[0] == '{')) {
        char buf[0xFF];
        int  len = strlen(s);
        if (len > 0xFF)
            len = 0xFF;

        memcpy(buf, s + 1, len - 2);
        buf[len - 2] = 0;
        //return uuid_parse(buf, guid);
        bt_string_to_uuid((bt_uuid_t *)guid, buf);
    }
    bt_string_to_uuid((bt_uuid_t *)guid, s);
    //return uuid_parse(s, *guid);
}

int AddrStringToBtAddr(const char *pszRemoteAddr, struct sockaddr_rc *pRemoteBtAddr, int& port) {
    int ulAddrData[6];

    port = 0;
    if ((pszRemoteAddr == NULL) || (pRemoteBtAddr == NULL))
        return 1;
    memset(pRemoteBtAddr, 0, sizeof(sockaddr_rc));
    if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x):%02i", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5], &port)) {
        if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x)", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5])) {
            if (sscanf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5]))
                return 1;
        }
    }
    for (int i = 0; i < 6; i++)
        pRemoteBtAddr->rc_bdaddr.b[i] = (unsigned char)ulAddrData[i];
    return 0;
}

int AddrStringToBtAddr2(const char *pszRemoteAddr, struct sockaddr_l2 *pRemoteBtAddr, int& port) {
    int ulAddrData[6];

    port = 0;
    if ((pszRemoteAddr == NULL) || (pRemoteBtAddr == NULL))
        return 1;
    memset(pRemoteBtAddr, 0, sizeof(sockaddr_l2));
    if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x):%02i", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5], &port)) {
        if (sscanf(pszRemoteAddr, "(%02x:%02x:%02x:%02x:%02x:%02x)", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5])) {
            if (sscanf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", &ulAddrData[0], &ulAddrData[1], &ulAddrData[2], &ulAddrData[3], &ulAddrData[4], &ulAddrData[5]))
                return 1;
        }
    }
    for (int i = 0; i < 6; i++)
        pRemoteBtAddr->l2_bdaddr.b[i] = (unsigned char)ulAddrData[i];
    return 0;
}

int BtAddrToAddrString(char *pszRemoteAddr, struct sockaddr_rc *address) {
    if (pszRemoteAddr == NULL)
        return 1;

    unsigned char *s = (unsigned char *)address->rc_bdaddr.b;
    sprintf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", (int)s[5], (int)s[4], (int)s[3], (int)s[2], (int)s[1], (int)s[0]);
    return 0;
}

int BtAddrToAddrString2(char *pszRemoteAddr, struct sockaddr_l2 *address) {
    if (pszRemoteAddr == NULL)
        return 1;

    unsigned char *s = (unsigned char *)address->l2_bdaddr.b;
    sprintf(pszRemoteAddr, "%02x:%02x:%02x:%02x:%02x:%02x", (int)s[5], (int)s[4], (int)s[3], (int)s[2], (int)s[1], (int)s[0]);
    return 0;
}
#endif
//=====================================================================================//
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
 #ifndef SO_NOSIGPIPE
    signal(SIGPIPE, SIG_IGN);
 #endif
#endif
    DEFINE_ECONSTANT(BT_PORT_ANY)
    DEFINE_SCONSTANT("ServiceDiscoveryServer_UUID", "\"{00001000-0000-1000-8000-00805F9B34FB}\"")
    DEFINE_SCONSTANT("BrowseGroupDescriptor_UUID", "\"{00001001-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("PublicBrowseGroup_UUID", "\"{00001002-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("SerialPort_UUID", "\"{00001101-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("LANAccessUsingPPP_UUID", "\"{00001102-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("DialupNetworking_UUID", "\"{00001103-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("IrMCSync_UUID", "\"{00001104-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("OBEXObjectPush_UUID", "\"{00001105-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("OBEXFileTransfer_UUID", "\"{00001106-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("IrMCSyncCommand_UUID", "\"{00001107-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("Headset_UUID", "\"{00001108-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("CordlessTelephony_UUID", "\"{00001109-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AudioSource_UUID", "\"{0000110A-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AudioSink_UUID", "\"{0000110B-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AVRemoteControlTarget_UUID", "\"{0000110C-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AdvancedAudioDistribution_UUID", "\"{0000110D-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AVRemoteControl_UUID", "\"{0000110E-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("VideoConferencing_UUID", "\"{0000110F-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("Intercom_UUID", "\"{00001110-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("Fax_UUID", "\"{00001111-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HeadsetAudioGateway_UUID", "\"{00001112-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("WAP_UUID", "\"{00001113-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("WAPClient_UUID", "\"{00001114-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("PANU_UUID", "\"{00001115-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("NAP_UUID", "\"{00001116-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("GN_UUID", "\"{00001117-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("DirectPrinting_UUID", "\"{00001118-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("ReferencePrinting_UUID", "\"{00001119-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("Imaging_UUID", "\"{0000111A-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("ImagingResponder_UUID", "\"{0000111B-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("ImagingAutomaticArchive_UUID", "\"{0000111C-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("ImagingReferenceObjects_UUID", "\"{0000111D-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("Handsfree_UUID", "\"{0000111E-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HandsfreeAudioGateway_UUID", "\"{0000111F-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("DirectPrintingReferenceObjects_UUID", "\"{00001120-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("ReflectedUI_UUID", "\"{00001121-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("BasicPringing_UUID", "\"{00001122-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("PrintingStatus_UUID", "\"{00001123-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HumanInterfaceDevice_UUID", "\"{00001124-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HardcopyCableReplacement_UUID", "\"{00001125-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HCRPrint_UUID", "\"{00001126-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("HCRScan_UUID", "\"{00001127-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("CommonISDNAccess_UUID", "\"{00001128-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("VideoConferencingGW_UUID", "\"{00001129-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("UDIMT_UUID", "\"{0000112A-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("UDITA_UUID", "\"{0000112B-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("AudioVideo_UUID", "\"{0000112C-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("SIMAccess_UUID", "\"{0000112D-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("PnPInformation_UUID", "\"{00001200-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("GenericNetworking_UUID", "\"{00001201-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("GenericFileTransfer_UUID", "\"{00001202-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("GenericAudio_UUID", "\"{00001203-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("GenericTelephony_UUID", "\"{00001204-0000-1000-8000-00805F9B34FB}\"");
    DEFINE_SCONSTANT("BD_LOCAL", "\"00:00:00:FF:FF:FF\"");
    DEFINE_SCONSTANT("BD_ANY", "\"00:00:00:00:00:00\"");
    DEFINE_ECONSTANT(BTPROTO_RFCOMM);
    DEFINE_ECONSTANT(BTPROTO_L2CAP);
    DEFINE_ECONSTANT(SOCK_SEQPACKET);
    DEFINE_ECONSTANT(SOCK_STREAM);
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTSocketCreate, 0, 2)
    int type = SOCK_STREAM;
    int proto = BTPROTO_RFCOMM;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(BTSocketCreate, 0)
        proto = PARAM_INT(0);
    }
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(BTSocketCreate, 1)
        type = PARAM_INT(1);
    }
    int sock = socket(AF_BTH, type, proto);
    int flag = 1;
    if (sock > -1) {
        setsockopt(sock,
                   SOL_SOCKET,
                   SO_KEEPALIVE,
                   (char *)&flag,
                   sizeof(flag));

        setsockopt(sock,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (char *)&flag,
                   sizeof(flag));

        struct linger so_linger;
        so_linger.l_onoff  = 1;
        so_linger.l_linger = 10;

        setsockopt(sock,
                   SOL_SOCKET,
                   SO_LINGER,
                   (char *)&so_linger,
                   sizeof(so_linger));
    }
#ifndef WIN32
 #ifdef SO_NOSIGPIPE
    if (sock > -1) {
        int flag = 1;
        setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &flag, sizeof(flag));
    }
 #endif
#endif
    RETURN_NUMBER(sock);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(BTSocketClose, 1)
    T_NUMBER(BTSocketClose, 0)

    int sock = PARAM_INT(0);
    if (sock > -1) {
#ifdef _WIN32
        shutdown((int)sock, SD_BOTH);
        RETURN_NUMBER(closesocket((int)sock));
#else
        shutdown((int)sock, SHUT_RDWR);
        RETURN_NUMBER(close((int)sock));
#endif
    }
    RETURN_NUMBER(-1);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTSocketListen, 1, 5)
    T_NUMBER(BTSocketListen, 0)
    int sock = PARAM_INT(0);
    int  nMaxCon  = 0x100;
    int  nPort    = BT_PORT_ANY;
    int  is_l2cap = 0;
    int  result   = -1;
    char *iface   = 0;

    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(BTSocketListen, 1)
        nPort = PARAM_INT(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(BTSocketListen, 2)
        nMaxCon = PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(BTSocketListen, 3)
        is_l2cap = PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_STRING(BTSocketListen, 4)
        if (PARAM_LEN(4))
            iface = PARAM(4);
    }

#ifdef _WIN32
    SOCKADDR_BTH addr;
    addr.addressFamily = AF_BTH;
    addr.btAddr        = 0;
    addr.port          = nPort;
    if (iface) {
        AnsiString ifc;
        if (iface[0] != '(') {
            ifc += "(";
            ifc += iface;
            ifc += ")";
        } else
            ifc = iface;
        ifc += ":0";
        int dummy;
        AddrStringToBtAddr(ifc.c_str(), &addr.btAddr, dummy);
    }
    result = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
#else
    if (is_l2cap) {
        struct sockaddr_l2 addr = {
            0
        };
        addr.l2_family = AF_BLUETOOTH;
        memset(&addr.l2_bdaddr, 0, sizeof(addr.l2_bdaddr));
        if (iface) {
            AnsiString ifc;
            if (iface[0] != '(') {
                ifc += "(";
                ifc += iface;
                ifc += ")";
            } else
                ifc = iface;
            ifc += ":0";
            int dummy;
            AddrStringToBtAddr2(ifc.c_str(), &addr, dummy);
        }
        addr.l2_psm = htobs((unsigned short)nPort);
        result      = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    } else {
        struct sockaddr_rc addr = {
            0
        };
        addr.rc_family = AF_BLUETOOTH;
        memset(&addr.rc_bdaddr, 0, sizeof(addr.rc_bdaddr));
        if (iface) {
            AnsiString ifc;
            if (iface[0] != '(') {
                ifc += "(";
                ifc += iface;
                ifc += ")";
            } else
                ifc = iface;
            ifc += ":0";
            int dummy;
            AddrStringToBtAddr(ifc.c_str(), &addr, dummy);
        }
        addr.rc_channel = (uint8_t)nPort;
        result          = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    }
#endif
    if (result) {
        RETURN_NUMBER(result);
        return 0;
    }

    result = listen(sock, nMaxCon);

    RETURN_NUMBER(result);
    return 0;
    }
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL(BTSocketAccept, 1)
        T_NUMBER(BTSocketAccept, 0)
        int sock = PARAM_INT(0);

        RETURN_NUMBER(accept((int)sock, NULL, NULL));
    END_IMPL
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTDiscover, 0, 1)
#ifdef _WIN32
        WSAQUERYSET queryset;

        GUID protocol = L2CAP_PROTOCOL_UUID;

        if (PARAMETERS_COUNT > 0) {
            T_STRING(BTDiscover, 0);
            StringToUUID(PARAM(0), &protocol);
        }
        memset(&queryset, 0, sizeof(WSAQUERYSET));
        queryset.dwSize      = sizeof(WSAQUERYSET);
        queryset.dwNameSpace = NS_BTH;

        HANDLE hLookup;

        int result = WSALookupServiceBegin(&queryset, LUP_CONTAINERS, &hLookup);

        if (result != 0) {
            RETURN_NUMBER(0);
            return 0;
        }
        CREATE_ARRAY(RESULT);

        BYTE buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        DWORD bufferLength    = sizeof(buffer);
        WSAQUERYSET *pResults = (WSAQUERYSET *)&buffer;
        INTEGER idxdev        = 0;
        while (result == 0) {
            result = WSALookupServiceNext(hLookup, LUP_FLUSHCACHE | LUP_RETURN_ALL, &bufferLength, pResults);
            if (result == 0) {
                void *var_owner = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, RESULT, idxdev++, &var_owner);
                CREATE_ARRAY(var_owner);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Name", (INTEGER)VARIABLE_STRING, (char *)pResults->lpszServiceInstanceName, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Comment", (INTEGER)VARIABLE_STRING, (char *)pResults->lpszComment, (NUMBER)0);
                WSAQUERYSET queryset2;
                memset(&queryset2, 0, sizeof(queryset2));
                queryset2.dwSize            = sizeof(queryset2);
                queryset2.dwNameSpace       = NS_BTH;
                queryset2.dwNumberOfCsAddrs = 0;
                CSADDR_INFO *addr = (CSADDR_INFO *)pResults->lpcsaBuffer;
                CHAR addressAsString[1000];
                DWORD addressSize = sizeof(addressAsString);
                WSAAddressToString(addr->RemoteAddr.lpSockaddr, addr->RemoteAddr.iSockaddrLength, NULL, addressAsString, &addressSize);
                queryset2.lpszContext = addressAsString;
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Address", (INTEGER)VARIABLE_STRING, (char *)addressAsString, (NUMBER)0);
                queryset2.lpServiceClassId = &protocol;
                HANDLE hLookup2;
                int result2 = WSALookupServiceBegin(&queryset2, LUP_RETURN_ALL | LUP_FLUSHCACHE | LUP_RETURN_TYPE, &hLookup2);

                if (result2 != 0) {
                    continue;
                }

                void *var_services = 0;
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var_owner, "Services", &var_services);
                CREATE_ARRAY(var_services);

                //Start quering for device services
                INTEGER idx = 0;
                while (result2 == 0) {
                    BYTE buffer2[4096];
                    memset(buffer2, 0, sizeof(buffer2));
                    DWORD bufferLength2    = sizeof(buffer2);
                    WSAQUERYSET *pResults2 = (WSAQUERYSET *)&buffer2;
                    result2 = WSALookupServiceNext(hLookup2, LUP_RETURN_ALL, &bufferLength2, pResults2);
                    if (result2 == 0) {
                        void *var = 0;
                        Invoke(INVOKE_ARRAY_VARIABLE, var_services, idx, &var);
                        CREATE_ARRAY(var);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Name", (INTEGER)VARIABLE_STRING, (char *)pResults2->lpszServiceInstanceName, (NUMBER)0);
                        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Comment", (INTEGER)VARIABLE_STRING, (char *)pResults2->lpszComment, (NUMBER)0);
                        if (pResults2->lpServiceClassId) {
                            char buf[0xFF];
                            sprintf(buf, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
                                    pResults2->lpServiceClassId->Data1, pResults2->lpServiceClassId->Data2, pResults2->lpServiceClassId->Data3,
                                    pResults2->lpServiceClassId->Data4[0], pResults2->lpServiceClassId->Data4[1], pResults2->lpServiceClassId->Data4[2],
                                    pResults2->lpServiceClassId->Data4[3], pResults2->lpServiceClassId->Data4[4], pResults2->lpServiceClassId->Data4[5],
                                    pResults2->lpServiceClassId->Data4[6], pResults2->lpServiceClassId->Data4[7]);

                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Service", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
                        }

                        if (pResults2->dwNumberOfCsAddrs > 0) {
                            void *addr_arr = 0;
                            //Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY,var,"Address",&addr_arr);
                            //CREATE_ARRAY(addr_arr);

                            CSADDR_INFO *addr = (CSADDR_INFO *)pResults2->lpcsaBuffer;
                            //for (int i=0;i<pResults2->dwNumberOfCsAddrs;i++) {
                            addressSize = sizeof(addressAsString);
                            memset(addressAsString, 0, addressSize);
                            WSAAddressToString(addr[0].RemoteAddr.lpSockaddr, addr[0].RemoteAddr.iSockaddrLength, NULL, addressAsString, &addressSize);
                            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Address", (INTEGER)VARIABLE_STRING, (char *)addressAsString, (NUMBER)0);
                            //}
                        }

                        idx++;
                    }
                }
                WSALookupServiceEnd(hLookup2);
            }
        }
        WSALookupServiceEnd(hLookup);
#else
        inquiry_info * ii = NULL;
        int max_rsp, num_rsp;
        int dev_id, sock, len, flags;
        int i;
        char addr[19]  = { 0 };
        char name[248] = { 0 };

        dev_id = hci_get_route(NULL);
        sock   = hci_open_dev(dev_id);
        if ((dev_id < 0) || (sock < 0)) {
            RETURN_NUMBER(0);
            return 0;
        }
        CREATE_ARRAY(RESULT);

        uuid_t svc_uuid;
        if (PARAMETERS_COUNT > 0) {
            T_STRING(BTDiscover, 0);
            StringToUUID(PARAM(0), &svc_uuid);
        } else
            StringToUUID("{00001000-0000-1000-8000-00805F9B34FB}", &svc_uuid);

        len     = 8;
        max_rsp = 255;
        flags   = IREQ_CACHE_FLUSH;
        ii      = (inquiry_info *)malloc(max_rsp * sizeof(inquiry_info));

        num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);

        for (i = 0; i < num_rsp; i++) {
            bdaddr_t target = (ii + i)->bdaddr;
            ba2str(&target, addr);
            memset(name, 0, sizeof(name));

            if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0)
                strcpy(name, "");

            void *var_owner = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &var_owner);
            CREATE_ARRAY(var_owner);

            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Name", (INTEGER)VARIABLE_STRING, (char *)name, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Comment", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            AnsiString address("(");
            address += addr;
            address += ")";
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var_owner, "Address", (INTEGER)VARIABLE_STRING, (char *)address.c_str(), (NUMBER)0);
            address += ":";

            int status;
            sdp_list_t *response_list, *search_list, *attrid_list;
            sdp_session_t *session = 0;
            uint32_t range         = 0x0000ffff;
            uint8_t port           = 0;
            bdaddr_t bd_any        = { { 0, 0, 0, 0, 0, 0 } };
            session = sdp_connect(&bd_any, &target, 0);

            search_list = sdp_list_append(0, &svc_uuid);
            attrid_list = sdp_list_append(0, &range);

            //get a list of service records that have UUID 0xabcd
            response_list = NULL;
            status        = sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
            sdp_list_t *r = response_list;
            if ((status == 0) && (r)) {
                void *var_services = 0;
                Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, var_owner, "Services", &var_services);
                CREATE_ARRAY(var_services);

                sdp_list_t *proto_list = NULL;
                //go through each of the service records
                int idx = 0;
                for ( ; r; r = r->next) {
                    sdp_record_t *rec = (sdp_record_t *)r->data;
                    //get a list of the protocol sequences
                    if (sdp_get_access_protos(rec, &proto_list) == 0) {
                        //get the RFCOMM port number
                        port = sdp_get_proto_port(proto_list, RFCOMM_UUID);
                        sdp_list_free(proto_list, 0);
                    }
                    void *var = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, var_services, idx, &var);
                    CREATE_ARRAY(var);
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Name", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Comment", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);

                    AnsiString temp = address + AnsiString((long)port);
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Address", (INTEGER)VARIABLE_STRING, (char *)temp.c_str(), (NUMBER)0);

                    char buf[0xFF];
                    bt_uuid_to_string((bt_uuid_t *)&rec->svclass, buf, 0xFF);
                    AnsiString temp2("{");
                    temp2 += buf;
                    temp2 += "}";
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, var, "Service", (INTEGER)VARIABLE_STRING, (char *)temp2.c_str(), (NUMBER)0);
                    sdp_record_free(rec);
                    idx++;
                }
            }
            sdp_list_free(response_list, 0);
            sdp_list_free(search_list, 0);
            sdp_list_free(attrid_list, 0);
            sdp_close(session);
        }
        free(ii);
        close(sock);
#endif
    END_IMPL
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTSocketConnect, 3, 4)
        T_NUMBER(BTSocketConnect, 0)
        T_STRING(BTSocketConnect, 1)
        T_STRING(BTSocketConnect, 2)

        int sock = PARAM_INT(0);
        int port     = 1;
        int is_l2cap = 0;
        if (PARAMETERS_COUNT > 3) {
            T_NUMBER(BTSocketConnect, 3)
            is_l2cap = PARAM_INT(3);
        }
#ifdef _WIN32
        GUID guid;
        memset(&guid, 0, sizeof(GUID));
        StringToUUID(PARAM(2), &guid);
        SOCKADDR_BTH SockAddrBthServer = { 0 };
        if (AddrStringToBtAddr(PARAM(1), &SockAddrBthServer.btAddr, port)) {
            RETURN_NUMBER(-1);
            return 0;
        }
        if (!port)
            port = 1;
        SockAddrBthServer.addressFamily  = AF_BTH;
        SockAddrBthServer.serviceClassId = guid;
        SockAddrBthServer.port           = port;

        RETURN_NUMBER(connect((int)sock, (sockaddr *)&SockAddrBthServer, sizeof(SockAddrBthServer)));
#else
        if (is_l2cap) {
            struct sockaddr_l2 SockAddrBthServer = {
                0
            };
            if (AddrStringToBtAddr2(PARAM(1), &SockAddrBthServer, port)) {
                RETURN_NUMBER(-1);
                return 0;
            }
            SockAddrBthServer.l2_family = AF_BLUETOOTH;
            SockAddrBthServer.l2_psm    = htobs(port);
            //is memset to 0
            //loc_addr.l2_bdaddr = *BDADDR_ANY;
            RETURN_NUMBER(connect((int)sock, (sockaddr *)&SockAddrBthServer, sizeof(SockAddrBthServer)));
        } else {
            struct sockaddr_rc SockAddrBthServer = {
                0
            };
            if (AddrStringToBtAddr(PARAM(1), &SockAddrBthServer, port)) {
                RETURN_NUMBER(-1);
                return 0;
            }
            SockAddrBthServer.rc_family  = AF_BLUETOOTH;
            SockAddrBthServer.rc_channel = (uint8_t)port;
            RETURN_NUMBER(connect((int)sock, (sockaddr *)&SockAddrBthServer, sizeof(SockAddrBthServer)));
        }
#endif
    END_IMPL
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTRegister, 2, 4)
        T_STRING(BTRegister, 0)
        T_STRING(BTRegister, 1)
        int port = BT_PORT_ANY;

        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(BTRegister, 2)
            port = PARAM_INT(2);
        }
#ifdef _WIN32
        GUID guid;
        memset(&guid, 0, sizeof(GUID));
        StringToUUID(PARAM(1), &guid);

        WSAQUERYSET service;
        memset(&service, 0, sizeof(service));
        service.dwSize = sizeof(service);
        service.lpszServiceInstanceName = PARAM(0);

        if (PARAMETERS_COUNT > 3) {
            T_STRING(BTRegister, 3)
            service.lpszComment = PARAM(3);
        }


        SOCKADDR_BTH address;
        address.addressFamily  = AF_BTH;
        address.btAddr         = 0;
        address.serviceClassId = GUID_NULL;
        address.port           = port;
        sockaddr *pAddr = (sockaddr *)&address;

        GUID serviceID = guid;

        service.lpServiceClassId  = &serviceID;
        service.dwNumberOfCsAddrs = 1;
        service.dwNameSpace       = NS_BTH;

        CSADDR_INFO csAddr;
        memset(&csAddr, 0, sizeof(csAddr));
        csAddr.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
        csAddr.LocalAddr.lpSockaddr      = pAddr;
        csAddr.iSocketType  = SOCK_STREAM;
        csAddr.iProtocol    = BTHPROTO_RFCOMM;
        service.lpcsaBuffer = &csAddr;

        if (WSASetService(&service, RNRSERVICE_REGISTER, 0)) {
            RETURN_NUMBER(-1);
        } else {
            RETURN_NUMBER(0);
        }
#else
        const char *service_name = PARAM(0);
        const char *service_dsc  = NULL;
        if (PARAMETERS_COUNT > 2) {
            T_STRING(BTRegister, 3)
            service_dsc = PARAM(3);
        }
        bdaddr_t bd_any   = { { 0, 0, 0, 0, 0, 0 } };
        bdaddr_t bd_local = { { 0, 0, 0, 0xff, 0xff, 0xff } };
        uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid, svc_class_uuid;
        sdp_list_t *l2cap_list        = 0,
                   *rfcomm_list       = 0,
                   *root_list         = 0,
                   *proto_list        = 0,
                   *access_proto_list = 0,
                   *svc_class_list    = 0,
                   *profile_list      = 0;
        sdp_data_t *channel           = (sdp_data_t *)port;
        sdp_profile_desc_t profile;
        sdp_record_t record    = { 0 };
        sdp_session_t *session = 0;

        StringToUUID(PARAM(1), &svc_uuid);

        sdp_set_service_id(&record, svc_uuid);

        sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
        svc_class_list = sdp_list_append(0, &svc_class_uuid);
        sdp_set_service_classes(&record, svc_class_list);

        sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
        profile.version = 0x0100;
        profile_list    = sdp_list_append(0, &profile);
        sdp_set_profile_descs(&record, profile_list);

        sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
        root_list = sdp_list_append(0, &root_uuid);
        sdp_set_browse_groups(&record, root_list);

        sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
        l2cap_list = sdp_list_append(0, &l2cap_uuid);
        proto_list = sdp_list_append(0, l2cap_list);

// register the RFCOMM channel for RFCOMM sockets
        sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
        channel     = sdp_data_alloc(SDP_UINT8, &port);
        rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
        sdp_list_append(rfcomm_list, channel);
        sdp_list_append(proto_list, rfcomm_list);
        access_proto_list = sdp_list_append(0, proto_list);
        sdp_set_access_protos(&record, access_proto_list);

        sdp_set_info_attr(&record, service_name, NULL, service_dsc);
        session = sdp_connect(&bd_any, &bd_local, 0);
        if (session) {
            int res = sdp_record_register(session, &record, 0);
            RegisteredSessions[service_name] = session;
            RETURN_NUMBER(res);
        } else {
            RETURN_NUMBER(-2);
        }
        sdp_data_free(channel);
        sdp_list_free(l2cap_list, 0);
        sdp_list_free(rfcomm_list, 0);
        sdp_list_free(root_list, 0);
        sdp_list_free(access_proto_list, 0);
#endif
    END_IMPL
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTUnregister, 2, 4)
        T_STRING(BTUnregister, 0)
        T_STRING(BTUnregister, 1)
        int port = BT_PORT_ANY;

        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(BTUnregister, 2)
            port = PARAM_INT(2);
        }
#ifdef _WIN32
        GUID guid;
        memset(&guid, 0, sizeof(GUID));
        StringToUUID(PARAM(1), &guid);

        WSAQUERYSET service;
        memset(&service, 0, sizeof(service));
        service.dwSize = sizeof(service);
        service.lpszServiceInstanceName = PARAM(0);

        if (PARAMETERS_COUNT > 3) {
            T_STRING(BTUnregister, 3)
            service.lpszComment = PARAM(3);
        }


        SOCKADDR_BTH address;
        address.addressFamily  = AF_BTH;
        address.btAddr         = 0;
        address.serviceClassId = GUID_NULL;
        address.port           = port;
        sockaddr *pAddr = (sockaddr *)&address;

        GUID serviceID = guid;

        service.lpServiceClassId  = &serviceID;
        service.dwNumberOfCsAddrs = 1;
        service.dwNameSpace       = NS_BTH;

        CSADDR_INFO csAddr;
        memset(&csAddr, 0, sizeof(csAddr));
        csAddr.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
        csAddr.LocalAddr.lpSockaddr      = pAddr;
        csAddr.iSocketType  = SOCK_STREAM;
        csAddr.iProtocol    = BTHPROTO_RFCOMM;
        service.lpcsaBuffer = &csAddr;

        if (WSASetService(&service, RNRSERVICE_DELETE, 0)) {
            RETURN_NUMBER(-1);
        } else {
            RETURN_NUMBER(0);
        }
#else
        sdp_session_t *session = RegisteredSessions[PARAM(0)];
        int res = -2;
        if (session) {
            uuid_t svc_uuid;
            sdp_record_t record    = { 0 };
            sdp_session_t *session = 0;
            StringToUUID(PARAM(1), &svc_uuid);
            sdp_set_service_id(&record, svc_uuid);

            RegisteredSessions.erase(PARAM(0));
            res = sdp_record_unregister(session, &record);
            sdp_close(session);
        }
        RETURN_NUMBER(res);
#endif
    END_IMPL
//=====================================================================================//
    CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(BTSocketInfo, 1, 2)
        T_NUMBER(BTSocketInfo, 0)
        int socket = PARAM_INT(0);
        CREATE_ARRAY(RESULT);
        int is_l2cap = 0;
        if (PARAMETERS_COUNT > 1) {
            T_NUMBER(BTSocketInfo, 1)
            is_l2cap = PARAM_INT(1);
        }
#ifdef _WIN32
        int length = sizeof(SOCKADDR_BTH);
        SOCKADDR_BTH address;
        if (!getsockname(socket, (sockaddr *)&address, &length)) {
            char buf[0xFF];
            BtAddrToAddrString(buf, &address);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "address", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)address.port);
        }
#else
        if (is_l2cap) {
            socklen_t length           = sizeof(sockaddr_l2);
            struct sockaddr_l2 address = {
                0
            };
            if (!getsockname(socket, (sockaddr *)&address, &length)) {
                char buf[0xFF];
                BtAddrToAddrString2(buf, &address);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "address", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
                int port = btohs(address.l2_psm);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)port);
            }
        } else {
            socklen_t length           = sizeof(sockaddr_rc);
            struct sockaddr_rc address = {
                0
            };
            if (!getsockname(socket, (sockaddr *)&address, &length)) {
                char buf[0xFF];
                BtAddrToAddrString(buf, &address);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "address", (INTEGER)VARIABLE_STRING, (char *)buf, (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "port", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)address.rc_channel);
            }
        }
#endif
    END_IMPL

