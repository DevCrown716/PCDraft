// ReportOutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportOutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportOutputDlg dialog


CReportOutputDlg::CReportOutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportOutputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportOutputDlg)
	m_nTarget = 0;
	//}}AFX_DATA_INIT
}


void CReportOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportOutputDlg)
	DDX_Radio(pDX, IDC_PRINTER, m_nTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportOutputDlg, CDialog)
	//{{AFX_MSG_MAP(CReportOutputDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportOutputDlg message handlers
