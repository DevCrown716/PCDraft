// FileDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FileDialogEx.h"

#include <dlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, 0)
{
}

BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFileDialogEx::OnFileNameOK()
{
	if(!m_bOpenFileDialog)
	{
		if(GetFileExt().IsEmpty())
		{
			UpdateFileExt();
			if(m_ofn.Flags & OFN_OVERWRITEPROMPT)
			{
				CString thePathName=GetPathName();
				CFileStatus theStatus;
				if(CFile::GetStatus(thePathName,theStatus))
				{
					CString thePrompt;
					thePrompt.Format(IDS_OVERWRITEPROMPT,thePathName);
					return AfxMessageBox(thePrompt,MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION)==IDYES?0:1;
				}
			}
		}
	}
	return 0;
}

void CFileDialogEx::UpdateFileExt()
{
	if(!m_bOpenFileDialog)
	{
		CString theFileName=GetFileTitle();
		LPCTSTR p=m_ofn.lpstrFilter,pszExt;
		for(DWORD i=0;i<m_ofn.nFilterIndex;i++)
		{
			p+=strlen(p)+1;
			pszExt=p+1;
			p+=strlen(p)+1;
		}
		theFileName+=pszExt;
		SetControlText(edt1,theFileName);
	}
}

void CFileDialogEx::OnTypeChange()
{
	UpdateFileExt();
}
