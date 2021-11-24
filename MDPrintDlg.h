#ifndef AFX_MDPRINTDLG_H__96321256_E539_11D3_9896_00A0C9A21BD0__INCLUDED_
#define AFX_MDPRINTDLG_H__96321256_E539_11D3_9896_00A0C9A21BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MDPrintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDPrintDlg dialog

class CMDPrintDlg : public CPrintDialog
{
	DECLARE_DYNAMIC(CMDPrintDlg)

public:
	CMDPrintDlg(BOOL bPrintSetupOnly,
		// TRUE for Print Setup, FALSE for Print Dialog
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOSELECTION,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CMDPrintDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDPRINTDLG_H__96321256_E539_11D3_9896_00A0C9A21BD0__INCLUDED_)
