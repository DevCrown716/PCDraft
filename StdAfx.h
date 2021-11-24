// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F8C83DF6_0F91_11D2_9806_000000000000__INCLUDED_)
#define AFX_STDAFX_H__F8C83DF6_0F91_11D2_9806_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#define OEMRESOURCE
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define GDIPVER 0x0110

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#include <afxdisp.h>
#include <afxpriv.h>
#include "XMLite.h"
#include "GDIHelper.h"
#include "OdaCommon.h"
#include <afxcontrolbars.h>
#include <afxcview.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
#include <afxwin.h>

#define _LIBRARY_PALETTE

inline CArchive& operator >> (CArchive& ar, bool& rhs)
{
	ar.Read(&rhs, sizeof(rhs));
	return ar;
}

inline CArchive& operator << (CArchive& ar, bool rhs)
{
	ar.Write(&rhs, sizeof(rhs));
	return ar;
}

CStringW get_uni(LPCSTR lpcsz,int iCodePage = CP_UTF8);
CString get_cp(LPCWSTR lpcwsz,int iCodePage = CP_UTF8);
CString get_utf8(LPCWSTR lpcwsz);
CSize get_screen_dpi();


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F8C83DF6_0F91_11D2_9806_000000000000__INCLUDED_)
