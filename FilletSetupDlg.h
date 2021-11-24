#if !defined(AFX_FILLETSETUPDLG_H__2CCC6AD4_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_FILLETSETUPDLG_H__2CCC6AD4_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilletSetupDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CFilletSetupDlg dialog

class CPCDraftDoc;
class CFilletContext;

class CFilletSetupDlg : public CDialog
{
// Construction
public:
	CFilletSetupDlg(CPCDraftDoc* pDoc,CFilletContext* pCtx,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilletSetupDlg)
	enum { IDD = IDD_FILLET_SETUP };
	CNumberEdit	m_Radius;
	int		m_nTrim;
	int		m_nArc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilletSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;
	CFilletContext* m_pCtx;

	// Generated message map functions
	//{{AFX_MSG(CFilletSetupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILLETSETUPDLG_H__2CCC6AD4_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
