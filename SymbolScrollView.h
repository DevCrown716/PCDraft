#if !defined(AFX_SYMBOLSCROLLVIEW_H__C73E9FB1_EAC8_11D2_9939_000000000000__INCLUDED_)
#define AFX_SYMBOLSCROLLVIEW_H__C73E9FB1_EAC8_11D2_9939_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SymbolScrollView.h : header file
//

class CMDShape;

/////////////////////////////////////////////////////////////////////////////
// CSymbolScrollView view

class CSymbolScrollView : public CScrollView
{
protected:
	DECLARE_DYNCREATE(CSymbolScrollView)

// Attributes
public:

// Operations
public:
	CSymbolScrollView();
	void CenterShapeDatum();
	void SetShape(CMDShape* pShape);
	void HideHandles(bool bHideHandles);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolScrollView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMDShape* m_pShape;
	bool m_bHideHandles;

	virtual ~CSymbolScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSymbolScrollView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMBOLSCROLLVIEW_H__C73E9FB1_EAC8_11D2_9939_000000000000__INCLUDED_)
