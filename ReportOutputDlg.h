#if !defined(AFX_REPORTOUTPUTDLG_H__04E99213_B83D_11D2_B546_AB6770E0047F__INCLUDED_)
#define AFX_REPORTOUTPUTDLG_H__04E99213_B83D_11D2_B546_AB6770E0047F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportOutputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportOutputDlg dialog

class CReportOutputDlg : public CDialog
{
// Construction
public:
	CReportOutputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReportOutputDlg)
	enum { IDD = IDD_REPORT_OUTPUT };
	int		m_nTarget;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportOutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReportOutputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTOUTPUTDLG_H__04E99213_B83D_11D2_B546_AB6770E0047F__INCLUDED_)
