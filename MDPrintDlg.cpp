// MDPrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "MDPrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDPrintDlg

IMPLEMENT_DYNAMIC(CMDPrintDlg, CPrintDialog)

CMDPrintDlg::CMDPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) :
	CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CMDPrintDlg, CPrintDialog)
	//{{AFX_MSG_MAP(CMDPrintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMDPrintDlg::OnInitDialog() 
{
	CPrintDialog::OnInitDialog();
//	GetDlgItem(1139)->EnableWindow(FALSE);	// printers combo
//	GetDlgItem(1025)->EnableWindow(FALSE);	// properties button
	return TRUE;
}
