#if !defined(AFX_PARALLELLINEDLG_H__2CCC6AD3_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
#define AFX_PARALLELLINEDLG_H__2CCC6AD3_735A_11D2_B4BB_9AD066F6B264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParallelLineDlg.h : header file
//

#include "NumberEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CParallelLineDlg dialog

struct ParallelAttrs;
class CPCDraftDoc;

class CParallelLineDlg : public CDialog
{
// Construction
public:
	CParallelLineDlg(ParallelAttrs* pAttrs,CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParallelLineDlg)
	enum { IDD = IDD_PARALLEL_LINE };
	CNumberEdit	m_LineWidth;
	int		m_nAlign;
	int		m_nEndCap;
	int		m_nJoint;
	CString	m_sLimits;
	CString	m_sUnits;
	//}}AFX_DATA
	ParallelAttrs* m_pAttrs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParallelLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CParallelLineDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARALLELLINEDLG_H__2CCC6AD3_735A_11D2_B4BB_9AD066F6B264__INCLUDED_)
