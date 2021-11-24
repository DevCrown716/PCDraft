#if !defined(AFX_FILLWND_H__9D285B53_ED20_11D2_B5B9_D1649F53267F__INCLUDED_)
#define AFX_FILLWND_H__9D285B53_ED20_11D2_B5B9_D1649F53267F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FillWnd.h : header file
//
#include "DSShape.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CFillWnd window

class CPCDraftDoc;

class CFillWnd : public CWnd
{
// Construction
public:
	CFillWnd();
	BOOL Create(CRect& rc,CWnd *pParent,UINT nID);

// Attributes
public:
	CPCDraftDoc* m_pDoc;
	FillModel m_Fill;
// Operations
public:
	void SetDoc(CPCDraftDoc* pDoc);
	void SetFill(const FillModel& fm);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFillWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFillWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFillWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILLWND_H__9D285B53_ED20_11D2_B5B9_D1649F53267F__INCLUDED_)
