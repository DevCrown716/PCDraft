#if !defined(AFX_BADDWGOBJECTDLG_H__9556FD63_94A1_11D6_9CB0_000021D1C71F__INCLUDED_)
#define AFX_BADDWGOBJECTDLG_H__9556FD63_94A1_11D6_9CB0_000021D1C71F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BadDWGObjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBadDWGObjectDlg dialog

class CBadDWGObjectDlg : public CDialog
{
// Construction
public:
	CBadDWGObjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBadDWGObjectDlg)
	enum { IDD = IDD_BADDWGOBJECT };
	CString	m_strText;
	//}}AFX_DATA

	bool m_bDontShow;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBadDWGObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBadDWGObjectDlg)
	afx_msg void OnDontshow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BADDWGOBJECTDLG_H__9556FD63_94A1_11D6_9CB0_000021D1C71F__INCLUDED_)
