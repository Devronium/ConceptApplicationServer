#ifndef __MAPINAMEDPROPERTY_H__
#define __MAPINAMEDPROPERTY_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: MAPINamedProperty.h
// Description: MAPI Named Property class wrapper
//
// Copyright (C) 2005-2006, Noel Dillabough
//
// This source code is free to use and modify provided this notice remains intact and that any enhancements
// or bug fixes are posted to the CodeProject page hosting this class for the community to benefit.
//
// Usage: see the Codeproject article at http://www.codeproject.com/internet/CMapiEx.asp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// CMAPINamedProperty

class AFX_EXT_CLASS CMAPINamedProperty : public CObject
{
public:
    CMAPINamedProperty(IMAPIProp *pItem);
    ~CMAPINamedProperty();

// Attributes
protected:
    IMAPIProp *m_pItem;

// Operations
public:
    BOOL GetOutlookProperty(ULONG ulData1, ULONG ulProperty, LPSPropValue& pProp);
    BOOL GetNamedProperty(LPCTSTR szFieldName, LPSPropValue& pProp);
    BOOL GetNamedProperty(LPCTSTR szFieldName, CString& strField);

    BOOL SetOutlookProperty(ULONG ulData1, ULONG ulProperty, LPCTSTR szField);
    BOOL SetOutlookProperty(ULONG ulData1, ULONG ulProperty, int nField);
    BOOL SetNamedProperty(LPCTSTR szFieldName, LPCTSTR szField, BOOL bCreate = TRUE);
};
#endif
