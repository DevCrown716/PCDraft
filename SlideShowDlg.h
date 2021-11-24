#if !defined(AFX_SLIDESHOWDLG_H__C4A92266_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_SLIDESHOWDLG_H__C4A92266_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SlideShowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlideShowDlg dialog

class CPCDraftDoc;

class CSlideShowDlg : public CDialog
{
// Construction
public:
	CSlideShowDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSlideShowDlg)
	enum { IDD = IDD_SLIDE_SHOW };
	int		m_nDisplayType;
	BOOL	m_bLayer1AsMaster;
	int		m_nSeconds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlideShowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CSlideShowDlg)
	afx_msg void OnManually();
	afx_msg void OnAutomatic();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDESHOWDLG_H__C4A92266_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
