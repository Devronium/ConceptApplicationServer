#ifndef __MAPISINK_H__
#define __MAPISINK_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPISink.h
// Description: MAPI Advise Sink Wrapper
//
// Copyright (C) 2005-2006, Noel Dillabough
//
// This source code is free to use and modify provided this notice remains intact and that any enhancements
// or bug fixes are posted to the CodeProject page hosting this class for the community to benefit.
//
// Usage: see the Codeproject article at http://www.codeproject.com/internet/CMapiEx.asp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMAPISink

class CMAPISink : public IMAPIAdviseSink
{
public:
    CMAPISink(LPNOTIFCALLBACK lpfnCallback, LPVOID lpvContext);

// Attributes
protected:
    LPNOTIFCALLBACK m_lpfnCallback;
    LPVOID          m_lpvContext;
    LONG            m_nRef;

// IUnknown
public:
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR * ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

// IMAPIAdviseSink
public:
    STDMETHOD_(ULONG, OnNotify)(ULONG cNotification, LPNOTIFICATION lpNotifications);
};
#endif
