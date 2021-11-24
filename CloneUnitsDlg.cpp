// CloneUnitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CloneUnitsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCloneUnitsDlg dialog


CCloneUnitsDlg::CCloneUnitsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCloneUnitsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloneUnitsDlg)
	m_nUnits = 0;
	//}}AFX_DATA_INIT
}


void CCloneUnitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloneUnitsDlg)
	DDX_Radio(pDX, IDC_FRACTIONAL, m_nUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloneUnitsDlg, CDialog)
	//{{AFX_MSG_MAP(CCloneUnitsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloneUnitsDlg message handlers
