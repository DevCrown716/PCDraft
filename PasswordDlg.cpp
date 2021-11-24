// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _PERSONAL_EDITION
#include "resource.h"       // main symbols
#include <OdaCommon.h>
#include <OdToolKit.h>
#include <DbSecurity.h>
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_sFileName = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_EDIT_PWD, m_pswCtrl);
	DDX_Text(pDX, IDC_PASS_FILE_NAME, m_sFileName);
	//}}AFX_DATA_MAP
  if(pDX->m_bSaveAndValidate)
  {
    int n = ::GetWindowTextLength(m_pswCtrl.m_hWnd);
    // m_password.resize(n);
	LPTSTR str = new char[n+1];
	//for(int i=0; i< n; i++)
	//	m_password.setAt
    ::GetWindowText(m_pswCtrl.m_hWnd, str, n+1);
	for(int i=0; i< n; i++)
		m_password.setAt(i, str[i]);
	delete[] str;
  }
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers
#endif