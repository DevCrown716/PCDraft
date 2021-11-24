#if !defined(AFX_SELECTOBJECTINFODLG_H__C4A92265_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_SELECTOBJECTINFODLG_H__C4A92265_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectObjectInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectObjectInfoDlg dialog

class CSelectObjectInfoDlg : public CDialog
{
// Construction
public:
	CSelectObjectInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectObjectInfoDlg)
	enum { IDD = IDD_SELECT_OBJECT_INFO };
	BOOL	m_bUse1;
	CString	m_sValue1;
	BOOL	m_bUse2;
	CString	m_sValue2;
	BOOL	m_bUse3;
	CString	m_sValue3;
	BOOL	m_bUse4;
	CString	m_sValue4;
	BOOL	m_bUse5;
	CString	m_sValue5;
	CString	m_sLabel1;
	CString	m_sLabel2;
	CString	m_sLabel3;
	CString	m_sLabel4;
	CString	m_sLabel5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectObjectInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectObjectInfoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTOBJECTINFODLG_H__C4A92265_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
