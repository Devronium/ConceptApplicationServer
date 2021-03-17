#ifndef __MAPIMESSAGE_H__
#define __MAPIMESSAGE_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIMessage.h
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

class CMAPIEx;

/////////////////////////////////////////////////////////////
// CMAPIMessage

class AFX_EXT_CLASS CMAPIMessage : public CObject
{
public:
    CMAPIMessage();
    CMAPIMessage(CMAPIMessage& message);
    ~CMAPIMessage();

    enum { PROP_RECIPIENT_TYPE, PROP_RECIPIENT_NAME, PROP_RECIPIENT_EMAIL, PROP_ADDR_TYPE, PROP_ENTRYID, RECIPIENT_COLS };

// Attributes
protected:
    CString m_strSenderName;
    CString m_strSenderEmail;
    CString m_strSubject;
    CString m_strBody;
    CString m_strRTF;
    int     m_nAttachments;

    CMAPIEx     *m_pMAPI;
    LPMESSAGE   m_pMessage;
    DWORD       m_dwMessageFlags;
    SBinary     m_entryID;
    LPMAPITABLE m_pRecipients;

// Operations
public:
    void SetEntryID(SBinary *pEntryID = NULL);

    SBinary *GetEntryID() { return &m_entryID; }
    LPMESSAGE GetMessage() { return m_pMessage; }

    BOOL IsUnread() { return !(m_dwMessageFlags & MSGFLAG_READ); }
    BOOL MarkAsRead(BOOL bRead = TRUE);
    BOOL Open(CMAPIEx *pMAPI, SBinary entryID, DWORD dwMessageFlags);
    void Close();
    BOOL Create(CMAPIEx *pMAPI, int nPriority = IMPORTANCE_NORMAL);  // IMPORTANCE_HIGH or IMPORTANCE_LOW also valid
    int ShowForm(CMAPIEx *pMAPI);
    BOOL Save(BOOL bClose = TRUE);
    BOOL Send();

    BOOL GetHeader(CString& strHeader);

    LPCTSTR GetSenderName() { return m_strSenderName; }
    LPCTSTR GetSenderEmail() { return m_strSenderEmail; }
    LPCTSTR GetSubject() { return m_strSubject; }
    LPCTSTR GetBody();
    LPCTSTR GetRTF();
    BOOL GetReceivedTime(SYSTEMTIME& tmReceived);
    BOOL GetReceivedTime(CString& strReceivedTime, LPCTSTR szFormat = NULL);  // NULL defaults to "MM/dd/yyyy hh:mm:ss tt"
    BOOL GetSubmitTime(SYSTEMTIME& tmSubmit);
    BOOL GetSubmitTime(CString& strSubmitTime, LPCTSTR szFormat = NULL);
    BOOL GetTo(CString& strTo);
    BOOL GetCC(CString& strCC);
    BOOL GetBCC(CString& strBCC);
    int GetSensitivity();
    BOOL GetNamedProperty(LPCTSTR szFieldName, CString& strField);
    int GetMessageFlags();

    BOOL GetRecipients();
    BOOL GetNextRecipient(CString& strName, CString& strEmail, int& nType);

    int GetAttachmentCount() { return m_nAttachments; }
    BOOL GetAttachmentName(CString& strAttachmentName, int nIndex);
    BOOL SaveAttachment(LPCTSTR szFolder, int nIndex = -1);
    BOOL DeleteAttachment(int nIndex = -1);

    BOOL SetMessageStatus(int nMessageStatus);               // used only by WinCE, pass in MSGSTATUS_RECTYPE_SMS to send an SMS
    BOOL AddRecipients(LPADRLIST pAddressList);
    BOOL AddRecipient(LPCTSTR szEmail, int nType = MAPI_TO); // MAPI_CC and MAPI_BCC also valid
    void SetSubject(LPCTSTR szSubject);
    void SetSenderName(LPCTSTR szSenderName);
    void SetSenderEmail(LPCTSTR szSenderEmail);
    void SetBody(LPCTSTR szBody);
    void SetRTF(LPCTSTR szRTF);
    BOOL AddAttachment(LPCTSTR szPath, LPCTSTR szName = NULL);
    BOOL SetReadReceipt(BOOL bSet = TRUE, LPCTSTR szReceiverEmail = NULL);
    BOOL SetDeliveryReceipt(BOOL bSet = TRUE);
    BOOL MarkAsPrivate();
    BOOL SetSensitivity(int nSensitivity);
    BOOL SetNamedProperty(LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate = TRUE);
    BOOL SetMessageFlags(int nFlags);

    // does not copy/reference m_pMessage, shallow copy only
    CMAPIMessage& operator=(CMAPIMessage& message);

    // compares entryID only
    BOOL operator==(CMAPIMessage& message);

protected:
    HRESULT GetProperty(ULONG ulProperty, LPSPropValue& prop);

    void FillSenderName();
    void FillSenderEmail();
    void FillSubject();
    void FillBody();
    void FillRTF();
    void FillAttachmentCount();

    BOOL SaveAttachment(LPATTACH pAttachment, LPCTSTR szPath);
};
#endif
