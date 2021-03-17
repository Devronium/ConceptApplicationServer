//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
//#include <map>

#ifdef _WIN32
 #include <windows.h>
 #include <winsock.h>
#endif


#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtppacket.h"
#include "rtpsourcedata.h"

using namespace jrtplib;

//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    //EventsMap.clear();
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPCreate, 2)
    T_ARRAY(RTPCreate, 0)

    RTPSession * sess = new RTPSession();

    INTEGER type = 0;
    char    *str = 0;
    NUMBER  nr   = 0;

    RTPSessionParams           sessparams;
    RTPUDPv4TransmissionParams transparams;

    char *sess_params[] = { "UsePollThread", "MaximumPacketSize", "AcceptOwnPackets", "ReceiveMode", "OwnTimestampUnit", "ResolveLocalHostname", "ProbationType", "SessionBandwidth", "ControlTrafficFraction", "SenderControlBandwidthFraction", "MinimumRTCPTransmissionInterval", "UseHalfRTCPIntervalAtStartup", "RequestImmediateBYE", "SenderReportForBYE", "SenderTimeoutMultiplier", "SourceTimeoutMultiplier", "BYETimeoutMultiplier", "CollisionTimeoutMultiplier", "NoteTimeoutMultiplier", "UsePredefinedSSRC", "PredefinedSSRC", "CNAME", 0 };
    char *recv_params[] = { "BindIP", "MulticastInterfaceIP", "Portbase", "MulticastTTL", "LocalIPList", "RTPSendBuffer", "RTPReceiveBuffer", "RTCPSendBuffer", 0 };

    int     index   = 0;
    INTEGER idx2    = -1;
    char    *target = sess_params[index++];
    void    *arr    = PARAMETER(0);
    while (target) {
        int result = Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, arr, idx2, target);
        if ((IS_OK(result)) && (result)) {
            InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, target, &type, &str, &nr);
            if (type == VARIABLE_NUMBER) {
                switch (index - 1) {
                    case 0:
                        sessparams.SetUsePollThread(nr);
                        break;

                    case 1:
                        sessparams.SetMaximumPacketSize(nr);
                        break;

                    case 2:
                        sessparams.SetAcceptOwnPackets(nr);
                        break;

                    case 3:
                        sessparams.SetReceiveMode((RTPTransmitter::ReceiveMode)(int)nr);
                        break;

                    case 4:
                        sessparams.SetOwnTimestampUnit(nr);
                        break;

                    case 5:
                        sessparams.SetResolveLocalHostname(nr);
                        break;

                    case 6:
                        sessparams.SetProbationType((RTPSources::ProbationType)(int)nr);
                        break;

                    case 7:
                        sessparams.SetSessionBandwidth(nr);
                        break;

                    case 8:
                        sessparams.SetControlTrafficFraction(nr);
                        break;

                    case 9:
                        sessparams.SetSenderControlBandwidthFraction(nr);
                        break;

                    case 10:
                        sessparams.SetMinimumRTCPTransmissionInterval(nr);
                        break;

                    case 11:
                        sessparams.SetUseHalfRTCPIntervalAtStartup(nr);
                        break;

                    case 12:
                        sessparams.SetRequestImmediateBYE(nr);
                        break;

                    case 13:
                        sessparams.SetSenderReportForBYE(nr);
                        break;

                    case 14:
                        sessparams.SetSenderTimeoutMultiplier(nr);
                        break;

                    case 15:
                        sessparams.SetSourceTimeoutMultiplier(nr);
                        break;

                    case 16:
                        sessparams.SetBYETimeoutMultiplier(nr);
                        break;

                    case 17:
                        sessparams.SetCollisionTimeoutMultiplier(nr);
                        break;

                    case 18:
                        sessparams.SetNoteTimeoutMultiplier(nr);
                        break;

                    case 19:
                        sessparams.SetUsePredefinedSSRC(nr);
                        break;

                    case 20:
                        sessparams.SetPredefinedSSRC(nr);
                        break;

                    default:
                        break;
                }
            } else
            if (type == VARIABLE_STRING) {
                switch (index - 1) {
                    case 21:
                        if (str)
                            sessparams.SetCNAME(str);
                        break;

                    default:
                        break;
                }
            }
        }
        target = sess_params[index++];
    }

    index  = 0;
    idx2   = -1;
    target = recv_params[index++];
//arr=PARAMETER(0);
    while (target) {
        int result = Invoke(INVOKE_ARRAY_ELEMENT_IS_SET, arr, idx2, target);
        if ((IS_OK(result)) && (result)) {
            InvokePtr(INVOKE_GET_ARRAY_ELEMENT_BY_KEY, arr, target, &type, &str, &nr);
            if (type == VARIABLE_NUMBER) {
                switch (index - 1) {
                    case 2:
                        transparams.SetPortbase(nr);
                        break;

                    case 3:
                        transparams.SetMulticastTTL(nr);
                        break;

                    case 4:
                        break;

                    case 5:
                        transparams.SetRTPSendBuffer(nr);
                        break;

                    case 6:
                        transparams.SetRTPReceiveBuffer(nr);
                        break;

                    case 7:
                        transparams.SetRTCPSendBuffer(nr);
                        break;
                }
            } else
            if (type == VARIABLE_STRING) {
                unsigned int ip = str ? inet_addr(str) : -1;
                if (ip != INADDR_NONE) {
                    ip = ntohl(ip);
                    switch (index - 1) {
                        case 0:
                            if (str)
                                transparams.SetBindIP(ip);
                            break;

                        case 1:
                            if (str)
                                transparams.SetMulticastInterfaceIP(ip);
                            break;

                        default:
                            break;
                    }
                }
            }
        }

        target = recv_params[index++];
    }
    int status = sess->Create(sessparams, &transparams);
    if (status < 0) {
        delete sess;
        sess = 0;
    }
    SET_NUMBER(1, status);
    RETURN_NUMBER((SYS_INT)sess)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPError, 1)
    T_NUMBER(RTPError, 0)
    std::string err = RTPGetErrorString(PARAM_INT(0));
    if (err.size()) {
        RETURN_STRING(err.c_str());
    } else {
        RETURN_STRING("");
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPBYEDestroy, 3)
    T_NUMBER(RTPBYEDestroy, 0)
    T_NUMBER(RTPBYEDestroy, 1)
    T_STRING(RTPBYEDestroy, 2)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    if (sess) {
        int ms = PARAM_INT(1);
        sess->BYEDestroy(RTPTime(ms / 1000, (ms % 1000) * 1000), PARAM(2), PARAM_LEN(2));
        delete sess;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPDestroy, 1)
    T_NUMBER(RTPDestroy, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    if (sess) {
        sess->Destroy();
        delete sess;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RTPSend, 2, 5)
    T_HANDLE(RTPSend, 0)
    T_STRING(RTPSend, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    uint8_t  pt     = 0;
    bool     mark   = false;
    uint32_t tstamp = 10;
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(RTPSend, 2)
        pt = PARAM_INT(2);
    }
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(RTPSend, 3)
        mark = (bool)PARAM_INT(3);
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(RTPSend, 4)
        tstamp = PARAM_INT(4);
    }
    int res = sess->SendPacket(PARAM(1), PARAM_LEN(1), pt, mark, tstamp);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPBeginDataAccess, 1)
    T_HANDLE(RTPBeginDataAccess, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->BeginDataAccess());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPEndDataAccess, 1)
    T_HANDLE(RTPEndDataAccess, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->EndDataAccess());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPGetNextPacket, 1)
    T_HANDLE(RTPGetNextPacket, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RTPPacket *pack = sess->GetNextPacket();
    CREATE_ARRAY(RESULT);
    if (pack) {
        size_t size = pack->GetPayloadLength();
        if (size > 0)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Payload", (INTEGER)VARIABLE_STRING, (char *)pack->GetPayloadData(), (NUMBER)size);

        //size=pack->GetPacketLength();
        //if (size>0)
        //    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Packet", (INTEGER)VARIABLE_STRING, (char *)pack->GetPacketData(), (NUMBER)size);

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Timestamp", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)pack->GetTimestamp());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SSRC", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)pack->GetSSRC());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ExtensionID", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)pack->GetExtensionID());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "PayloadType", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)pack->GetPayloadType());

        size = pack->GetExtensionLength();
        if (size > 0)
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Extension", (INTEGER)VARIABLE_STRING, (char *)pack->GetExtensionData(), (NUMBER)size);
        RTPTime t = pack->GetReceiveTime();
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ReceiveTime", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)t.GetDouble() * 1000 + (NUMBER)t.GetMicroSeconds() / 1000);
        sess->DeletePacket(pack);
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPGotoFirstSourceWithData, 1)
    T_HANDLE(RTPGotoFirstSourceWithData, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->GotoFirstSourceWithData());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPGotoNextSourceWithData, 1)
    T_HANDLE(RTPGotoNextSourceWithData, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->GotoNextSourceWithData());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPGotoPreviousSourceWithData, 1)
    T_HANDLE(RTPGotoPreviousSourceWithData, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->GotoPreviousSourceWithData());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPCurrentSourceReceivedBYE, 1)
    T_HANDLE(RTPCurrentSourceReceivedBYE, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    int           res   = 0;
    RTPSourceData *data = sess->GetCurrentSourceInfo();
    if (data) {
        res = data->ReceivedBYE();
    }
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(RTPGetCurrentSourceInfo, 1, 2)
    T_HANDLE(RTPGetCurrentSourceInfo, 0)

    bool full_info = false;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(RTPGetCurrentSourceInfo, 1);
        full_info = PARAM_INT(1);
    }
    RTPSession    *sess = (RTPSession *)PARAM_INT(0);
    RTPSourceData *data = sess->GetCurrentSourceInfo();
    CREATE_ARRAY(RESULT);
    if (data) {
        RTPIPv4Address *addr = (RTPIPv4Address *)(data->GetRTPDataAddress());
        if (addr) {
            in_addr ip;
#ifdef _WIN32
            ip.S_un.S_addr = htonl(addr->GetIP());
#else
            ip.s_addr = htonl(addr->GetIP());
#endif
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RTPDataAddress", (INTEGER)VARIABLE_STRING, (char *)inet_ntoa(ip), (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RTPDataPort", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)addr->GetPort());
        }
        addr = (RTPIPv4Address *)(data->GetRTCPDataAddress());
        if (addr) {
            in_addr ip;
#ifdef _WIN32
            ip.S_un.S_addr = htonl(addr->GetIP());
#else
            ip.s_addr = htonl(addr->GetIP());
#endif
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RTCPDataAddress", (INTEGER)VARIABLE_STRING, (char *)inet_ntoa(ip), (NUMBER)0);
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RTCPDataPort", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)addr->GetPort());
        }

        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "HasData", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->HasData());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SSRC", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->GetSSRC());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsOwnSSRC", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsOwnSSRC());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsCSRC", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsCSRC());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsSender", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsSender());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsValidated", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsValidated());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsActive", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsActive());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsProcessedInRTCP", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsProcessedInRTCP());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsRTPAddressSet", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsRTPAddressSet());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "IsRTCPAddressSet", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->IsRTCPAddressSet());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ReceivedBYE", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->ReceivedBYE());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "GetTimestampUnit", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->GetTimestampUnit());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_HasInfo", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_HasInfo());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_RTPTimestamp", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_GetRTPTimestamp());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_PacketCount", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_GetPacketCount());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_ByteCount", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_GetByteCount());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_Prev_HasInfo", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_Prev_HasInfo());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_Prev_RTPTimestamp", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_Prev_GetRTPTimestamp());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_Prev_PacketCount", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_Prev_GetPacketCount());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SR_Prev_ByteCount", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->SR_Prev_GetByteCount());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_HasInfo", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_HasInfo());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_FractionLost", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetFractionLost());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_PacketsLost", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetPacketsLost());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_ExtendedHighestSequenceNumber", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetExtendedHighestSequenceNumber());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_Jitter", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetJitter());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_LastSRTimestamp", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetLastSRTimestamp());
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "RR_DelaySinceLastSR", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)data->RR_GetDelaySinceLastSR());
        if (full_info) {
            // it leaks !!!
            size_t  len;
            uint8_t *s = data->SDES_GetCNAME(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_CNAME", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetName(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_Name", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetEMail(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_EMail", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetPhone(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_Phone", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetLocation(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_Location", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetTool(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_Tool", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
            s = data->SDES_GetNote(&len);
            if (len)
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "SDES_Note", (INTEGER)VARIABLE_STRING, (char *)s, (NUMBER)len);
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPPoll, 1)
    T_HANDLE(RTPPoll, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    RETURN_NUMBER(sess->Poll());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPAddDestination, 3)
    T_HANDLE(RTPAddDestination, 0)
    T_STRING(RTPAddDestination, 1)
    T_NUMBER(RTPAddDestination, 2)

    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : INADDR_NONE;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip, PARAM_INT(2));
    RETURN_NUMBER(sess->AddDestination(addr));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPDeleteDestination, 3)
    T_HANDLE(RTPDeleteDestination, 0)
    T_STRING(RTPDeleteDestination, 1)
    T_NUMBER(RTPDeleteDestination, 2)

    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : INADDR_NONE;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip, PARAM_INT(2));
    RETURN_NUMBER(sess->DeleteDestination(addr));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPClearDestinations, 1)
    T_HANDLE(RTPClearDestinations, 0)

    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->ClearDestinations();

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetDefaultPayloadType, 2)
    T_HANDLE(RTPSetDefaultPayloadType, 0)
    T_NUMBER(RTPSetDefaultPayloadType, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetDefaultPayloadType(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetDefaultMark, 2)
    T_HANDLE(RTPSetDefaultMark, 0)
    T_NUMBER(RTPSetDefaultMark, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetDefaultMark(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetDefaultTimestampIncrement, 2)
    T_HANDLE(RTPSetDefaultTimestampIncrement, 0)
    T_NUMBER(RTPSetDefaultTimestampIncrement, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetDefaultTimestampIncrement(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPIncrementTimestamp, 2)
    T_HANDLE(RTPIncrementTimestamp, 0)
    T_NUMBER(RTPIncrementTimestamp, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->IncrementTimestamp(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPIncrementTimestampDefault, 1)
    T_HANDLE(RTPIncrementTimestampDefault, 0)
    T_NUMBER(RTPIncrementTimestampDefault, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->IncrementTimestampDefault();
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetPreTransmissionDelay, 2)
    T_HANDLE(RTPSetPreTransmissionDelay, 0)
    T_NUMBER(RTPSetPreTransmissionDelay, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetPreTransmissionDelay(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetReceiveMode, 2)
    T_HANDLE(RTPSetReceiveMode, 0)
    T_NUMBER(RTPSetReceiveMode, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetReceiveMode((RTPTransmitter::ReceiveMode)PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPAddToIgnoreList, 2)
    T_HANDLE(RTPAddToIgnoreList, 0)
    T_STRING(RTPAddToIgnoreList, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : -1;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip);

    sess->AddToIgnoreList(addr);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPDeleteFromIgnoreList, 2)
    T_HANDLE(RTPDeleteFromIgnoreList, 0)
    T_STRING(RTPDeleteFromIgnoreList, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : -1;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip);
    sess->DeleteFromIgnoreList(addr);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPClearIgnoreList, 1)
    T_HANDLE(RTPClearIgnoreList, 0)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->ClearIgnoreList();
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPAddToAcceptList, 2)
    T_HANDLE(RTPAddToAcceptList, 0)
    T_STRING(RTPAddToAcceptList, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : -1;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip);

    sess->AddToAcceptList(addr);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPDeleteFromAcceptList, 2)
    T_HANDLE(RTPDeleteFromAcceptList, 0)
    T_STRING(RTPDeleteFromAcceptList, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    char         *str = PARAM(1);
    unsigned int ip   = str ? inet_addr(str) : -1;
    if (ip != INADDR_NONE) {
        ip = ntohl(ip);
    }
    RTPIPv4Address addr(ip);
    sess->DeleteFromAcceptList(addr);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetMaximumPacketSize, 2)
    T_HANDLE(RTPSetMaximumPacketSize, 0)
    T_NUMBER(RTPSetMaximumPacketSize, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetMaximumPacketSize(PARAM_INT(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetSessionBandwidth, 2)
    T_HANDLE(RTPSetSessionBandwidth, 0)
    T_NUMBER(RTPSetSessionBandwidth, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetSessionBandwidth(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetTimestampUnit, 2)
    T_HANDLE(RTPSetTimestampUnit, 0)
    T_NUMBER(RTPSetTimestampUnit, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetTimestampUnit(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetNameInterval, 2)
    T_HANDLE(RTPSetNameInterval, 0)
    T_NUMBER(RTPSetNameInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetNameInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetEMailInterval, 2)
    T_HANDLE(RTPSetEMailInterval, 0)
    T_NUMBER(RTPSetEMailInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetEMailInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocationInterval, 2)
    T_HANDLE(RTPSetLocationInterval, 0)
    T_NUMBER(RTPSetLocationInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocationInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetPhoneInterval, 2)
    T_HANDLE(RTPSetPhoneInterval, 0)
    T_NUMBER(RTPSetPhoneInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetPhoneInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetToolInterval, 2)
    T_HANDLE(RTPSetToolInterval, 0)
    T_NUMBER(RTPSetToolInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetToolInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetNoteInterval, 2)
    T_HANDLE(RTPSetNoteInterval, 0)
    T_NUMBER(RTPSetNoteInterval, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetNoteInterval(PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalName, 2)
    T_HANDLE(RTPSetLocalName, 0)
    T_STRING(RTPSetLocalName, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalName(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalEMail, 2)
    T_HANDLE(RTPSetLocalEMail, 0)
    T_STRING(RTPSetLocalEMail, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalEMail(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalLocation, 2)
    T_HANDLE(RTPSetLocalLocation, 0)
    T_STRING(RTPSetLocalLocation, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalLocation(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalPhone, 2)
    T_HANDLE(RTPSetLocalPhone, 0)
    T_STRING(RTPSetLocalPhone, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalPhone(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalTool, 2)
    T_HANDLE(RTPSetLocalTool, 0)
    T_STRING(RTPSetLocalTool, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalTool(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPSetLocalNote, 2)
    T_HANDLE(RTPSetLocalNote, 0)
    T_STRING(RTPSetLocalNote, 1)
    RTPSession * sess = (RTPSession *)PARAM_INT(0);
    sess->SetLocalNote(PARAM(1), PARAM_LEN(1));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(RTPWait, 1)
    T_NUMBER(RTPWait, 0)
    int ms = PARAM_INT(0);
    RTPTime::Wait(RTPTime(ms / 1000, (ms % 1000) * 1000));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//

