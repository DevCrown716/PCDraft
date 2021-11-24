#if !defined(AFX_BETADLG_H__9564DD23_8118_11D5_9C72_000021D1C71F__INCLUDED_)
#define AFX_BETADLG_H__9564DD23_8118_11D5_9C72_000021D1C71F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BetaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBetaDlg dialog

class CBetaDlg : public CDialog
{
// Construction
public:
	void OnCancel();
	void OnOK();
	void UpdateSeconds();
	int m_nSeconds;
	CBetaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBetaDlg)
	enum { IDD = IDD_BETA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBetaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBetaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BETADLG_H__9564DD23_8118_11D5_9C72_000021D1C71F__INCLUDED_)
