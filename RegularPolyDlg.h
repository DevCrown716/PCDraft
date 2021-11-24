#if !defined(AFX_REGULARPOLYDLG_H__2CCC6AD8_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_REGULARPOLYDLG_H__2CCC6AD8_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegularPolyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegularPolyDlg dialog

class CRegularPolyDlg : public CDialog
{
// Construction
public:
	CRegularPolyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegularPolyDlg)
	enum { IDD = IDD_REG_POLY };
	int		m_nSides;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegularPolyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegularPolyDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGULARPOLYDLG_H__2CCC6AD8_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
