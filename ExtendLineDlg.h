#if !defined(AFX_EXTENDLINEDLG_H__F2BAB927_C89E_11D2_B573_F24958CE0E7F__INCLUDED_)
#define AFX_EXTENDLINEDLG_H__F2BAB927_C89E_11D2_B573_F24958CE0E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtendLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtendLineDlg dialog

class CExtendLineDlg : public CDialog
{
// Construction
public:
	CExtendLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExtendLineDlg)
	enum { IDD = IDD_EXTEND_LINE };
	int		m_nExtendTo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtendLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtendLineDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTENDLINEDLG_H__F2BAB927_C89E_11D2_B573_F24958CE0E7F__INCLUDED_)
