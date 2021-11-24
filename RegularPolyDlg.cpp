// RegularPolyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "RegularPolyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegularPolyDlg dialog


CRegularPolyDlg::CRegularPolyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegularPolyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegularPolyDlg)
	m_nSides = 0;
	//}}AFX_DATA_INIT
}


void CRegularPolyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegularPolyDlg)
	DDX_Text(pDX, IDC_SIDES, m_nSides);
	DDV_MinMaxInt(pDX, m_nSides, 3, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegularPolyDlg, CDialog)
	//{{AFX_MSG_MAP(CRegularPolyDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegularPolyDlg message handlers
