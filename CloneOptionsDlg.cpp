// CloneOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CloneOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCloneOptionsDlg dialog


CCloneOptionsDlg::CCloneOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCloneOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloneOptionsDlg)
	m_nCopyType = -1;
	//}}AFX_DATA_INIT
}


void CCloneOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloneOptionsDlg)
	DDX_Radio(pDX, IDC_COPY_ONLY, m_nCopyType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloneOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCloneOptionsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloneOptionsDlg message handlers
