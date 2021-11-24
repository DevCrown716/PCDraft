#if !defined(AFX_FIELDNAMESDLG_H__7F1F8051_524B_11D2_B464_E13F5088D6A4__INCLUDED_)
#define AFX_FIELDNAMESDLG_H__7F1F8051_524B_11D2_B464_E13F5088D6A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FieldNamesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FieldNamesDlg dialog

class CFieldNamesDlg : public CDialog
{
// Construction
public:
	CFieldNamesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldNamesDlg)
	enum { IDD = IDD_FIELD_NAMES };
	CString	m_sName1;
	CString	m_sName2;
	CString	m_sName3;
	CString	m_sName4;
	CString	m_sName5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldNamesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFieldNamesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDNAMESDLG_H__7F1F8051_524B_11D2_B464_E13F5088D6A4__INCLUDED_)
