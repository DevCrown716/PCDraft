#if !defined(AFX_CLONEUNITSDLG_H__9E1C1583_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
#define AFX_CLONEUNITSDLG_H__9E1C1583_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloneUnitsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloneUnitsDlg dialog

class CCloneUnitsDlg : public CDialog
{
// Construction
public:
	CCloneUnitsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCloneUnitsDlg)
	enum { IDD = IDD_CLONE_UNITS };
	int		m_nUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloneUnitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCloneUnitsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLONEUNITSDLG_H__9E1C1583_7CB3_11D3_A119_00A0C9A0C319__INCLUDED_)
