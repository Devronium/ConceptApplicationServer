/*
 * main.h
 *
 * A simple OPAL "net telephone" application.
 *
 * Copyright (c) 2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 28481 $
 * $Author: rjongbloed $
 * $Date: 2012-10-05 03:13:17 -0500 (Fri, 05 Oct 2012) $
 */

#ifndef _SimpleOpal_MAIN_H
#define _SimpleOpal_MAIN_H

#include <ptclib/ipacl.h>
#include <opal/manager.h>
#include <ep/localep.h>
//#include <ep/pcss.h>
//#include <ep/ivr.h>


#if OPAL_SIP
 #include <sip/sip.h>
#endif

#if OPAL_FAX
 #include <t38/t38proto.h>
#endif

class MyManager;
class SIPEndPoint;
class H323EndPoint;
class H323SEndPoint;
class OpalLineEndPoint;


class MyPCSSEndPoint : public OpalLocalEndPoint {
    PCLASSINFO(MyPCSSEndPoint, OpalLocalEndPoint);
public:
    MyManager *owner;
    MyPCSSEndPoint(MyManager& manager);

    //virtual OpalLocalConnection::AnswerCallResponse OnAnswerCall(const PString & callerName);
    virtual PBoolean OnOutgoingCall(OpalLocalConnection& connection);
    virtual PBoolean OnIncomingCall(OpalLocalConnection& connection);
    virtual bool OnReadMediaFrame(const OpalLocalConnection& connection, const OpalMediaStream& mediaStream, RTP_DataFrame& frame);
    virtual bool OnWriteMediaFrame(const OpalLocalConnection& connection, const OpalMediaStream& mediaStream, RTP_DataFrame& frame);

    PString incomingConnectionToken;
    bool    autoAnswer;
};


class MyManager : public OpalManager {
    PCLASSINFO(MyManager, OpalManager);
public:
    void *Invoke;
    void *OnEstablishedCallDelegate;
    void *OnClearedCallDelegate;
    void *OnOpenMediaStreamDelegate;

    void *OnAnswerCallDelegate;
    void *OnHoldDelegateDelegate;
    void *OnForwardedDelegate;
    void *OnTransferNotifyDelegate;
    void *OnAlertingDelegate;

    void *OnShowIncomingDelegate;
    void *OnShowOutgoingDelegate;
    void *OnUserInputStringDelegate;
    void *OnMediaFrameDelegate;
    void *OnMediaFrameRequestDelegate;
    void *OnConnectRequest;

    int CallDelegate(void *deleg, PString str);
    int CallDelegateWithResult(void *deleg, PString str);
    int CallDelegate2(void *deleg, BYTE *data, int len, int type);
    int CallDelegate3(void *deleg, RTP_DataFrame& frame);

    MyManager();
    ~MyManager();

    PBoolean Initialise(PString username, PString password, PString proxy, PString domain, PString endpoint, PString iface = "", PString user_agent = "ConceptSIP/UA_OPL3");
    void Main();
    virtual PBoolean OnIncomingConnection(OpalConnection& connection, unsigned options, OpalConnection::StringOptions *stringOptions);

    virtual SIPConnection *CreateConnection(const SIPConnection::Init& init);
    virtual void OnEstablishedCall(OpalCall& call);
    virtual void OnClearedCall(OpalCall& call);
    virtual void OnAlerting(OpalConnection& connection);
    virtual PBoolean OnOpenMediaStream(OpalConnection& connection, OpalMediaStream& stream);
    virtual void OnUserInputString(OpalConnection& connection, const PString& value);

    bool SetJitter(int min, int max);
    void SetSilenceDetector(bool on);
    void SetPortBase(int portbase, int portmax);

    void HangupCurrentCall();
    bool StartCall(const PString& ostr);
    bool HoldRetrieveCall();
    void TransferCall(const PString& dest);
    bool ForwardCall(PString dest);
    void SendMessageToRemoteNode(const PString& ostr);
    void SendTone(const char tone);
    bool Reject(OpalConnection::CallEndReason code = OpalConnection::EndedByAnswerDenied);
    bool Answer();
    bool Transfer(PString address);
    void HangupAll(OpalConnection::CallEndReason code = OpalConnection::EndedByLocalUser);
    int StartListener(PString iface);
    int StopListener(PString iface);
    void Unregister();
    PStringArray GetCodecs();
    bool isRegistered();

    PString currentCallToken;
    PString heldCallToken;
protected:
    PString             forward;
    PString             aor;
    SIPRegister::Params params;
    OpalMediaFormatList allMediaFormats;

    bool InitialiseH323EP(PArgList& args, PBoolean secure, H323EndPoint *h323EP);


#if OPAL_LID
    OpalLineEndPoint *potsEP;
#endif
    MyPCSSEndPoint *pcssEP;
#if OPAL_H323
    H323EndPoint *h323EP;
#endif
#if OPAL_SIP
    SIPEndPoint *sipEP;
#endif
#if OPAL_FAX
    OpalFaxEndPoint *faxEP;
#endif

    bool    pauseBeforeDialing;
    PString srcEP;
};
#endif  // _SimpleOpal_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
