#if !defined(AFX_SLIDESHOWVIEW_H__9E277628_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_)
#define AFX_SLIDESHOWVIEW_H__9E277628_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlideShowView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlideShowView view
class CPCDraftDoc;

class CSlideShowView : public CView
{
protected:
	CSlideShowView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSlideShowView)

// Attributes
public:
	bool m_bLayer1AsMaster;
	int m_nSeconds;
	int m_nSlide;

// Operations
public:
	void NextSlide(bool bForward);
	UINT m_nTimerID;
	CPCDraftDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlideShowView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSlideShowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSlideShowView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PCDraftView.cpp
inline CPCDraftDoc* CSlideShowView::GetDocument()
   { return (CPCDraftDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDESHOWVIEW_H__9E277628_FBB6_11D2_B5E7_BF3C23BA337F__INCLUDED_)
