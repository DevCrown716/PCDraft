#if !defined(AFX_SAVEVIEWDLG_H__04A3A786_D25A_11D2_B586_BDB76B20297F__INCLUDED_)
#define AFX_SAVEVIEWDLG_H__04A3A786_D25A_11D2_B586_BDB76B20297F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveViewDlg dialog
class CPCDraftDoc;

class CSaveViewDlg : public CDialog
{
// Construction
public:
	int m_nShortcut;
	CSaveViewDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveViewDlg)
	enum { IDD = IDD_SAVE_VIEW };
	CButton	m_OK;
	CButton	m_UseKey;
	CString	m_sTitle;
	BOOL	m_bUseKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CSaveViewDlg)
	afx_msg void OnChangeViewName();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEVIEWDLG_H__04A3A786_D25A_11D2_B586_BDB76B20297F__INCLUDED_)
