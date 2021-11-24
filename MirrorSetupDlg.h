#if !defined(AFX_MIRRORSETUPDLG_H__2CCC6AD7_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_MIRRORSETUPDLG_H__2CCC6AD7_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MirrorSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMirrorSetupDlg dialog

class CMirrorSetupDlg : public CDialog
{
// Construction
public:
	CMirrorSetupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMirrorSetupDlg)
	enum { IDD = IDD_MIRROR_SETUP };
	int		m_nMirror;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirrorSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMirrorSetupDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIRRORSETUPDLG_H__2CCC6AD7_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
