#if !defined(AFX_REGISTRATIONDLG_H__7E2509B3_259C_11D4_9968_000000000000__INCLUDED_)
#define AFX_REGISTRATIONDLG_H__7E2509B3_259C_11D4_9968_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegistrationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg dialog

class CRegistrationDlg : public CDialog
{
// Construction
public:
	CRegistrationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegistrationDlg)
	enum { IDD = IDD_REGISTRATION };
	CString	m_sName;
	CString	m_sOrganization;
	CString	m_sSerialNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistrationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Check();

	// Generated message map functions
	//{{AFX_MSG(CRegistrationDlg)
	afx_msg void OnChangeName();
	afx_msg void OnChangeSerial();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRATIONDLG_H__7E2509B3_259C_11D4_9968_000000000000__INCLUDED_)
