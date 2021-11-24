#if !defined(AFX_SELECTVIEWDLG_H__077B9DB6_F0C0_11D2_B5C3_B70A4B4D0E7F__INCLUDED_)
#define AFX_SELECTVIEWDLG_H__077B9DB6_F0C0_11D2_B5C3_B70A4B4D0E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectViewDlg dialog

class CPCDraftDoc;

class CSelectViewDlg : public CDialog
{
// Construction
public:
	CSelectViewDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectViewDlg)
	enum { IDD = IDD_SELECT_VIEW };
	CButton	m_OK;
	CListBox	m_ViewList;
	int		m_nView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CSelectViewDlg)
	afx_msg void OnSelchangeViewList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTVIEWDLG_H__077B9DB6_F0C0_11D2_B5C3_B70A4B4D0E7F__INCLUDED_)
