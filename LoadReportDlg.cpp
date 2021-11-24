// LoadReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LoadReportDlg.h"
#include "PCDraftDoc.h"
#include "MDReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadReportDlg dialog


CLoadReportDlg::CLoadReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadReportDlg)
	m_nReportIndex = -1;
	//}}AFX_DATA_INIT
}


void CLoadReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadReportDlg)
	DDX_Control(pDX, IDC_REPORT_LIST, m_ReportList);
	DDX_LBIndex(pDX, IDC_REPORT_LIST, m_nReportIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadReportDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadReportDlg)
	ON_LBN_SELCHANGE(IDC_REPORT_LIST, OnSelchangeReportList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadReportDlg message handlers

BOOL CLoadReportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ReportList.ResetContent();
	for(int i=0;i<m_pDoc->m_Reports.GetSize();i++)
	{
		CMDReport* pReport=m_pDoc->m_Reports[i];
		m_ReportList.AddString(pReport->m_sTitle);
	}

	CString theTitle;
	theTitle.Format(IDS_REPORTS,m_pDoc->GetTitle());
	SetWindowText(theTitle);
	
	return TRUE;
}

void CLoadReportDlg::OnSelchangeReportList() 
{
	GetDlgItem(IDOK)->EnableWindow(m_ReportList.GetCurSel()!=LB_ERR);
}
