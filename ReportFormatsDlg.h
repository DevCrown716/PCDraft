#if !defined(AFX_REPORTFORMATSDLG_H__F7322BC6_A4B2_11D2_B526_CA941952EF7E__INCLUDED_)
#define AFX_REPORTFORMATSDLG_H__F7322BC6_A4B2_11D2_B526_CA941952EF7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportFormatsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportFormatsDlg dialog

class CPCDraftDoc;
class CMDReport;

class CReportFormatsDlg : public CDialog
{
// Construction
public:
	void AddReport(CMDReport* pReport);
	CPCDraftDoc* m_pDoc;
	CReportFormatsDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReportFormatsDlg)
	enum { IDD = IDD_REPORT_FORMATS };
	CListBox	m_List;
	CEdit	m_Name;
	//}}AFX_DATA
	int	m_nSelReport;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportFormatsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReportFormatsDlg)
	afx_msg void OnNew();
	afx_msg void OnSelchangeList();
	afx_msg void OnDelete();
	afx_msg void OnRename();
	afx_msg void OnChangeName();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTFORMATSDLG_H__F7322BC6_A4B2_11D2_B526_CA941952EF7E__INCLUDED_)
