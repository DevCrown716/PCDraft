// MoreWindowsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "MoreWindowsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoreWindowsDlg dialog


CMoreWindowsDlg::CMoreWindowsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoreWindowsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoreWindowsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMoreWindowsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoreWindowsDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoreWindowsDlg, CDialog)
	//{{AFX_MSG_MAP(CMoreWindowsDlg)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoreWindowsDlg message handlers

BOOL CMoreWindowsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMDIFrameWnd* pMDIFrame=(CMDIFrameWnd* )AfxGetMainWnd();
	CMDIChildWnd* pActiveChild=pMDIFrame->MDIGetActive();
	HWND hWndMDIClient=pMDIFrame->m_hWndMDIClient;
	for(HWND hWnd=::GetWindow(hWndMDIClient,GW_CHILD);hWnd;hWnd=::GetWindow(hWnd,GW_HWNDNEXT))
	{
		char buf[256];
		::GetWindowText(hWnd,buf,256);
		int n=m_List.AddString(buf);
		m_List.SetItemData(n, (unsigned __int64)hWnd);
		if(hWnd==pActiveChild->GetSafeHwnd())
			m_List.SetCurSel(n);
    }
	return TRUE;
}

void CMoreWindowsDlg::OnDblclkList() 
{
	ActivateCurSel();
	EndDialog(IDOK);
}

void CMoreWindowsDlg::ActivateCurSel()
{
	int n=m_List.GetCurSel();
	HWND hWnd=(HWND)m_List.GetItemData(n);
	CWnd* pWnd=CWnd::FromHandle(hWnd);
	((CMDIFrameWnd* )AfxGetMainWnd())->MDIActivate(pWnd);
	if(pWnd->IsIconic())
		pWnd->ShowWindow(SW_SHOWNORMAL);
}

void CMoreWindowsDlg::OnOK() 
{
	ActivateCurSel();
	CDialog::OnOK();
}
