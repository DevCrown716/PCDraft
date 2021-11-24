// TrimSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "TrimSetupDlg.h"

#include "TrimContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrimSetupDlg dialog


CTrimSetupDlg::CTrimSetupDlg(CTrimContext* pCtx,CWnd* pParent /*=NULL*/)
	: CDialog(CTrimSetupDlg::IDD, pParent)
{
	m_pCtx=pCtx;
	//{{AFX_DATA_INIT(CTrimSetupDlg)
	m_nTrim = m_pCtx->m_nTrimMethod;
	//}}AFX_DATA_INIT
}


void CTrimSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrimSetupDlg)
	DDX_Radio(pDX, IDC_AUTO_TRIM, m_nTrim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrimSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CTrimSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrimSetupDlg message handlers

void CTrimSetupDlg::OnOK() 
{
	CDialog::OnOK();
	m_pCtx->m_nTrimMethod=m_nTrim;
}
