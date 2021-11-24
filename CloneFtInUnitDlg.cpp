// CloneFtInUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CloneFtInUnitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCloneFtInUnitDlg dialog


CCloneFtInUnitDlg::CCloneFtInUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCloneFtInUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloneFtInUnitDlg)
	m_nUnits = 0;
	//}}AFX_DATA_INIT
}


void CCloneFtInUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloneFtInUnitDlg)
	DDX_Radio(pDX, IDC_FRACTIONAL, m_nUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloneFtInUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CCloneFtInUnitDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloneFtInUnitDlg message handlers
