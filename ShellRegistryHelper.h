#if !defined(AFX_SHELLREGISTRYHELPER_H__B1880846_7F17_11D4_99E6_000021D1C7AF__INCLUDED_)
#define AFX_SHELLREGISTRYHELPER_H__B1880846_7F17_11D4_99E6_000021D1C7AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
{
	kRegisterOpen = 0x0001,
	kRegisterPrint = 0x0002,
	kRegisterNew = 0x0004,
	kRegisterAll = kRegisterOpen|kRegisterPrint|kRegisterNew
};


void RegisterFileExt(UINT nID,LPCTSTR lpszExt);
void RegisterFile(UINT nID,int nIconIndex=0,int nFlags=kRegisterAll,LPCTSTR lpszAppName=NULL);
void RegisterFileExt(LPCTSTR lpszFullString,LPCTSTR lpszExt);
void RegisterFile(LPCTSTR lpszFullString,int nIconIndex=0,int nFlags=kRegisterAll,LPCTSTR lpszAppName=NULL);

void RegisterFileExt(CDocTemplate* pTemplate,LPCTSTR lpszExt);
void RegisterFile(CDocTemplate* pTemplate,int nIconIndex=0,int nFlags=kRegisterAll,LPCTSTR lpszAppName=NULL);

#endif // !defined(AFX_SHELLREGISTRYHELPER_H__B1880846_7F17_11D4_99E6_000021D1C7AF__INCLUDED_)
