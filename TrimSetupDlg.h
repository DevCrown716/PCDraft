#if !defined(AFX_TRIMSETUPDLG_H__2CCC6AD5_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_TRIMSETUPDLG_H__2CCC6AD5_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrimSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrimSetupDlg dialog

class CTrimContext;

class CTrimSetupDlg : public CDialog
{
// Construction
public:
	CTrimSetupDlg(CTrimContext* pCtx,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrimSetupDlg)
	enum { IDD = IDD_TRIM_SETUP };
	int		m_nTrim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrimSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTrimContext* m_pCtx;

	// Generated message map functions
	//{{AFX_MSG(CTrimSetupDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIMSETUPDLG_H__2CCC6AD5_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
