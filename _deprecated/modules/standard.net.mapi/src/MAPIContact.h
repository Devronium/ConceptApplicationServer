#ifndef __MAPICONTACT_H__
#define __MAPICONTACT_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIContact.h
// Description: MAPI Contact class wrapper
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
class CMAPIContact;

/////////////////////////////////////////////////////////////
// CContactAddress

class AFX_EXT_CLASS CContactAddress : public CObject
{
public:
    CContactAddress();

    enum AddressType { HOME, BUSINESS, OTHER, MAX_ADDRESS_TYPES };

// Attributes
public:
    AddressType m_nType;
    CString     m_strStreet;
    CString     m_strCity;
    CString     m_strStateOrProvince;
    CString     m_strPostalCode;
    CString     m_strCountry;

// Operations
protected:
    BOOL FillAddress(CMAPIContact& contact, AddressType nType);
    BOOL SaveAddress(CMAPIContact& contact, AddressType nType);

    friend CMAPIContact;
};

/////////////////////////////////////////////////////////////
// CMAPIContact

class AFX_EXT_CLASS CMAPIContact : public CObject
{
public:
    CMAPIContact();
    ~CMAPIContact();

    enum {
        OUTLOOK_DATA1  =0x00062004, OUTLOOK_EMAIL1=0x8083, OUTLOOK_EMAIL2=0x8093, OUTLOOK_EMAIL3=0x80A3,
        OUTLOOK_FILE_AS=0x8005, OUTLOOK_POSTAL_ADDRESS=0x8022, OUTLOOK_DISPLAY_ADDRESS_HOME=0x801A
    };

// Attributes
protected:
    CMAPIEx    *m_pMAPI;
    LPMAILUSER m_pUser;
    SBinary    m_entry;

// Operations
public:
    void SetEntryID(SBinary *pEntry = NULL);

    SBinary *GetEntryID() { return &m_entry; }

    BOOL Open(CMAPIEx *pMAPI, SBinary entry);
    void Close();
    BOOL Save(BOOL bClose = TRUE);

    BOOL GetPropertyString(CString& strProperty, ULONG ulProperty);
    BOOL GetName(CString& strName, ULONG ulNameID = PR_DISPLAY_NAME);
    BOOL GetEmail(CString& strEmail, int nIndex = 1);  // 1, 2 or 3 for outlook email addresses
    BOOL GetEmailDisplayAs(CString& strDisplayAs, int nIndex = 1);
    BOOL GetHomePage(CString& strHomePage);
    BOOL GetPhoneNumber(CString& strPhoneNumber, ULONG ulPhoneNumberID);
    BOOL GetAddress(CContactAddress& address, CContactAddress::AddressType nType);
    BOOL GetPostalAddress(CString& strAddress);
    BOOL GetNotes(CString& strNotes, BOOL bRTF = FALSE);
    int GetSensitivity();
    BOOL GetFileAs(CString& strFileAs);
    BOOL GetTitle(CString& strTitle);
    BOOL GetCompany(CString& strCompany);
    BOOL GetDisplayNamePrefix(CString& strPrefix);
    BOOL GetGeneration(CString& strGeneration);
    BOOL GetDepartment(CString& strDepartment);
    BOOL GetOffice(CString& strOffice);
    BOOL GetManagerName(CString& strManagerName);
    BOOL GetAssistantName(CString& strAssistantName);
    BOOL GetNickName(CString& strNickName);
    BOOL GetSpouseName(CString& strSpouseName);
    BOOL GetBirthday(SYSTEMTIME& tmBirthday);
    BOOL GetBirthday(CString& strBirthday, LPCTSTR szFormat = NULL);  // NULL defaults to "MM/dd/yyyy"
    BOOL GetAnniversary(SYSTEMTIME& tmAnniversary);
    BOOL GetAnniversary(CString& strAnniversary, LPCTSTR szFormat = NULL);
    BOOL GetNamedProperty(LPCTSTR szFieldName, CString& strField);

    BOOL SetPropertyString(LPCTSTR szProperty, ULONG ulProperty);
    BOOL SetName(LPCTSTR szName, ULONG ulNameID = PR_DISPLAY_NAME);
    BOOL SetEmail(LPCTSTR szEmail, int nIndex = 1);
    BOOL SetEmailDisplayAs(LPCTSTR szDisplayAs, int nIndex = 1);
    BOOL SetHomePage(LPCTSTR szHomePage);
    BOOL SetPhoneNumber(LPCTSTR szPhoneNumber, ULONG ulPhoneNumberID);
    BOOL SetAddress(CContactAddress& address, CContactAddress::AddressType nType);
    BOOL SetPostalAddress(CContactAddress::AddressType nType);
    BOOL UpdateDisplayAddress(CContactAddress::AddressType nType);
    BOOL SetNotes(LPCTSTR szNotes, BOOL bRTF = FALSE);
    BOOL SetSensitivity(int nSensitivity);
    BOOL SetFileAs(LPCTSTR szFileAs);
    BOOL SetTitle(LPCTSTR szTitle);
    BOOL SetCompany(LPCTSTR szCompany);
    BOOL SetDisplayNamePrefix(LPCTSTR szPrefix);
    BOOL SetGeneration(LPCTSTR szGeneration);
    BOOL UpdateDisplayName();
    BOOL SetDepartment(LPCTSTR szDepartment);
    BOOL SetOffice(LPCTSTR szOffice);
    BOOL SetManagerName(LPCTSTR szManagerName);
    BOOL SetAssistantName(LPCTSTR szAssistantName);
    BOOL SetNickName(LPCTSTR szNickName);
    BOOL SetSpouseName(LPCTSTR szSpouseName);
    BOOL SetBirthday(SYSTEMTIME& tmBirthday);
    BOOL SetAnniversary(SYSTEMTIME& tmAnniversary);
    BOOL SetNamedProperty(LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate = TRUE);

protected:
    int GetOutlookEmailID(int nIndex);
    HRESULT GetProperty(ULONG ulProperty, LPSPropValue& pProp);
};
#endif
