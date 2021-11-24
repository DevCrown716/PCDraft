// MirrorSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "MirrorSetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMirrorSetupDlg dialog


CMirrorSetupDlg::CMirrorSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMirrorSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMirrorSetupDlg)
	m_nMirror = 0;
	//}}AFX_DATA_INIT
}


void CMirrorSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMirrorSetupDlg)
	DDX_Radio(pDX, IDC_MIRROR, m_nMirror);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMirrorSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CMirrorSetupDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirrorSetupDlg message handlers
