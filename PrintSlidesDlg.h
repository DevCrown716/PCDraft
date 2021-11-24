#if !defined(AFX_PRINTSLIDESDLG_H__948D4BC0_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
#define AFX_PRINTSLIDESDLG_H__948D4BC0_5892_11D2_B477_9B4FC958F3A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrintSlidesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintSlidesDlg dialog

class CPCDraftDoc;

class CPrintSlidesDlg : public CDialog
{
// Construction
public:
	CPrintSlidesDlg(CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintSlidesDlg)
	enum { IDD = IDD_PRINT_SLIDES };
	int		m_nFitFull;
	BOOL	m_bLayer1AsMaster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintSlidesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CPrintSlidesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTSLIDESDLG_H__948D4BC0_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
