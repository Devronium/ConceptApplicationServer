#ifndef __MAPIEX_H__
#define __MAPIEX_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIEx.h
// Description: Windows Extended MAPI class
//				Works in Windows CE and Windows XP with Outlook 2000 and 2003, maybe others but untested
//
// Copyright (C) 2005-2006, Noel Dillabough
//
// This source code is free to use and modify provided this notice remains intact and that any enhancements
// or bug fixes are posted to the CodeProject page hosting this class for all to benefit.
//
// Usage: see the Codeproject article at http://www.codeproject.com/internet/CMapiEx.asp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
 #include <cemapi.h>
 #include <objidl.h>
#else
 #include <mapix.h>
 #include <objbase.h>
#endif

#define RELEASE(s)    if (s != NULL) { s->Release(); s = NULL; }

#define MAPI_NO_CACHE    ((ULONG)0x00000200)

#include "MAPIMessage.h"
#include "MAPIContact.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMAPIEx

class AFX_EXT_CLASS CMAPIEx
{
public:
    CMAPIEx();
    ~CMAPIEx();

    enum { PROP_MESSAGE_FLAGS, PROP_ENTRYID, MESSAGE_COLS };

// Attributes
public:
    static int cm_nMAPICode;

protected:
    IMAPISession *m_pSession;
    LPMDB        m_pMsgStore;
    LPMAPIFOLDER m_pFolder;
    LPMAPITABLE  m_pHierarchy;
    LPMAPITABLE  m_pContents;
    ULONG        m_sink;
    ULONG        m_ulMDBFlags;

// Operations
public:
    static BOOL Init(BOOL bMultiThreadedNotifcations = TRUE);
    static void Term();

    IMAPISession *GetSession() { return m_pSession; }
    LPMDB GetMessageStore() { return m_pMsgStore; }
    LPMAPIFOLDER GetFolder() { return m_pFolder; }

    BOOL Login(LPCTSTR szProfileName = NULL);
    void Logout();

    LPCTSTR GetProfileName();
    BOOL OpenMessageStore(LPCTSTR szStore = NULL, ULONG ulFlags = MAPI_MODIFY | MAPI_NO_CACHE);
    ULONG GetMessageStoreSupport();

    // use bInternal to specify that MAPIEx keeps track of and subsequently RELEASEs the folder
    // remember to eventually RELEASE returned folders if calling with bInternal=FALSE
    LPMAPIFOLDER OpenFolder(unsigned long ulFolderID, BOOL bInternal);
    LPMAPIFOLDER OpenSpecialFolder(unsigned long ulFolderID, BOOL bInternal);
    LPMAPIFOLDER OpenRootFolder(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenInbox(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenOutbox(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenSentItems(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenDeletedItems(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenContacts(BOOL bInternal = TRUE);
    LPMAPIFOLDER OpenDrafts(BOOL bInternal = TRUE);

    LPMAPITABLE GetHierarchy(LPMAPIFOLDER pFolder = NULL);
    LPMAPIFOLDER GetNextSubFolder(CString& strFolder, LPMAPIFOLDER pFolder = NULL);
    LPMAPIFOLDER OpenSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder = NULL);
    LPMAPIFOLDER CreateSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder = NULL);
    BOOL DeleteSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder = NULL);
    BOOL DeleteSubFolder(LPMAPIFOLDER pSubFolder, LPMAPIFOLDER pFolder = NULL);

    BOOL GetContents(LPMAPIFOLDER pFolder = NULL);
    int GetRowCount();
    BOOL SortContents(ULONG ulSortParam = TABLE_SORT_ASCEND, ULONG ulSortField = PR_MESSAGE_DELIVERY_TIME);
    BOOL GetNextMessage(CMAPIMessage& message, BOOL bUnreadOnly = FALSE);
    BOOL GetNextContact(CMAPIContact& contact);

    BOOL DeleteMessage(CMAPIMessage& message, LPMAPIFOLDER pFolder = NULL);
    BOOL CopyMessage(CMAPIMessage& message, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc = NULL);
    BOOL MoveMessage(CMAPIMessage& message, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc = NULL);

    BOOL Notify(LPNOTIFCALLBACK lpfnCallback, LPVOID lpvContext, ULONG ulEventMask = fnevNewMail);

    int ShowAddressBook(LPADRLIST& pAddressList, LPCTSTR szCaption = NULL);
    BOOL GetEmail(ADRENTRY& adrEntry, CString& strEmail);
    BOOL GetExEmail(SBinary entryID, CString& strEmail);
    static LPCTSTR GetValidString(SPropValue& prop);
    static void GetNarrowString(SPropValue& prop, CString& strNarrow);
    static void GetSystemTime(SYSTEMTIME& tm, int wYear, int wMonth, int wDay, int wHour = 0, int wMinute = 0, int wSecond = 0, int wMilliSeconds = 0);
    static void ReleaseAddressList(LPADRLIST pAddressList);
};

#ifndef PR_BODY_HTML
 #define PR_BODY_HTML    PROP_TAG(PT_TSTRING, 0x1013)
#endif

#ifndef STORE_HTML_OK
 #define STORE_HTML_OK    ((ULONG)0x00010000)
#endif

#ifndef PR_SMTP_ADDRESS
 #define PR_SMTP_ADDRESS              PROP_TAG(PT_TSTRING, 0x39FE)
#endif

#define PR_IPM_APPOINTMENT_ENTRYID    (PROP_TAG(PT_BINARY, 0x36D0))
#define PR_IPM_CONTACT_ENTRYID        (PROP_TAG(PT_BINARY, 0x36D1))
#define PR_IPM_JOURNAL_ENTRYID        (PROP_TAG(PT_BINARY, 0x36D2))
#define PR_IPM_NOTE_ENTRYID           (PROP_TAG(PT_BINARY, 0x36D3))
#define PR_IPM_TASK_ENTRYID           (PROP_TAG(PT_BINARY, 0x36D4))
#define PR_IPM_DRAFTS_ENTRYID         (PROP_TAG(PT_BINARY, 0x36D7))
#endif
