////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPIContact.cpp
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

#include "MAPIExPCH.h"
#include "MAPIEx.h"
#include "MAPINamedProperty.h"

/////////////////////////////////////////////////////////////
// CContactAddress

const ULONG ContactAddressTag[][5] = {
    { PR_HOME_ADDRESS_CITY,     PR_HOME_ADDRESS_COUNTRY,     PR_HOME_ADDRESS_STATE_OR_PROVINCE,
      PR_HOME_ADDRESS_STREET, PR_HOME_ADDRESS_POSTAL_CODE },
    { PR_BUSINESS_ADDRESS_CITY, PR_BUSINESS_ADDRESS_COUNTRY, PR_BUSINESS_ADDRESS_STATE_OR_PROVINCE,
      PR_BUSINESS_ADDRESS_STREET, PR_BUSINESS_ADDRESS_POSTAL_CODE },
    { PR_OTHER_ADDRESS_CITY,    PR_OTHER_ADDRESS_COUNTRY,    PR_OTHER_ADDRESS_STATE_OR_PROVINCE,
      PR_OTHER_ADDRESS_STREET, PR_OTHER_ADDRESS_POSTAL_CODE },
};

CContactAddress::CContactAddress() {
}

BOOL CContactAddress::FillAddress(CMAPIContact& contact, AddressType nType) {
    if ((nType < HOME) || (nType > OTHER)) return FALSE;
    m_nType = nType;

    if (!contact.GetPropertyString(m_strCity, ContactAddressTag[nType][0])) return FALSE;
    if (!contact.GetPropertyString(m_strCountry, ContactAddressTag[nType][1])) return FALSE;
    if (!contact.GetPropertyString(m_strStateOrProvince, ContactAddressTag[nType][2])) return FALSE;
    if (!contact.GetPropertyString(m_strStreet, ContactAddressTag[nType][3])) return FALSE;
    if (!contact.GetPropertyString(m_strPostalCode, ContactAddressTag[nType][4])) return FALSE;
    return TRUE;
}

BOOL CContactAddress::SaveAddress(CMAPIContact& contact, AddressType nType) {
    if ((nType < HOME) || (nType > OTHER)) return FALSE;
    m_nType = nType;

    if (!contact.SetPropertyString(m_strCity, ContactAddressTag[nType][0])) return FALSE;
    if (!contact.SetPropertyString(m_strCountry, ContactAddressTag[nType][1])) return FALSE;
    if (!contact.SetPropertyString(m_strStateOrProvince, ContactAddressTag[nType][2])) return FALSE;
    if (!contact.SetPropertyString(m_strStreet, ContactAddressTag[nType][3])) return FALSE;
    if (!contact.SetPropertyString(m_strPostalCode, ContactAddressTag[nType][4])) return FALSE;
    return TRUE;
}

/////////////////////////////////////////////////////////////
// CMAPIContact

const ULONG PhoneNumberIDs[] = {
    PR_PRIMARY_TELEPHONE_NUMBER,  PR_BUSINESS_TELEPHONE_NUMBER,  PR_HOME_TELEPHONE_NUMBER,
    PR_CALLBACK_TELEPHONE_NUMBER, PR_BUSINESS2_TELEPHONE_NUMBER, PR_MOBILE_TELEPHONE_NUMBER,
    PR_RADIO_TELEPHONE_NUMBER,    PR_CAR_TELEPHONE_NUMBER,       PR_OTHER_TELEPHONE_NUMBER,
    PR_PAGER_TELEPHONE_NUMBER,    PR_PRIMARY_FAX_NUMBER,         PR_BUSINESS_FAX_NUMBER,
    PR_HOME_FAX_NUMBER,           PR_TELEX_NUMBER,               PR_ISDN_NUMBER,              PR_ASSISTANT_TELEPHONE_NUMBER,
    PR_HOME2_TELEPHONE_NUMBER,    PR_TTYTDD_PHONE_NUMBER,        PR_COMPANY_MAIN_PHONE_NUMBER, 0
};

const ULONG NameIDs[] = { PR_DISPLAY_NAME, PR_GIVEN_NAME, PR_MIDDLE_NAME, PR_SURNAME, PR_INITIALS, 0 };

CMAPIContact::CMAPIContact() {
    m_pUser    = NULL;
    m_pMAPI    = NULL;
    m_entry.cb = 0;
    SetEntryID(NULL);
}

CMAPIContact::~CMAPIContact() {
    Close();
}

void CMAPIContact::SetEntryID(SBinary *pEntry) {
    if (m_entry.cb) delete [] m_entry.lpb;
    m_entry.lpb = NULL;

    if (pEntry) {
        m_entry.cb = pEntry->cb;
        if (m_entry.cb) {
            m_entry.lpb = new BYTE[m_entry.cb];
            memcpy(m_entry.lpb, pEntry->lpb, m_entry.cb);
        }
    } else {
        m_entry.cb = 0;
    }
}

BOOL CMAPIContact::Open(CMAPIEx *pMAPI, SBinary entry) {
    Close();
    m_pMAPI = pMAPI;
    ULONG ulObjType;
    if (m_pMAPI->GetSession()->OpenEntry(entry.cb, (LPENTRYID)entry.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, (LPUNKNOWN *)&m_pUser) != S_OK) return FALSE;

    SetEntryID(&entry);
    return TRUE;
}

void CMAPIContact::Close() {
    SetEntryID(NULL);
    RELEASE(m_pUser);
    m_pMAPI = NULL;
}

BOOL CMAPIContact::Save(BOOL bClose) {
    ULONG ulFlags = bClose ? 0 : KEEP_OPEN_READWRITE;

    if (m_pUser && (m_pUser->SaveChanges(ulFlags) == S_OK)) {
        if (bClose) Close();
        return TRUE;
    }
    return FALSE;
}

HRESULT CMAPIContact::GetProperty(ULONG ulProperty, LPSPropValue& pProp) {
    ULONG ulPropCount;
    ULONG p[2] = { 1, ulProperty };

    return m_pUser->GetProps((LPSPropTagArray)p, CMAPIEx::cm_nMAPICode, &ulPropCount, &pProp);
}

BOOL CMAPIContact::GetPropertyString(CString& strProperty, ULONG ulProperty) {
    LPSPropValue pProp;

    if (GetProperty(ulProperty, pProp) == S_OK) {
        strProperty = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    } else {
        strProperty = _T("");
        return FALSE;
    }
}

BOOL CMAPIContact::GetName(CString& strName, ULONG ulNameID) {
    ULONG i = 0;

    while (NameIDs[i] != ulNameID && NameIDs[i] > 0) i++;
    if (!NameIDs[i]) {
        strName = _T("");
        return FALSE;
    } else {
        return GetPropertyString(strName, ulNameID);
    }
}

int CMAPIContact::GetOutlookEmailID(int nIndex) {
    ULONG ulProperty[] = { OUTLOOK_EMAIL1, OUTLOOK_EMAIL2, OUTLOOK_EMAIL3 };

    if ((nIndex < 1) || (nIndex > 3)) return 0;
    return ulProperty[nIndex - 1];
}

// uses the built in outlook email fields, OUTLOOK_EMAIL1 etc, minus 1 for ADDR_TYPE and +1 for EmailOriginalDisplayName
BOOL CMAPIContact::GetEmail(CString& strEmail, int nIndex) {
    ULONG nID = GetOutlookEmailID(nIndex);

    if (!nID) return FALSE;

    LPSPropValue       pProp;
    CMAPINamedProperty prop(m_pUser);
    if (prop.GetOutlookProperty(OUTLOOK_DATA1, nID - 1, pProp)) {
        CString strAddrType = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        if (prop.GetOutlookProperty(OUTLOOK_DATA1, nID, pProp)) {
            strEmail = CMAPIEx::GetValidString(*pProp);
            MAPIFreeBuffer(pProp);
            if (strAddrType == _T("EX")) {
                // for EX types we use the original display name (seems to contain the appropriate data)
                if (prop.GetOutlookProperty(OUTLOOK_DATA1, nID + 1, pProp)) {
                    strEmail = CMAPIEx::GetValidString(*pProp);
                    MAPIFreeBuffer(pProp);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CMAPIContact::GetEmailDisplayAs(CString& strDisplayAs, int nIndex) {
    ULONG nID = GetOutlookEmailID(nIndex);

    if (!nID) return FALSE;

    LPSPropValue       pProp;
    CMAPINamedProperty prop(m_pUser);
    if (prop.GetOutlookProperty(OUTLOOK_DATA1, nID - 3, pProp)) {
        strDisplayAs = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetHomePage(CString& strHomePage) {
    return GetPropertyString(strHomePage, PR_BUSINESS_HOME_PAGE);
}

BOOL CMAPIContact::GetPhoneNumber(CString& strPhoneNumber, ULONG ulPhoneNumberID) {
    ULONG i = 0;

    while (PhoneNumberIDs[i] != ulPhoneNumberID && PhoneNumberIDs[i] > 0) i++;
    if (!PhoneNumberIDs[i]) {
        strPhoneNumber = _T("");
        return FALSE;
    } else {
        return GetPropertyString(strPhoneNumber, ulPhoneNumberID);
    }
}

BOOL CMAPIContact::GetAddress(CContactAddress& address, CContactAddress::AddressType nType) {
    return address.FillAddress(*this, nType);
}

BOOL CMAPIContact::GetPostalAddress(CString& strAddress) {
    return GetPropertyString(strAddress, PR_POSTAL_ADDRESS);
}

BOOL CMAPIContact::GetNotes(CString& strNotes, BOOL bRTF) {
    strNotes = _T("");
    IStream *pStream;

#ifdef _WIN32_WCE
    const int BUF_SIZE = 1024;
#else
    const int BUF_SIZE = 16384;
#endif
    char  szBuf[BUF_SIZE + 1];
    ULONG ulNumChars;

    if (bRTF) {
        if (m_pUser->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_READ, 0, (LPUNKNOWN *)&pStream) != S_OK) return FALSE;

        IStream *pUncompressed;
        if (WrapCompressedRTFStream(pStream, 0, &pUncompressed) == S_OK) {
            do {
                pUncompressed->Read(szBuf, BUF_SIZE, &ulNumChars);
                szBuf[min(BUF_SIZE, ulNumChars)] = 0;
                strNotes += szBuf;
            } while (ulNumChars >= BUF_SIZE);
            RELEASE(pUncompressed);
        }
    } else {
        if (m_pUser->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, NULL, (LPUNKNOWN *)&pStream) != S_OK) return FALSE;

        do {
            pStream->Read(szBuf, BUF_SIZE * sizeof(TCHAR), &ulNumChars);
            ulNumChars /= sizeof(TCHAR);
            szBuf[min(BUF_SIZE, ulNumChars)] = 0;
            strNotes += szBuf;
        } while (ulNumChars >= BUF_SIZE);
    }
    RELEASE(pStream);
    return TRUE;
}

int CMAPIContact::GetSensitivity() {
    int          nSensitivity = -1;
    LPSPropValue pProp;

    if (GetProperty(PR_SENSITIVITY, pProp) == S_OK) {
        nSensitivity = pProp->Value.l;
        MAPIFreeBuffer(pProp);
    }
    return nSensitivity;
}

BOOL CMAPIContact::GetFileAs(CString& strFileAs) {
    LPSPropValue       pProp;
    CMAPINamedProperty prop(m_pUser);

    if (prop.GetOutlookProperty(OUTLOOK_DATA1, OUTLOOK_FILE_AS, pProp)) {
        strFileAs = CMAPIEx::GetValidString(*pProp);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetTitle(CString& strTitle) {
    return GetPropertyString(strTitle, PR_TITLE);
}

BOOL CMAPIContact::GetCompany(CString& strCompany) {
    return GetPropertyString(strCompany, PR_COMPANY_NAME);
}

BOOL CMAPIContact::GetDisplayNamePrefix(CString& strPrefix) {
    return GetPropertyString(strPrefix, PR_DISPLAY_NAME_PREFIX);
}

BOOL CMAPIContact::GetGeneration(CString& strGeneration) {
    return GetPropertyString(strGeneration, PR_GENERATION);
}

BOOL CMAPIContact::GetDepartment(CString& strDepartment) {
    return GetPropertyString(strDepartment, PR_DEPARTMENT_NAME);
}

BOOL CMAPIContact::GetOffice(CString& strOffice) {
    return GetPropertyString(strOffice, PR_OFFICE_LOCATION);
}

BOOL CMAPIContact::GetManagerName(CString& strManagerName) {
    return GetPropertyString(strManagerName, PR_MANAGER_NAME);
}

BOOL CMAPIContact::GetAssistantName(CString& strAssistantName) {
    return GetPropertyString(strAssistantName, PR_ASSISTANT);
}

BOOL CMAPIContact::GetNickName(CString& strNickName) {
    return GetPropertyString(strNickName, PR_NICKNAME);
}

BOOL CMAPIContact::GetSpouseName(CString& strSpouseName) {
    return GetPropertyString(strSpouseName, PR_SPOUSE_NAME);
}

BOOL CMAPIContact::GetBirthday(SYSTEMTIME& tmBirthday) {
    LPSPropValue pProp;

    if (GetProperty(PR_BIRTHDAY, pProp) == S_OK) {
        FileTimeToSystemTime(&pProp->Value.ft, &tmBirthday);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetBirthday(CString& strBirthday, LPCTSTR szFormat) {
    SYSTEMTIME tm;

    if (GetBirthday(tm)) {
        TCHAR szDate[256];
        if (!szFormat) szFormat = _T("MM/dd/yyyy");
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szFormat, szDate, 256);
        strBirthday = szDate;
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetAnniversary(SYSTEMTIME& tmAnniversary) {
    LPSPropValue pProp;

    if (GetProperty(PR_WEDDING_ANNIVERSARY, pProp) == S_OK) {
        FileTimeToSystemTime(&pProp->Value.ft, &tmAnniversary);
        MAPIFreeBuffer(pProp);
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetAnniversary(CString& strAnniversary, LPCTSTR szFormat) {
    SYSTEMTIME tm;

    if (GetAnniversary(tm)) {
        TCHAR szDate[256];
        if (!szFormat) szFormat = _T("MM/dd/yyyy");
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &tm, szFormat, szDate, 256);
        strAnniversary = szDate;
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::GetNamedProperty(LPCTSTR szFieldName, CString& strField) {
    CMAPINamedProperty prop(m_pUser);

    return prop.GetNamedProperty(szFieldName, strField);
}

BOOL CMAPIContact::SetPropertyString(LPCTSTR szProperty, ULONG ulProperty) {
    SPropValue prop;

    prop.ulPropTag  = ulProperty;
    prop.Value.LPSZ = (LPTSTR)szProperty;
    return m_pUser->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIContact::SetName(LPCTSTR szName, ULONG ulNameID) {
    ULONG i = 0;

    while (NameIDs[i] != ulNameID && NameIDs[i] > 0) i++;
    if (!NameIDs[i]) return FALSE;
    return SetPropertyString(szName, ulNameID);
}

// only supports setting SMTP Email Addresses, sets both display properties, use SetEmailDisplayAs to set the
// outlook DisplayAs property to some other text.  You should check that the text is a valid SMTP email address
// ie name.name@domain.domain because this function will allow any string
BOOL CMAPIContact::SetEmail(LPCTSTR szEmail, int nIndex) {
    ULONG nID = GetOutlookEmailID(nIndex);

    if (!nID) return FALSE;

    CMAPINamedProperty prop(m_pUser);
    if (prop.SetOutlookProperty(OUTLOOK_DATA1, nID - 1, _T("SMTP"))) {
        // we set the email field and both display properties
        if (!prop.SetOutlookProperty(OUTLOOK_DATA1, nID, szEmail)) return FALSE;
        if (!prop.SetOutlookProperty(OUTLOOK_DATA1, nID + 1, szEmail)) return FALSE;
        if (!prop.SetOutlookProperty(OUTLOOK_DATA1, nID - 3, szEmail)) return FALSE;
        return TRUE;
    }
    return FALSE;
}

BOOL CMAPIContact::SetEmailDisplayAs(LPCTSTR szDisplayAs, int nIndex) {
    ULONG nID = GetOutlookEmailID(nIndex);

    if (!nID) return FALSE;

    CMAPINamedProperty prop(m_pUser);
    return prop.SetOutlookProperty(OUTLOOK_DATA1, nID - 3, szDisplayAs);
}

BOOL CMAPIContact::SetHomePage(LPCTSTR szHomePage) {
    return SetPropertyString(szHomePage, PR_BUSINESS_HOME_PAGE);
}

BOOL CMAPIContact::SetPhoneNumber(LPCTSTR szPhoneNumber, ULONG ulPhoneNumberID) {
    ULONG i = 0;

    while (PhoneNumberIDs[i] != ulPhoneNumberID && PhoneNumberIDs[i] > 0) i++;
    if (!PhoneNumberIDs[i] || !szPhoneNumber) return FALSE;

    // lets do a check for North American phone numbers and change the format if so to "(code) xxx-yyyy"
    CString strPhoneNumber = szPhoneNumber;
    if ((strPhoneNumber.GetLength() == 12) && (szPhoneNumber[3] == (TCHAR)'-') && (szPhoneNumber[7] == (TCHAR)'-')) {
        strPhoneNumber.Format(_T("(%s) %s"), strPhoneNumber.Mid(0, 3), strPhoneNumber.Right(8));
    }
    return SetPropertyString(strPhoneNumber, ulPhoneNumberID);
}

BOOL CMAPIContact::SetAddress(CContactAddress& address, CContactAddress::AddressType nType) {
    return address.SaveAddress(*this, nType);
}

// Sets the PR_POSTAL_ADDRESS text and checks the appropriate outlook checkbox by setting the index
BOOL CMAPIContact::SetPostalAddress(CContactAddress::AddressType nType) {
    CContactAddress address;

    if (!GetAddress(address, nType)) return FALSE;

    CString strPostalAddress;
    strPostalAddress.Format(_T("%s\r\n%s  %s  %s\r\n%s"), address.m_strStreet, address.m_strCity, address.m_strStateOrProvince, address.m_strPostalCode, address.m_strCountry);
    if (!SetPropertyString(strPostalAddress, PR_POSTAL_ADDRESS)) return FALSE;

    CMAPINamedProperty prop(m_pUser);
    return prop.SetOutlookProperty(OUTLOOK_DATA1, OUTLOOK_POSTAL_ADDRESS, (int)nType + 1);
}

// updates the outlook display address; call this after changing address fields
BOOL CMAPIContact::UpdateDisplayAddress(CContactAddress::AddressType nType) {
    CContactAddress address;

    if (!GetAddress(address, nType)) return FALSE;

    CString strDisplayAddress;
    strDisplayAddress.Format(_T("%s\r\n%s  %s  %s\r\n%s"), address.m_strStreet, address.m_strCity, address.m_strStateOrProvince, address.m_strPostalCode, address.m_strCountry);

    ULONG ulProperty = OUTLOOK_DISPLAY_ADDRESS_HOME + (int)nType;
    CMAPINamedProperty prop(m_pUser);
    return prop.SetOutlookProperty(OUTLOOK_DATA1, ulProperty, strDisplayAddress);
}

BOOL CMAPIContact::SetNotes(LPCTSTR szNotes, BOOL bRTF) {
    if (!m_pUser || !szNotes) return FALSE;
    ULONG nLen = (ULONG)_tcslen(szNotes);

    HRESULT  hr      = E_FAIL;
    LPSTREAM pStream = NULL;
    if (bRTF) {
        if (m_pUser->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_CREATE | STGM_WRITE, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
            IStream *pUncompressed;
            if (WrapCompressedRTFStream(pStream, MAPI_MODIFY, &pUncompressed) == S_OK) {
                hr = pUncompressed->Write(szNotes, nLen * sizeof(TCHAR), NULL);
                if (pUncompressed->Commit(STGC_DEFAULT) == S_OK) pStream->Commit(STGC_DEFAULT);
                RELEASE(pUncompressed);
            }
        }
    } else {
        if (m_pUser->OpenProperty(PR_BODY, &IID_IStream, 0, MAPI_MODIFY | MAPI_CREATE, (LPUNKNOWN *)&pStream) == S_OK) {
            hr = pStream->Write(szNotes, (nLen + 1) * sizeof(TCHAR), NULL);
        }
    }
    RELEASE(pStream);
    return hr == S_OK;
}

BOOL CMAPIContact::SetSensitivity(int nSensitivity) {
    SPropValue prop;

    prop.ulPropTag = PR_SENSITIVITY;
    prop.Value.l   = nSensitivity;
    return m_pUser->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIContact::SetFileAs(LPCTSTR szFileAs) {
    CMAPINamedProperty prop(m_pUser);

    return prop.SetOutlookProperty(OUTLOOK_DATA1, OUTLOOK_FILE_AS, szFileAs);
}

BOOL CMAPIContact::SetTitle(LPCTSTR szTitle) {
    return SetPropertyString(szTitle, PR_TITLE);
}

BOOL CMAPIContact::SetCompany(LPCTSTR szCompany) {
    return SetPropertyString(szCompany, PR_COMPANY_NAME);
}

BOOL CMAPIContact::SetDisplayNamePrefix(LPCTSTR szPrefix) {
    return SetPropertyString(szPrefix, PR_DISPLAY_NAME_PREFIX);
}

BOOL CMAPIContact::SetGeneration(LPCTSTR szGeneration) {
    return SetPropertyString(szGeneration, PR_GENERATION);
}

// After changing any name field you should call this to update PR_INITIALS and PR_DISPLAY_NAME
BOOL CMAPIContact::UpdateDisplayName() {
    CString strPrefix, strFirst, strMiddle, strLast, strGeneration, strDisplayName, strInitials;

    if (GetDisplayNamePrefix(strPrefix) && strPrefix.GetLength()) {
        strDisplayName += strPrefix;
    }
    if (GetName(strFirst, PR_GIVEN_NAME) && strFirst.GetLength()) {
        if (strDisplayName.GetLength()) strDisplayName += (TCHAR)' ';
        strDisplayName += strFirst;
        strInitials    += strFirst.GetAt(0);
        strInitials    += (TCHAR)'.';
    }
    if (GetName(strMiddle, PR_MIDDLE_NAME) && strMiddle.GetLength()) {
        if (strDisplayName.GetLength()) strDisplayName += (TCHAR)' ';
        strDisplayName += strMiddle;
        strInitials    += (TCHAR)strMiddle.GetAt(0);
        strInitials    += (TCHAR)'.';

        int i = 1, nLen = strMiddle.GetLength();
        while (i < nLen) {
            if (strMiddle.GetAt(i - 1) == (TCHAR)' ') {
                strInitials += (TCHAR)strMiddle.GetAt(i);
                strInitials += (TCHAR)'.';
            }
            i++;
        }
    }
    if (GetName(strLast, PR_SURNAME) && strLast.GetLength()) {
        if (strDisplayName.GetLength()) strDisplayName += (TCHAR)' ';
        strDisplayName += strLast;
        strInitials    += (TCHAR)strLast.GetAt(0);
        strInitials    += (TCHAR)'.';
    }
    if (GetGeneration(strGeneration) && strGeneration.GetLength()) {
        if (strDisplayName.GetLength()) strDisplayName += (TCHAR)' ';
        strDisplayName += strGeneration;
    }

    if (!SetName(strInitials, PR_INITIALS)) return FALSE;
    return SetName(strDisplayName, PR_DISPLAY_NAME);
}

BOOL CMAPIContact::SetDepartment(LPCTSTR szDepartment) {
    return SetPropertyString(szDepartment, PR_DEPARTMENT_NAME);
}

BOOL CMAPIContact::SetOffice(LPCTSTR szOffice) {
    return SetPropertyString(szOffice, PR_OFFICE_LOCATION);
}

BOOL CMAPIContact::SetManagerName(LPCTSTR szManagerName) {
    return SetPropertyString(szManagerName, PR_MANAGER_NAME);
}

BOOL CMAPIContact::SetAssistantName(LPCTSTR szAssistantName) {
    return SetPropertyString(szAssistantName, PR_ASSISTANT);
}

BOOL CMAPIContact::SetNickName(LPCTSTR szNickName) {
    return SetPropertyString(szNickName, PR_NICKNAME);
}

BOOL CMAPIContact::SetSpouseName(LPCTSTR szSpouseName) {
    return SetPropertyString(szSpouseName, PR_SPOUSE_NAME);
}

BOOL CMAPIContact::SetBirthday(SYSTEMTIME& tmBirthday) {
    SPropValue prop;

    prop.ulPropTag = PR_BIRTHDAY;
    SystemTimeToFileTime(&tmBirthday, &prop.Value.ft);
    return m_pUser->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIContact::SetAnniversary(SYSTEMTIME& tmAnniversary) {
    SPropValue prop;

    prop.ulPropTag = PR_WEDDING_ANNIVERSARY;
    SystemTimeToFileTime(&tmAnniversary, &prop.Value.ft);
    return m_pUser->SetProps(1, &prop, NULL) == S_OK;
}

BOOL CMAPIContact::SetNamedProperty(LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate) {
    CMAPINamedProperty prop(m_pUser);

    return prop.SetNamedProperty(szFieldName, szField, bCreate);
}
