// CloneAgainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CloneAgainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCloneAgainDlg dialog


CCloneAgainDlg::CCloneAgainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCloneAgainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloneAgainDlg)
	m_nType = -1;
	m_CloneAgain = _T("");
	//}}AFX_DATA_INIT
}


void CCloneAgainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloneAgainDlg)
	DDX_Radio(pDX, IDC_BREAK_CLONE, m_nType);
	DDX_Text(pDX, IDC_CLONE_AGAIN, m_CloneAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloneAgainDlg, CDialog)
	//{{AFX_MSG_MAP(CCloneAgainDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloneAgainDlg message handlers
