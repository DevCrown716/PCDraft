#if !defined(AFX_CHAMFERSETUPDLG_H__2CCC6AD6_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_CHAMFERSETUPDLG_H__2CCC6AD6_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChamferSetupDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CChamferSetupDlg dialog

class CPCDraftDoc;
class CChamferContext;

class CChamferSetupDlg : public CDialog
{
// Construction
public:
	CChamferSetupDlg(CPCDraftDoc* pDoc,CChamferContext* pCtx,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChamferSetupDlg)
	enum { IDD = IDD_CHAMFER_SETUP };
	CNumberEdit	m_Length2;
	CNumberEdit	m_Length1;
	int		m_nTrim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChamferSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;
	CChamferContext* m_pCtx;

	// Generated message map functions
	//{{AFX_MSG(CChamferSetupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAMFERSETUPDLG_H__2CCC6AD6_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
