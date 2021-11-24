#if !defined(AFX_MOREWINDOWSDLG_H__2F4FC993_DA6C_11D3_AFAB_000000000000__INCLUDED_)
#define AFX_MOREWINDOWSDLG_H__2F4FC993_DA6C_11D3_AFAB_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoreWindowsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMoreWindowsDlg dialog

class CMoreWindowsDlg : public CDialog
{
// Construction
public:
	CMoreWindowsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMoreWindowsDlg)
	enum { IDD = IDD_MORE_WINDOWS };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoreWindowsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ActivateCurSel();

	// Generated message map functions
	//{{AFX_MSG(CMoreWindowsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOREWINDOWSDLG_H__2F4FC993_DA6C_11D3_AFAB_000000000000__INCLUDED_)
