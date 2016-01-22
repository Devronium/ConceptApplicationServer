/*
 * main.cxx
 *
 * A simple H.323 "net telephone" application.
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
 * $Revision: 28820 $
 * $Author: rjongbloed $
 * $Date: 2013-01-09 00:19:34 -0600 (Wed, 09 Jan 2013) $
 */

#include <ptlib.h>

#include <opal/buildopts.h>
#if OPAL_SIP
#include <sip/sip.h>
#include <sip/sdp.h>
#endif

#if OPAL_FAX
#include <t38/t38proto.h>
#endif

#include <opal/transcoders.h>
#include <codec/opalpluginmgr.h>
#include <lids/lidep.h>

#include <ptlib/config.h>
#include <ptclib/pstun.h>

#include "SIPManager.h"
#include "stdlibrary.h"
#define new PNEW

MyManager::MyManager()
{
#if OPAL_LID
  potsEP = NULL;
#endif
  pcssEP = NULL;

#if OPAL_H323
  h323EP = NULL;
#endif
#if OPAL_SIP
  sipEP  = NULL;
#endif
#if OPAL_FAX
  faxEP = NULL;
#endif
  Invoke = NULL;
  OnEstablishedCallDelegate = NULL;
  OnClearedCallDelegate = NULL;
  OnOpenMediaStreamDelegate = NULL;

  OnAnswerCallDelegate = NULL;
  OnHoldDelegateDelegate = NULL;
  OnForwardedDelegate = NULL;
  OnTransferNotifyDelegate = NULL;
  OnAlertingDelegate = NULL;

  OnShowIncomingDelegate = NULL;
  OnShowOutgoingDelegate = NULL;
  OnUserInputStringDelegate = NULL;

  OnMediaFrameDelegate = NULL;
  OnMediaFrameRequestDelegate = NULL;

  OnConnectRequest = NULL;
  pauseBeforeDialing = false;
}


#define RESET_VAR(var) if (var) { ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,var); var = NULL; }

MyManager::~MyManager() {
#if OPAL_LID
  // Must do this before we destroy the manager or a crash will result
  if (potsEP != NULL)
    potsEP->RemoveAllLines();
#endif

  if (Invoke) {
      RESET_VAR(OnEstablishedCallDelegate)
      RESET_VAR(OnClearedCallDelegate)
      RESET_VAR(OnOpenMediaStreamDelegate)

      RESET_VAR(OnAnswerCallDelegate)
      RESET_VAR(OnHoldDelegateDelegate)
      RESET_VAR(OnForwardedDelegate)
      RESET_VAR(OnTransferNotifyDelegate)
      RESET_VAR(OnAlertingDelegate)

      RESET_VAR(OnShowIncomingDelegate)
      RESET_VAR(OnShowOutgoingDelegate)
      RESET_VAR(OnUserInputStringDelegate)

      RESET_VAR(OnMediaFrameDelegate)
      RESET_VAR(OnMediaFrameRequestDelegate)

      RESET_VAR(OnConnectRequest)
  }

  if (sipEP) {
      //delete sipEP;
      sipEP = NULL;
  }
}

PBoolean MyManager::OnIncomingConnection(OpalConnection & connection, unsigned options, OpalConnection::StringOptions * stringOptions) {
    if (!forward.IsEmpty())
        forward.MakeEmpty();
    if (!OpalManager::OnIncomingConnection(connection, options, stringOptions))
        return false;
    if (OnConnectRequest) {
        int res=this->CallDelegateWithResult(OnConnectRequest, connection.GetRemotePartyName());
        if (!forward.IsEmpty())
            connection.ForwardCall(forward);
        return res;
    }
    return true;
}

SIPConnection * MyManager::CreateConnection(const SIPConnection::Init & init) {
    SIPConnection *res=new SIPConnection(init);
    OpalConnection::StringOptions so=res->GetStringOptions();
    so.SetBoolean(OPAL_OPT_OFFER_SDP_PTIME, true);
    res->SetStringOptions(so, true);
    return res;
}

PBoolean MyManager::SetJitter(int minJitter, int maxJitter) {
    if (minJitter >= 20 && minJitter <= maxJitter && maxJitter <= 1000)
      SetAudioJitterDelay(minJitter, maxJitter);
    else
      return false;
    return true;
}

void MyManager::SetSilenceDetector(bool on) {
    if (on)
        silenceDetectParams.m_mode=OpalSilenceDetector::NoSilenceDetection;
    else
        silenceDetectParams.m_mode=OpalSilenceDetector::AdaptiveSilenceDetection;
}

void MyManager::SetPortBase(int portbase, int portmax) {
    SetTCPPorts  (portbase, portmax);
    SetUDPPorts  (portbase, portmax);
    SetRtpIpPorts(portbase, portmax);
}

bool MyManager::isRegistered() {
    if (sipEP)
        return sipEP->IsRegistered(aor);
    return false;
}

int MyManager::StartListener(PString iface) {
    int res=0;
    if (sipEP) {
        res=sipEP->StartListener(iface);
        if (res)
            res=sipEP->Register(params, aor);
    }
    return res;
}

int MyManager::StopListener(PString iface) {
    int res=0;
    if (sipEP)
        res=sipEP->StopListener(iface);
    return res;
}

PBoolean MyManager::Initialise(PString username, PString password, PString proxy, PString domain, PString endpoint, PString iface, PString user_agent) {
    bool registered;
#if OPAL_SIP

  ///////////////////////////////////////
  // Create SIP protocol handler
    if (sipEP != NULL)
        delete sipEP;

    sipEP = new SIPEndPoint(*this);
    sipEP->SetUserAgent(user_agent);
    //if (str *= "rfc2833")
      sipEP->SetSendUserInputMode(OpalConnection::SendUserInputAsRFC2833);
    //else if (str *= "info-tone")*/
      //sipEP->SetSendUserInputMode(OpalConnection::SendUserInputAsTone);
    /*else if (str *= "info-string")*/
      //sipEP->SetSendUserInputMode(OpalConnection::SendUserInputAsString);

      sipEP->SetProxy(proxy);

    // Get local username, multiple uses of -u indicates additional aliases
      sipEP->SetDefaultLocalPartyName(username);

    // Start the listener thread for incoming calls.
    registered=sipEP->StartListener(iface);
    //if (!sipEP->StartListener(iface))
    //  return false;

    pcssEP = new MyPCSSEndPoint(*this);
    pcssEP->owner=this;

    //cout <<  "SIP started on " << setfill(',') << sipEP->GetListeners() << setfill(' ') << endl;

    //SIPRegister::Params params;
    params.m_registrarAddress = proxy;
    params.m_addressOfRecord = username;
    params.m_password = password;
    params.m_realm = domain;
    
    if (registered)
        registered=sipEP->Register(params, aor);
    /*if (sipEP->Register(params, aor))
        cout << "Using SIP registrar " << params.m_registrarAddress << " for " << aor << endl;
    else
        cout << "Could not use SIP registrar " << params.m_registrarAddress << endl;*/
    pauseBeforeDialing = true;
#endif


#if OPAL_FAX
#if OPAL_PTLIB_ASN
  ///////////////////////////////////////
  // Create T38 protocol handler
  {
    OpalMediaFormat fmt(OpalT38); // Force instantiation of T.38 media format
    faxEP = new OpalFaxEndPoint(*this);
    allMediaFormats += faxEP->GetMediaFormats();
  }
#endif
#endif

#if OPAL_SIP
    if (sipEP != NULL) {
#if OPAL_FAX
      AddRouteEntry("t38:.*             = sip:<da>");
      AddRouteEntry("sip:.*\tfax@.*     = t38:received_fax_%s.tif;receive");
      AddRouteEntry("sip:.*\tsip:329@.* = t38:received_fax_%s.tif;receive");
#endif
      AddRouteEntry("pots:.*\\*.*\\*.*  = sip:<dn2ip>");
      AddRouteEntry("pots:.*            = sip:<da>");
      AddRouteEntry("local:.*              = sip:<da>");
    }
#endif

#if OPAL_LID
    if (potsEP != NULL) {
#if OPAL_H323
      AddRouteEntry("h323:.* = pots:<du>");
#if OPAL_PTLIB_SSL
      //if (h323sEP != NULL) 
        AddRouteEntry("h323s:.* = pots:<du>");
#endif
#endif
#if OPAL_SIP
      AddRouteEntry("sip:.*  = pots:<du>");
#endif
    } else
#endif // OPAL_LID
    if (pcssEP != NULL) {
#if OPAL_H323
      AddRouteEntry("h323:.* = local:");
#if OPAL_PTLIB_SSL
      //if (h323sEP != NULL) 
        AddRouteEntry("h323s:.* = local:");
#endif
#endif
#if OPAL_SIP
      AddRouteEntry("sip:.*  = local:");
#endif
    }

#if OPAL_FAX
  if (faxEP != NULL) {
    AddRouteEntry("sip:.*  = t38:<da>");
    AddRouteEntry("sip:.*  = fax:<da>");
  }
#endif

  PString defaultSrcEP = pcssEP != NULL ? "local:*"
                                      #if OPAL_SIP
                                        : sipEP != NULL ? "sip:localhost"
                                      #endif
                                        : "";

  if (endpoint!="")
    srcEP = endpoint;
  else
    srcEP = defaultSrcEP;

  if (FindEndPoint(srcEP.Left(srcEP.Find(':'))) == NULL)
    srcEP = defaultSrcEP;

  allMediaFormats = OpalTranscoder::GetPossibleFormats(allMediaFormats); // Add transcoders
  allMediaFormats.RemoveNonTransportable();
  allMediaFormats.Remove(GetMediaFormatMask());
  allMediaFormats.Reorder(GetMediaFormatOrder());

  return registered;
}

PStringArray MyManager::GetCodecs() {
  //cout << "Local endpoint type: " << srcEP << "\n"
          //"Codecs removed: " << setfill(',') << GetMediaFormatMask() << "\n"
    return GetMediaFormatOrder();
}

void MyManager::Unregister() {
    if (sipEP) {
        sipEP->UnregisterAll();
    }
}

bool MyManager::Reject(OpalConnection::CallEndReason code) {
    if (pcssEP != NULL &&
        !pcssEP->incomingConnectionToken &&
        !pcssEP->RejectIncomingCall(pcssEP->incomingConnectionToken, code))
        return false;
    return true;
}

bool MyManager::Answer() {
    if ( pcssEP != NULL &&
        !pcssEP->incomingConnectionToken &&
        !pcssEP->AcceptIncomingCall(pcssEP->incomingConnectionToken))
        return false;
    return true;
}

bool MyManager::Transfer(PString address) {
    PString & token = currentCallToken.IsEmpty() ? heldCallToken : currentCallToken;

    PSafePtr<OpalCall> call = FindCallWithLock(token);

    if (call == NULL) {
        return false;
    } else {
        if (call->Transfer(address, call->GetConnection(1))) {
            token.MakeEmpty();
        } else
            return false;
    }
    return true;
}

bool MyManager::ForwardCall(PString dest) {
    forward=dest;
    return true;
}

void MyManager::HangupAll(OpalConnection::CallEndReason code) {
    currentCallToken.MakeEmpty();
    heldCallToken.MakeEmpty();
    this->ClearAllCalls(code);
}

void MyManager::HangupCurrentCall() {
  PString & token = currentCallToken.IsEmpty() ? heldCallToken : currentCallToken;

  PSafePtr<OpalCall> call = FindCallWithLock(token);

  if (call == NULL) {
    //cout << "No call to hang up!\n";
  } else {
    //cout << "Clearing call " << *call << endl;
    call->Clear();
    token.MakeEmpty();
  }
}


bool MyManager::HoldRetrieveCall()
{
  if (currentCallToken.IsEmpty() && heldCallToken.IsEmpty()) {
    return false;
  }

  if (heldCallToken.IsEmpty()) {
    PSafePtr<OpalCall> call = FindCallWithLock(currentCallToken);
    if (call == NULL)
      return false;
    else if (call->Hold()) {
      heldCallToken = currentCallToken;
      currentCallToken.MakeEmpty();
      return true;
    }
  }
  else {
    PSafePtr<OpalCall> call = FindCallWithLock(heldCallToken);
    if (call == NULL)
      return false;
    else if (call->Retrieve()) {
      currentCallToken = heldCallToken;
      heldCallToken.MakeEmpty();
      return true;
    }
  }
}


void MyManager::TransferCall(const PString & dest)
{
  if (currentCallToken.IsEmpty()) {
    cout << "Cannot do transfer while no call in progress\n";
    return;
  }

  if (dest.IsEmpty() && heldCallToken.IsEmpty()) {
    cout << "Must supply a destination for transfer, or have a call on hold!\n";
    return;
  }

  PSafePtr<OpalCall> call = FindCallWithLock(currentCallToken);
  if (call == NULL) {
    cout << "Current call disappeared!\n";
    return;
  }

  if (!call->Transfer(dest))
    cout << "Call transfer failed!\n";
}


void MyManager::SendMessageToRemoteNode(const PString & str)
{
  if (str.IsEmpty()) {
    cout << "Must supply a message to send!\n";
    return;
  }

  PSafePtr<OpalCall> call = FindCallWithLock(currentCallToken);
  if (call == NULL) {
    cout << "Cannot send a message while no call in progress\n";
    return;
  }

  PSafePtr<OpalConnection> conn = call->GetConnection(0);
  while (conn != NULL) {
    conn->SendUserInputString(str);
    cout << "Sent \"" << str << "\" to " << conn->GetRemotePartyName() << endl;
    ++conn;
  }
}

void MyManager::SendTone(const char tone)
{
  if (currentCallToken.IsEmpty()) {
    return;
  }

  PSafePtr<OpalCall> call = FindCallWithLock(currentCallToken);
  if (call == NULL) {
    return;
  }

  PSafePtr<OpalConnection> conn = call->GetConnection(0);
  while (conn != NULL) {
    conn->SendUserInputTone(tone, 180);
    ++conn;
  }
}


bool MyManager::StartCall(const PString & dest)
{
  if (!currentCallToken.IsEmpty()) {
    return false;
  }

  if (dest.IsEmpty()) {
    return false;
  }

  PString str = dest;

  if (!str.IsEmpty())
    return SetUpCall(srcEP, str, currentCallToken);

  return false;
}

int MyManager::CallDelegate(void *deleg, PString str) {
  if ((Invoke) && (deleg)) {
    void *RES=0;
    void *EXCEPTION=0;
    char *d_static="";
    char *data=d_static;
    int len=str.GetLength();
    if (len>0)
        data=(char *)str.GetPointer();
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)1);
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,deleg,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_STRING,data,(NUMBER)(SYS_INT)len);
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)0);
    return 1;
  }
  return 0;
}

int MyManager::CallDelegateWithResult(void *deleg, PString str) {
  if ((Invoke) && (deleg)) {
    void *RES=0;
    void *EXCEPTION=0;
    char *d_static="";
    char *data=d_static;
    int len=str.GetLength();
    int res=0;
    if (len>0)
        data=(char *)str.GetPointer();
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)1);
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,deleg,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_STRING,data,(NUMBER)(SYS_INT)len);
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES) {
        char *s=0;
        INTEGER type;
        NUMBER nvalue=0;
        ((INVOKE_CALL)Invoke)(INVOKE_GET_VARIABLE,RES,&type,&s,&nvalue);
        if (type==VARIABLE_NUMBER)
            res=(int)nvalue;
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    }
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)0);
    return res;
  }
  return 0;
}

int MyManager::CallDelegate2(void *deleg, BYTE *data, int len, int type) {
  if ((Invoke) && (deleg) && (len>0)) {
    void *RES=0;
    void *EXCEPTION=0;
    char *d_static="";
    if (!len)
        data=(BYTE *)d_static;
    //((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)1);
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,deleg,&RES,&EXCEPTION,(INTEGER)2,(INTEGER)VARIABLE_STRING,data,(NUMBER)len,(INTEGER)VARIABLE_NUMBER,"",(NUMBER)type);
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    //((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)0);
    return 1;
  }
  return 0;
}

int MyManager::CallDelegate3(void *deleg, RTP_DataFrame & frame) {
  if ((Invoke) && (deleg)) {
    void *RES=0;
    void *EXCEPTION=0;
    //((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)1);
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,deleg,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_NUMBER,"",(NUMBER)frame.GetPayloadType());
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES) {
        char *s=0;
        INTEGER type;
        NUMBER nvalue=0;
        ((INVOKE_CALL)Invoke)(INVOKE_GET_VARIABLE,RES,&type,&s,&nvalue);
        if (type==VARIABLE_STRING) {
            int len=(int)nvalue;
            if ((s) && (len>0)) {
                frame.SetPayloadSize(len);
                memcpy(frame.GetPayloadPtr(), s, len);
            }
        }
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    }
    //((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, deleg, (INTEGER)0);
    return 1;
  }
  return 0;
}

void MyManager::OnAlerting(OpalConnection & connection) {
  if ((Invoke) && (OnAlertingDelegate)) {
    OpalCall call=connection.GetCall();
    currentCallToken = call.GetToken();
    //void *RES=0;
    //void *EXCEPTION=0;
    this->CallDelegate(OnAlertingDelegate, call.GetPartyB());
    /*((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,OnAlertingDelegate,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_STRING,s.GetPointer(),(NUMBER)(SYS_INT)s.GetLength());
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);*/
  }
  //connection.AnsweringCall(OpalConnection::AnswerCallPending);
  OpalManager::OnAlerting(connection);
}

void MyManager::OnEstablishedCall(OpalCall & call) {
  currentCallToken = call.GetToken();
  if ((Invoke) && (OnEstablishedCallDelegate)) {
    this->CallDelegate(OnEstablishedCallDelegate, call.GetPartyB());
    /*((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,OnEstablishedCallDelegate,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_STRING,s.GetPointer(),(NUMBER)(SYS_INT)s.GetLength());
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);*/
  }
  OpalManager::OnEstablishedCall(call);
  //cout << "In call with " << call.GetPartyB() << " using " << call.GetPartyA() << endl;  
}

void MyManager::OnClearedCall(OpalCall & call)
{
  if (currentCallToken == call.GetToken())
    currentCallToken.MakeEmpty();
  else if (heldCallToken == call.GetToken())
    heldCallToken.MakeEmpty();

  if ((Invoke) && (OnEstablishedCallDelegate)) {
    void *RES=0;
    void *EXCEPTION=0;
    PString s=call.GetPartyB();
    char *d_static="";
    char *data=d_static;
    int len=s.GetLength();
    if (len>0)
        data=(char *)s.GetPointer();
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, OnClearedCallDelegate, (INTEGER)1);
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,OnClearedCallDelegate,&RES,&EXCEPTION,(INTEGER)2,(INTEGER)VARIABLE_STRING,data,(NUMBER)(SYS_INT)len, (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(SYS_INT)call.GetCallEndReason());
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, OnClearedCallDelegate, (INTEGER)0);
  }

  /*PString remoteName = '"' + call.GetPartyB() + '"';
  switch (call.GetCallEndReason()) {
    case OpalConnection::EndedByRemoteUser :
      cout << remoteName << " has cleared the call";
      break;
    case OpalConnection::EndedByCallerAbort :
      cout << remoteName << " has stopped calling";
      break;
    case OpalConnection::EndedByRefusal :
      cout << remoteName << " did not accept your call";
      break;
    case OpalConnection::EndedByNoAnswer :
      cout << remoteName << " did not answer your call";
      break;
    case OpalConnection::EndedByTransportFail :
      cout << "Call with " << remoteName << " ended abnormally";
      break;
    case OpalConnection::EndedByCapabilityExchange :
      cout << "Could not find common codec with " << remoteName;
      break;
    case OpalConnection::EndedByNoAccept :
      cout << "Did not accept incoming call from " << remoteName;
      break;
    case OpalConnection::EndedByAnswerDenied :
      cout << "Refused incoming call from " << remoteName;
      break;
    case OpalConnection::EndedByNoUser :
      cout << "Gatekeeper or registrar could not find user " << remoteName;
      break;
    case OpalConnection::EndedByNoBandwidth :
      cout << "Call to " << remoteName << " aborted, insufficient bandwidth.";
      break;
    case OpalConnection::EndedByUnreachable :
      cout << remoteName << " could not be reached.";
      break;
    case OpalConnection::EndedByNoEndPoint :
      cout << "No phone running for " << remoteName;
      break;
    case OpalConnection::EndedByHostOffline :
      cout << remoteName << " is not online.";
      break;
    case OpalConnection::EndedByConnectFail :
      cout << "Transport error calling " << remoteName;
      break;
    default :
      cout << "Call with " << remoteName << " completed";
  }
  PTime now;
  cout << ", on " << now.AsString("w h:mma") << ". Duration "
       << setprecision(0) << setw(5) << (now - call.GetStartTime())
       << "s." << endl;*/

  OpalManager::OnClearedCall(call);
}


PBoolean MyManager::OnOpenMediaStream(OpalConnection & connection,
                                  OpalMediaStream & stream)
{
  if (!OpalManager::OnOpenMediaStream(connection, stream))
    return false;
  if ((Invoke) && (OnOpenMediaStreamDelegate)) {
    void *RES=0;
    void *EXCEPTION=0;
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, OnOpenMediaStreamDelegate, (INTEGER)1);
    stream.SafeReference();
    ((INVOKE_CALL)Invoke)(INVOKE_CALL_DELEGATE,OnOpenMediaStreamDelegate,&RES,&EXCEPTION,(INTEGER)1,(INTEGER)VARIABLE_NUMBER, "",(NUMBER)(SYS_INT)&stream);
    if (EXCEPTION)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,EXCEPTION);
    if (RES)
        ((INVOKE_CALL)Invoke)(INVOKE_FREE_VARIABLE,RES);
    ((INVOKE_CALL)Invoke)(INVOKE_THREAD_LOCK, OnOpenMediaStreamDelegate, (INTEGER)0);
  }
  /*if (stream.IsSink()) {
      // play
  } else {
      // record
  }

  PCaselessString prefix = connection.GetEndPoint().GetPrefixName();
  if (prefix == "pc" || prefix == "pots")
    cout << (stream.IsSink() ? "playing " : "grabbing ") << stream.GetMediaFormat();
  else if (prefix == "ivr")
    cout << (stream.IsSink() ? "streaming " : "recording ") << stream.GetMediaFormat();
  else
    cout << (stream.IsSink() ? "sending " : "receiving ") << stream.GetMediaFormat()
          << (stream.IsSink() ? " to " : " from ")<< prefix;

  cout << endl;*/


  return true;//OpalManager::OnOpenMediaStream(connection, stream);
  //return true;
}



void MyManager::OnUserInputString(OpalConnection & connection, const PString & value)
{
  //cout << "User input received: \"" << value << '"' << endl;
  this->CallDelegate(OnUserInputStringDelegate, value);
  OpalManager::OnUserInputString(connection, value);
}


///////////////////////////////////////////////////////////////

MyPCSSEndPoint::MyPCSSEndPoint(MyManager & mgr)
  : OpalLocalEndPoint(mgr)
{
    owner=0;
    SetDeferredAnswer(true);
    SetDefaultAudioSynchronicity(e_Asynchronous);
}

/*OpalLocalConnection::AnswerCallResponse MyPCSSEndPoint::OnAnswerCall(const PString & callerName) {
    return OpalConnection::AnswerCallPending;
}*/

PBoolean MyPCSSEndPoint::OnIncomingCall(OpalLocalConnection & connection)
{
  incomingConnectionToken = connection.GetToken();

  /*if (autoAnswer)
    AcceptIncomingConnection(incomingConnectionToken);
  else {
    PTime now;
    cout << "\nCall on " << now.AsString("w h:mma")
         << " from " << connection.GetRemotePartyName()
         << ", answer (Yes/No/Busy)? " << flush;
  }*/
  if (owner)
      owner->CallDelegate(owner->OnShowIncomingDelegate, connection.GetRemotePartyName());

  return OpalLocalEndPoint::OnIncomingCall(connection);
}


PBoolean MyPCSSEndPoint::OnOutgoingCall(OpalLocalConnection & connection)
{
  /*PTime now;
  cout << connection.GetRemotePartyName() << " is ringing on "
       << now.AsString("w h:mma") << " ..." << endl;*/
  owner->currentCallToken=connection.GetCall().GetToken();
  bool res=OpalLocalEndPoint::OnOutgoingCall(connection);
  if (owner)
      owner->CallDelegate(owner->OnShowOutgoingDelegate, connection.GetRemotePartyName());

  return res;
}

bool MyPCSSEndPoint::OnReadMediaFrame(const OpalLocalConnection & connection, const OpalMediaStream & mediaStream, RTP_DataFrame & frame) {
/*#ifdef _WIN32
    Sleep(2);
#else
    usleep(2500);
#endif
    return true;*///return OpalLocalEndPoint::OnReadMediaFrame(connection, mediaStream, frame);//connection->OnReadMediaFrame(mediaStream, frame);
    if ((owner) && (owner->OnMediaFrameRequestDelegate))
        owner->CallDelegate3(owner->OnMediaFrameRequestDelegate, frame);
    return true;
}

bool MyPCSSEndPoint::OnWriteMediaFrame(const OpalLocalConnection & connection, const OpalMediaStream & mediaStream, RTP_DataFrame & frame) {
/*#ifdef _WIN32
    Sleep(2);
#else
    usleep(2500);
#endif
    return true;*///return OpalLocalEndPoint::OnWriteMediaFrame(connection, mediaStream, frame);//OpalPCSSEndPoint->OnWriteMediaFrame(mediaStream, frame);
    if ((owner) && (owner->OnMediaFrameDelegate)) {
        owner->CallDelegate2(owner->OnMediaFrameDelegate, frame.GetPayloadPtr(), frame.GetPayloadSize(), frame.GetPayloadType());
    }       
    return true;
}
// End of File ///////////////////////////////////////////////////////////////
