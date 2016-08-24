////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIEx.cpp
// Description: Windows Extended MAPI class
//
// Copyright (C) 2005-2006, Noel Dillabough
//
// This source code is free to use and modify provided this notice remains intact and that any enhancements
// or bug fixes are posted to the CodeProject page hosting this class for the community to benefit.
//
// Usage: see the Codeproject article at http://www.codeproject.com/internet/CMapiEx.asp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MAPIExPCH.h"
#include "MAPIEx.h"
#include "MAPISink.h"

#ifdef _WIN32_WCE
 #pragma comment(lib,"cemapi.lib")
 #pragma comment(lib,"UUID.lib")
#else
 #pragma comment (lib,"mapi32.lib")
#endif
#pragma comment(lib,"Ole32.lib")

/////////////////////////////////////////////////////////////
// CMAPIEx

#ifdef UNICODE
int CMAPIEx::cm_nMAPICode = MAPI_UNICODE;
#else
int CMAPIEx::cm_nMAPICode = 0;
#endif

CMAPIEx::CMAPIEx() {
    m_pSession   = NULL;
    m_pMsgStore  = NULL;
    m_pFolder    = NULL;
    m_pContents  = NULL;
    m_pHierarchy = NULL;
    m_sink       = 0;
}

CMAPIEx::~CMAPIEx() {
    Logout();
}

BOOL CMAPIEx::Init(BOOL bMultiThreadedNotifcations) {
#ifdef _WIN32_WCE
    if (CoInitializeEx(NULL, COINIT_MULTITHREADED) != S_OK) return FALSE;
#endif
    if (bMultiThreadedNotifcations) {
        MAPIINIT_0 MAPIInit = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
        if (MAPIInitialize(&MAPIInit) != S_OK) return FALSE;
    } else {
        if (MAPIInitialize(NULL) != S_OK) return FALSE;
    }
    return TRUE;
}

void CMAPIEx::Term() {
    MAPIUninitialize();
#ifdef _WIN32_WCE
    CoUninitialize();
#endif
}

BOOL CMAPIEx::Login(LPCTSTR szProfileName) {
    return MAPILogonEx(NULL, (LPTSTR)szProfileName, NULL, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &m_pSession) == S_OK;
}

void CMAPIEx::Logout() {
    if (m_sink) {
        if (m_pMsgStore) m_pMsgStore->Unadvise(m_sink);
        m_sink = 0;
    }

    RELEASE(m_pHierarchy);
    RELEASE(m_pContents);
    RELEASE(m_pFolder);
    RELEASE(m_pMsgStore);
    RELEASE(m_pSession);
}

// if I try to use MAPI_UNICODE when UNICODE is defined I get the MAPI_E_BAD_CHARWIDTH
// error so I force narrow strings here
LPCTSTR CMAPIEx::GetProfileName() {
    if (!m_pSession) return NULL;

    static CString strProfileName;
    LPSRowSet      pRows       = NULL;
    const int      nProperties = 2;
    SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_DISPLAY_NAME_A, PR_RESOURCE_TYPE } };

    IMAPITable *pStatusTable;
    if (m_pSession->GetStatusTable(0, &pStatusTable) == S_OK) {
        if (pStatusTable->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) {
            while (TRUE) {
                if (pStatusTable->QueryRows(1, 0, &pRows) != S_OK) MAPIFreeBuffer(pRows);
                else if (pRows->cRows != 1) FreeProws(pRows);
                else if (pRows->aRow[0].lpProps[1].Value.ul == MAPI_SUBSYSTEM) {
                    strProfileName = (LPSTR)GetValidString(pRows->aRow[0].lpProps[0]);
                    FreeProws(pRows);
                } else {
                    FreeProws(pRows);
                    continue;
                }
                break;
            }
        }
        RELEASE(pStatusTable);
    }
    return strProfileName;
}

BOOL CMAPIEx::OpenMessageStore(LPCTSTR szStore, ULONG ulFlags) {
    if (!m_pSession) return FALSE;

    m_ulMDBFlags = ulFlags;

    LPSRowSet pRows       = NULL;
    const int nProperties = 3;
    SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_DISPLAY_NAME, PR_ENTRYID, PR_DEFAULT_STORE } };

    BOOL       bResult = FALSE;
    IMAPITable *pMsgStoresTable;
    if (m_pSession->GetMsgStoresTable(0, &pMsgStoresTable) == S_OK) {
        if (pMsgStoresTable->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) {
            while (TRUE) {
                if (pMsgStoresTable->QueryRows(1, 0, &pRows) != S_OK) MAPIFreeBuffer(pRows);
                else if (pRows->cRows != 1) FreeProws(pRows);
                else {
                    if (!szStore) {
                        if (pRows->aRow[0].lpProps[2].Value.b) bResult = TRUE;
                    } else {
                        CString strStore = GetValidString(pRows->aRow[0].lpProps[0]);
                        if (strStore.Find(szStore) != -1) bResult = TRUE;
                    }
                    if (!bResult) {
                        FreeProws(pRows);
                        continue;
                    }
                }
                break;
            }
            if (bResult) {
                RELEASE(m_pMsgStore);
                bResult = (m_pSession->OpenMsgStore(NULL, pRows->aRow[0].lpProps[1].Value.bin.cb, (ENTRYID *)pRows->aRow[0].lpProps[1].Value.bin.lpb, NULL, MDB_NO_DIALOG | MAPI_BEST_ACCESS, &m_pMsgStore) == S_OK);
                FreeProws(pRows);
            }
        }
        RELEASE(pMsgStoresTable);
    }
    return bResult;
}

ULONG CMAPIEx::GetMessageStoreSupport() {
    if (!m_pMsgStore) return FALSE;

    LPSPropValue props     = NULL;
    ULONG        cValues   = 0;
    ULONG        rgTags[]  = { 1, PR_STORE_SUPPORT_MASK };
    ULONG        ulSupport = 0;

    if (m_pMsgStore->GetProps((LPSPropTagArray)rgTags, CMAPIEx::cm_nMAPICode, &cValues, &props) == S_OK) {
        ulSupport = props->Value.ul;
        MAPIFreeBuffer(props);
    }
    return ulSupport;
}

LPMAPIFOLDER CMAPIEx::OpenFolder(unsigned long ulFolderID, BOOL bInternal) {
    if (!m_pMsgStore) return NULL;

    LPSPropValue props   = NULL;
    ULONG        cValues = 0;
    DWORD        dwObjType;
    ULONG        rgTags[] = { 1, ulFolderID };
    LPMAPIFOLDER pFolder;

    if (m_pMsgStore->GetProps((LPSPropTagArray)rgTags, cm_nMAPICode, &cValues, &props) != S_OK) return NULL;
    m_pMsgStore->OpenEntry(props[0].Value.bin.cb, (LPENTRYID)props[0].Value.bin.lpb, NULL, m_ulMDBFlags, &dwObjType, (LPUNKNOWN *)&pFolder);
    MAPIFreeBuffer(props);

    if (pFolder && bInternal) {
        RELEASE(m_pFolder);
        m_pFolder = pFolder;
    }
    return pFolder;
}

LPMAPIFOLDER CMAPIEx::OpenSpecialFolder(unsigned long ulFolderID, BOOL bInternal) {
    LPMAPIFOLDER pInbox = OpenInbox(FALSE);

    if (!pInbox || !m_pMsgStore) return FALSE;

    LPSPropValue props   = NULL;
    ULONG        cValues = 0;
    DWORD        dwObjType;
    ULONG        rgTags[] = { 1, ulFolderID };
    LPMAPIFOLDER pFolder;

    if (pInbox->GetProps((LPSPropTagArray)rgTags, cm_nMAPICode, &cValues, &props) != S_OK) return NULL;
    m_pMsgStore->OpenEntry(props[0].Value.bin.cb, (LPENTRYID)props[0].Value.bin.lpb, NULL, m_ulMDBFlags, &dwObjType, (LPUNKNOWN *)&pFolder);
    MAPIFreeBuffer(props);
    RELEASE(pInbox);

    if (pFolder && bInternal) {
        RELEASE(m_pFolder);
        m_pFolder = pFolder;
    }
    return pFolder;
}

LPMAPIFOLDER CMAPIEx::OpenRootFolder(BOOL bInternal) {
    return OpenFolder(PR_IPM_SUBTREE_ENTRYID, bInternal);
}

LPMAPIFOLDER CMAPIEx::OpenInbox(BOOL bInternal) {
    if (!m_pMsgStore) return NULL;

#ifdef _WIN32_WCE
    return OpenFolder(PR_CE_IPM_INBOX_ENTRYID);
#else
    ULONG        cbEntryID;
    LPENTRYID    pEntryID;
    DWORD        dwObjType;
    LPMAPIFOLDER pFolder;

    if (m_pMsgStore->GetReceiveFolder(NULL, 0, &cbEntryID, &pEntryID, NULL) != S_OK) return NULL;
    m_pMsgStore->OpenEntry(cbEntryID, pEntryID, NULL, m_ulMDBFlags, &dwObjType, (LPUNKNOWN *)&pFolder);
    MAPIFreeBuffer(pEntryID);
#endif

    if (pFolder && bInternal) {
        RELEASE(m_pFolder);
        m_pFolder = pFolder;
    }
    return pFolder;
}

LPMAPIFOLDER CMAPIEx::OpenOutbox(BOOL bInternal) {
    return OpenFolder(PR_IPM_OUTBOX_ENTRYID, bInternal);
}

LPMAPIFOLDER CMAPIEx::OpenSentItems(BOOL bInternal) {
    return OpenFolder(PR_IPM_SENTMAIL_ENTRYID, bInternal);
}

LPMAPIFOLDER CMAPIEx::OpenDeletedItems(BOOL bInternal) {
    return OpenFolder(PR_IPM_WASTEBASKET_ENTRYID, bInternal);
}

LPMAPIFOLDER CMAPIEx::OpenContacts(BOOL bInternal) {
    return OpenSpecialFolder(PR_IPM_CONTACT_ENTRYID, bInternal);
}

LPMAPIFOLDER CMAPIEx::OpenDrafts(BOOL bInternal) {
    return OpenSpecialFolder(PR_IPM_DRAFTS_ENTRYID, bInternal);
}

LPMAPITABLE CMAPIEx::GetHierarchy(LPMAPIFOLDER pFolder) {
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return NULL;
    }
    RELEASE(m_pHierarchy);
    if (pFolder->GetHierarchyTable(0, &m_pHierarchy) != S_OK) return NULL;

    const int nProperties = 2;
    SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_DISPLAY_NAME, PR_ENTRYID } };
    if (m_pHierarchy->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) return m_pHierarchy;
    return NULL;
}

LPMAPIFOLDER CMAPIEx::GetNextSubFolder(CString& strFolderName, LPMAPIFOLDER pFolder) {
    if (!m_pHierarchy) return NULL;
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return FALSE;
    }

    DWORD     dwObjType;
    LPSRowSet pRows = NULL;

    LPMAPIFOLDER pSubFolder = NULL;
    if (m_pHierarchy->QueryRows(1, 0, &pRows) == S_OK) {
        if (pRows->cRows) {
            if (pFolder->OpenEntry(pRows->aRow[0].lpProps[PROP_ENTRYID].Value.bin.cb, (LPENTRYID)pRows->aRow[0].lpProps[PROP_ENTRYID].Value.bin.lpb, NULL, MAPI_MODIFY, &dwObjType, (LPUNKNOWN *)&pSubFolder) == S_OK) {
                strFolderName = GetValidString(pRows->aRow[0].lpProps[0]);
            }
        }
        FreeProws(pRows);
    }
    MAPIFreeBuffer(pRows);
    return pSubFolder;
}

// High Level function to open a sub folder by iterating recursively (DFS) over all folders
// (use instead of manually calling GetHierarchy and GetNextSubFolder)
LPMAPIFOLDER CMAPIEx::OpenSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder) {
    LPMAPIFOLDER pSubFolder = NULL;
    LPMAPITABLE  pHierarchy;

    RELEASE(m_pHierarchy);
    pHierarchy = GetHierarchy(pFolder);
    if (pHierarchy) {
        CString      strFolder;
        LPMAPIFOLDER pRecurse = NULL;
        do {
            RELEASE(pSubFolder);
            m_pHierarchy = pHierarchy;
            pSubFolder   = GetNextSubFolder(strFolder, pFolder);
            if (pSubFolder) {
                if (!strFolder.CompareNoCase(szSubFolder)) break;
                m_pHierarchy = NULL;               // so we don't release it in subsequent drilldown
                pRecurse     = OpenSubFolder(szSubFolder, pSubFolder);
                if (pRecurse) {
                    RELEASE(pSubFolder);
                    pSubFolder = pRecurse;
                    break;
                }
            }
        } while (pSubFolder);
        RELEASE(pHierarchy);
        m_pHierarchy = NULL;
    }
    // this may occur many times depending on how deep the recursion is; make sure we haven't already assigned m_pFolder
    if (pSubFolder && (m_pFolder != pSubFolder)) {
        RELEASE(m_pFolder);
        m_pFolder = pSubFolder;
    }
    return pSubFolder;
}

BOOL CMAPIEx::GetContents(LPMAPIFOLDER pFolder) {
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return FALSE;
    }
    RELEASE(m_pContents);
    if (pFolder->GetContentsTable(CMAPIEx::cm_nMAPICode, &m_pContents) != S_OK) return FALSE;

    const int nProperties = MESSAGE_COLS;
    SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_MESSAGE_FLAGS, PR_ENTRYID } };
    return m_pContents->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK;
}

int CMAPIEx::GetRowCount() {
    ULONG ulCount;

    if (!m_pContents || (m_pContents->GetRowCount(0, &ulCount) != S_OK)) return -1;
    return ulCount;
}

BOOL CMAPIEx::SortContents(ULONG ulSortParam, ULONG ulSortField) {
    if (!m_pContents) return FALSE;

    SizedSSortOrderSet(1, SortColums) = { 1, 0, 0, { { ulSortField, ulSortParam } } };
    return m_pContents->SortTable((LPSSortOrderSet) & SortColums, 0) == S_OK;
}

BOOL CMAPIEx::GetNextMessage(CMAPIMessage& message, BOOL bUnreadOnly) {
    if (!m_pContents) return FALSE;

    DWORD     dwMessageFlags;
    LPSRowSet pRows   = NULL;
    BOOL      bResult = FALSE;
    while (m_pContents->QueryRows(1, 0, &pRows) == S_OK) {
        if (pRows->cRows) {
            dwMessageFlags = pRows->aRow[0].lpProps[PROP_MESSAGE_FLAGS].Value.ul;
            if (bUnreadOnly && dwMessageFlags & MSGFLAG_READ) {
                FreeProws(pRows);
                continue;
            }
            bResult = message.Open(this, pRows->aRow[0].lpProps[PROP_ENTRYID].Value.bin, dwMessageFlags);
        }
        FreeProws(pRows);
        break;
    }
    MAPIFreeBuffer(pRows);
    return bResult;
}

BOOL CMAPIEx::GetNextContact(CMAPIContact& contact) {
    if (!m_pContents) return FALSE;

    LPSRowSet pRows   = NULL;
    BOOL      bResult = FALSE;
    while (m_pContents->QueryRows(1, 0, &pRows) == S_OK) {
        if (pRows->cRows) bResult = contact.Open(this, pRows->aRow[0].lpProps[PROP_ENTRYID].Value.bin);
        FreeProws(pRows);
        break;
    }
    MAPIFreeBuffer(pRows);
    return bResult;
}

// Creates a subfolder under pFolder, opens the folder if it already exists
LPMAPIFOLDER CMAPIEx::CreateSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder) {
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return NULL;
    }

    LPMAPIFOLDER pSubFolder   = NULL;
    ULONG        ulFolderType = FOLDER_GENERIC;
    ULONG        ulFlags      = OPEN_IF_EXISTS | cm_nMAPICode;

    pFolder->CreateFolder(ulFolderType, (LPTSTR)szSubFolder, NULL, NULL, ulFlags, &pSubFolder);
    return pSubFolder;
}

// Deletes a sub folder and ALL sub folders/messages
BOOL CMAPIEx::DeleteSubFolder(LPCTSTR szSubFolder, LPMAPIFOLDER pFolder) {
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return FALSE;
    }

    LPMAPIFOLDER pSubFolder = NULL;
    if (GetHierarchy(pFolder)) {
        CString strFolder;
        do {
            RELEASE(pSubFolder);
            pSubFolder = GetNextSubFolder(strFolder, pFolder);
            if (pSubFolder && !strFolder.CompareNoCase(szSubFolder)) break;
        } while (pSubFolder);
    }
    return DeleteSubFolder(pSubFolder, pFolder);
}

// Deletes a sub folder and ALL sub folders/messages
BOOL CMAPIEx::DeleteSubFolder(LPMAPIFOLDER pSubFolder, LPMAPIFOLDER pFolder) {
    if (!pSubFolder) return FALSE;

    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return FALSE;
    }

    LPSPropValue props    = NULL;
    ULONG        cValues  = 0;
    ULONG        rgTags[] = { 1, PR_ENTRYID };

    if (pSubFolder->GetProps((LPSPropTagArray)rgTags, CMAPIEx::cm_nMAPICode, &cValues, &props) == S_OK) {
        HRESULT hr = pFolder->DeleteFolder(props[0].Value.bin.cb, (LPENTRYID)props[0].Value.bin.lpb, NULL, NULL, DEL_FOLDERS | DEL_MESSAGES);
        MAPIFreeBuffer(props);
        return hr == S_OK;
    }
    return FALSE;
}

BOOL CMAPIEx::DeleteMessage(CMAPIMessage& message, LPMAPIFOLDER pFolder) {
    if (!pFolder) {
        pFolder = m_pFolder;
        if (!pFolder) return FALSE;
    }

    ENTRYLIST entries = { 1, message.GetEntryID() };
    HRESULT   hr      = pFolder->DeleteMessages(&entries, NULL, NULL, 0);
    return hr == S_OK;
}

BOOL CMAPIEx::CopyMessage(CMAPIMessage& message, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc) {
    if (!pFolderDest) return FALSE;

    if (!pFolderSrc) {
        pFolderSrc = m_pFolder;
        if (!pFolderSrc) return FALSE;
    }

    ENTRYLIST entries = { 1, message.GetEntryID() };
    HRESULT   hr      = pFolderSrc->CopyMessages(&entries, NULL, pFolderDest, NULL, NULL, 0);
    return hr == S_OK;
}

BOOL CMAPIEx::MoveMessage(CMAPIMessage& message, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc) {
    if (!pFolderDest) return FALSE;

    if (!pFolderSrc) {
        pFolderSrc = m_pFolder;
        if (!pFolderSrc) return FALSE;
    }

    ENTRYLIST entries = { 1, message.GetEntryID() };
    HRESULT   hr      = pFolderSrc->CopyMessages(&entries, NULL, pFolderDest, NULL, NULL, MESSAGE_MOVE);
    return hr == S_OK;
}

// call with ulEventMask set to ALL notifications ORed together, only one Advise Sink is used.
BOOL CMAPIEx::Notify(LPNOTIFCALLBACK lpfnCallback, LPVOID lpvContext, ULONG ulEventMask) {
    if (GetMessageStoreSupport() & STORE_NOTIFY_OK) {
        if (m_sink) m_pMsgStore->Unadvise(m_sink);
        CMAPISink *pAdviseSink = new CMAPISink(lpfnCallback, lpvContext);
        if (m_pMsgStore->Advise(0, NULL, ulEventMask, pAdviseSink, &m_sink) == S_OK) return TRUE;
        delete pAdviseSink;
        m_sink = 0;
    }
    return FALSE;
}

// sometimes the string in prop is invalid, causing unexpected crashes
LPCTSTR CMAPIEx::GetValidString(SPropValue& prop) {
    LPCTSTR s = prop.Value.LPSZ;

    if (s && !::IsBadStringPtr(s, (UINT_PTR)-1)) return s;
    return NULL;
}

// special case of GetValidString to take the narrow string in UNICODE
void CMAPIEx::GetNarrowString(SPropValue& prop, CString& strNarrow) {
    LPCTSTR s = GetValidString(prop);

    if (!s) strNarrow = _T("");
    else {
#ifdef UNICODE
        // VS2005 can copy directly
        if (_MSC_VER >= 1400) {
            strNarrow = (char *)s;
        } else {
            WCHAR wszWide[256];
            MultiByteToWideChar(CP_ACP, 0, (LPCSTR)s, -1, wszWide, 255);
            strNarrow = wszWide;
        }
#else
        strNarrow = s;
#endif
    }
}

// Shows the default Address Book dialog, return FALSE on failure, IDOK or IDCANCEL on success
// I force narrow strings here because it for some reason doesn't work in UNICODE
int CMAPIEx::ShowAddressBook(LPADRLIST& pAddressList, LPCTSTR szCaption) {
    int nResult = FALSE;

#ifndef _WIN32_WCE
    if (!m_pSession) return FALSE;

    LPADRBOOK pAddressBook;
    if (m_pSession->OpenAddressBook(0, NULL, 0, &pAddressBook) == S_OK) {
        pAddressList = NULL;

        char  *lppszDestTitles[] = { "To" };
        ULONG lpulDestComps[]    = { MAPI_TO };

        ADRPARM adrparm;
        memset(&adrparm, 0, sizeof(ADRPARM));
        adrparm.ulFlags = DIALOG_MODAL | AB_RESOLVE;

        adrparm.lpszCaption = (LPTSTR)szCaption;
 #ifdef UNICODE
        if (szCaption) {
            char szNarrowCaption[256];
            WideCharToMultiByte(CP_ACP, 0, szCaption, -1, szNarrowCaption, 255, NULL, NULL);
            adrparm.lpszCaption = (LPTSTR)szNarrowCaption;
        }
 #endif
        adrparm.cDestFields     = 1;
        adrparm.lppszDestTitles = (LPTSTR *)lppszDestTitles;
        adrparm.lpulDestComps   = lpulDestComps;

        HWND    hDesktop = ::GetDesktopWindow();
        HRESULT hr       = pAddressBook->Address((ULONG_PTR *)&hDesktop, &adrparm, &pAddressList);
        RELEASE(pAddressBook);
        if (hr == S_OK) return IDOK;
        if (hr == MAPI_E_USER_CANCEL) return IDCANCEL;
    }
#endif
    return nResult;
}

// utility function to release ADRLIST entries
void CMAPIEx::ReleaseAddressList(LPADRLIST pAddressList) {
    FreePadrlist(pAddressList);
}

// ADDRENTRY objects from Address don't come in unicode so I check for _A and force narrow strings
BOOL CMAPIEx::GetEmail(ADRENTRY& adrEntry, CString& strEmail) {
    LPSPropValue pProp = PpropFindProp(adrEntry.rgPropVals, adrEntry.cValues, PR_ADDRTYPE);

    if (!pProp) pProp = PpropFindProp(adrEntry.rgPropVals, adrEntry.cValues, PR_ADDRTYPE_A);
    if (pProp) {
        CString strAddrType;
        GetNarrowString(*pProp, strAddrType);
        if (strAddrType == _T("EX")) {
            pProp = PpropFindProp(adrEntry.rgPropVals, adrEntry.cValues, PR_ENTRYID);

            SBinary entryID;
            entryID.cb  = pProp->Value.bin.cb;
            entryID.lpb = pProp->Value.bin.lpb;

            return GetExEmail(entryID, strEmail);
        }
    }
    pProp = PpropFindProp(adrEntry.rgPropVals, adrEntry.cValues, PR_EMAIL_ADDRESS);
    if (!pProp) pProp = PpropFindProp(adrEntry.rgPropVals, adrEntry.cValues, PR_EMAIL_ADDRESS_A);
    if (pProp) {
        GetNarrowString(*pProp, strEmail);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIEx::GetExEmail(SBinary entryID, CString& strEmail) {
    BOOL bResult = FALSE;

#ifndef _WIN32_WCE
    if (!m_pSession) return FALSE;

    LPADRBOOK pAddressBook;
    if (m_pSession->OpenAddressBook(0, NULL, AB_NO_DIALOG, &pAddressBook) == S_OK) {
        ULONG     ulObjType;
        IMAPIProp *pItem = NULL;
        if (pAddressBook->OpenEntry(entryID.cb, (ENTRYID *)entryID.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, (LPUNKNOWN *)&pItem) == S_OK) {
            if (ulObjType == MAPI_MAILUSER) {
                LPSPropValue pProp;
                ULONG        ulPropCount;
                ULONG        p[2] = { 1, PR_SMTP_ADDRESS };

                if (pItem->GetProps((LPSPropTagArray)p, CMAPIEx::cm_nMAPICode, &ulPropCount, &pProp) == S_OK) {
                    strEmail = CMAPIEx::GetValidString(*pProp);
                    MAPIFreeBuffer(pProp);
                    bResult = TRUE;
                }
            }
            RELEASE(pItem);
        }
        RELEASE(pAddressBook);
    }
#endif
    return bResult;
}

void CMAPIEx::GetSystemTime(SYSTEMTIME& tm, int wYear, int wMonth, int wDay, int wHour, int wMinute, int wSecond, int wMilliSeconds) {
    tm.wYear         = (WORD)wYear;
    tm.wMonth        = (WORD)wMonth;
    tm.wDay          = (WORD)wDay;
    tm.wHour         = (WORD)wHour;
    tm.wMinute       = (WORD)wMinute;
    tm.wSecond       = (WORD)wSecond;
    tm.wMilliseconds = (WORD)wMilliSeconds;
    tm.wDayOfWeek    = 0;
}
