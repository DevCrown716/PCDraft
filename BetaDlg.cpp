// BetaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "BetaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBetaDlg dialog


CBetaDlg::CBetaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBetaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBetaDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBetaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBetaDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBetaDlg, CDialog)
	//{{AFX_MSG_MAP(CBetaDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBetaDlg message handlers

#define kTimerID	1

BOOL CBetaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_nSeconds=15;
	UpdateSeconds();
	SetTimer(kTimerID,1000,NULL);
	
	return TRUE;
}

void CBetaDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(kTimerID==nIDEvent)
	{
		m_nSeconds--;
		UpdateSeconds();
		if(m_nSeconds<0)
		{
			KillTimer(kTimerID);
			EndDialog(IDOK);
		}
	}
}

void CBetaDlg::UpdateSeconds()
{
	CString strSeconds;
	strSeconds.Format(IDS_BETA_SECONDS,m_nSeconds);
	GetDlgItem(IDC_SECONDS)->SetWindowText(strSeconds);
}

void CBetaDlg::OnOK()
{
	// do nothing
}

void CBetaDlg::OnCancel()
{
	// do nothing
}
