#if !defined(AFX_LINESTYLEWND_H__9D285B54_ED20_11D2_B5B9_D1649F53267F__INCLUDED_)
#define AFX_LINESTYLEWND_H__9D285B54_ED20_11D2_B5B9_D1649F53267F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineStyleWnd.h : header file
//
#include "DSShape.h"	// Added by ClassView
#include "DocMapper.h"

/////////////////////////////////////////////////////////////////////////////
// CLineStyleWnd window

class CPCDraftDoc;

class CLineStyleWnd : public CWnd
{
// Construction
public:
	CLineStyleWnd();
	BOOL Create(CRect& rc,CWnd *pParent,UINT nID);

// Attributes
public:
	CPCDraftDoc* m_pDoc;
	PenModel m_Pen;
	CDocMapper	m_Mapper;

// Operations
public:
	void SetDoc(CPCDraftDoc* pDoc);
	void SetPen(const PenModel& pm);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineStyleWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLineStyleWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineStyleWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINESTYLEWND_H__9D285B54_ED20_11D2_B5B9_D1649F53267F__INCLUDED_)
