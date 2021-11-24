// CommandLineInfoEx.h: interface for the CCommandLineInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEINFOEX_H__141B07D5_D4CA_47ED_AF04_DA7354AA2A7A__INCLUDED_)
#define AFX_COMMANDLINEINFOEX_H__141B07D5_D4CA_47ED_AF04_DA7354AA2A7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommandLineInfoEx : public CCommandLineInfo  
{
public:
	CCommandLineInfoEx();
	~CCommandLineInfoEx();

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
	CStringArray m_arrFileNames;
    bool m_bEnableFilewall;

};

#endif // !defined(AFX_COMMANDLINEINFOEX_H__141B07D5_D4CA_47ED_AF04_DA7354AA2A7A__INCLUDED_)
