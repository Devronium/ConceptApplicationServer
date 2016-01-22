#include <ptlib.h>
#include "SIPManager.h"
//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

class SimpleOpalProcess : public PProcess {
    PCLASSINFO(SimpleOpalProcess, PProcess)
public:
    SimpleOpalProcess() : PProcess("Open Phone Abstraction Library", "ConceptOPALAPI",
                                   3, 6, ReleaseCode, 1) {
    }

    void Main() {
    }
};

static INVOKE_CALL InvokePtr  = 0;
SimpleOpalProcess  *pInstance = 0;

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
    pInstance = new SimpleOpalProcess();
    //pInstance->PreInitialise(0, 0);


    DEFINE_ICONSTANT("EndedByLocalUser", OpalConnection::EndedByLocalUser)
    DEFINE_ICONSTANT("EndedByNoAccept", OpalConnection::EndedByNoAccept)
    DEFINE_ICONSTANT("EndedByAnswerDenied", OpalConnection::EndedByAnswerDenied)
    DEFINE_ICONSTANT("EndedByRemoteUser", OpalConnection::EndedByRemoteUser)
    DEFINE_ICONSTANT("EndedByRefusal", OpalConnection::EndedByRefusal)
    DEFINE_ICONSTANT("EndedByNoAnswer", OpalConnection::EndedByNoAnswer)
    DEFINE_ICONSTANT("EndedByCallerAbort", OpalConnection::EndedByCallerAbort)
    DEFINE_ICONSTANT("EndedByTransportFail", OpalConnection::EndedByTransportFail)
    DEFINE_ICONSTANT("EndedByConnectFail", OpalConnection::EndedByConnectFail)
    DEFINE_ICONSTANT("EndedByGatekeeper", OpalConnection::EndedByGatekeeper)
    DEFINE_ICONSTANT("EndedByNoUser", OpalConnection::EndedByNoUser)
    DEFINE_ICONSTANT("EndedByNoBandwidth", OpalConnection::EndedByNoBandwidth)
    DEFINE_ICONSTANT("EndedByCapabilityExchange", OpalConnection::EndedByCapabilityExchange)
    DEFINE_ICONSTANT("EndedByCallForwarded", OpalConnection::EndedByCallForwarded)
    DEFINE_ICONSTANT("EndedBySecurityDenial", OpalConnection::EndedBySecurityDenial)
    DEFINE_ICONSTANT("EndedByLocalBusy", OpalConnection::EndedByLocalBusy)
    DEFINE_ICONSTANT("EndedByLocalCongestion", OpalConnection::EndedByLocalCongestion)
    DEFINE_ICONSTANT("EndedByRemoteBusy", OpalConnection::EndedByRemoteBusy)
    DEFINE_ICONSTANT("EndedByRemoteCongestion", OpalConnection::EndedByRemoteCongestion)
    DEFINE_ICONSTANT("EndedByUnreachable", OpalConnection::EndedByUnreachable)
    DEFINE_ICONSTANT("EndedByNoEndPoint", OpalConnection::EndedByNoEndPoint)
    DEFINE_ICONSTANT("EndedByHostOffline", OpalConnection::EndedByHostOffline)
    DEFINE_ICONSTANT("EndedByTemporaryFailure", OpalConnection::EndedByTemporaryFailure)
    DEFINE_ICONSTANT("EndedByQ931Cause", OpalConnection::EndedByQ931Cause)
    DEFINE_ICONSTANT("EndedByDurationLimit", OpalConnection::EndedByDurationLimit)
    DEFINE_ICONSTANT("EndedByInvalidConferenceID", OpalConnection::EndedByInvalidConferenceID)
    DEFINE_ICONSTANT("EndedByNoDialTone", OpalConnection::EndedByNoDialTone)
    DEFINE_ICONSTANT("EndedByNoRingBackTone", OpalConnection::EndedByNoRingBackTone)
    DEFINE_ICONSTANT("EndedByOutOfService", OpalConnection::EndedByOutOfService)
    DEFINE_ICONSTANT("EndedByAcceptingCallWaiting", OpalConnection::EndedByAcceptingCallWaiting)
    DEFINE_ICONSTANT("EndedByGkAdmissionFailed", OpalConnection::EndedByGkAdmissionFailed)
    DEFINE_ICONSTANT("EndedByMediaFailed", OpalConnection::EndedByMediaFailed)
    DEFINE_ICONSTANT("EndedByCallCompletedElsewhere", OpalConnection::EndedByCallCompletedElsewhere)
    DEFINE_ICONSTANT("EndedByCertificateAuthority", OpalConnection::EndedByCertificateAuthority)
    DEFINE_ICONSTANT("EndedByIllegalAddress", OpalConnection::EndedByIllegalAddress)

    DEFINE_ICONSTANT("RTP_PCMU", RTP_DataFrame::PCMU)
    DEFINE_ICONSTANT("RTP_FS1016", RTP_DataFrame::FS1016)
    DEFINE_ICONSTANT("RTP_G721", RTP_DataFrame::G721)
    DEFINE_ICONSTANT("RTP_G726", RTP_DataFrame::G726)
    DEFINE_ICONSTANT("RTP_GSM", RTP_DataFrame::GSM)
    DEFINE_ICONSTANT("RTP_G7231", RTP_DataFrame::G7231)
    DEFINE_ICONSTANT("RTP_DVI4_8k", RTP_DataFrame::DVI4_8k)
    DEFINE_ICONSTANT("RTP_DVI4_16k", RTP_DataFrame::DVI4_16k)
    DEFINE_ICONSTANT("RTP_LPC", RTP_DataFrame::LPC)
    DEFINE_ICONSTANT("RTP_PCMA", RTP_DataFrame::PCMA)
    DEFINE_ICONSTANT("RTP_G722", RTP_DataFrame::G722)
    DEFINE_ICONSTANT("RTP_L16_Stereo", RTP_DataFrame::L16_Stereo)
    DEFINE_ICONSTANT("RTP_L16_Mono", RTP_DataFrame::L16_Mono)
    DEFINE_ICONSTANT("RTP_G723", RTP_DataFrame::G723)
    DEFINE_ICONSTANT("RTP_CN", RTP_DataFrame::CN)
    DEFINE_ICONSTANT("RTP_MPA", RTP_DataFrame::MPA)
    DEFINE_ICONSTANT("RTP_G728", RTP_DataFrame::G728)
    DEFINE_ICONSTANT("RTP_DVI4_11k", RTP_DataFrame::DVI4_11k)
    DEFINE_ICONSTANT("RTP_DVI4_22k", RTP_DataFrame::DVI4_22k)
    DEFINE_ICONSTANT("RTP_G729", RTP_DataFrame::G729)
    DEFINE_ICONSTANT("RTP_Cisco_CN", RTP_DataFrame::Cisco_CN)

    DEFINE_ICONSTANT("RTP_CelB", RTP_DataFrame::CelB)
    DEFINE_ICONSTANT("RTP_JPEG", RTP_DataFrame::JPEG)
    DEFINE_ICONSTANT("RTP_H261", RTP_DataFrame::H261)
    DEFINE_ICONSTANT("RTP_MPV", RTP_DataFrame::MPV)
    DEFINE_ICONSTANT("RTP_MP2T", RTP_DataFrame::MP2T)
    DEFINE_ICONSTANT("RTP_H263", RTP_DataFrame::H263)

    //DEFINE_ICONSTANT("RTP_T38", RTP_DataFrame::T38)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    // temporary solution (a deadlock problem for broken connections)
    if (pInstance) {
        //pInstance->InternalMain();
        delete pInstance;
        pInstance = 0;
    }
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneCreate, 0)
    MyManager * opal = new MyManager();
    opal->Invoke     = (void *)Invoke;
    RETURN_NUMBER((SYS_INT)opal);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetNoMediaTimeout, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    PTimeInterval i;
    i.SetInterval(PARAM_INT(1));
    opal->SetNoMediaTimeout(i);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneGetNoMediaTimeout, 1)
    T_HANDLE(0)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    int i = opal->GetNoMediaTimeout().GetMilliSeconds();
    RETURN_NUMBER(i);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneDestroy, 1)
    T_NUMBER(0)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    if (opal) {
        opal->ShutDownEndpoints();
        delete opal;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetPortBase, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    opal->SetPortBase(PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetSilenceDetector, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    opal->SetSilenceDetector(PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetJitter, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->SetJitter(PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SIPPhoneConnect, 4, 8)
    T_HANDLE(0)
    T_STRING(1)
    T_STRING(2)
    T_STRING(3)

    PString ua = "";
    PString domain   = "";
    PString endpoint = "";
    if (PARAMETERS_COUNT > 4) {
        T_STRING(4)
        endpoint = PARAM(4);
    }
    if (PARAMETERS_COUNT > 5) {
        T_STRING(5)
        domain = PARAM(5);
    }
    if (PARAMETERS_COUNT > 6) {
        T_STRING(6)
        ua = PARAM(6);
    }
    PString iface = "";
    if (PARAMETERS_COUNT > 7) {
        T_STRING(7)
        iface = PARAM(7);
    }
    MyManager *opal = (MyManager *)PARAM_INT(0);
    int       res   = opal->Initialise(PARAM(1), PARAM(2), PARAM(3), domain, endpoint, iface, ua);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneAddListener, 2)
    T_HANDLE(0)
    T_STRING(1);
    MyManager *opal = (MyManager *)PARAM_INT(0);
    int       res   = opal->StartListener(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneUnregister, 0)
    T_HANDLE(0)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    opal->Unregister();
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneRemoveListener, 2)
    T_HANDLE(0)
    T_STRING(1);
    MyManager *opal = (MyManager *)PARAM_INT(0);
    int       res   = opal->StopListener(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneCall, 2)
    T_HANDLE(0)
    T_STRING(1)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->StartCall(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneHoldRetrieveCall, 1)
    T_HANDLE(0)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->HoldRetrieveCall();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneHangupCurrentCall, 1)
    T_HANDLE(0)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    opal->HangupCurrentCall();
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneAnswer, 1)
    T_HANDLE(0)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->Answer();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneTransfer, 2)
    T_HANDLE(0)
    T_STRING(1)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->Transfer(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneForward, 2)
    T_HANDLE(0)
    T_STRING(1)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->ForwardCall(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneIsRegistered, 1)
    T_HANDLE(0)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = (int)opal->isRegistered();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetSTUNServer, 2)
    T_HANDLE(0)
    T_STRING(1)
    MyManager * opal = (MyManager *)PARAM_INT(0);
    int res = opal->SetSTUNServer(PARAM(1));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SIPPhoneReject, 1, 2)
    T_HANDLE(0)
    OpalConnection::CallEndReason code = OpalConnection::EndedByAnswerDenied;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        code = (OpalConnection::CallEndReason)PARAM_INT(1);
    }
    MyManager *opal = (MyManager *)PARAM_INT(0);
    int       res   = opal->Reject(code);
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SIPPhoneHangupAll, 1, 2)
    T_HANDLE(0)
    OpalConnection::CallEndReason code = OpalConnection::EndedByLocalUser;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        code = (OpalConnection::CallEndReason)PARAM_INT(1);
    }
    MyManager *opal = (MyManager *)PARAM_INT(0);
    opal->HangupAll(code);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSendTone, 2)
    T_HANDLE(0)
    T_STRING(1)
    char *s = PARAM(1);
    MyManager *opal = (MyManager *)PARAM_INT(0);
    if (s)
        opal->SendTone(s[0]);
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnConnectRequest, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnConnectRequest)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnConnectRequest);
    opal->OnConnectRequest = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnEstablishedCall, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnEstablishedCallDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnEstablishedCallDelegate);
    opal->OnEstablishedCallDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnClearedCall, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnClearedCallDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnClearedCallDelegate);
    opal->OnClearedCallDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnShowIncoming, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnShowIncomingDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnShowIncomingDelegate);
    opal->OnShowIncomingDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnShowOutgoing, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnShowOutgoingDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnShowOutgoingDelegate);
    opal->OnShowOutgoingDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnMediaFrame, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnMediaFrameDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnMediaFrameDelegate);
    opal->OnMediaFrameDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnMediaFrameRequest, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnMediaFrameRequestDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnMediaFrameRequestDelegate);
    opal->OnMediaFrameRequestDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnUserInputString, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnUserInputStringDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnUserInputStringDelegate);
    opal->OnUserInputStringDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneOnAlerting, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnAlertingDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnAlertingDelegate);
    opal->OnAlertingDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------
CONCEPT_FUNCTION_IMPL(SIPPhoneOnOpenMediaStreamDelegate, 2)
    T_HANDLE(0)
    T_DELEGATE(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    Invoke(INVOKE_EXTERNAL_THREADING, PARAMETER(1));
    Invoke(INVOKE_LOCK_VARIABLE, PARAMETER(1));
    if (opal->OnOpenMediaStreamDelegate)
        Invoke(INVOKE_FREE_VARIABLE, opal->OnOpenMediaStreamDelegate);
    opal->OnOpenMediaStreamDelegate = PARAMETER(1);
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamRead, 3)
    T_HANDLE(0)
    T_NUMBER(2)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int len    = PARAM_INT(2);
    int length = -1;
    if (stream->IsOpen()) {
        char *buffer = 0;
        CORE_NEW(len, buffer);
        if (!stream->ReadData((BYTE *)buffer, len, length)) {
            CORE_DELETE(buffer);
            SET_STRING(1, "");
            buffer = 0;
        }

        if (buffer) {
            if (length > 0) {
                SetVariable(PARAMETER(1), -1, buffer, length);
            } else {
                SET_STRING(1, "");
                CORE_DELETE(buffer);
            }
        } else {
            SET_STRING(1, "");
        }
    }
    RETURN_NUMBER(length);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamIsOpen, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->IsOpen();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamIsSource, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->IsSource();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamIsPaused, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->IsPaused();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamIsSynchronous, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->IsSynchronous();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamWrite, 2)
    T_HANDLE(0)
    T_STRING(1)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int length = -1;
    if (stream->IsOpen()) {
        if (!stream->WriteData((BYTE *)PARAM(1), PARAM_LEN(1), length))
            length = -1;
    }
    RETURN_NUMBER(length);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamWriteAll, 2)
    T_HANDLE(0)
    T_STRING(1)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int length = -1;
    if (stream->IsOpen()) {
        char *buf    = PARAM(1);
        int  len     = PARAM_LEN(1);
        int  length2 = 0;
        while (len > 0) {
            if (!stream->WriteData((BYTE *)buf, len, length2))
                break;
            else {
                if (length2 == -1)
                    break;

                if (length == -1)
                    length = 0;
                length += length2;
                buf    += length2;
                len    -= length2;
            }
        }
    }
    RETURN_NUMBER(length);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamClose, 1)
    T_NUMBER(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    if (stream) {
        if (stream->IsOpen())
            stream->Close();
        stream->SafeDereference();
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamNotUsed, 1)
    T_NUMBER(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    if (stream)
        stream->SafeDereference();
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamIsSink, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->IsSink();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamGetDataSize, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->GetDataSize();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamSetDataSize, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_NUMBER(2)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->SetDataSize(PARAM_INT(1), PARAM_INT(2));
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamPause, 2)
    T_HANDLE(0)
    T_NUMBER(1)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);

    stream->SetPaused(PARAM_INT(1));
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamStart, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    int res = stream->Start();
    RETURN_NUMBER(res);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPMediaStreamFormat, 1)
    T_HANDLE(0)
    OpalMediaStream * stream = (OpalMediaStream *)PARAM_INT(0);
    OpalMediaFormat fmt = stream->GetMediaFormat();
    CREATE_ARRAY(RESULT);
    PString name = fmt.GetName();
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Name", (INTEGER)VARIABLE_STRING, (char *)name.GetPointer(), (NUMBER)name.GetLength());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "ClockRate", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetClockRate());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "CodecVersionTime", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetCodecVersionTime());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "EncodingName", (INTEGER)VARIABLE_STRING, (char *)fmt.GetEncodingName(), (NUMBER)0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "FrameSize", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetFrameSize());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "FrameTime", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetFrameTime());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxBandwidth.Rx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetMaxBandwidth().Rx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxBandwidth.Tx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetMaxBandwidth().Tx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxBandwidth.RxTx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetMaxBandwidth().RxTx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "MaxBandwidth.TxRx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetMaxBandwidth().TxRx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "PayloadType", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)fmt.GetPayloadType());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "Size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)fmt.GetSize());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "TimeUnits", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)fmt.GetTimeUnits());
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "UsedBandwidth.Rx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetUsedBandwidth().Rx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "UsedBandwidth.Tx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetUsedBandwidth().Tx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "UsedBandwidth.RxTx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetUsedBandwidth().RxTx);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "UsedBandwidth.TxRx", (INTEGER)VARIABLE_NUMBER, (char *)0, (NUMBER)fmt.GetUsedBandwidth().TxRx);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneGetCodecs, 1)
    T_HANDLE(0)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);
    PStringArray arr = opal->GetCodecs();
    int          len = arr.GetSize();
    for (int i = 0; i < len; i++) {
        PString p   = arr[i];
        int     len = p.GetSize();
        if (len > 0) {
            const char *ptr = p.GetPointer();
            Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, ptr, (INTEGER)VARIABLE_STRING, (char *)ptr, (NUMBER)len);
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(SIPPhoneSetCodecsOrder, 2)
    T_HANDLE(0)
    T_ARRAY(1)

    MyManager * opal = (MyManager *)PARAM_INT(0);
    PStringArray order;
    int          len = Invoke(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));

    for (INTEGER i = 0; i < len; i++) {
        INTEGER type    = 0;
        char    *szData = 0;
        NUMBER  nData   = 0;
        Invoke(INVOKE_GET_ARRAY_ELEMENT, PARAMETER(1), i, &type, &szData, &nData);
        if ((type == VARIABLE_STRING) && (szData) && (szData[0])) {
            PString p = szData;
            order += p;
        }
    }
    opal->SetMediaFormatOrder(order);
    RETURN_NUMBER(0);
END_IMPL
