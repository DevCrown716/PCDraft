#if !defined(AFX_CONTRACTEXPANDDLG_H__18015BA3_9105_46C7_902A_95023CED909C__INCLUDED_)
#define AFX_CONTRACTEXPANDDLG_H__18015BA3_9105_46C7_902A_95023CED909C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContractExpandDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContractExpandDlg dialog

class CContractExpandDlg : public CDialog
{
// Construction
public:
	void ValidateData();
	CContractExpandDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CContractExpandDlg)
	enum { IDD = IDD_CONTRACT_EXPAND };
	//}}AFX_DATA

	static double m_nX;
	static double m_nY;
	static BOOL m_bLinked;

	bool m_bUseX;
	bool m_bContract;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContractExpandDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateLinkedFeedback();

	// Generated message map functions
	//{{AFX_MSG(CContractExpandDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLinked();
	afx_msg void OnChangeX();
	afx_msg void OnChangeY();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTRACTEXPANDDLG_H__18015BA3_9105_46C7_902A_95023CED909C__INCLUDED_)
