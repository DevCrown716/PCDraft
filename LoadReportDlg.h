#if !defined(AFX_LOADREPORTDLG_H__E00EE257_089E_11D3_9960_000000000000__INCLUDED_)
#define AFX_LOADREPORTDLG_H__E00EE257_089E_11D3_9960_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadReportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadReportDlg dialog

class CPCDraftDoc;

class CLoadReportDlg : public CDialog
{
// Construction
public:
	CLoadReportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadReportDlg)
	enum { IDD = IDD_LOAD_REPORT };
	CListBox	m_ReportList;
	int		m_nReportIndex;
	//}}AFX_DATA
	CPCDraftDoc* m_pDoc;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadReportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeReportList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADREPORTDLG_H__E00EE257_089E_11D3_9960_000000000000__INCLUDED_)
