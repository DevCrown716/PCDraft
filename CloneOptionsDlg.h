#if !defined(AFX_CLONEOPTIONSDLG_H__72D5C073_7CEA_11D3_B71B_A832AFBCAC74__INCLUDED_)
#define AFX_CLONEOPTIONSDLG_H__72D5C073_7CEA_11D3_B71B_A832AFBCAC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloneOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloneOptionsDlg dialog

class CCloneOptionsDlg : public CDialog
{
// Construction
public:
	CCloneOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCloneOptionsDlg)
	enum { IDD = IDD_CLONE_OPTIONS };
	int		m_nCopyType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloneOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCloneOptionsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLONEOPTIONSDLG_H__72D5C073_7CEA_11D3_B71B_A832AFBCAC74__INCLUDED_)
