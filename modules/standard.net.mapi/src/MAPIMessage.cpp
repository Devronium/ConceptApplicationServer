////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIMessage.cpp
// Description: MAPI Message class wrapper
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
#include "MAPINamedProperty.h"

/////////////////////////////////////////////////////////////
// CMAPIMessage

CMAPIMessage::CMAPIMessage() {
    m_pMessage     = NULL;
    m_pRecipients  = NULL;
    m_nAttachments = 0;
    m_pMAPI        = NULL;
    m_entryID.cb   = 0;
    SetEntryID(NULL);
}

CMAPIMessage::CMAPIMessage(CMAPIMessage& message) {
    m_pMessage     = NULL;
    m_pRecipients  = NULL;
    m_nAttachments = 0;
    m_pMAPI        = NULL;
    m_entryID.cb   = 0;
    *this          = message;
}

CMAPIMessage::~CMAPIMessage() {
    Close();
}

void CMAPIMessage::SetEntryID(SBinary *pEntryID) {
    if (m_entryID.cb) delete [] m_entryID.lpb;
    m_entryID.lpb = NULL;

    if (pEntryID) {
        m_entryID.cb = pEntryID->cb;
        if (m_entryID.cb) {
            m_entryID.lpb = new BYTE[m_entryID.cb];
            memcpy(m_entryID.lpb, pEntryID->lpb, m_entryID.cb);
        }
    } else {
        m_entryID.cb = 0;
    }
}

BOOL CMAPIMessage::Open(CMAPIEx *pMAPI, SBinary entryID, DWORD dwMessageFlags) {
    Close();
    m_pMAPI          = pMAPI;
    m_dwMessageFlags = dwMessageFlags;
    ULONG ulObjType;
    if (m_pMAPI->GetSession()->OpenEntry(entryID.cb, (LPENTRYID)entryID.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, (LPUNKNOWN *)&m_pMessage) != S_OK) return FALSE;

    SetEntryID(&entryID);
    FillSenderName();
    FillSenderEmail();
    FillSubject();
    FillAttachmentCount();

    return TRUE;
}

void CMAPIMessage::FillAttachmentCount() {
    BOOL         bHasAttachments = FALSE;
    LPSPropValue pProp;

    if (GetProperty(PR_HASATTACH, pProp) == S_OK) {
        bHasAttachments = pProp->Value.b;
        MAPIFreeBuffer(pProp);
    }

    if (bHasAttachments) {
        LPMAPITABLE pAttachTable = NULL;
        if (m_pMessage->GetAttachmentTable(0, &pAttachTable) != S_OK) return;

        ULONG ulCount;
        if (pAttachTable->GetRowCount(0, &ulCount) == S_OK) {
            m_nAttachments = ulCount;
        }
        RELEASE(pAttachTable);
    }
}

void CMAPIMessage::Close() {
    SetEntryID(NULL);
    RELEASE(m_pRecipients);
    RELEASE(m_pMessage);
    m_nAttachments = 0;
    m_pMAPI        = NULL;
    m_strBody      = _T("");
    m_strRTF       = _T("");
}

HRESULT CMAPIMessage::GetProperty(ULONG ulProperty, LPSPropValue& prop) {
    ULONG ulPropCount;
    ULONG p[2] = { 1, ulProperty };

    return m_pMessage->GetProps((LPSPropTagArray)p, CMAPIEx::cm_nMAPICode, &ulPropCount, &prop);
}

BOOL CMAPIMessage::GetHeader(CString& strHeader) {
    LPSPropValue pProp;

    if (GetProperty(PR_TRANSPORT_MESSAGE_HEADERS, pProp) == S_OK) {
        strHeader = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

LPCTSTR CMAPIMessage::GetBody() {
    if (m_strBody.IsEmpty()) FillBody();
    return m_strBody;
}

LPCTSTR CMAPIMessage::GetRTF() {
    if (m_strRTF.IsEmpty()) FillRTF();

    // does this RTF contain encoded HTML? If so decode it
    // code taken from Lucian Wischik's example at http://www.wischik.com/lu/programmer/mapi_utils.html
    if (m_strRTF.Find(_T("\\fromhtml")) != -1) {
        LPCTSTR s = m_strRTF;

        // scan to <html tag
        // Ignore { and }. These are part of RTF markup.
        // Ignore \htmlrtf...\htmlrtf0. This is how RTF keeps its equivalent markup separate from the html.
        // Ignore \r and \n. The real carriage returns are stored in \par tags.
        // Ignore \pntext{..} and \liN and \fi-N. These are RTF junk.
        // Convert \par and \tab into \r\n and \t
        // Convert \'XX into the ascii character indicated by the hex number XX
        // Convert \{ and \} into { and }. This is how RTF escapes its curly braces.
        // When we get \*\mhtmltagN, keep the tag, but ignore the subsequent \*\htmltagN
        // When we get \*\htmltagN, keep the tag as long as it isn't subsequent to a \*\mhtmltagN
        // All other text should be kept as it is.

        while (*s) {
            if (_tcsnccmp(s, _T("<html"), 5) == 0) break;
            s++;
        }

        CString strHTML;
        int     nTag = -1, nIgnoreTag = -1;
        while (*s) {
            if (*s == (TCHAR)'{') s++;
            else if (*s == (TCHAR)'}') s++;
            else if ((*s == (TCHAR)'\r') || (*s == (TCHAR)'\n')) s++;
            else if (!_tcsnccmp(s, _T("\\*\\htmltag"), 10)) {
                s   += 10;
                nTag = 0;
                while (*s >= (TCHAR)'0' && *s <= (TCHAR)'9') {
                    nTag = nTag * 10 + *s - (TCHAR)'0';
                    s++;
                }
                if (*s == (TCHAR)' ') s++;
                if (nTag == nIgnoreTag) {
                    while (*s) {
                        if (*s == (TCHAR)'}') break;
                        s++;
                    }
                    nIgnoreTag = -1;
                }
            } else if (_tcsnccmp(s, _T("\\*\\mhtmltag"), 11) == 0) {
                s += 11;
                while (*s >= (TCHAR)'0' && *s <= (TCHAR)'9') {
                    nTag = nTag * 10 + *s - (TCHAR)'0';
                    s++;
                }
                if (*s == (TCHAR)' ') s++;
                nIgnoreTag = nTag;
            } else if (_tcsnccmp(s, _T("\\par"), 4) == 0) {
                strHTML += _T("\r\n");
                s       += 4;
                if (*s == (TCHAR)' ') s++;
            } else if (_tcsnccmp(s, _T("\\tab"), 4) == 0) {
                strHTML += _T("\t");
                s       += 4;
                if (*s == (TCHAR)' ') s++;
            } else if (_tcsnccmp(s, _T("\\li"), 3) == 0) {
                s += 3;
                while (*s >= (TCHAR)'0' && *s <= (TCHAR)'9') s++;
                if (*s == (TCHAR)' ') s++;
            } else if (_tcsnccmp(s, _T("\\fi-"), 4) == 0) {
                s += 4;
                while (*s >= (TCHAR)'0' && *s <= (TCHAR)'9') s++;
                if (*s == (TCHAR)' ') s++;
            } else if (_tcsnccmp(s, _T("\\'"), 2) == 0) {
                TCHAR hi = s[2], lo = s[3];
                if ((hi >= '0') && (hi <= '9')) hi -= '0';
                else if ((hi >= 'A') && (hi <= 'Z')) hi = hi - 'A' + 10;
                else if ((hi >= 'a') && (hi <= 'z')) hi = hi - 'a' + 10;
                if ((lo >= '0') && (lo <= '9')) lo -= '0';
                else if ((lo >= 'A') && (lo <= 'Z')) lo = lo - 'A' + 10;
                else if ((lo >= 'a') && (lo <= 'z')) lo = lo - 'a' + 10;
                strHTML += (TCHAR)(hi * 16 + lo);
                s       += 4;
            } else if (_tcsnccmp(s, _T("\\pntext"), 7) == 0) {
                s += 7;
                while (*s) {
                    if (*s == (TCHAR)'}') break;
                    s++;
                }
            } else if (_tcsnccmp(s, _T("\\htmlrtf"), 8) == 0) {
                s += 8;
                while (*s) {
                    if (_tcsnccmp(s, _T("\\htmlrtf0"), 9) == 0) {
                        s += 9;
                        if (*s == (TCHAR)' ') s++;
                        break;
                    }
                    s++;
                }
            } else if (_tcsnccmp(s, _T("\\{"), 2) == 0) {
                strHTML += '{';
                s       += 2;
            } else if (_tcsnccmp(s, _T("\\}"), 2) == 0) {
                strHTML += '}';
                s       += 2;
            } else {
                strHTML += *s;
                s++;
            }
        }
        m_strRTF = strHTML;
    }
    return m_strRTF;
}

void CMAPIMessage::FillSenderName() {
    LPSPropValue pProp;

    if (GetProperty(PR_SENDER_NAME, pProp) == S_OK) {
        m_strSenderName = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
    } else m_strSenderName = _T("");
}

void CMAPIMessage::FillSenderEmail() {
    CString      strAddrType;
    LPSPropValue pProp;

    if (GetProperty(PR_SENDER_ADDRTYPE, pProp) == S_OK) {
        strAddrType = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
    }

    if (GetProperty(PR_SENDER_EMAIL_ADDRESS, pProp) == S_OK) {
        m_strSenderEmail = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
    } else m_strSenderEmail = _T("");

    // for Microsoft Exchange server internal mails we want to try to resolve the SMTP email address
    if (strAddrType == _T("EX")) {
        if (GetProperty(PR_SENDER_ENTRYID, pProp) == S_OK) {
            if (m_pMAPI) m_pMAPI->GetExEmail(pProp->Value.bin, m_strSenderEmail);
            MAPIFreeBuffer(pProp);
        }
    }
}

void CMAPIMessage::FillSubject() {
    LPSPropValue pProp;

    if (GetProperty(PR_SUBJECT, pProp) == S_OK) {
        m_strSubject = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
    } else m_strSubject = _T("");
}

void CMAPIMessage::FillRTF() {
    m_strRTF = _T("");
    IStream *pStream;
    if (m_pMessage->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_READ, 0, (LPUNKNOWN *)&pStream) != S_OK) return;

#ifdef _WIN32_WCE
    const int BUF_SIZE = 1024;
#else
    const int BUF_SIZE = 16384;
#endif
    char  szBuf[BUF_SIZE + 1];
    ULONG ulNumChars;

    IStream *pUncompressed;
    if (WrapCompressedRTFStream(pStream, 0, &pUncompressed) == S_OK) {
        do {
            pUncompressed->Read(szBuf, BUF_SIZE, &ulNumChars);
            szBuf[min(BUF_SIZE, ulNumChars)] = 0;
            m_strRTF += szBuf;
        } while (ulNumChars >= BUF_SIZE);
        RELEASE(pUncompressed);
    }

    RELEASE(pStream);
}

void CMAPIMessage::FillBody() {
    m_strBody = _T("");
    IStream *pStream;
    if (m_pMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, NULL, (LPUNKNOWN *)&pStream) != S_OK) return;

#ifdef _WIN32_WCE
    const int BUF_SIZE = 1024;
#else
    const int BUF_SIZE = 16384;
#endif
    TCHAR szBuf[BUF_SIZE + 1];
    ULONG ulNumChars;

    do {
        pStream->Read(szBuf, BUF_SIZE * sizeof(TCHAR), &ulNumChars);
        ulNumChars /= sizeof(TCHAR);
        szBuf[min(BUF_SIZE, ulNumChars)] = 0;
        m_strBody += szBuf;
    } while (ulNumChars >= BUF_SIZE);

    RELEASE(pStream);
}

BOOL CMAPIMessage::GetReceivedTime(SYSTEMTIME& tmReceived) {
    LPSPropValue pProp;

    if (GetProperty(PR_MESSAGE_DELIVERY_TIME, pProp) == S_OK) {
        FILETIME tmLocal;
        FileTimeToLocalFileTime(&pProp->Value.ft, &tmLocal);
        FileTimeToSystemTime(&tmLocal, &tmReceived);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetReceivedTime(CString& strReceivedTime, LPCTSTR szFormat) {
    SYSTEMTIME tm;

    if (GetReceivedTime(tm)) {
        TCHAR szTime[256];
        if (!szFormat) szFormat = _T("MM/dd/yyyy hh:mm:ss tt");
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szFormat, szTime, 256);
        GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szTime, szTime, 256);
        strReceivedTime = szTime;
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetSubmitTime(SYSTEMTIME& tmSubmit) {
    LPSPropValue pProp;

    if (GetProperty(PR_CLIENT_SUBMIT_TIME, pProp) == S_OK) {
        FILETIME tmLocal;
        FileTimeToLocalFileTime(&pProp->Value.ft, &tmLocal);
        FileTimeToSystemTime(&tmLocal, &tmSubmit);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetSubmitTime(CString& strSubmitTime, LPCTSTR szFormat) {
    SYSTEMTIME tm;

    if (GetSubmitTime(tm)) {
        TCHAR szTime[256];
        if (!szFormat) szFormat = _T("MM/dd/yyyy hh:mm:ss tt");
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szFormat, szTime, 256);
        GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szTime, szTime, 256);
        strSubmitTime = szTime;
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetTo(CString& strTo) {
    LPSPropValue pProp;

    if (GetProperty(PR_DISPLAY_TO, pProp) == S_OK) {
        strTo = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetCC(CString& strCC) {
    LPSPropValue pProp;

    if (GetProperty(PR_DISPLAY_CC, pProp) == S_OK) {
        strCC = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::GetBCC(CString& strBCC) {
    LPSPropValue pProp;

    if (GetProperty(PR_DISPLAY_BCC, pProp) == S_OK) {
        strBCC = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

int CMAPIMessage::GetSensitivity() {
    int          nSensitivity = -1;
    LPSPropValue pProp;

    if (GetProperty(PR_SENSITIVITY, pProp) == S_OK) {
        nSensitivity = pProp->Value.l;
        MAPIFreeBuffer(pProp);
    }
    return nSensitivity;
}

BOOL CMAPIMessage::GetNamedProperty(LPCTSTR szFieldName, CString& strField) {
    CMAPINamedProperty prop(m_pMessage);

    return prop.GetNamedProperty(szFieldName, strField);
}

int CMAPIMessage::GetMessageFlags() {
    int          nFlags = 0;
    LPSPropValue pProp;

    if (GetProperty(PR_MESSAGE_FLAGS, pProp) == S_OK) {
        nFlags = pProp->Value.l;
        MAPIFreeBuffer(pProp);
    }
    return nFlags;
}

BOOL CMAPIMessage::GetRecipients() {
    if (!m_pMessage) return FALSE;
    RELEASE(m_pRecipients);

    if (m_pMessage->GetRecipientTable(CMAPIEx::cm_nMAPICode, &m_pRecipients) != S_OK) return FALSE;

    const int nProperties = RECIPIENT_COLS;
    SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_RECIPIENT_TYPE, PR_DISPLAY_NAME, PR_EMAIL_ADDRESS, PR_ADDRTYPE, PR_ENTRYID } };
    return m_pRecipients->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK;
}

BOOL CMAPIMessage::GetNextRecipient(CString& strName, CString& strEmail, int& nType) {
    if (!m_pRecipients) return FALSE;

    LPSRowSet pRows   = NULL;
    BOOL      bResult = FALSE;
    if (m_pRecipients->QueryRows(1, 0, &pRows) == S_OK) {
        if (pRows->cRows) {
            nType   = pRows->aRow[0].lpProps[PROP_RECIPIENT_TYPE].Value.ul;
            strName = CMAPIEx::GetValidString(pRows->aRow[0].lpProps[PROP_RECIPIENT_NAME]);

            // for Microsoft Exchange server internal mails we want to try to resolve the SMTP email address
            CString strAddrType = CMAPIEx::GetValidString(pRows->aRow[0].lpProps[PROP_ADDR_TYPE]);
            if (strAddrType == _T("EX")) {
                if (m_pMAPI) m_pMAPI->GetExEmail(pRows->aRow[0].lpProps[PROP_ENTRYID].Value.bin, strEmail);
            } else {
                strEmail = CMAPIEx::GetValidString(pRows->aRow[0].lpProps[PROP_RECIPIENT_EMAIL]);
            }
            bResult = TRUE;
        }
        FreeProws(pRows);
    }
    MAPIFreeBuffer(pRows);
    return bResult;
}

BOOL CMAPIMessage::GetAttachmentName(CString& strAttachmentName, int nIndex) {
    strAttachmentName = _T("");

    LPMAPITABLE pAttachTable = NULL;
    if (m_pMessage->GetAttachmentTable(0, &pAttachTable) == S_OK) {
        enum {
            PROP_ATTACH_LONG_FILENAME, PROP_ATTACH_FILENAME, ATTACH_COLS
        };
        static SizedSPropTagArray(ATTACH_COLS, Columns) = { ATTACH_COLS, PR_ATTACH_LONG_FILENAME, PR_ATTACH_FILENAME };
        ULONG ulCount;
        if ((pAttachTable->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) &&
            (pAttachTable->GetRowCount(0, &ulCount) == S_OK) && (nIndex < (int)ulCount)) {
            LPSRowSet pRows = NULL;
            if (pAttachTable->QueryRows(ulCount, 0, &pRows) != S_OK) MAPIFreeBuffer(pRows);
            else {
                if (nIndex < (int)pRows->cRows) {
                    if (CMAPIEx::GetValidString(pRows->aRow[nIndex].lpProps[PROP_ATTACH_LONG_FILENAME])) strAttachmentName = pRows->aRow[nIndex].lpProps[PROP_ATTACH_LONG_FILENAME].Value.LPSZ;
                    else if (CMAPIEx::GetValidString(pRows->aRow[nIndex].lpProps[PROP_ATTACH_FILENAME])) strAttachmentName = pRows->aRow[nIndex].lpProps[PROP_ATTACH_FILENAME].Value.LPSZ;
                    else strAttachmentName = _T("");
                }
                FreeProws(pRows);
            }
        }
        RELEASE(pAttachTable);
    }
    return !strAttachmentName.IsEmpty();
}

BOOL CMAPIMessage::SaveAttachment(LPATTACH pAttachment, LPCTSTR szPath) {
    CFile file;

    if (!file.Open(szPath, CFile::modeCreate | CFile::modeWrite)) return FALSE;

    IStream *pStream;
    if (pAttachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, NULL, (LPUNKNOWN *)&pStream) != S_OK) {
        file.Close();
        return FALSE;
    }

    const int BUF_SIZE = 4096;
    BYTE      b[BUF_SIZE];
    ULONG     ulRead;

    do {
        pStream->Read(&b, BUF_SIZE, &ulRead);
        if (ulRead) file.Write(b, ulRead);
    } while (ulRead >= BUF_SIZE);

    file.Close();
    RELEASE(pStream);
    return TRUE;
}

// use nIndex of -1 to save all attachments to szFolder
BOOL CMAPIMessage::SaveAttachment(LPCTSTR szFolder, int nIndex) {
    LPMAPITABLE pAttachTable = NULL;

    if (m_pMessage->GetAttachmentTable(0, &pAttachTable) != S_OK) return FALSE;

    CString strPath;
    BOOL    bResult = FALSE;
    enum {
        PROP_ATTACH_NUM, PROP_ATTACH_LONG_FILENAME, PROP_ATTACH_FILENAME, ATTACH_COLS
    };
    static SizedSPropTagArray(ATTACH_COLS, Columns) = { ATTACH_COLS, PR_ATTACH_NUM, PR_ATTACH_LONG_FILENAME, PR_ATTACH_FILENAME };
    if (pAttachTable->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) {
        int       i     = 0;
        LPSRowSet pRows = NULL;
        while (TRUE) {
            if (pAttachTable->QueryRows(1, 0, &pRows) != S_OK) MAPIFreeBuffer(pRows);
            else if (!pRows->cRows) FreeProws(pRows);
            else if (i < nIndex) {
                i++;
                continue;
            } else {
                LPATTACH pAttachment;
                if (m_pMessage->OpenAttach(pRows->aRow[0].lpProps[PROP_ATTACH_NUM].Value.bin.cb, NULL, 0, &pAttachment) == S_OK) {
                    if (CMAPIEx::GetValidString(pRows->aRow[0].lpProps[PROP_ATTACH_LONG_FILENAME])) strPath.Format(_T("%s\\%s"), szFolder, pRows->aRow[0].lpProps[PROP_ATTACH_LONG_FILENAME].Value.LPSZ);
                    else if (CMAPIEx::GetValidString(pRows->aRow[0].lpProps[PROP_ATTACH_FILENAME])) strPath.Format(_T("%s\\%s"), szFolder, pRows->aRow[0].lpProps[PROP_ATTACH_FILENAME].Value.LPSZ);
                    else strPath.Format(_T("%s\\Attachment.dat"), szFolder);

                    if (!SaveAttachment(pAttachment, strPath)) {
                        pAttachment->Release();
                        FreeProws(pRows);
                        RELEASE(pAttachTable);
                        return FALSE;
                    }
                    bResult = TRUE;
                    pAttachment->Release();
                }

                FreeProws(pRows);
                if (nIndex == -1) continue;
            }
            break;
        }
    }
    RELEASE(pAttachTable);
    return bResult;
}

// use nIndex of -1 to delete all attachments
BOOL CMAPIMessage::DeleteAttachment(int nIndex) {
    LPMAPITABLE pAttachTable = NULL;

    if (m_pMessage->GetAttachmentTable(0, &pAttachTable) != S_OK) return FALSE;

    BOOL bResult = FALSE;
    enum {
        PROP_ATTACH_NUM, ATTACH_COLS
    };
    static SizedSPropTagArray(ATTACH_COLS, Columns) = { ATTACH_COLS, PR_ATTACH_NUM };
    if (pAttachTable->SetColumns((LPSPropTagArray) & Columns, 0) == S_OK) {
        int       i     = 0;
        LPSRowSet pRows = NULL;
        while (TRUE) {
            if (pAttachTable->QueryRows(1, 0, &pRows) != S_OK) MAPIFreeBuffer(pRows);
            else if (!pRows->cRows) FreeProws(pRows);
            else if (i < nIndex) {
                i++;
                continue;
            } else {
                if (m_pMessage->DeleteAttach(pRows->aRow[0].lpProps[PROP_ATTACH_NUM].Value.bin.cb, 0, NULL, 0) != S_OK) {
                    FreeProws(pRows);
                    RELEASE(pAttachTable);
                    return FALSE;
                }
                bResult = TRUE;
                FreeProws(pRows);
                if (nIndex == -1) continue;
            }
            break;
        }
    }
    RELEASE(pAttachTable);
    if (bResult) m_pMessage->SaveChanges(FORCE_SAVE | KEEP_OPEN_READWRITE);
    return bResult;
}

BOOL CMAPIMessage::Create(CMAPIEx *pMAPI, int nPriority) {
    if (!pMAPI) return FALSE;
    Close();
    LPMAPIFOLDER pOutbox = pMAPI->GetFolder();
    if (!pOutbox) pOutbox = pMAPI->OpenOutbox();
    if (!pOutbox || (pOutbox->CreateMessage(NULL, 0, &m_pMessage) != S_OK)) return FALSE;
    m_pMAPI = pMAPI;

    SPropValue prop;
    SetMessageFlags(MSGFLAG_UNSENT | MSGFLAG_FROMME);

#ifdef _WIN32_WCE
    // the following uses non outlook attachments (no winmail.dat) so other clients can read mail and attachments
    static const GUID ID_USE_TNEF   = { 0x00062008, 0x000, 0x0000, { 0xc0, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    MAPINAMEID        nameid        = { (GUID *)&ID_USE_TNEF, MNID_ID, 0x8582 };
    MAPINAMEID        *rgpnameid[1] = { &nameid };
    _SPropTagArray    *lpSPropTags;
    HRESULT           hr        = m_pMessage->GetIDsFromNames(1, rgpnameid, MAPI_CREATE, &lpSPropTags);
    unsigned long     ulPropTag = lpSPropTags->aulPropTag[0];
    ulPropTag = PROP_TAG(PT_BOOLEAN, PROP_ID(ulPropTag));

    prop.ulPropTag = ulPropTag;
    prop.Value.b   = FALSE;
    m_pMessage->SetProps(1, &prop, NULL);

    prop.ulPropTag   = PR_MESSAGE_CLASS;
    prop.Value.lpszA = (TCHAR *)"IPM.Note";
    m_pMessage->SetProps(1, &prop, NULL);
#endif

    LPSPropValue props    = NULL;
    ULONG        cValues  = 0;
    ULONG        rgTags[] = { 1, PR_IPM_SENTMAIL_ENTRYID };

    if (m_pMAPI->GetMessageStore()->GetProps((LPSPropTagArray)rgTags, CMAPIEx::cm_nMAPICode, &cValues, &props) == S_OK) {
        prop.ulPropTag     = PR_SENTMAIL_ENTRYID;
        prop.Value.bin.cb  = props[0].Value.bin.cb;
        prop.Value.bin.lpb = props[0].Value.bin.lpb;
        m_pMessage->SetProps(1, &prop, NULL);
    }

    if (nPriority != IMPORTANCE_NORMAL) {
        prop.ulPropTag = PR_IMPORTANCE;
        prop.Value.l   = nPriority;
        m_pMessage->SetProps(1, &prop, NULL);
    }

#ifdef _WIN32_WCE
    prop.ulPropTag = PR_MSG_STATUS;
    prop.Value.ul  = MSGSTATUS_RECTYPE_SMTP;
    m_pMessage->SetProps(1, &prop, NULL);
#endif

    MAPIFreeBuffer(props);
    return TRUE;
}

// used only by WinCE, pass in MSGSTATUS_RECTYPE_SMS to send an SMS
// (default is set to MSGSTATUS_RECTYPE_SMTP in Create(...) above)
BOOL CMAPIMessage::SetMessageStatus(int nMessageStatus) {
    SPropValue prop;

    prop.ulPropTag = PR_MSG_STATUS;
    prop.Value.ul  = nMessageStatus;
    return m_pMessage->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIMessage::MarkAsRead(BOOL bRead) {
    ULONG ulFlags = bRead ? 0 : CLEAR_READ_FLAG;

    if (m_pMessage) return m_pMessage->SetReadFlag(ulFlags) == S_OK;
    return FALSE;
}

BOOL CMAPIMessage::AddRecipients(LPADRLIST pAddressList) {
#ifdef _WIN32_WCE
    HRESULT hr = m_pMessage->ModifyRecipients(MODRECIP_ADD, pAddressList);
#else
    LPADRBOOK pAddressBook;
    if (m_pMAPI->GetSession()->OpenAddressBook(0, NULL, AB_NO_DIALOG, &pAddressBook) != S_OK) return FALSE;

    HRESULT hr = E_INVALIDARG;
    if (pAddressBook->ResolveName(0, CMAPIEx::cm_nMAPICode, NULL, pAddressList) == S_OK) hr = m_pMessage->ModifyRecipients(MODRECIP_ADD, pAddressList);
    RELEASE(pAddressBook);
#endif
    return hr == S_OK;
}

BOOL CMAPIMessage::AddRecipient(LPCTSTR szEmail, int nType) {
    if (!m_pMessage || !m_pMAPI) return FALSE;

    int       nBufSize     = CbNewADRLIST(1);
    LPADRLIST pAddressList = NULL;
    MAPIAllocateBuffer(nBufSize, (LPVOID FAR *)&pAddressList);
    memset(pAddressList, 0, nBufSize);

    const int nProperties = 2;
    pAddressList->cEntries = 1;

    pAddressList->aEntries[0].ulReserved1 = 0;
    pAddressList->aEntries[0].cValues     = nProperties;

    MAPIAllocateBuffer(sizeof(SPropValue) * nProperties, (LPVOID FAR *)&pAddressList->aEntries[0].rgPropVals);
    memset(pAddressList->aEntries[0].rgPropVals, 0, sizeof(SPropValue) * nProperties);

    pAddressList->aEntries[0].rgPropVals[0].ulPropTag = PR_RECIPIENT_TYPE;
    pAddressList->aEntries[0].rgPropVals[0].Value.ul  = nType;

#ifdef _WIN32_WCE
    pAddressList->aEntries[0].rgPropVals[1].ulPropTag  = PR_EMAIL_ADDRESS;
    pAddressList->aEntries[0].rgPropVals[1].Value.LPSZ = (TCHAR *)szEmail;
#else
    pAddressList->aEntries[0].rgPropVals[1].ulPropTag  = PR_DISPLAY_NAME;
    pAddressList->aEntries[0].rgPropVals[1].Value.LPSZ = (TCHAR *)szEmail;
#endif

    BOOL bResult = AddRecipients(pAddressList);
    FreePadrlist(pAddressList);
    return bResult;
}

void CMAPIMessage::SetSubject(LPCTSTR szSubject) {
    m_strSubject = szSubject;
    if (m_strSubject.GetLength() && m_pMessage) {
        SPropValue prop;
        prop.ulPropTag  = PR_SUBJECT;
        prop.Value.LPSZ = (TCHAR *)szSubject;
        m_pMessage->SetProps(1, &prop, NULL);
    }
}

void CMAPIMessage::SetSenderName(LPCTSTR szSenderName) {
    m_strSenderName = szSenderName;
    if (m_strSenderName.GetLength() && m_pMessage) {
        SPropValue prop;
        prop.ulPropTag  = PR_SENDER_NAME;
        prop.Value.LPSZ = (TCHAR *)szSenderName;
        m_pMessage->SetProps(1, &prop, NULL);
    }
}

void CMAPIMessage::SetSenderEmail(LPCTSTR szSenderEmail) {
    m_strSenderEmail = szSenderEmail;
    if (m_strSenderEmail.GetLength() && m_pMessage) {
        SPropValue prop;
        prop.ulPropTag  = PR_SENDER_EMAIL_ADDRESS;
        prop.Value.LPSZ = (TCHAR *)szSenderEmail;
        m_pMessage->SetProps(1, &prop, NULL);
    }
}

void CMAPIMessage::SetBody(LPCTSTR szBody) {
    m_strBody = szBody;
    if (m_strBody.GetLength() && m_pMessage) {
        LPSTREAM pStream = NULL;
        if (m_pMessage->OpenProperty(PR_BODY, &IID_IStream, 0, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
            pStream->Write(szBody, (ULONG)(_tcslen(szBody) + 1) * sizeof(TCHAR), NULL);
            RELEASE(pStream);
        }
    }
}

// use this function to set the body of the message to HTML or RTF
void CMAPIMessage::SetRTF(LPCTSTR szRTF) {
    if (!m_pMessage) return;
    m_strBody = szRTF;

    ULONG ulSupport = m_pMAPI ? m_pMAPI->GetMessageStoreSupport() : 0;
    if (_tcsnccmp(szRTF, _T("<html"), 5) == 0) {
        // is this HTML?  Does this store support HTML directly?
        if (ulSupport & STORE_HTML_OK) {
            LPSTREAM pStream = NULL;
            if (m_pMessage->OpenProperty(PR_BODY_HTML, &IID_IStream, 0, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
                pStream->Write(szRTF, (ULONG)(_tcslen(szRTF) + 1) * sizeof(TCHAR), NULL);
            }
            RELEASE(pStream);
            return;
        } else {
            // otherwise lets encode it into RTF
            TCHAR szCodePage[6] = _T("1252");           // default codepage is ANSI - Latin I
            GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, szCodePage, sizeof(szCodePage));

            m_strRTF.Format(_T("{\\rtf1\\ansi\\ansicpg%s\\fromhtml1 {\\*\\htmltag1 "), szCodePage);
            m_strRTF += szRTF;
            m_strRTF += _T(" }}");
        }
    }

    if (m_strRTF.GetLength()) {
        LPSTREAM pStream = NULL;
        if (m_pMessage->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_CREATE | STGM_WRITE, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
            IStream *pUncompressed;
            if (WrapCompressedRTFStream(pStream, MAPI_MODIFY, &pUncompressed) == S_OK) {
                pUncompressed->Write(m_strRTF, (ULONG)m_strRTF.GetLength() * sizeof(TCHAR), NULL);
                if (pUncompressed->Commit(STGC_DEFAULT) == S_OK) pStream->Commit(STGC_DEFAULT);
                RELEASE(pUncompressed);
            }
            RELEASE(pStream);
        }
    }
}

BOOL CMAPIMessage::AddAttachment(LPCTSTR szPath, LPCTSTR szName) {
    if (!m_pMessage) return FALSE;

    IAttach *pAttachment    = NULL;
    ULONG   ulAttachmentNum = 0;

    CFile file;
    if (!file.Open(szPath, CFile::modeRead)) return FALSE;

    LPTSTR szFileName = (LPTSTR)szName;
    if (!szFileName) {
        szFileName = (LPTSTR)szPath;
        for (int i = (int)_tcsclen(szPath) - 1; i >= 0; i--) if ((szPath[i] == '\\') || (szPath[i] == '/')) {
                szFileName = (LPTSTR)&szPath[i + 1];
                break;
            }
    }

    if (m_pMessage->CreateAttach(NULL, 0, &ulAttachmentNum, &pAttachment) != S_OK) {
        file.Close();
        return FALSE;
    }

    const int  nProperties = 4;
    SPropValue prop[nProperties];
    memset(prop, 0, sizeof(SPropValue) * nProperties);
    prop[0].ulPropTag  = PR_ATTACH_METHOD;
    prop[0].Value.ul   = ATTACH_BY_VALUE;
    prop[1].ulPropTag  = PR_ATTACH_LONG_FILENAME;
    prop[1].Value.LPSZ = (TCHAR *)szFileName;
    prop[2].ulPropTag  = PR_ATTACH_FILENAME;
    prop[2].Value.LPSZ = (TCHAR *)szFileName;
    prop[3].ulPropTag  = PR_RENDERING_POSITION;
    prop[3].Value.l    = -1;
    if (pAttachment->SetProps(nProperties, prop, NULL) == S_OK) {
        LPSTREAM pStream = NULL;
        if (pAttachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, 0, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
            const int BUF_SIZE = 4096;
            BYTE      pData[BUF_SIZE];
            ULONG     ulSize = 0, ulRead, ulWritten;

            ulRead = file.Read(pData, BUF_SIZE);
            while (ulRead) {
                pStream->Write(pData, ulRead, &ulWritten);
                ulSize += ulRead;
                ulRead  = file.Read(pData, BUF_SIZE);
            }

            pStream->Commit(STGC_DEFAULT);
            RELEASE(pStream);
            file.Close();

            prop[0].ulPropTag = PR_ATTACH_SIZE;
            prop[0].Value.ul  = ulSize;
            pAttachment->SetProps(1, prop, NULL);

            pAttachment->SaveChanges(KEEP_OPEN_READONLY);
            RELEASE(pAttachment);
            m_nAttachments++;
            return TRUE;
        }
    }

    file.Close();
    RELEASE(pAttachment);
    return FALSE;
}

// request a Read Receipt sent to szReceiverEmail
BOOL CMAPIMessage::SetReadReceipt(BOOL bSet, LPCTSTR szReceiverEmail) {
    if (!m_pMessage) return FALSE;

    SPropValue prop;
    prop.ulPropTag = PR_READ_RECEIPT_REQUESTED;
    prop.Value.b   = (unsigned short)bSet;
    if (m_pMessage->SetProps(1, &prop, NULL) != S_OK) return FALSE;

    if (bSet && szReceiverEmail && (_tcslen(szReceiverEmail) > 0)) {
        prop.ulPropTag  = PR_READ_RECEIPT_SEARCH_KEY;
        prop.Value.LPSZ = (TCHAR *)szReceiverEmail;
        m_pMessage->SetProps(1, &prop, NULL);
    }
    return TRUE;
}

BOOL CMAPIMessage::SetDeliveryReceipt(BOOL bSet) {
    if (!m_pMessage) return FALSE;

    SPropValue prop;
    prop.ulPropTag = PR_ORIGINATOR_DELIVERY_REPORT_REQUESTED;
    prop.Value.b   = (unsigned short)bSet;
    return m_pMessage->SetProps(1, &prop, NULL) != S_OK;
}

// shallow copy only, no message pointer
CMAPIMessage& CMAPIMessage::operator=(CMAPIMessage& message) {
    m_strSenderName  = message.m_strSenderName;
    m_strSenderEmail = message.m_strSenderEmail;
    m_strSubject     = message.m_strSubject;
    m_strBody        = message.m_strBody;
    m_strRTF         = message.m_strRTF;

    m_dwMessageFlags = message.m_dwMessageFlags;
    SetEntryID(message.GetEntryID());
    return *this;
}

// limited compare, compares entry IDs and subject to determine if two emails are equal
BOOL CMAPIMessage::operator==(CMAPIMessage& message) {
    if (!m_entryID.cb || !message.m_entryID.cb || (m_entryID.cb != message.m_entryID.cb)) return FALSE;
    if (memcmp(&m_entryID.lpb, &message.m_entryID.lpb, m_entryID.cb)) return FALSE;
    return !m_strSubject.Compare(message.m_strSubject);
}

// Novell GroupWise customization by jcadmin
#ifndef GROUPWISE
BOOL CMAPIMessage::MarkAsPrivate() {
    return FALSE;
}

#else
 #include GWMAPI.h
//(from Novell Developer Kit)
 #define SEND_OPTIONS_MARK_PRIVATE    0x00080000L

void CMAPIMessage::MarkAsPrivate() {
    SPropValue prop;

    prop.ulPropTag = PR_NGW_SEND_OPTIONS;
    prop.Value.l   = NGW_SEND_OPTIONS_MARK_PRIVATE;
    return m_pMessage->SetProps(1, &prop, NULL) == S_OK;
}
#endif

BOOL CMAPIMessage::SetSensitivity(int nSensitivity) {
    SPropValue prop;

    prop.ulPropTag = PR_SENSITIVITY;
    prop.Value.l   = nSensitivity;
    return m_pMessage->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIMessage::SetNamedProperty(LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate) {
    CMAPINamedProperty prop(m_pMessage);

    return prop.SetNamedProperty(szFieldName, szField, bCreate);
}

BOOL CMAPIMessage::SetMessageFlags(int nFlags) {
    SPropValue prop;

    prop.ulPropTag = PR_MESSAGE_FLAGS;
    prop.Value.l   = nFlags;
    return m_pMessage->SetProps(1, &prop, NULL) == S_OK;
}

// Shows the default MAPI form for IMessage, returns FALSE on failure, IDOK on success or close existing messages
// and IDCANCEL on close new messages
int CMAPIMessage::ShowForm(CMAPIEx *pMAPI) {
    IMAPISession *pSession = pMAPI->GetSession();
    ULONG        ulMessageToken;

    if (pSession && (pSession->PrepareForm(NULL, m_pMessage, &ulMessageToken) == S_OK)) {
        ULONG ulMessageStatus = 0, ulMessageFlags = 0, ulAccess = 0;

        LPSPropValue pProp;
        if (GetProperty(PR_MSG_STATUS, pProp) == S_OK) {
            ulMessageStatus = pProp->Value.ul;
            MAPIFreeBuffer(pProp);
        }
        if (GetProperty(PR_MESSAGE_FLAGS, pProp) == S_OK) {
            ulMessageFlags = pProp->Value.ul;
            MAPIFreeBuffer(pProp);
        }
        if (GetProperty(PR_ACCESS, pProp) == S_OK) {
            ulAccess = pProp->Value.ul;
            MAPIFreeBuffer(pProp);
        }
        if (GetProperty(PR_MESSAGE_CLASS, pProp) == S_OK) {
#ifdef UNICODE
            char szMessageClass[256];
            WideCharToMultiByte(CP_ACP, 0, pProp->Value.LPSZ, -1, szMessageClass, 255, NULL, NULL);
#else
            char *szMessageClass = pProp->Value.LPSZ;
#endif
            HRESULT hr = pSession->ShowForm(NULL, pMAPI->GetMessageStore(), pMAPI->GetFolder(), NULL, ulMessageToken, NULL, 0, ulMessageStatus, ulMessageFlags, ulAccess, szMessageClass);
            MAPIFreeBuffer(pProp);
            if (hr == S_OK) return IDOK;
            if (hr == MAPI_E_USER_CANCEL) return IDCANCEL;
        }
    }
    return FALSE;
}

BOOL CMAPIMessage::Save(BOOL bClose) {
    ULONG ulFlags = bClose ? 0 : KEEP_OPEN_READWRITE;

    if (m_pMessage && (m_pMessage->SaveChanges(ulFlags) == S_OK)) {
        if (bClose) Close();
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIMessage::Send() {
    if (m_pMessage && (m_pMessage->SubmitMessage(0) == S_OK)) {
        Close();
        return TRUE;
    }
    return FALSE;
}
