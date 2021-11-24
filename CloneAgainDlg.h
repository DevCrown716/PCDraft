#if !defined(AFX_CLONEAGAINDLG_H__9E1C1584_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
#define AFX_CLONEAGAINDLG_H__9E1C1584_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloneAgainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloneAgainDlg dialog

class CCloneAgainDlg : public CDialog
{
// Construction
public:
	CCloneAgainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCloneAgainDlg)
	enum { IDD = IDD_CLONE_AGAIN };
	int		m_nType;
	CString	m_CloneAgain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloneAgainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCloneAgainDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLONEAGAINDLG_H__9E1C1584_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
