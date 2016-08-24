#ifndef __NETMAPI_H__
#define __NETMAPI_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: NetMAPI.h
// Description: Exported functions for Extended MAPI, meant to be called by .NET
//
// Copyright (C) 2006, Noel Dillabough
//
// This source code is free to use and modify provided this notice remains intact and that any enhancements
// or bug fixes are posted to the CodeProject page hosting this class for all to benefit.
//
// Usage: see the Codeproject article at http://www.codeproject.com/internet/CMapiEx.asp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MAPIEx.h"

extern "C" {
// Initialize and Terminate
AFX_EXT_CLASS BOOL MAPIInit();
AFX_EXT_CLASS void MAPITerm();

// Profiles, Message Store
AFX_EXT_CLASS CMAPIEx *MAPILogin(LPCTSTR szProfile);
AFX_EXT_CLASS void MAPILogout(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenMessageStore(CMAPIEx *pMAPI, LPCTSTR szStore);
AFX_EXT_CLASS void MAPIGetProfileName(CMAPIEx *pMAPI, LPTSTR szProfile, int nMaxLength);

// Folders
AFX_EXT_CLASS BOOL MAPIOpenRootFolder(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenInbox(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenOutbox(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenSentItems(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenDeletedItems(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenContacts(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIOpenDrafts(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPIGetHierarchy(CMAPIEx *pMAPI, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS LPMAPIFOLDER MAPIGetFolder(CMAPIEx *pMAPI);
AFX_EXT_CLASS LPMAPIFOLDER MAPIGetNextSubFolder(CMAPIEx *pMAPI, LPTSTR szFolderName, int nMaxLength);
AFX_EXT_CLASS BOOL MAPIOpenSubFolder(CMAPIEx *pMAPI, LPCTSTR szSubFolder, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS LPMAPIFOLDER MAPICreateSubFolder(CMAPIEx *pMAPI, LPCTSTR szSubFolder, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS BOOL MAPIDeleteSubFolderByName(CMAPIEx *pMAPI, LPCTSTR szSubFolder, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS BOOL MAPIDeleteSubFolder(CMAPIEx *pMAPI, LPMAPIFOLDER pSubFolder, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS void MAPIReleaseFolder(LPMAPIFOLDER pFolder);

// Messages, Contacts
AFX_EXT_CLASS BOOL MAPIGetContents(CMAPIEx *pMAPI, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS int MAPIGetRowCount(CMAPIEx *pMAPI);
AFX_EXT_CLASS BOOL MAPISortContents(CMAPIEx *pMAPI, BOOL bAscending, int nSortField);
AFX_EXT_CLASS BOOL MAPIGetNextMessage(CMAPIEx *pMAPI, CMAPIMessage *& pMessage, BOOL bUnreadOnly);
AFX_EXT_CLASS BOOL MAPIGetNextContact(CMAPIEx *pMAPI, CMAPIContact *& pContact);
AFX_EXT_CLASS BOOL MAPIDeleteMessage(CMAPIEx *pMAPI, CMAPIMessage *pMessage, LPMAPIFOLDER pFolder);
AFX_EXT_CLASS BOOL MAPICopyMessage(CMAPIEx *pMAPI, CMAPIMessage *pMessage, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc);
AFX_EXT_CLASS BOOL MAPIMoveMessage(CMAPIEx *pMAPI, CMAPIMessage *pMessage, LPMAPIFOLDER pFolderDest, LPMAPIFOLDER pFolderSrc);

// Message functions
AFX_EXT_CLASS void MessageClose(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageCreate(CMAPIEx *pMAPI, CMAPIMessage *& pMessage, int nPriority);
AFX_EXT_CLASS int MessageShowForm(CMAPIEx *pMAPI, CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageSave(CMAPIMessage *pMessage, BOOL bClose);
AFX_EXT_CLASS BOOL MessageSend(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageIsUnread(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageMarkAsRead(CMAPIMessage *pMessage, BOOL bRead);
AFX_EXT_CLASS BOOL MessageGetHeader(CMAPIMessage *pMessage, LPTSTR szHeader, int nMaxLength);
AFX_EXT_CLASS void MessageGetSenderName(CMAPIMessage *pMessage, LPTSTR szSenderName, int nMaxLength);
AFX_EXT_CLASS void MessageGetSenderEmail(CMAPIMessage *pMessage, LPTSTR szSenderEmail, int nMaxLength);
AFX_EXT_CLASS void MessageGetSubject(CMAPIMessage *pMessage, LPTSTR szSubject, int nMaxLength);
AFX_EXT_CLASS int MessageGetBodySize(CMAPIMessage *pMessage);
AFX_EXT_CLASS void MessageGetBody(CMAPIMessage *pMessage, LPTSTR szBody, int nMaxLength);
AFX_EXT_CLASS int MessageGetRTFSize(CMAPIMessage *pMessage);
AFX_EXT_CLASS void MessageGetRTF(CMAPIMessage *pMessage, LPTSTR szRTF, int nMaxLength);
AFX_EXT_CLASS BOOL MessageGetReceivedTime(CMAPIMessage *pMessage, int& nYear, int& nMonth, int& nDay, int& nHour, int& nMinute, int& nSecond);
AFX_EXT_CLASS BOOL MessageGetReceivedTimeString(CMAPIMessage *pMessage, LPTSTR szReceivedTime, int nMaxLength, LPCTSTR szFormat);
AFX_EXT_CLASS BOOL MessageGetSubmitTime(CMAPIMessage *pMessage, int& nYear, int& nMonth, int& nDay, int& nHour, int& nMinute, int& nSecond);
AFX_EXT_CLASS BOOL MessageGetSubmitTimeString(CMAPIMessage *pMessage, LPTSTR szSubmitTime, int nMaxLength, LPCTSTR szFormat);
AFX_EXT_CLASS BOOL MessageGetTo(CMAPIMessage *pMessage, LPTSTR szTo, int nMaxLength);
AFX_EXT_CLASS BOOL MessageGetCC(CMAPIMessage *pMessage, LPTSTR szCC, int nMaxLength);
AFX_EXT_CLASS BOOL MessageGetBCC(CMAPIMessage *pMessage, LPTSTR szBCC, int nMaxLength);
AFX_EXT_CLASS int MessageGetSensitivity(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageGetNamedProperty(CMAPIMessage *pMessage, LPCTSTR szFieldName, LPTSTR szField, int nMaxLength);
AFX_EXT_CLASS int MessageGetMessageFlags(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageGetRecipients(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageGetNextRecipient(CMAPIMessage *pMessage, LPTSTR szName, int nMaxLenName, LPTSTR szEmail, int nMaxLenEmail, int& nType);
AFX_EXT_CLASS int MessageGetAttachmentCount(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageGetAttachmentName(CMAPIMessage *pMessage, LPTSTR szAttachmentName, int nMaxLength, int nIndex);
AFX_EXT_CLASS BOOL MessageSaveAttachment(CMAPIMessage *pMessage, LPCTSTR szFolder, int nIndex);
AFX_EXT_CLASS BOOL MessageDeleteAttachment(CMAPIMessage *pMessage, int nIndex);
AFX_EXT_CLASS BOOL MessageSetMessageStatus(CMAPIMessage *pMessage, int nMessageStatus);
AFX_EXT_CLASS BOOL MessageAddRecipient(CMAPIMessage *pMessage, LPCTSTR szEmail, int nType);
AFX_EXT_CLASS void MessageSetSubject(CMAPIMessage *pMessage, LPCTSTR szSubject);
AFX_EXT_CLASS void MessageSetSenderName(CMAPIMessage *pMessage, LPCTSTR szSenderName);
AFX_EXT_CLASS void MessageSetSenderEmail(CMAPIMessage *pMessage, LPCTSTR szSenderEmail);
AFX_EXT_CLASS void MessageSetBody(CMAPIMessage *pMessage, LPCTSTR szBody);
AFX_EXT_CLASS void MessageSetRTF(CMAPIMessage *pMessage, LPCTSTR szRTF);
AFX_EXT_CLASS BOOL MessageAddAttachment(CMAPIMessage *pMessage, LPCTSTR szPath, LPCTSTR szName);
AFX_EXT_CLASS BOOL MessageSetReadReceipt(CMAPIMessage *pMessage, BOOL bSet, LPCTSTR szReceiverEmail);
AFX_EXT_CLASS BOOL MessageSetDeliveryReceipt(CMAPIMessage *pMessage, BOOL bSet);
AFX_EXT_CLASS BOOL MessageMarkAsPrivate(CMAPIMessage *pMessage);
AFX_EXT_CLASS BOOL MessageSetSensitivity(CMAPIMessage *pMessage, int nSensitivity);
AFX_EXT_CLASS BOOL MessageSetNamedProperty(CMAPIMessage *pMessage, LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate);
AFX_EXT_CLASS BOOL MessageSetMessageFlags(CMAPIMessage *pMessage, int nFlags);

// Contact functions
AFX_EXT_CLASS void ContactClose(CMAPIContact *pContact);
AFX_EXT_CLASS BOOL ContactSave(CMAPIContact *pContact, BOOL bClose);
AFX_EXT_CLASS BOOL ContactGetPropertyString(CMAPIContact *pContact, LPTSTR szProperty, int nMaxLength, ULONG ulProperty);
AFX_EXT_CLASS BOOL ContactGetName(CMAPIContact *pContact, LPTSTR szName, int nMaxLength, int nType);
AFX_EXT_CLASS BOOL ContactGetEmail(CMAPIContact *pContact, LPTSTR szEmail, int nMaxLength, int nIndex);
AFX_EXT_CLASS BOOL ContactGetEmailDisplayAs(CMAPIContact *pContact, LPTSTR szDisplayAs, int nMaxLength, int nIndex);
AFX_EXT_CLASS BOOL ContactGetHomePage(CMAPIContact *pContact, LPTSTR szHomePage, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetPhoneNumber(CMAPIContact *pContact, LPTSTR szPhoneNumber, int nMaxLength, int nType);
AFX_EXT_CLASS BOOL ContactGetAddress(CMAPIContact *pContact, CContactAddress *& pAddress, int nType);
AFX_EXT_CLASS BOOL ContactGetPostalAddress(CMAPIContact *pContact, LPTSTR szAddress, int nMaxLength);
AFX_EXT_CLASS int ContactGetNotesSize(CMAPIContact *pContact, BOOL bRTF);
AFX_EXT_CLASS BOOL ContactGetNotes(CMAPIContact *pContact, LPTSTR szNotes, int nMaxLength, BOOL bRTF);
AFX_EXT_CLASS int ContactGetSensitivity(CMAPIContact *pContact);
AFX_EXT_CLASS BOOL ContactGetTitle(CMAPIContact *pContact, LPTSTR szTitle, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetCompany(CMAPIContact *pContact, LPTSTR szCompany, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetDisplayNamePrefix(CMAPIContact *pContact, LPTSTR szDisplayNamePrefix, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetGeneration(CMAPIContact *pContact, LPTSTR szGeneration, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetDepartment(CMAPIContact *pContact, LPTSTR szDepartment, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetOffice(CMAPIContact *pContact, LPTSTR szOffice, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetManagerName(CMAPIContact *pContact, LPTSTR szManagerName, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetAssistantName(CMAPIContact *pContact, LPTSTR szAssistantName, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetNickName(CMAPIContact *pContact, LPTSTR szNickName, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetSpouseName(CMAPIContact *pContact, LPTSTR szSpouseName, int nMaxLength);
AFX_EXT_CLASS BOOL ContactGetBirthday(CMAPIContact *pContact, int& nYear, int& nMonth, int& nDay);
AFX_EXT_CLASS BOOL ContactGetBirthdayString(CMAPIContact *pContact, LPTSTR szBirthday, int nMaxLength, LPCTSTR szFormat);
AFX_EXT_CLASS BOOL ContactGetAnniversary(CMAPIContact *pContact, int& nYear, int& nMonth, int& nDay);
AFX_EXT_CLASS BOOL ContactGetAnniversaryString(CMAPIContact *pContact, LPTSTR szAnniversary, int nMaxLength, LPCTSTR szFormat);
AFX_EXT_CLASS BOOL ContactGetNamedProperty(CMAPIContact *pContact, LPCTSTR szFieldName, LPTSTR szField, int nMaxLength);
AFX_EXT_CLASS BOOL ContactSetPropertyString(CMAPIContact *pContact, LPCTSTR szProperty, ULONG ulProperty);
AFX_EXT_CLASS BOOL ContactSetName(CMAPIContact *pContact, LPCTSTR szName, int nType);
AFX_EXT_CLASS BOOL ContactSetEmail(CMAPIContact *pContact, LPCTSTR szEmail, int nIndex);
AFX_EXT_CLASS BOOL ContactSetEmailDisplayAs(CMAPIContact *pContact, LPCTSTR szDisplayAs, int nIndex);
AFX_EXT_CLASS BOOL ContactSetHomePage(CMAPIContact *pContact, LPCTSTR szHomePage);
AFX_EXT_CLASS BOOL ContactSetPhoneNumber(CMAPIContact *pContact, LPCTSTR szPhoneNumber, int nType);
AFX_EXT_CLASS BOOL ContactSetAddress(CMAPIContact *pContact, CContactAddress *pAddress, CContactAddress::AddressType nType);
AFX_EXT_CLASS BOOL ContactSetPostalAddress(CMAPIContact *pContact, CContactAddress::AddressType nType);
AFX_EXT_CLASS BOOL ContactUpdateDisplayAddress(CMAPIContact *pContact, CContactAddress::AddressType nType);
AFX_EXT_CLASS BOOL ContactSetNotes(CMAPIContact *pContact, LPCTSTR szNotes, BOOL bRTF);
AFX_EXT_CLASS BOOL ContactSetSensitivity(CMAPIContact *pContact, int nSensitivity);
AFX_EXT_CLASS BOOL ContactSetFileAs(CMAPIContact *pContact, LPCTSTR szFileAs);
AFX_EXT_CLASS BOOL ContactSetTitle(CMAPIContact *pContact, LPCTSTR szTitle);
AFX_EXT_CLASS BOOL ContactSetCompany(CMAPIContact *pContact, LPCTSTR szCompany);
AFX_EXT_CLASS BOOL ContactSetDisplayNamePrefix(CMAPIContact *pContact, LPCTSTR szPrefix);
AFX_EXT_CLASS BOOL ContactSetGeneration(CMAPIContact *pContact, LPCTSTR szGeneration);
AFX_EXT_CLASS BOOL ContactUpdateDisplayName(CMAPIContact *pContact);
AFX_EXT_CLASS BOOL ContactSetDepartment(CMAPIContact *pContact, LPCTSTR szDepartment);
AFX_EXT_CLASS BOOL ContactSetOffice(CMAPIContact *pContact, LPCTSTR szOffice);
AFX_EXT_CLASS BOOL ContactSetManagerName(CMAPIContact *pContact, LPCTSTR szManagerName);
AFX_EXT_CLASS BOOL ContactSetAssistantName(CMAPIContact *pContact, LPCTSTR szAssistantName);
AFX_EXT_CLASS BOOL ContactSetNickName(CMAPIContact *pContact, LPCTSTR szNickName);
AFX_EXT_CLASS BOOL ContactSetSpouseName(CMAPIContact *pContact, LPCTSTR szSpouseName);
AFX_EXT_CLASS BOOL ContactSetBirthday(CMAPIContact *pContact, int nYear, int nMonth, int nDay);
AFX_EXT_CLASS BOOL ContactSetAnniversary(CMAPIContact *pContact, int nYear, int nMonth, int nDay);
AFX_EXT_CLASS BOOL ContactSetNamedProperty(CMAPIContact *pContact, LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate);

// Address functions
AFX_EXT_CLASS void AddressClose(CContactAddress *pAddress);
AFX_EXT_CLASS void AddressGetStreet(CContactAddress *pAddress, LPTSTR szStreet, int nMaxLength);
AFX_EXT_CLASS void AddressGetCity(CContactAddress *pAddress, LPTSTR szCity, int nMaxLength);
AFX_EXT_CLASS void AddressGetStateOrProvince(CContactAddress *pAddress, LPTSTR szStateOrProvince, int nMaxLength);
AFX_EXT_CLASS void AddressGetPostalCode(CContactAddress *pAddress, LPTSTR szPostalCode, int nMaxLength);
AFX_EXT_CLASS void AddressGetCountry(CContactAddress *pAddress, LPTSTR szCountry, int nMaxLength);
AFX_EXT_CLASS void AddressSetStreet(CContactAddress *pAddress, LPCTSTR szStreet);
AFX_EXT_CLASS void AddressSetCity(CContactAddress *pAddress, LPCTSTR szCity);
AFX_EXT_CLASS void AddressSetStateOrProvince(CContactAddress *pAddress, LPCTSTR szStateOrProvince);
AFX_EXT_CLASS void AddressSetPostalCode(CContactAddress *pAddress, LPCTSTR szPostalCode);
AFX_EXT_CLASS void AddressSetCountry(CContactAddress *pAddress, LPCTSTR szCountry);
}
#endif
